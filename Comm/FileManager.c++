/**
 * @file FileManager.c++
 * Contains the implementation of the FileManager class.
 * @author
 * Jeremy Guillory
 * @date
 * Created on November 12, 2010, 2:06 PM
 */

#include "FileManager.h"

/**
 * Initializes POSIX lock, creates the session file if it does not already
 * exist, or recovers session data from a pre-existing session file.
 */
FileManager::FileManager()
{
    int session;
    string fileName;
    bool isUpload;
    ifstream sessionFile;

    //Create directory file
     system("ls -p | grep -v \"/\">ls.txt");

    //Create and initialize lock
    fileLock_ = new pthread_mutex_t;
    pthread_mutex_init(fileLock_, NULL);
    
    //Acuire lock to access filesystem to read session file
    acquireLock();
    sessionFile.open("session.dat");
    if (sessionFile.is_open())
    {
        //TODO: >> will throw exception if the type read does not match type 
        //expected this needs to be fixed
        sessionFile>>session;
        //Read all entries in file
        while(!sessionFile.eof())
        {
            sessionFile>>fileName>>isUpload;
            //cout<<session<<" "<<fileName<<"\n";
            sessionFiles_[session].name = fileName;
            sessionFiles_[session].isUpload = isUpload;
            sessionFile>>session;
        }
        sessionFile.close();
        //Done with file access so release lock
        releaseLock();
    }
    else
    {
        releaseLock();
        //TODO : Error point here..could not open session file
    }
}

/**
 * Write length bytes from data to the location corresponding to blockId in the
 * file associated with sessioId. This file access ensures mutual exclusion
 * through the use of a POSIX thread lock.  After the operation is performed, a
 * response is created and sent to the communication interface for transmission.
 */
void FileManager::writeBlock(uchar* packet)
{
    ::tempDataBuffer = NULL;
    int length = packet[0] - 0x80;
    int sessionId = packet[1];
    int blockId = 0;
    blockId += (packet[2]<<16);
    blockId += (packet[3]<<8);
    blockId += (packet[4]);


    //Ensure mutual exclusion for file access
    acquireLock();

    //Get the name of the file corresponding to the session
    char *fileNameChar = (char*)sessionFiles_[sessionId].name.c_str();
    
    //Open the file that is associated with the transfer session
    fstream file;
    file.open(fileNameChar,ios::binary|ios::in|ios::out);
    
    //Seek the starting point of the write
    file.seekp((blockId)*MAX_PACKET_SIZE, ios::beg);

    //Write the data
    file.write((char*)(packet+5), length);
    file.close();

    //File access complete
    releaseLock();
    
    //Create and send response
    uchar response[64] = {0x00};
    response[0] = 0x00;
    response[1] = 0xF0;
    response[2] = 0x01;
    response[3] = (uchar)sessionId;
    ::port1->sendPacket(response, 64);

}

/**
 * Reads a block of a file as specified by the parameters.  This is done through
 * the use of a lock to ensure mutual exclusion.  The data read from the file
 * will be sent to the communication interface to be transmitted.
 */
void FileManager::readBlock(int sessionId, int blockId)
{
    uchar* data = new uchar[MAX_PACKET_SIZE];
    //Ensure mutual exclusion for file access
    acquireLock();

    //Get the name of the file corresponding to the session
    char *fileNameChar = (char*)sessionFiles_[sessionId].name.c_str();

    //Open the file that is associated with the transfer session
    ifstream file;
    file.open(fileNameChar,ios::binary|ios::in);
    if(!file.is_open())
    {
        //printf("\nFAIL\n");

    }
    //Get the length of the file
    file.seekg(0, ios::end);
    long length = file.tellg();

    //Seek the starting point of the write
    file.seekg((blockId-1)*MAX_PACKET_SIZE, ios::beg);

    long remaining = length - file.tellg();
    //printf("\nBLOCKID: %i REMAIN: %d  length: %d\n",blockId ,remaining, length);
    //Check if this is the last packet in the file
    if(remaining < MAX_PACKET_SIZE)
    {
        file.read((char*)data, (int)remaining);

        //Generate and send response
        //Create and send response
        uchar headerData[] = {0xC0, 0x00, 0xA8, 0x8A, 0xA6, 0xA8, 0x40, 0x40,
                              0x60, 0x96, 0x6A, 0xAA, 0xA6, 0x98, 0x40, 0x61,
                              0x03, 0xF0, 0x00, 0xFD, 0x00, 0x00, 0x00, 0x00,
                              0x00};
        headerData[20] = (uchar)remaining;
        headerData[21] = (uchar)sessionId;
        headerData[24] = (uchar)blockId;
        uchar lastByte[] = {0xC0};
        ::port1->sendPacket(headerData, 25);
        ::port1->sendPacket(data, (int)remaining);
        ::port1->sendPacket(lastByte, 1);

    }
    else
    {
        //Read the data
        file.read(reinterpret_cast<char*>(data), MAX_PACKET_SIZE);
    
        //Generate and send response
        //Create and send response
        uchar headerData[] = {0xC0, 0x00, 0xA8, 0x8A, 0xA6, 0xA8, 0x40, 0x40, 
                              0x60, 0x96, 0x6A, 0xAA, 0xA6, 0x98, 0x40, 0x61, 
                              0x03, 0xF0, 0x00, 0xFD, 0x00, 0x00, 0x00, 0x00,
                              0x00};
        headerData[20] = (uchar)MAX_PACKET_SIZE;
        headerData[21] = (uchar)sessionId;
        headerData[24] = (uchar)blockId;
        uchar lastByte[] = {0xC0};
        ::port1->sendPacket(headerData, 25);
        ::port1->sendPacket(data, MAX_PACKET_SIZE);
        ::port1->sendPacket(lastByte, 1);
    
    }


    file.close();

    //File access complete
    releaseLock();
}

/**
 * Creates an entry in the session file to begin a file transfer.  The name
 * of the file as well as the ID for the session will be stored in the session
 * file, and then a response is created and sent to the communication interface
 * for transmission. Note that only 20 sessions may exist at any given time.
 */
void FileManager::startSession(uchar* packet)
{
    ::tempDataBuffer = NULL;
    bool isUpload = false;
    if(packet[1] = 0xFB)
    {
        isUpload = true;
    }

    string fileName((char*)(packet+5));

    //sessionId will remain -1 if no free sessionId's are available
    int sessionId = -1;
    int i = 0;
    bool isError = false;
    //Check that a session does not already exist for this file name
    while(i<20)
    {
        if (sessionFiles_[i].name == fileName)
        {
            //TODO error point here...session already exists for this file name
            //cout<<"A session already exists for this file name.\n";
            isError = true;
        }
        //Make the sessionId the lowest available sessionId
        if((sessionFiles_[i].name == "")&&(sessionId == -1))
            sessionId = i;
        i++;
    }
    if (sessionId == -1)
    {
        isError = true;
        //TODO error point - no more free sessions available, must free some up
    }
    //If a session was assigned, update the session file
    if(!isError)
    {
        sessionFiles_[sessionId].name = fileName;
        sessionFiles_[sessionId].isUpload = isUpload;
        ofstream sessionFile;
        //Starting file access
        acquireLock();
        sessionFile.open("session.dat", ios::app);
        if (sessionFile.is_open())
        {
            //Write the sessionId and file name to the file
            sessionFile<<sessionId<<" "<<fileName<<" "<<isUpload<<"\n";
            sessionFile.close();
  
            if (isUpload)
            {
                //printf("\nUploading\n");
                //Create the file that is to be uploaded
                sessionFile.open((char*)(packet+5), ios::trunc);
                if(!sessionFile.is_open())
                {
                    //TODO: Error point, file was not created
            
                }
                sessionFile.close();

                //Update the text file that keeps track of files in directory
                system("ls -p | grep -v \"/\">ls.txt");

                //End file access
                releaseLock();
 
                //Generate and send response
                uchar response[64] = {0x00};
                response[0] = 0x00;
                response[1] = 0xFB;
                response[2] = 0x01;
                response[3] = (uchar)sessionId;
                ::port1->sendPacket(response, 64);
            }
            else
            {
                //printf("Downloading\n");
                //Find how many blocks will be needed for transfer

   
                ifstream file;
                file.open((char*)(packet+5),ios::binary|ios::in);

                if (file.is_open())
                {
                    //Seek the end of the file
                    file.seekg(0, ios::end);
                    //Get the position of the end of the file
                    long length = file.tellg();
                    if (length%MAX_PACKET_SIZE == 0)
                    {
                        length = length/MAX_PACKET_SIZE;
                    }
                    else
                    {
                        length = (length/MAX_PACKET_SIZE) +1;
                    }
                    file.close();
                    releaseLock();

                    //Generate and send response
                    uchar response[64] = {0x00};
                    response[0] = 0x00;
                    response[1] = 0xFC;
                    response[2] = 0x04;
                    response[3] = (uchar)sessionId;
                    response[4] = (uchar)(length>>16);
                    response[5] = (uchar)(length>>8);
                    response[6] = (uchar)(length); 
                }

                else
                {
                    //TODO: error point, file could not be opened
                    releaseLock();
                }

            }

        }
        else
        {
             releaseLock();
            //TODO : Error point here...could not open session file
        }
        //TODO: remove after testing
        //cout<<"\nReturning to main!\n";
    }
    delete[] packet;
}

/**
 * Removes the session from the session file and sends a response regarding if
 * any errors were encountered in the procedure.
 */
void FileManager::endSession(uchar* packet)
{
    int sessionId = packet[3];
    //Remove the session from the private member data
    sessionFiles_[sessionId].name = "";

    ofstream sessionFile;

    //Begin file access
    acquireLock();

    //The sessionFile is deleted and completely rewritten
    sessionFile.open("session.dat", ios::trunc);
     
    if (sessionFile.is_open())
    {
        //Restore the contents of the session file
        for(int i = 0; i < 20; i++)
        {
            if(sessionFiles_[i].name != "")
            {

                sessionFile<<i<<" "<<sessionFiles_[i].name<<" "
                           <<sessionFiles_[i].isUpload<<"\n";
            }
        }

        sessionFile.close();

        //End file access
        releaseLock();

        //Create and send response
        uchar response[64] = {0x00};
        response[0] = 0x00;
        response[1] = 0xFA;
        response[2] = 0x01;
        response[3] = (uchar)sessionId;
        ::port1->sendPacket(response, 64);
    }
    else
    {
        releaseLock();
        //TODO: error point, file could not be opened
    }
}

/**
 * Private member function used to obtain the lock to the filesystem.
 */
void FileManager::acquireLock()
{
    pthread_mutex_lock(fileLock_);
}

/**
 * Private member function used to release the lock to the filesystem.
 */
void FileManager::releaseLock()
{
    pthread_mutex_unlock(fileLock_);
}

/**
 * Deletes all dynamically allocated memory.
 */
FileManager::~FileManager()
{
}
