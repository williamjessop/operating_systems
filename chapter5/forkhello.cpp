#include <iostream>
#include <unistd.h>

using namespace std;

void printAnswer(int id){
    if(id == 0){
        cout << "Hello" << endl;
    }else{
        for(int i = 0; i < 1000; i++) continue;
        cout << "Goodbye" << endl;
    }
}

int main(){


    int id = fork();

    printAnswer(id);
    

    return 0;
}