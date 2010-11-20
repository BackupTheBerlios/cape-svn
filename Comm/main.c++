/**
 * @file
 * main.cpp
 *
 * @author
 * Jeremy Guillory
 *
 * @date
 * Last modified on September 14, 2010, 11:30 AM
 */


#include <pthread.h>
#include "FileManager.h"
#include "ComPort.h"

using namespace std;
#define uchar unsigned char
 
//Globals start here

const int MAX_PACKET_SIZE = 59;

int sig = 0;
bool isCommandReceived = false;
char command[5];
ComPort* port1;
//this is only for testing
uchar* tempDataBuffer;
//this is only for testing
bool isFilePacket = false;
//TESTING
int ses, block, leng;


void signalHandler1 (int status)
{
    ::port1->signalHandler();
}




int main(int argc, char** argv)
{
    FileManager fileManager;
    ::port1 = new ComPort(1);
    printf("Starting.\n");
    while(1)
    {
        sleep(100);
        if (::isCommandReceived)
        {
            //cout<<"\nCommand Received!\n";
            if(::isFilePacket)
            {
                //printf("\nFile byte: %2x\n", tempDataBuffer[0]);
                fileManager.writeBlock(::ses, ::block, ::leng, ::tempDataBuffer);

            }
            else
            {
                if (::tempDataBuffer[0] == 0xFB)
                {
                    string fileName((char*)(::tempDataBuffer+2));
                    //sprintf(fileName, "%s", tempDataBuffer+2);
                    fileManager.startSession(fileName);
                    delete[] ::tempDataBuffer;
                    ::tempDataBuffer = NULL;
                }
                else if (::tempDataBuffer[0] == 0xFA)
                {
                    //Abort Session
                    //printf("\n%2x\n", tempDataBuffer[1]);
                    //printf("\n%2x\n", tempDataBuffer[2]);
                    
                    fileManager.endSession((int)(::tempDataBuffer[2]));
                    delete[] ::tempDataBuffer;
                    ::tempDataBuffer = NULL;
                }
            }
            ::isCommandReceived = false;

        }
    }
    return 0;
}




