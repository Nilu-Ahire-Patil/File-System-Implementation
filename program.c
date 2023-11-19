#include <stdio.h>
#include <stdlib.h>
#include "fs.h"
#define MAXPARAMETER 5
#define PARAMETERZISE 20
#define MAXCOMMANDSIZE 60

int main(int argc, char* argv[])
{
    
    while(1)
    {

        char ch;
        int i = 0, j = 0, k = 0;
        char command[MAXPARAMETER][PARAMETERZISE];
        char cmd[MAXCOMMANDSIZE];
        scanf("%s",cmd);
        while(!(cmd[k] == '\000'))
        {
            if(cmd[k] == ' ')
            {
                i++;
                j = 0;
            }
            else if(cmd[k] != '\n'){
                command[i][j] = cmd[k];
                j++;
            }
            k++;
            //printf("%s ",command[i]);

        }

        printf("Printing\n");
        i = 0;
        while(i<MAXPARAMETER)
        {
            printf("%s\n",command[i]);
            i++;
        }

        // char command[MAXCOMMANDSIZE];
        printf("\n#");
        // scanf("%s",&command);
        // file(argc,argv);
    }
    
    return 0;
}