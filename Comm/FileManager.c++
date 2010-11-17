/* 
 * File:   FileManager.c++
 * Author: jeremy
 * 
 * Created on November 12, 2010, 2:06 PM
 */

#include "FileManager.h"


FileManager::FileManager() {
    int session;
    string fileName;
    fileLock_ = new pthread_mutex_t;
    pthread_mutex_init(fileLock_, NULL);
    ifstream sessionFile;

    sessionFile.open("session.dat");
    if (sessionFile.is_open())
    {
        //TODO: >> will throw exception if the type read does not match type 
        //expected this needs to be fixed
        sessionFile>>session;
        while(!sessionFile.eof())
        {
            sessionFile>>fileName;
              cout<<session<<" "<<fileName<<"\n";
            sessionFiles_[session] = fileName;
            sessionFile>>session;
        }
        sessionFile.close();
    }
    else
    {
    //TODO : Error point here..could not open session file
    }
}

FileManager::~FileManager() {
}

void FileManager::writeBlock(int sessionId, int blockId, int length, uchar* data){

    acquireLock();
      char *fileNameChar = (char*)sessionFiles_[sessionId].c_str();
    ofstream file;
    file.open(fileNameChar, ios::app);
    file.seekp(blockId*MAX_PACKET_SIZE, ios::beg);
    /printf("\nwriting:\n%s", data);
    file.write(reinterpret_cast<char*>(data), length);
    releaseLock();
    //Respond
    uchar buffer[] = {0xC0, 0x00, 0xA8, 0x8A, 0xA6, 0xA8, 0x40,
            0x40, 0x60, 0x96, 0x6A, 0xAA, 0xA6, 0x98, 0x40, 0x61, 0x03, 0xF0,
            0x00, 0xF0, 0x00, 0x00, 0x00, 0x00 ,0xC0};
            buffer[20] = (uchar)sessionId;
            buffer[23] = (uchar)blockId;
            port1->sendCommandPacket(buffer, 25);


}

void FileManager::readBlock(uchar sessionId, uchar blockId){
}

void FileManager::startSession(string fileName){
    int sessionId = -1;
    int i = 0;
    bool isError = false;
    while(i<20)
    {
        if (sessionFiles_[i] == fileName)
        {
            //TODO error point here...session already exists for this filename
            cout<<"A session already exists for this file name.\n";
            isError = true;
        }
        if((sessionFiles_[i] == "")&&(sessionId == -1))
            sessionId = i;
        i++;
    }
    if (sessionId == -1)
    {
        isError = true;
        //TODO error point - no more free sessions available, must free some up
    }
    if(!isError)
    {
        sessionFiles_[sessionId] = fileName;
        
        ofstream sessionFile;
        acquireLock();
        sessionFile.open("session.dat", ios::app);
        if (sessionFile.is_open())
        {
            sessionFile<<sessionId<<" "<<fileName<<"\n";
            sessionFile.close();
  
            char *fileNameChar = (char*)fileName.c_str();

            //TODO: need to check if the file was created correctly. Error point
            FILE *newFile = fopen(fileNameChar, "w");
            fclose(newFile);

            releaseLock();
            //generate response
            uchar buffer[] = {0xC0, 0x00, 0xA8, 0x8A, 0xA6, 0xA8, 0x40,
            0x40, 0x60, 0x96, 0x6A, 0xAA, 0xA6, 0x98, 0x40, 0x61, 0x03, 0xF0,
            0x00, 0xFB, 0x01, 0x00, 0xC0};
            buffer[21] = (uchar)sessionId;
            port1->sendCommandPacket(buffer, 23);
        }
        else
        {
             releaseLock();
            //TODO : Error point here...could not open session file
        }
        //TODO: remove after testing
        cout<<"\nReturning to main!\n";
    }

}

void FileManager::endSession(uchar sessionId){
}

void FileManager::acquireLock(){
    pthread_mutex_lock(fileLock_);
}

void FileManager::releaseLock(){
    pthread_mutex_unlock(fileLock_);
}