#include <iostream>
#include <unistd.h>

using namespace std;

typedef void ThreadFun(int threadNum);

struct Context{
  // int pc;
  // int sp;
  long x1;
  long fp;
  long lr;
  long sp;
};

void saveRegi(Context *threadContext){
  asm("str x1, [x0, #0]");
  //asm("str x29, [x0, #8]");
  //asm("str x30, [x0, #16]");
  asm("mov x1, sp");
  asm("str x1, [x0, #24]");
  //cout << "Saving regis" << endl;
}

void loadRegi(Context *threadContext){
  //asm("ldr x29, [x0, #8]");
  //asm("ldr x30, [x0, #16]");
  asm("ldr x1, [x0, #24]");
  asm("mov sp, x1");
  asm("ldr x1, [x0, #0]");
}

Context thread0 = {0, 0, 0, 0}; //also known as main
Context thread1 = {0, 0, 0, 0};
Context thread2 = {0, 0, 0, 0};

int threadCount = 0;
int activeThread = 1;

void startThread(ThreadFun *f) {
  //This function needs to enumerate the thread

  threadCount += 1;
  if(threadCount == 1){ //AKA main is calling
    //asm("mov x27, x29");
    //asm("mov x28, x30");
    saveRegi(&thread0);
  }
  
  //make a new stack in the heap for the thread
  char *newFramePointer = (char *) calloc(8*1000,1);

  newFramePointer = newFramePointer + (8*8*1000); //This line should be adding!!!

  asm("ldr x1, [sp, #8]");
  asm("mov sp, %[nf]" : [nf] "=r" (newFramePointer));
  asm("str x1, [sp, #8]");

  f(threadCount);
}

void sharecpu(int threadNum) {
  //save my state
  //cout << threadNum << endl;
  //OK so my method will just be round robin
  //If main is the caller, load thread1
  //If thread1 is the caller, load thread2
  //if thread2 is the caller, load main
  //In this case only main and main1 are live, there is no thread 2 to load

  if(threadCount == 1){
    saveRegi(&thread1);
    loadRegi(&thread0);
  }else if(threadNum == 0){ //AKA main is the caller
    saveRegi(&thread0);
    loadRegi(&thread1); //Thread 1 regis were not saved or loaded correctly
  }else if(threadNum == 1){
    saveRegi(&thread1);
    loadRegi(&thread2);
  }else if(threadNum == 2){
    saveRegi(&thread2);
    loadRegi(&thread0);
  }

  //call next thread to go
  //At the end of this function is an implied "ret"
}

/* Change nothing below this line.  Get the program to execute the code
 * of main, main1, and main2
 * It does not matter the order that the code for main, main1, and 
 * main2 gets executed, but it should execute a piece of the code
 * and then use sharecpu to gives other threads a chance to execute.
 */

void main1(int threadNum) {
  for (int i=0;i<10;i++) {
      cout << "Main "<< threadNum <<" says Hello" << endl;
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