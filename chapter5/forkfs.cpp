#include <iostream>
#include <unistd.h>
#include <fstream>
#include <string>

using namespace std;

int main(){

    ofstream in;
    in.open("test.txt");

    int id = fork();

    in << "I am process " << id << " ";

    // if(id == 0){

    //     string test = "I AM CHILD";
    //     in << test;
    //     //cout << "child: " << test << endl;
    // }
    // else{
    //     cout << "I am slower";
    //     string test = "I AM PARENT";
    //     in << test;
    //     //cout << "parent: " << test << endl;
    // }
    return 0;
}