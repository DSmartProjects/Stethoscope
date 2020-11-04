#ifndef SOIPTXS_H
#define SOIPTXS_H

#include "ssoiptxapi_global.h"

/*! \mainpage SSOIP TX version 0.01
 *
 * \section intro Introduction
 *
 * This documentation refers to the SSOIP TX – the transmission side for captured audio stream over network.
 * The TX is packaged as a DLL with header files and a .lib file to link to. The code is responsible for
 * capturing a selected audio device and using RTP/RTSP streaming the captured audio over network.
 * The TX DLL is dependent on QT DLLs (http://doc.qt.nokia.com), pleases ee the Dependencies section.  
 * The DLL comes with a Visual Studio 2008 solution demonstrating the setup and usage of the DLL functions. 
 * Please see the SoipTXSDemo directory that should be provided with the TX DLL. 
 * 
 *
 * \section usage Usage
 *
 * Start by getting the available recording devices in the system, call GetRecordingDevices, 
 * pick one that will be used to capture audio.
 * The audio stream should be encrypted over public networks so a call to SetCredentials 
 * will establish the user/password that will need to be matched on the receiver side. 
 * Assuming that “port” is the desired port to start the RTSP protocol setting a user/password 
 * will determine the existence of a SSL server on port+3  allowing the authentication and exchange
 * of AES keys for encryption.
 * Now you are ready to call StartStreaming providing the device index from first step, the desired local port
 * to run RTSP on (port) , localMonitoring set to true will direct audio to the default speaker I your system. Providing a name for the log file will activate the DLL logging with details of operations that DLL performs saved in this log file. Provide a place (url) where the TX will return the RTSP URL that can be used in a RTSP player or RX product. urlSiseLimit tells TX the limit of the provided buffer.
 * The result of the StartStreaming will be a pointer to a stream that needs to be kept and delivered to
 * the StopStreaming call when you want to stop the streaming.
 * StopStreaming when it is desired.
 * 
 *  
 */

/**
* This interface is implemented by the library using a ring buffer so
* it is thread safe
*/
SOIPTXS_EXPORT typedef void (*ARB_callback)();

class SOIPTXS_EXPORT AbstractRingBuffer {
 public:
	typedef enum {
	   byteSample = 0x00010001,
	   int16Sample = 0x00020001,
	   int24Sample = 0x00040001,
	   floatSample = 0x0004000F
	} sampleFormat;

	virtual ~AbstractRingBuffer(){};

   virtual int AvailForGet()=0;
   virtual int Get(char* buffer, sampleFormat format, int samples)=0;
   virtual int Discard(int samples)=0;
   virtual int getBufferSize()=0;
   virtual sampleFormat getStoredFormat()=0;
   virtual void getCodecName(char* codecName, int size)=0;
   virtual void setCallback(ARB_callback cb)=0;
};

#ifndef QOS_DATA
#define QOS_DATA	1
class QoSData
{
public:
	unsigned totNumPacketsLost;
	unsigned jitter;
	unsigned roundTripDelay;
	unsigned outNumBytes;
	double outElapsedTime;
};
#endif

/**
*	
*/
class SOIPTXS_EXPORT AbstractReceiverRecorderInterceptor
{
public:
	virtual bool setParams(char* fileName, AbstractRingBuffer* rb) =0;
	virtual void stop() =0;
	virtual void start() =0;
};

class SOIPTXS_EXPORT SoipRecorder
{
public:
	virtual bool isRecording()=0;
	virtual bool record(const char* fileName)=0;
	virtual bool stopRecording()=0;
};

class SOIPTXS_EXPORT iPlayer {
public:
	virtual bool play(long long sec=-1)=0;
	virtual void stop()=0;
	virtual void setFileToPlay(const char* ftp)=0;
	virtual void getMetaInfo(const char* fileName, char* title, char* comment, int sz)=0;
	virtual long long getPlayTimeForFile(const char* fileName)=0;
	virtual void* getQObject()=0;
	virtual void setFilterParameters(double cutOffFrequency)=0;
	virtual void setFilterType(const char* type)=0;
};

typedef bool (__stdcall *SoipStreamStateCallbackFct)(bool sending, SoipRecorder* soipRecorder, void* userData);

/** Start the streaming with desired parameters
*   deviceIndex - the audio capture device , use GetRecordingDevices to get a list
*   port - network port that will run a RTSP server
*   localMonitoring - if you want to hear teh audio stream in the speakers while streaming
*   logFileName - if provided the TX will create/overwrite a log file with logs internals to TX
*   url - buffer to retrun the RTSP URL where the server is listening , can be used directelly in VLC
*         or QuickTime players to allow them to connect to this server (started stream).
*   urlSizeLimit - url buffer size provided
*   
*   Upon success returns a pointer to the stream that will need to be provided in the StopStreaming call for cleanup
*   The stream is transported over RTP using UDP. TX is capable of streaming on only TCP/IP - allowing firewall traversals 
*   if paired with RX. An external player will not be capable or running TCP/IP from TX.
*/
SOIPTXS_EXPORT void* StartStreaming(int deviceIndex, double fq, const char* coderName, int port, float gain, bool localMonitoring, const char* logFileName, char* url, int urlSizeLimit, SoipStreamStateCallbackFct stateCallback = NULL, void* callbackUserData = NULL);
SOIPTXS_EXPORT void* StartStreaming(int inputDeviceIndex, int outputDeviceIndex, double fq, const char* coderName, int port, float gain, bool localMonitoring, const char* logFileName, char* url, int urlSizeLimit, SoipStreamStateCallbackFct stateCallback = NULL, void* callbackUserData = NULL);


/** Stop Streaming
*  Stops the provided stream , releasing ressources.
*/
SOIPTXS_EXPORT void  StopStreaming(void* stream);

// gets the transmission quality of service
SOIPTXS_EXPORT void GetQoS(void* stream, QoSData& qos);

/** Test local audio capture and rendering
*
* outDeviceIndex should be set to -1 for default output 
*
*/
SOIPTXS_EXPORT void* LocalStartStreaming(int inDeviceIndex, int outDeviceIndex, const char* coderName, float gain, const char* logFileName, char* url, int urlSizeLimit);
SOIPTXS_EXPORT void* LocalStartStreaming(int inDeviceIndex, const char* coderName, float gain, const char* logFileName, char* url, int urlSizeLimit);
SOIPTXS_EXPORT void  LocalStopStreaming(void* stream);
SOIPTXS_EXPORT void  LocalStartRecording(void* stream, const char* filename);
SOIPTXS_EXPORT void  LocalStopRecording(void* stream);
SOIPTXS_EXPORT int  LocalIsRecording(void* stream);

/** 
*	Start capture usinng a specific device
*
*
*/
SOIPTXS_EXPORT void* StartSoundCapture(int deviceIndex, const char* codec, const char* logFileName, AbstractRingBuffer** pRawBuffer, short rawSeconds, AbstractRingBuffer** pFilteredBuffer, short filteredSeconds);

/**
* The interceptor demonstrates and tests that an abstract buffer can be linked into a file
* proving the usage for the abstract ring buffer
*/
SOIPTXS_EXPORT AbstractReceiverRecorderInterceptor* GetInterceptor(void* stream);
SOIPTXS_EXPORT void  StopSoundCapture(void* stream);

#define SSOIP_BELL				1
#define SSOIP_DIAPHRAGM			2
#define SSOIP_EXTENDED_RANGE	3
#define SSOIP_LUNGS				4
SOIPTXS_EXPORT void	SetFilter(void* stream, unsigned short f);

/** GetRecordingDevices
*	Lists all the available audio recoding devices in the system. If your expected device is not in
*   check it is plugin and call this function again - a lot of modern sound devices are showing only when connected
*/
SOIPTXS_EXPORT int	 GetRecordingDevices(int cidList[], char* cdeviceNameList[], int sizeLimit);
SOIPTXS_EXPORT int	 GetRecordingDevices(int cidList[], char* cdeviceNameList[], char* cdeviceTypeList[], int sizeLimit);

SOIPTXS_EXPORT int GetAudioRenderingDevices(int cidList[], char* cdeviceNameList[], char* cdeviceTypeList[], int sizeLimit);

SOIPTXS_EXPORT int	 GetInterfaceDeviceIdFromGlobalDeviceId(int globalId, int type = 13);
SOIPTXS_EXPORT void GetDeviceIdForLegacyIndex(int legacyDeviceIndex, void* id);

/** Set the user and password to activate encryption
*	An SLL server will start on port+3 (the port from the StartStreaming call)  
*   When this function is called with NULL,NULL it will reset credentials and a following StartStreaming call
*   will perform unencrypted. The encryption is done using AES and if you try to use VLC to listen to teh encrypted stream
*   only noise will play
*
*/
SOIPTXS_EXPORT void  SetCredentials(const char* name, const char* password);

/** Set the frequency for the low pass filter
*	cutOffFrequency - the cut off frequency, 0.0 means no filtering 
*/
SOIPTXS_EXPORT void  SetFilterProperties(double cutOffFrequency);

/** Get the library signature
*
*/
SOIPTXS_EXPORT void GetSignature(char* sig, int size);
SOIPTXS_EXPORT void GetString(int sid, char* s, int size, int location);
SOIPTXS_EXPORT bool CheckSignature();
SOIPTXS_EXPORT void GetReason(char* buf, int size);

SOIPTXS_EXPORT void* RoundTripEncodeDecodeStart(const char* codecName);
SOIPTXS_EXPORT void RoundTripEncodeDecodeStop(void* lp);
SOIPTXS_EXPORT int RoundTripEncodeDecodeProcess(void* lp, const char* in, char* out, int size);
SOIPTXS_EXPORT unsigned RoundTripEncodeDecodeSampleSize(void* lp);

SOIPTXS_EXPORT void detectCodecs(char* codec_name_list, char* display_name, int size);

int SOIPTXS_EXPORT GetLicensedValue(const char* key, char* value, int size);


int SOIPTXS_EXPORT StartUDPMonitor();
int SOIPTXS_EXPORT StopUDPMonitor();
int SOIPTXS_EXPORT StartUDPBeacon();
void SOIPTXS_EXPORT StopUDPBeacon();
int SOIPTXS_EXPORT UDPBeaconGotEcho();

SOIPTXS_EXPORT iPlayer* CreatePlayer();
SOIPTXS_EXPORT void ReleasePlayer(iPlayer* player);

SOIPTXS_EXPORT long GetPlayTimeForFile(const char* fileName);
SOIPTXS_EXPORT void SetMetaInfo(const char* fileName, const char* name, const char* comment);

#endif // SOIPTXS_H
