/**
 * @file FileManager.h
 * Contains the prototype for the FileManager class.
 * @author
 * Jeremy Guillory
 * @date
 * Created on November 12, 2010, 2:06 PM
 */

#ifndef uchar
#define uchar unsigned char
#endif
#ifndef CAPE_CDH_FILEMANAGER_H
#define	CAPE_CDH_FILEMANAGER_H

//iostream and cout for testing purposes only!!!!
#include <iostream>
using std::cout;


#include <string>
#include <fstream>
#include <stdio.h>
#include <pthread.h>
#include "ComPort.h"
using std::string;
using std::ifstream;
using std::ofstream;
using std::ios;
using std::fstream;

extern ComPort* port1;
extern const int MAX_PACKET_SIZE;

struct Session
{
    string name;
    bool isUpload;
};

/**
 * A class to handle data transfers with the filesystem.
 *
 * Will be used to perform reads and writes to an SD card on the CDH subsystem.
 * Mutual exclusion must be enforced to avoid race conditions which may occur
 * due to the multithreaded approach of our implementation.
 */
class FileManager{
public:

    ///Default constructor for the class.
    FileManager();

    ///Writes a block of file data to the filesystem.
    void writeBlock(uchar* packet);

    ///Reads a block of data from the filesystem.
    void readBlock(uchar* packet);

    ///Opens a file transfer session.
    void startSession(uchar* packet);

    ///Closes a file transfer session.
    void endSession(uchar* packet);

    ///Standard destructor.
    ~FileManager();

private:

    ///Used to acquire the lock protecting the filesystem.
    void acquireLock();

    ///Releases the filesystem lock.
    void releaseLock();

    ///POSIX thread lock used to ensure mutual exclusion of the filesystem.
    pthread_mutex_t* fileLock_;

    ///An internal list of the file names associated with a transfer session.
    Session sessionFiles_[20];
};

#endif	/* CAPE_CDH_FILEMANAGER_H */

