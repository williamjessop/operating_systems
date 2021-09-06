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
  //cout << "Saving regis" << endl;
}

void loadRegi(Context *threadContext){
  asm("ldr x27, [x0, #8]");
  asm("ldr x28, [x0, #16]");
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
  cout << "starting thread" << endl;
  if(threadCount == 0){ //AKA main is calling
    asm("mov x27, x29");
    asm("mov x28, x30");
    saveRegi(&thread0);
  }
  
  threadCount += 1;

  //asm("blr x0");
  f(threadCount);
}

void sharecpu(int threadNum) {

  //save my state

  // asm("mov x1, pc\n\t"
  //   "stp x0, x1, [sp, #8]"
  // );

  //decide the next thread to go
  //load the next threads state


  //OK so my method will just be round robin
  //If main is the caller, load thread1
  //If thread1 is the caller, load thread2
  //if thread2 is the caller, load main
  //In this case only main and main1 are live, there is no thread 2 to load
  asm("mov x27, x29");
  asm("mov x28, x30");
  
  if(threadCount == 1){
    saveRegi(&thread1);
    loadRegi(&thread0);
  }else if(threadNum == 0){ //AKA main is the caller
    saveRegi(&thread0);
    loadRegi(&thread1);
  }else if(threadNum == 1){
    saveRegi(&thread1);
    loadRegi(&thread2);
  }else if(threadNum == 2){
    saveRegi(&thread2);
    loadRegi(&thread0);
  }

  asm("mov x29, x27");
  asm("mov x30, x28"); 
    
  // else if(threadNum == 1)
  //   asm("sub sp, sp, #32\n\t"
  //       "ldp x0, ip, [sp, #8]\n\t"
  //   );
  // else if(threadNum == 2)
  //   asm("add sp, sp, #32\n\t"
  //       "ldp x0, ip, [sp, #8]\n\t"
  //   );

  //call next thread to go

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
  //cout << "Top of main" << endl;
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
