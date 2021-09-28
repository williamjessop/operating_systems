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
  asm("str x27, [x0, #8]");
  asm("str x28, [x0, #16]");
  asm("mov x1, sp");
  asm("str x1, [x0, #24]");
}

void loadRegi(Context *threadContext){
  asm("ldr x27, [x0, #8]");
  asm("ldr x28, [x0, #16]");
  asm("ldr x1, [x0, #24]");
  asm("mov sp, x1");
  asm("ldr x1, [x0, #0]");
}

Context * contexts[10];
int threadCount = 1;
int activeThread = 0;

void startThread(ThreadFun *f) {
  cout << "Starting thread" << endl;
  if(threadCount == 1){
    contexts[activeThread] = new Context;
    //asm("ldp	x29, x30, [sp, #32]");
    asm("mov x27, x29");
    asm("mov x28, x30");
    saveRegi(contexts[activeThread]);
  }

  threadCount += 1;
  activeThread++;
  contexts[activeThread] = new Context;
  // if(threadCount == 1){ //AKA main is calling
    
  //   saveRegi(&thread0);
  // }
  
  //make a new stack in the heap for the thread
  char *newFramePointer = (char *) calloc(8*1000,1);
  newFramePointer = newFramePointer + (8*8*1000);

  asm("ldr x1, [sp, #8]");
  asm("mov sp, %[nf]" : [nf] "=r" (newFramePointer));
  asm("str x1, [sp, #8]");

  f(activeThread);
}

void sharecpu(int threadNum) {
  //cout << threadNum;
  //cout << "Active Thread: " << activeThread << endl;
  asm("mov x27, x29");
  asm("mov x28, x30");
  saveRegi(contexts[threadNum]);

  //cout << "saved" << endl;

  //activeThread = (activeThread + 1) % threadCount;

  //cout << "Active Thread: " << activeThread << endl;

  loadRegi(contexts[0]);
  asm("stp	x27, x28, [sp, #16]");


  // if(threadCount == 1){
  //   saveRegi(&thread1);
  //   loadRegi(&thread0);
  // }else if(threadNum == 0){ //AKA main is the caller
  //   saveRegi(&thread0);
  //   loadRegi(&thread1); //Thread 1 regis were not saved or loaded correctly
  // }else if(threadNum == 1){
  //   saveRegi(&thread1);
  //   loadRegi(&thread2);
  // }else if(threadNum == 2){
  //   saveRegi(&thread2);
  //   loadRegi(&thread0);
  // }



  //call next thread to go
  //Implied "folded reload"
  //Implied "ret"
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


//bt tool in GDB