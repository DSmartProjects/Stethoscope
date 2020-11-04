#pragma once
#include"ssoiptxapi.h"
#include<iostream>
#include<vector>
#include<map>
#include"LogfileUtils.h"
using namespace std;
typedef void(__stdcall *callbackFunc)(const char*);
class ssoipTX
{
public:
	ssoipTX();
	~ssoipTX();

public:
	

	struct RecordingDeviceInfo
	{
		string deviceName;
		int id;
		string deviceType;

	};

	int GetRecordingList(int id[], string deviceNamelist[], string deviceType="Windows WASAPI");
	int	   RecordingDevicesList();
	void ReadConfigFile();
	void GenerateRecordingDeviceFile();
	void RegisterCallback(callbackFunc);

	 void SetLogfile(LogfileUtils& file) { logfile = file; }
	 void Logconfiguration(int id, int port, double freq, float gain, string user, string currentCodec, double filter, bool islungs = false );
private:
map<string,	vector<RecordingDeviceInfo>>AllRecordingDeivce;
SoipRecorder* soipRecorder;
map<string, string>configdata;
string StreamMessage;
char url[1024];
LogfileUtils logfile;
public:
void* stream;
void StartStreaming(int stethoscopeOption,bool lungs);
void StopStreaming();
callbackFunc NotifyClient;
static bool __stdcall SoipStreamStateCallback(bool sending, SoipRecorder* soipRecorder, void* userData);
bool SoipStreamStateCallback(bool sending, SoipRecorder* soipRecorder);
void StartRecording();
void StopRecording();
int GetDeviceID(int index);
};

