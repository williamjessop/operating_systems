#include <iostream>
#include <unistd.h>

using namespace std;

typedef void ThreadFun(int threadNum);

struct context{
  // int pc;
  // int sp;
  int fp; //AKA x29
  int lr; //AKA x30
  int x0;
  int x1;
};

context thread1 = {0, 0, 0, 0};
context thread2 = {0, 0, 0, 0};

int threadCount = 0;
int activeThread = 1;

void startThread(ThreadFun *f) {
  //This function needs to enumerate the thread
  threadCount += 1;

  f(threadCount);
}

void sharecpu(int threadNum) {

  //save my state
  if(threadNum == 1){
    asm("ldr x2, =_thread1\n\t"
      "stp x29, x30, [x2]\n\t"
      "stp x0, x1, [x2, #8]"
    );
  }else if(threadNum == 2){
    asm("ldr x2, =_thread2\n\t"
      "stp x29, x30, [x2]\n\t"
      "stp x0, x1, [x2, #8]"
    );
  }
  

  //decide the next thread to go

  //load the next threads state
  if(!(threadCount > 1))
    return;
  else if(threadNum == 1)
    asm("ldr x2, =_thread2\n\t"
        "ldp x29, x30, [x2]\n\t"
        "ldp x0, x1, [x2, #8]"
    );
  else if(threadNum == 2)
    asm("ldr x2, =_thread1\n\t"
        "ldp x29, x30, [x2]\n\t"
        "ldp x0, x1, [x2, #8]"
    );

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
