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
#include <sys/time.h>
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


void signalHandler1 (int status)
{
    ::port1->signalHandler();
}




int main(int argc, char** argv)
{
    FileManager fileManager;
    ::port1 = new ComPort(1);
    printf("Starting.\n");

    time_t tim = time(NULL);
    cout<<tim<<"\n";



    while(1)
    {
        sleep(100);
        if (::isCommandReceived)
        {
        
            
            ::isCommandReceived = false;
            if(::tempDataBuffer[0] >= 0x80)
            {
  
                fileManager.writeBlock(::tempDataBuffer);
                
            }
            
            else if(::tempDataBuffer[1] == 0xFB)
                {
                    fileManager.startSession(::tempDataBuffer);
                }
                else if (::tempDataBuffer[1] == 0xFA)
                {
                    fileManager.endSession(::tempDataBuffer);
                }
                else if (::tempDataBuffer[1] == 0xFC)
                {
                    string fileName((char*)(::tempDataBuffer+2));
                    //fileManager.startSession(fileName, false);
                }
                else if(::tempDataBuffer[1] == 0xFD)
                {
                    //printf("Download request sent!\n");
                    int sessionId = (int)(::tempDataBuffer[2]);
                    int blockId = (int)(::tempDataBuffer[5]);
                    fileManager.readBlock(sessionId, blockId);
                }

              }
        }
    
    return 0;
}




