#include <unistd.h>
#include <stdio.h>

int main()
{
    int pid = fork();
    
    if (pid>0)
    {
        wait(4);
        sleep(8);
        printf("Hello there! I'm a parent with id %d \n",getpid());
    }
    else
    {
        sleep(8);
        printf("Hello there! I'm a child with id %d \n",getpid());
    }
    
    return 0;
}