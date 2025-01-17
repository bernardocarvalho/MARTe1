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

/**
 * @file UDP driver which can be used to connect several MARTe systems or to 
 * interface into external systems. For data streaming see also the StreamingDriver.
 * This code is heavily based in the ATMDrv, but in this case the header is implicit 
 * (not retrieved from the DDB) when used in UDP_MODULE_TRANSMITTER mode
 */
#if !defined (_UDP_DRV)
#define _UDP_DRV

#include "System.h"
#include "GenericAcqModule.h"
#include "UDPSocket.h"

/// Number buffers for data storage
static const int32 nOfDataBuffers = 3;
OBJECT_DLL(UDPDrv)

// Callback declaration
 void ReceiverCallback(void *userData);

// If the driver is to be used as a receiver or as a transmitter
enum UDPDrvModuleType{
    UDP_MODULE_UNDEFINED   = -1,
    UDP_MODULE_RECEIVER    = 0,
    UDP_MODULE_TRANSMITTER = 1
};

/// UDP Module Class
class UDPDrv : public GenericAcqModule {

OBJECT_DLL_STUFF(UDPDrv)

/// Receiver CallBack
friend void ReceiverCallback(void *userData);

private:
    
    /** Defines the module type*/
    UDPDrvModuleType       moduleType;

    /**   Input/Output Buffer Size in Byte */
    int32                  packetByteSize;

    /** Fast mutex to protect reading and writing buffer */
    FastPollingMutexSem    mux;

    /** bool to flag if packet is fresh */
    bool                   freshPacket;

    /**   Init all module entries */
    bool                   Init();

///////////////////////////////////////////////////////////////////////////////
//                           Receiver Type Module                            //
///////////////////////////////////////////////////////////////////////////////
private:

    /** Triple buffer for receiver type module */
    uint32                 *dataBuffer[nOfDataBuffers];

    /** Index of the write only buffer.
        The next write only buffer index is equal to (writeBuffer+1)%3
        The read only buffer index is equal to (writeBuffer+2)%3 */
    int32                   writeBuffer;
    
    /** */
    int32                   globalReadBuffer;

    /** Max data age in usec. It is used to decided if the read data are ready or not */
    int32                  maxDataAgeUsec;

    /** Max number of lost packets after a rollover */
    int32                  maxNOfLostPackets;

    /** Last packetId received */
    int32                  lastPacketID;

    /** The nSampleTime in the header of the last packet */
    int32                  lastPacketUsecTime;

    /** Packet producer specified microsecond period */
    int32                  producerUsecPeriod;

    /** Only used with the producerUsecPeriod and contains the 
     * nSampleNumber of the last packet */
    uint32                 originalNSampleNumber;

    /** Used to store HRTCounter to check producerUsecPeriod */
    int64                  lastCounter;

    /** CPU mask of the receiver thread (in case it's a receiver module) */
    int32                  cpuMask;

    /** Enable the System Acquisition */
    bool EnableAcquisition();

    /** Disable the System Acquisition */
    bool DisableAcquisition();

    /** Receive callback method */
    void RecCallback(void* arg);

    /** Flag to control the receiving thread */    
    bool                    keepRunning;

    /** The id of the receiving thread */
    TID                     threadID;

    /** Receiver thread priority within real time class */
    int32                   receiverThreadPriority;

     /** Dumps Input Data to the Stream
        @param s output stream
     */
    bool InputDump(StreamInterface &s) const;

    /** Driver liveness */
    int32 liveness;

    /** The packet number to be sent in producer mode*/
    uint32 packetNumber;

    /** If true the endianity will be applied*/
    bool applyEndianity;

public:

    /** Gets Data From the Module to the DDB
        @param usecTime Microseconds Time
        @return -1 on Error, 1 on success
    */
    int32 GetData(uint32 usecTime, int32 *buffer, int32 bufferNumber = 0);

///////////////////////////////////////////////////////////////////////////////
//                           Transmitter Type Module                         //
///////////////////////////////////////////////////////////////////////////////
private:

    /** The actual packet to be sent */
    void                  *outputPacket;

    /** Dumps the module outputs on the Stream */
    bool OutputDump(StreamInterface &s)const;

public:

    /** Sends the data in the DDB to the UDP switch */
    bool WriteData(uint32 usecTime, const int32 *buffer);

///////////////////////////////////////////////////////////////////////////////
//                           Monitoring Counters                             //
///////////////////////////////////////////////////////////////////////////////
private:

    /** Counts the number of size mismatch events. Reset as soon as the first correct packet is received */
    int32                  sizeMismatchErrorCounter;

    /** Counts the number of packet was too old. Reset as soon as the first correct packet is received */
    int32                  previousPacketTooOldErrorCounter;

    /** Counts the number of lost packets. Reset as soon as the first correct packet is received */
    int32                  lostPacketErrorCounter;

    /** Counts the number of lost packets. Reset as soon as the first correct packet is received */
    int32                  lostPacketErrorCounterAux;

    /** Counts the number of lost packets. Reset as soon as the first correct packet is received */
    int32                  samePacketErrorCounter;

    /** Counts the number of lost packets. Reset as soon as the first correct packet is received */
    int32                  recoveryCounter;

    /** Counts the number of lost packets. Reset as soon as the first correct packet is received */
    int32                  rolloverErrorCounter;

    /** Counts the number of lost packets. Reset as soon as the first correct packet is received */
    int32                  deviationErrorCounter;

///////////////////////////////////////////////////////////////////////////////
//                           Init & Mixed routine                            //
///////////////////////////////////////////////////////////////////////////////
private:    

    /** Copy constructors (since it is defined private it won't allow a public use!!) */
    UDPDrv(const UDPDrv&){};

    /** Operator=  (since it is defined private it won't allow a public use!!) */
    UDPDrv& operator=(const UDPDrv&){};

    /** Connection socket */
    UDPSocket socket;

    /** The destination server address, when used as an UDP_MODULE_TRANSMITTER*/
    FString destinationServerAddress;

    /** The server port, either when used as an UDP_MODULE_TRANSMITTER or an UDP_MODULE_RECEIVER*/
    uint32 destinationServerPort;

    /** The local server port, when used as an UDP_MODULE_RECEIVER*/
    uint32 serverPort;

public:

    /** Constructor */
    UDPDrv();

    /** Deconstructor */
    ~UDPDrv();

    /** Load Object Parameters from the ConfigurationDataBase */
    virtual bool ObjectLoadSetup(ConfigurationDataBase &info,StreamInterface *err);

    /** Saves Object Parameters to the ConfigurationDataBase */
    virtual bool ObjectSaveSetup(ConfigurationDataBase &info,StreamInterface *err){return True;};

    /** Object Description */
    virtual bool ObjectDescription(StreamInterface &s,bool full,StreamInterface *err);

    /** Set board used as input */
    virtual bool SetInputBoardInUse(bool on = True) {
        if(moduleType == UDP_MODULE_TRANSMITTER) {
            AssertErrorCondition(InitialisationError, "UDPDrv::SetInputBoardInUse: Board %s is a Transmitter", Name());
            return False;
        }
        if(inputBoardInUse && on) {
            AssertErrorCondition(InitialisationError, "UDPDrv::SetInputBoardInUse: Board %s is already in use", Name());
            return False;
        }
        inputBoardInUse  = on;
        outputBoardInUse = on;
        return True;
    }

    /** Set board used as output */
    virtual bool SetOutputBoardInUse(bool on = True) {
        if(moduleType == UDP_MODULE_RECEIVER) {
            AssertErrorCondition(InitialisationError, "UDPDrv::SetOutputBoardInUse: Board %s is a Receiver", Name());
            return False;
        }
        if(outputBoardInUse && on) {
            AssertErrorCondition(InitialisationError, "UDPDrv::SetOutputBoardInUse: Board %s is already in use", Name());
            return False;
        }
        inputBoardInUse  = on;
        outputBoardInUse = on;
        return True;
    }

///////////////////////////////////////////////////////////////////////////////
//                           From Time Module                                //
///////////////////////////////////////////////////////////////////////////////

public:

    // Get the Time
    int64  GetUsecTime();

///////////////////////////////////////////////////////////////////////////////
//                                  General                                  //
///////////////////////////////////////////////////////////////////////////////

    // Serve webpage
    bool   ProcessHttpMessage(HttpStream &hStream);

    // Called at pulse start
    bool PulseStart() {
        packetNumber = 0;
        return True;
    }
};

#endif
