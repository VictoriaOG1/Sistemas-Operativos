// C++ program to demonstrate creating processes using fork()
#include <unistd.h>
#include <stdio.h>
  
int main()
{
    // Creating first child
    int n1 = fork();
  
    // Creating second child. First child
    // also executes this line and creates
    // grandchild.
    int n2 = fork();
    
    if (n1 > 0 && n2 > 0) 
    {
        //waitpid(-1);
        //sleep(8);
        printf("Hello there! I'm parent with id %d \n", getpid());
        printf("pid1 = %d pid2 = %d \n", n1, n2);
        printf("\n \n");
    }
    else if (n1 == 0 && n2 > 0)
    {
        //waitpid(-1);
        //sleep(8);
        printf("Hello there! I'm first child with id %d \n", getpid());
        printf("pid1 = %d pid2 = %d \n", n1, n2);
        printf("\n \n");
    }
    else if (n1 > 0 && n2 == 0)
    {
        //waitpid(-1);
        //sleep(16);
        printf("Hello there! I'm second child with id %d \n", getpid());
        printf("pid1 = %d pid2 = %d \n", n1, n2);
        printf("\n \n");
    }
    else 
    {
        printf("Hello there! I'm third child with id %d \n", getpid());
        printf("pid1 = %d pid2 = %d \n", n1, n2);
        printf("\n \n");
    }
    return 0;
}