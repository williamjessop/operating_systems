#include <iostream>
#include <unistd.h>

using namespace std;

int main(){
    int x = 100;

    int id = fork();
    if(id == 0){
        x = 1;
        cout << "child: " << x << endl;
    }
    else{
        x++;
        cout << "parent: " << x << endl;
    }
    return 0;
}