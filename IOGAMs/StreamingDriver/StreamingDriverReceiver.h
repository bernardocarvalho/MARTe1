/*
 * Copyright 2011 EFDA | European Fusion Development Agreement
 *
 * Licensed under the EUPL, Version 1.1 or - as soon they 
   will be approved by the European Commission - subsequent  
   versions of the EUPL (the "Licence"); 
 * You may not use this work except in compliance with the 
   Licence. 
 * You may obtain a copy of the Licence at: 
 *  
 * http://ec.europa.eu/idabc/eupl
 *
 * Unless required by applicable law or agreed to in 
   writing, software distributed under the Licence is 
   distributed on an "AS IS" basis, 
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either 
   express or implied. 
 * See the Licence for the specific language governing 
   permissions and limitations under the Licence. 
 *
 * $Id$
 *
**/

#ifndef STREAMINGDRIVER_REC_H_
#define STREAMINGDRIVER_REC_H_

#include "System.h"
#include "GenericAcqModule.h"
#include "Atomic.h"
#include "UDPSocket.h"

/** The streaming thread signature*/
extern "C"{
    void StreamingDriverReceiverThread(void *args);
}

OBJECT_DLL(StreamingDriverReceiver)

class StreamingDriverReceiver: public GenericAcqModule{

OBJECT_DLL_STUFF(StreamingDriverReceiver)

private:

    /** The states used in the buffer lock*/
    enum BufferLockStates{
        Unlocked = 0,
        Locked   = 1
    };
    
    /** The states used in the buffer full array*/
    enum BufferFullStates{
        Free = 0,
        Full = 1
    };

    /** Memory Buffer where data is stored in pages. For every cycle data is copied into this buffer*/
    char     *memoryBuffer;

    /** This buffer is used by the receiver thread to store the data which is sent each time the numberOfTransferBuffers
     * is reached*/
    char     *transferBuffer;
    
    /** The number of buffers*/
    int32     numberOfBuffers;

    /** The number of buffers which is to be sent by each consumer packet*/
    int32     numberOfTransferBuffers; 

    /** The size of each buffer in bytes*/
    int32     memBufferSize;

    /** Flags which control if a buffer is locked (for writing or reading)*/
    int32    *bufferLockStates;

    /** Flags which control if a buffer is full*/
    int32    *bufferFreeStates;

    /** The last unlocked buffer index found*/
    int32     lastUnlockedBufferIndex;

    /** The event sem which is used by the receiver to wake the Poll*/
    EventSem  synchEventSem;

    /** Last time mark */
    uint32    lastUsecTime;
    
    /** The receiver UDP port*/
    int32     receiverUDPPort;

    /** The id of the consumer thread*/
    TID       receiverThreadID;
    
    /** The CPU mask where the thread should be started*/
    int32     cpuMask;
 
    /** The consumer thread will stay alive as long as this flag is true*/
    bool      receiverThreadIsAlive;

    /** The UDP socket to send the data*/
    UDPSocket receiverSocket;

    /** The last index read by the GetData*/
    int32     lastReadIdx;

    /** This is true if the driver is also assumed to be a timing source*/
    bool      synchronizing;

    /** Timeout when synchronizing*/
    int32     synchronizingMSTimeout;

    /** The number of buffers lost (only used for the warnings)*/
    int32     lostBuffers;

    /** Counter value when the last warning was issued. The system will not send more than a warning per second*/
    int64     lastWarningCounter; 

private:

    /** Return the actual Time as microseconds */
    virtual int64 GetUsecTime(){
        int64 time = lastUsecTime;
        return time;
    }

    /** Returns the index of the next unlocked buffer. If none
     * is found, return -1*/
    int GetNextUnlockedBufferIndexAndLock();
public:
    
    /** Constructor. */
    StreamingDriverReceiver();

    /** Destructor */
    virtual ~StreamingDriverReceiver();

    friend void StreamingDriverReceiverThread(void *args);
    
public:

    /** ObjectLoadSetup */
    virtual bool ObjectLoadSetup(ConfigurationDataBase &info,StreamInterface *err);
    
    /** Save parameter */
    virtual bool ObjectDescription(StreamInterface &s,bool full=False,StreamInterface *err=NULL){
        return True;
    }

public:
    
    /** Set board used as input */
    virtual bool SetInputBoardInUse(bool on = True){

        if(inputBoardInUse && on) {
            AssertErrorCondition(InitialisationError, "StreamingDriverReceiver::SetInputBoardInUse: Board %s is already in use", Name());
            return False;
        }

        inputBoardInUse  = on;
        return True;
    }

    virtual bool SetOutputBoardInUse(bool on = True){

        if(outputBoardInUse && on){
            AssertErrorCondition(InitialisationError, "StreamingDriverReceiver::SetOutputBoardInUse: Board %s is already in use", Name());
            return False;
        }

        outputBoardInUse = on;
        return True;
    }

public:
    
    /////////////////////////////////////////////////////////////////////////////////
    //                            Output Modules Methods                           //
    /////////////////////////////////////////////////////////////////////////////////

    /** Writes the data to the circular buffer */
    virtual bool WriteData(uint32 usecTime, const int32 *buffer){
        return False;
    }

    
    /////////////////////////////////////////////////////////////////////////////////
    //                             Input Modules Methods                           //
    /////////////////////////////////////////////////////////////////////////////////
    
    /** Copy local input buffer in destination buffer:
        return  0 if data not ready
        return <0 if error
        return >0 if OK
    */
    virtual int32 GetData(uint32 usecTime, int32 *buffer, int32 bufferNumber = 0);
    
    /** Poll to synchronize*/
    virtual bool Poll();

};

#endif /*STREAMINGDRIVER_REC_H_*/
