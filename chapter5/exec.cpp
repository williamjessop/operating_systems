#include <iostream>
#include <unistd.h>

using namespace std;

int main(){


    int id = fork();

    execl("/bin/ls", "/bin/ls", NULL);
    

    return 0;
}