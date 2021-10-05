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

Context contexts[10];
int threadCount = 1;
int activeThread = 0;

char* stacks[3];

void saveRegi(Context *threadContext){
  asm("str x1, [x0, #0]");
  asm("str x29, [x0, #8]");
  asm("str x30, [x0, #16]");
  
  asm("mov x1, sp");
  asm("str x1, [x0, #24]");
}

void loadRegi(Context *threadContext){
  asm("ldr x29, [x0, #8]");
  asm("ldr x30, [x0, #16]");
  
  asm("ldr x1, [x0, #24]");
  asm("mov sp, x1");

  asm("ldr x1, [x0, #0]");
}

void setStackRun(ThreadFun *f, char* stack){
  asm("mov sp, x1");
  asm("str x0, [sp, #8]");
  f(activeThread);
}

void startThread(ThreadFun *f) {
  stacks[threadCount] = (char *) malloc(64000) + 64000;

  saveRegi(&(contexts[activeThread]));

  activeThread = threadCount;
  
  threadCount++;
  
  setStackRun(f,stacks[activeThread]);
}

void sharecpu(int threadNum) {
  saveRegi(&(contexts[activeThread]));

  activeThread++;
  if(activeThread>=threadCount) activeThread=0;

  loadRegi(&(contexts[activeThread]));
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
