#include <iostream>
#include <unistd.h>

using namespace std;

typedef void ThreadFun(int threadNum);

struct Context{
  //Numbers written in comments are byte alignment in struct
  long fp; //AKA x29  0
  long lr; //AKA x30  8
  long sp; // 16
  long x1; // 24
  long x2; // 32
  long x3; // 40
  long x4; // 48
  long x5; // 56
  long x6; // 64
  long x7; // 72
  long x8; // 80
  long x9; // 88
  long x10; // 96
  long x11; // 104
  long x12; // 112
  long x13; // 120
  long x14; // 128
  long x15; // 136
  long x0; // 144
};

//These arrays could be made bigger to accadate more threads
//This program only needs 3 so I have chosen to only keep 3 contexts and 3 stacks
Context contexts[3];
char* stacks[3];

int threadCount = 0;
int activeThread = 0;

void saveRegi(Context *threadContext){ //Skips saving the FP and LR
  asm("str x1, [x0, #24]"); 

  asm("str x2, [x0, #32]");
  asm("str x3, [x0, #40]");
  asm("str x4, [x0, #48]");
  asm("str x5, [x0, #56]");
  asm("str x6, [x0, #64]");
  asm("str x7, [x0, #72]");
  asm("str x8, [x0, #80]");
  asm("str x9, [x0, #88]");
  asm("str x10, [x0, #96]");
  asm("str x11, [x0, #104]");
  asm("str x12, [x0, #112]");
  asm("str x13, [x0, #120]");
  asm("str x14, [x0, #128]");
  asm("str x15, [x0, #136]");

  //You cannot load or store the stack pointer directly in ARM64
  //Therefor I have to use another register as a temporary space
  asm("mov x1, sp");
  asm("str x1, [x0, #16]");
}

void loadRegi(Context *threadContext){
  asm("ldr x2, [x0, #32]");
  asm("ldr x3, [x0, #40]");
  asm("ldr x4, [x0, #48]");
  asm("ldr x5, [x0, #56]");
  asm("ldr x6, [x0, #64]");
  asm("ldr x7, [x0, #72]");
  asm("ldr x8, [x0, #80]");
  asm("ldr x9, [x0, #88]");
  asm("ldr x10, [x0, #96]");
  asm("ldr x11, [x0, #104]");
  asm("ldr x12, [x0, #112]");
  asm("ldr x13, [x0, #120]");
  asm("ldr x14, [x0, #128]");
  asm("ldr x15, [x0, #136]");

  //Register x18 is reserved for the platform
  
  //You cannot load or store the stack pointer directly in ARM64
  //Therefor I have to use another register as a temporary space
  asm("ldr x1, [x0, #16]");
  asm("mov sp, x1");

  asm("ldr x1, [x0, #24]");
}

void setStackRun(ThreadFun *f, char* stack){
  //This function will automatically store x0 on the old stack before this operation
  asm("mov sp, x1");

  //I need to push x0 back on the stack where the compiler expexts it to be
  asm("str x0, [sp, #8]");
  f(activeThread);
}

void startThread(ThreadFun *f) {
  //When we branch into saveRegi the fp and lr will get overwritten
  //Therefor I need to save those 2 registers and x0 before we jump into saveRegi
  Context * contextPointer = &(contexts[activeThread]);
  asm("str fp, [%[cp], #0]" : [cp] "=r" (contextPointer));
  asm("str lr, [%[cp], #8]" : [cp] "=r" (contextPointer));
  asm("str x0, [%[cp], #144]" : [cp] "=r" (contextPointer));
  
  saveRegi(&(contexts[activeThread]));

  threadCount++;
  activeThread = threadCount;

  //Creating a new stack for the new thread
  stacks[threadCount] = (char *) malloc(64000) + 64000;
  
  setStackRun(f,stacks[activeThread]);
}

void sharecpu(int threadNum) {
  //When we branch into saveRegi the fp and lr will get overwritten
  //Therefor I need to save those 2 registers and x0 before we jump into saveRegi
  Context * contextPointer = &(contexts[activeThread]);
  asm("str fp, [%[cp], #0]" : [cp] "=r" (contextPointer));
  asm("str lr, [%[cp], #8]" : [cp] "=r" (contextPointer));
  asm("str x0, [%[cp], #144]" : [cp] "=r" (contextPointer));
  
  saveRegi(&(contexts[activeThread]));

  //Select next thread by RR
  activeThread++;

  //Select next thread at random
  //Note: When using the random selection the program will die whenever one of the created threads finishes
  //activeThread = (rand() % (threadCount+1));

  //this check is needed for either RR or Random
  if(activeThread>threadCount) activeThread=0;

  loadRegi(&(contexts[activeThread]));
  asm("ldr fp, [%[cp], #0]" : [cp] "=r" (contextPointer));
  asm("ldr lr, [%[cp], #8]" : [cp] "=r" (contextPointer));
  asm("ldr x0, [%[cp], #144]" : [cp] "=r" (contextPointer));
}

/* Change nothing below this line.  Get the program to execute the code
 * of main, main1, and main2
 * It does not matter the order that the code for main, main1, and 
 * main2 gets executed, but it should execute a piece of the code
 * and then use sharecpu to gives other threads a chance to execute.
 */

void main1(int threadNum) {
  for (int i=0;i<10;i++) {
      cout << "Main 1 says Hello" << endl;
      usleep(1000);
      sharecpu(threadNum);
  }
}

void main2(int thread) {
  for (int i=0;i<10;i++) {
    cout << "Main 2 says Hello" << endl;
    usleep(1000);
    sharecpu(thread);
  }
}

int main() { 
  startThread(main1);
  cout << "Back to Main" << endl;
  startThread(main2);
  while (true) {
        cout << "Main says hello" << endl;
        usleep(1000);
        sharecpu(0);
  }
  return 0;
}
