#ifndef SOIPRXS_H
#define SOIPRXS_H

#include "ssoiprxapi_global.h"


class SOIPRXS_EXPORT StatusListener
{
public:
	virtual void onStatus(const char* one, const char* two)=0;
	virtual void onQoS(const QoSInfo& info)=0;
	virtual ~StatusListener(){};
};

/**
	This class will be exposed from the API as a way to get 
	samples after transport and before entering the rendering on 
	local hardware.
*/
class SOIPRXS_EXPORT AbstractRingBuffer {
 public:
	typedef enum {
	   int16Sample = 0x00020001,
	   int24Sample = 0x00040001,
	   floatSample = 0x0004000F
	} sampleFormat;

	virtual ~AbstractRingBuffer(){};

   virtual int AvailForGet()=0;
   virtual int Get(char* buffer, sampleFormat format, int samples)=0;
   virtual int Discard(int samples)=0;
   virtual int getBufferSize()=0;

};

class SOIPRXS_EXPORT AbstractReceiverRecorderInterceptor
{
public:
	virtual bool setParams(char* fileName, AbstractRingBuffer* rb) =0;
	virtual void stop() =0;
	virtual void start() =0;
};

class SOIPRXS_EXPORT iPlayer {
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

typedef bool (__stdcall *SoipSenderCommandCallbackFct)(const char* command, void* result, int* resultSize, void* userData);

class SOIPRXS_EXPORT SoipSender
{
public:
	virtual bool init(short port, SoipSenderCommandCallbackFct commandCallback, void* callbackUserData)=0;

	virtual bool start()=0;
	virtual void stop()=0;
};

class SOIPRXS_EXPORT SoipPlayer 
{
public:
	// provide the parameters to run the player, do this before calling start
	// StatusListener is an interface that the caller implements and instantiates
	// The player will call onStatus with progress and event messages/codes for 
	// operations.
	virtual void init(const char* ip,
		short port,
		bool useTcpIp,
		int wantStreamAfterTransportSeconds,
		bool doRender,
		unsigned qosMeasurementIntervalMS, // how often to read QoS information, 0 means not interested in QoS
		const char* loggerName,
		StatusListener& statusListener,
		SoipSender* soipSender = NULL)=0;

	virtual bool start()=0;
	virtual bool isPlaying()=0;
	virtual bool isRecording()=0;
	virtual bool stop()=0;
	virtual bool record(const char* fileName)=0;
	virtual bool stopRecording()=0;
	virtual void setCredentials(const char* username, const char* password)=0;	
	virtual AbstractRingBuffer* getBufferAfterTransport()=0;
	virtual void setFilterParameters(double cutOffFrequency)=0;
	virtual void setFilterType(const char* type)=0;

	virtual ~SoipPlayer(){};
};

#define SSOIP_BELL				1
#define SSOIP_DIAPHRAGM			2
#define SSOIP_EXTENDED_RANGE	3
#define SSOIP_LUNGS				4
class SOIPRXS_EXPORT SoipPlayerFactory
{
public:
	static SoipPlayer* create();
	static iPlayer* createPlayer();
	static void releasePlayer(iPlayer* player);
	static AbstractReceiverRecorderInterceptor* newRecorderInterceptor();
	static long long getPlayTimeForFile(const char* fileName);
	static void setMetaInfo(const char* fileName, const char* name, const char* comment);
	static void getSignature(char* sig, int size);
	static void getString(int sid, char* s, int size, int location);
	static int getLicensedString(const char* key, char* value, int size);
	static bool checkSignature();
	static void getReason(char* buf, int size);
	static int decode(const char* coder, const char* inBytes, unsigned int inSize, void* sampleBuffer, unsigned int noSamples);
	static void filter(unsigned short type, void* sampleBuffer, unsigned int pSampleFormat, unsigned int noSamples);
	static void filter(double cutoff, double sample_fq, void* sampleBuffer, unsigned int pSampleFormat, unsigned int noSamples);
	static SoipSender* createSender();
};



#endif // SOIPRXS_H
