#include <iostream>
#include <unistd.h>

using namespace std;

typedef void ThreadFun(int threadNum);

struct Context{
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
};

Context contexts[10];
int threadCount = 0;
int activeThread = 0;

char* stacks[5];

void saveRegi(Context *threadContext){
  asm("str x1, [x0, #24]"); //Will be using x1 for moving SP

  asm("str fp, [x0, #0]");
  asm("str lr, [x0, #8]");
  asm("mov x1, sp");
  asm("str x1, [x0, #16]");

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
}

void saveRegiSome(Context *threadContext){ //Skips saving the FP and LR
  asm("str x1, [x0, #24]"); //Will be using x1 for moving SP

  asm("mov x1, sp");
  asm("str x1, [x0, #16]");

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
}

void loadRegi(Context *threadContext){
  asm("ldr fp, [x0, #0]");
  asm("ldr lr, [x0, #8]");

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
  
  asm("ldr x1, [x0, #16]");
  asm("mov sp, x1");

  asm("ldr x1, [x0, #24]");
}

void setStackRun(ThreadFun *f, char* stack){
  asm("mov sp, x1");
  asm("str x0, [sp, #8]");
  f(activeThread);
}

void startThread(ThreadFun *f) {
  Context * contextPointer = &(contexts[activeThread]);
  asm("str fp, [%[cp], #0]" : [cp] "=r" (contextPointer));
  
  contextPointer = &(contexts[activeThread]);
  asm("str lr, [%[cp], #8]" : [cp] "=r" (contextPointer));
  
  saveRegiSome(&(contexts[activeThread]));

  threadCount++;
  activeThread = threadCount;

  stacks[threadCount] = (char *) malloc(64000) + 64000;
  
  setStackRun(f,stacks[activeThread]);
}

void sharecpu(int threadNum) {
  Context * contextPointer = &(contexts[activeThread]);
  asm("str fp, [%[cp], #0]" : [cp] "=r" (contextPointer));
  
  contextPointer = &(contexts[activeThread]);
  asm("str lr, [%[cp], #8]" : [cp] "=r" (contextPointer));
  
  saveRegiSome(&(contexts[activeThread]));
  cout << activeThread << endl;

  activeThread++;
  if(activeThread>=threadCount) activeThread=0;

  loadRegi(&(contexts[activeThread]));

  //This is for stack alignment and to preserve values loaded by loadregi
  //asm("stp x29, x30, [sp, #0x20]");
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
      usleep(100000);
      sharecpu(threadNum);
  }
}

void main2(int thread) {
  for (int i=0;i<10;i++) {
    cout << "Main 2 says Hello" << endl;
    usleep(100000);
    sharecpu(thread);
  }
}

int main() { 
  startThread(main1);
  cout << "Back to Main" << endl;
  startThread(main2);
  while (true) {
        cout << "Main says hello" << endl;
        usleep(100000);
        sharecpu(0);
  }
  return 0;
}
