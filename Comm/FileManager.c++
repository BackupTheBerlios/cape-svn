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
    ifstream sessionFile;

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
            sessionFile>>fileName;
              cout<<session<<" "<<fileName<<"\n";
            sessionFiles_[session] = fileName;
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
void FileManager::writeBlock(int sessionId, int blockId, int length, uchar* data)
{
    //Ensure mutual exclusion for file access
    acquireLock();

    //Get the name of the file corresponding to the session
    char *fileNameChar = (char*)sessionFiles_[sessionId].c_str();
    
    //Open the file that is associated with the transfer session
    ofstream file;
    file.open(fileNameChar,ios::binary | ios::app);
    
    //Seek the starting point of the write
    file.seekp(blockId*MAX_PACKET_SIZE, ios::beg);

    //Write the data
    file.write(reinterpret_cast<char*>(data), length);
    file.close();

    //File access complete
    releaseLock();
    
    //Create and send response
    uchar buffer[] = {0xC0, 0x00, 0xA8, 0x8A, 0xA6, 0xA8, 0x40,
            0x40, 0x60, 0x96, 0x6A, 0xAA, 0xA6, 0x98, 0x40, 0x61, 0x03, 0xF0,
            0x00, 0xF0, 0x00, 0x00, 0x00, 0x00 ,0xC0};
    buffer[20] = (uchar)sessionId;
    buffer[23] = (uchar)blockId;
    ::port1->sendPacket(buffer, 25);
}

/**
 * Reads a block of a file as specified by the parameters.  This is done through
 * the use of a lock to ensure mutual exclusion.  The data read from the file
 * will be sent to the communication interface to be transmitted.
 */
void FileManager::readBlock(uchar sessionId, uchar blockId)
{
}

/**
 * Creates an entry in the session file to begin a file transfer.  The name
 * of the file as well as the ID for the session will be stored in the session
 * file, and then a response is created and sent to the communication interface
 * for transmission. Note that only 20 sessions may exist at any given time.
 */
void FileManager::startSession(string fileName)
{
    //sessionId will remain -1 if no free sessionId's are available
    int sessionId = -1;
    int i = 0;
    bool isError = false;
    //Check that a session does not already exist for this file name
    while(i<20)
    {
        if (sessionFiles_[i] == fileName)
        {
            //TODO error point here...session already exists for this file name
            cout<<"A session already exists for this file name.\n";
            isError = true;
        }
        //Make the sessionId the lowest available sessionId
        if((sessionFiles_[i] == "")&&(sessionId == -1))
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
        sessionFiles_[sessionId] = fileName;
        ofstream sessionFile;
        //Starting file access
        acquireLock();
        sessionFile.open("session.dat", ios::app);
        if (sessionFile.is_open())
        {
            //Write the sessionId and file name to the file
            sessionFile<<sessionId<<" "<<fileName<<"\n";
            sessionFile.close();
  
            char *fileNameChar = (char*)fileName.c_str();

            //TODO: this needs to be removed for downloads

            //Create the file that is to be uploaded
            sessionFile.open(fileNameChar, ios::trunc);
            if(!sessionFile.is_open())
            {
                //TODO: Error point, file was not created
            
            }
            sessionFile.close();

            //End file access
            releaseLock();
            //Generate and send response
            uchar buffer[] = {0xC0, 0x00, 0xA8, 0x8A, 0xA6, 0xA8, 0x40,
            0x40, 0x60, 0x96, 0x6A, 0xAA, 0xA6, 0x98, 0x40, 0x61, 0x03, 0xF0,
            0x00, 0xFB, 0x01, 0x00, 0xC0};
            buffer[21] = (uchar)sessionId;
            ::port1->sendPacket(buffer, 23);
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

/**
 * Removes the session from the session file and sends a response regarding if
 * any errors were encountered in the procedure.
 */
void FileManager::endSession(int sessionId)
{
    //Remove the session from the private member data
    sessionFiles_[sessionId] = "";

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
            if(sessionFiles_[i] != "")
            {

                sessionFile<<i<<" "<<sessionFiles_[i]<<"\n";
            }
        }

        sessionFile.close();

        //End file access
        releaseLock();

        //Generate and send response
        uchar buffer[] = {0xC0, 0x00, 0xA8, 0x8A, 0xA6, 0xA8, 0x40, 0x40, 0x60,
                          0x96, 0x6A, 0xAA, 0xA6, 0x98, 0x40, 0x61, 0x03, 0xF0,
                          0x00, 0xFA, 0x01, 0x00, 0xC0};

        buffer[21] = (uchar)sessionId;
        ::port1->sendPacket(buffer, 23);
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
