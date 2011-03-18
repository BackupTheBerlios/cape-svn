/* File:    config.cpp
 * Author:  matt
 * Purpose: Implements functions used to configure the software at run-time.
 */

#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include "config.h"
using namespace std;

extern int errno;

/* Matt: from "global_data.h" */
extern bool printError;
extern bool printDiag;
extern bool confAbsent;
extern char confPath[128];
extern char lowDevice[128];
extern char hiDevice[128];

void readConf(char *path)
{
    printf("Opening config file '%s'...", path);

    FILE *file = fopen(path, "r");

    /* Matt: Ensure the file opened correctly. */
    if(!file)
    {
        printf("Failed : %s\n", strerror(errno));
        exit(1);
    }

    else
    {
        printf("Success\n");
    }

    /* Matt: keep reading from the file until told to break */
    while(true)
    {
        char str1[128];
        char str2[128];
        
        int ret = fscanf(file, "%s %s", str1, str2);
        printf("%i / <%s> / <%s>\n", ret, str1, str2);


         /* Matt: eof, done reading from the config file */
        if(ret < 0)
        {
            break;
        }

        /* Matt: All strings in the file should come in pairs, if a 1 is seen,
         * something is wrong.
         */
        else if(ret == 1)
        {
            printf("Error detected in the configuration file's formating.\n");
            exit(1);
        }

        /* Matt: determine the option and set it */
        else
        {
            if(!strcmp(str1, "low_device"))
            {
                strcpy(lowDevice, str2);
            }

            else if(!strcmp(str1, "hi_device"))
            {
                strcpy(hiDevice, str2);
            }

            /* Matt: An unknown option was encountered. */
            else
            {
                printf("Unknown option '%s' in the configuration file.\n", str1);
                exit(1);
            }
        }
    }

    fclose(file);
}

void readArgs(int argc, char **argv)
{
     for(int i = 1; i < argc; i++)
    {
        /* Matt: If the "-d" argument is passed, set printDiag to true. This
         * will output diagnostic logging events to stdout.
         */
        if(!strcmp(argv[i], "-d"))
        {
            printDiag = true;

        }

        /* Matt: If the "-e" argument is passed, set printError to true. This
         * will output error logging events to stdout.
         */
        else if(!strcmp(argv[i], "-e"))
        {
            printError = true;

        }

        /* Matt: This argument must be accompanied by a string which is a path
         * to the main configuration file.
         */
        else if(!strcmp(argv[i], "-conf"))
        {
            /* Matt: If true, the user didn't pass the argument with a string.
             */
            if(i == argc-1)
            {
                printf("-conf must be followed by a string\n");
                exit(1);
            }

            /* Matt: If true, the user trued to specify another argument as the
             * string for -conf
             */
            else if(argv[i+1][0] == '-')
            {
                printf("An erronious string followed -conf\n");
                exit(1);
            }

            /* Matt: Made a deep copy of the configuration file path. */
            else
            {
                i++;
                confAbsent = false;
                
                for(int j = 0; j < 128; j++)
                {
                    confPath[j] = argv[i][j];

                    /* Matt: The end of the string was reached. */
                    if(argv[i][j] == 0)
                        break;
                }

                i++;
            }
        }
        
        else
        {
            // Matt: An unknown argument was passed. Indicate such and exit.
            printf("'%s' is an unknown argument.\n", argv[i]);
            exit(1);
        }
    }
}

