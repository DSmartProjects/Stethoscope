#pragma once
#include"ssoiprxapi.h"
#include"LogfileUtils.h"
#include<map>
#include<string>
using namespace std;
typedef void(__stdcall *callbackFunc)(const char*);
//Implementation of ssoipRx SDK
class ssoipRX: public StatusListener
{
	
public:
	ssoipRX();
	~ssoipRX();
public:
	void Recorded_files();
	void Cuttoff_frequency_change(const char* cutoff);
	void Record();
	void Connect(bool islungs =false);
	void Disconnect();
	void SetLogfile(LogfileUtils& file) { logfile = file; }
	void ReadConfigFile();
	void RegisterCallback(callbackFunc func);
private:
	void playStream(bool catchAfterTransport, bool renderLocalSound, bool useTcpIp,bool islungs=false);
	void stopStream(); 
	static bool __stdcall SoipSenderCommandCallback(const char* command, void* result, int* resultSize, void* userData);
	bool SoipSenderCommandCallback(const char* command, void* result, int* resultSize);
	virtual void onStatus(const char* one, const char* two);
	virtual void onQoS(const QoSInfo& info);

private:
	LogfileUtils logfile;
	bool programaticAccessToFilters;
	unsigned int qosMeasurementIntervalMS;
	SoipPlayer* player;
	AbstractReceiverRecorderInterceptor* rr;	 
	SoipSender* soipSender;

void	ManageNewRecording(const char* filename);
callbackFunc NotifyClient;
	

private:
	
	map<string, string>configdata;
	string _ipaddress ;
	string _port ;
	string _cutoff ;
	string  _username ;
	string _password ;
public:
	void Setup(const char*ip, short port, const char* cutoff,const char* username,const char*password)
	{
		// config.recordingFile= "RXtestRecord.wav";
		string _ipaddress= ip;
		string _port = to_string(port);
		string _cutoff = cutoff;
		string  _username = username;
		string _password = password;

		configdata["IP"] = _ipaddress;
		configdata["PORT"] = _port;
		configdata["CUTOFF"] = _cutoff;
		configdata["USERNAME"] = _username;
		configdata["PASSWORD"] = _password;
		configdata["RECORDINGFILE"] = "RXtestRecord.wav";

		logfile.LogMessage(ip);
		logfile.LogMessage(cutoff);
		logfile.LogMessage(username);
		logfile.LogMessage(password);
	 
	}

};



 

