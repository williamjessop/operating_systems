#include <iostream>
#include <unistd.h>

using namespace std;

typedef void ThreadFun(int threadNum);

//DONT FORGET THIS!!!
    // ; move register from special register
    // mrs     Rd, apsr        ; Rd = APSR

    // ; move special register from register
    // msr     apsr, Rd        ; APSR = Rd

//This is to save the current flags and then load them back!
//https://developer.arm.com/documentation/dui0489/c/arm-and-thumb-instructions/coprocessor-instructions/ldc--ldc2--stc--and-stc2?lang=en
//ldc
//stc
//load and store coprocessor regis
struct Context{
  // int pc;
  // int sp;
  long x1;
  long fp;
  long lr;
  long sp;
};

void saveRegi(Context *threadContext){
  asm("str x1, [x0, #0]\n\t"
    "str x27, [x0, #8]\n\t"
    "str x28, [x0, #16]\n\t"
    "str x26, [x0, #24]\n\t"
  );
  //cout << "Saving regis" << endl;
}

void loadRegi(Context *threadContext){
  asm("ldr x27, [x0, #8]\n\t"
    "ldr x28, [x0, #16]\n\t"
    "ldr x1, [x0, #24]\n\t"
    "ldr x26, [x0, #0]"
  );
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
    asm("mov x27, x29\n\t"
      "mov x28, x30\t\n"
      "mov x26, sp"
    );
    saveRegi(&thread0);
  }
  
  threadCount += 1;

  //asm("blr x0");
  f(threadCount);
}

void sharecpu(int threadNum) {
  cout << "Thread " << threadNum << " is Sharing" << " and Threadcount is " << threadCount << endl;
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
  asm("mov x27, x29\n\t"
    "mov x28, x30\n\t"
    "mov x26, sp"
  );
  
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

  asm("mov x29, x27\n\t"
    "mov x30, x28"
    //"mov sp, x26"
  ); 
    
  //There is an assumed ret at the end of a void function
  //There has to be some problem with the stack frame...

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
  cout << &thread1<< endl;
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
