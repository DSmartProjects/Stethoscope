// ssoipTXLib.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include"ssoipTXLib.h"
#include"ssoiptxapi.h"
#include"LogfileUtils.h"
#include"iostream"
#include<list>
#include<map>
#include<vector>
#include"ssoipTX.h"
using namespace std;
ssoipTX ssoiptx;
LogfileUtils ssoipRXLog;


//generate recording devices
SSOIPTXEXPORT void GenerateRecordingDeviceFile()
{
	ssoiptx.GenerateRecordingDeviceFile();
}

//Read Configuration File
SSOIPTXEXPORT void ReadTXConfigurationFile()
{
	ssoiptx.ReadConfigFile();
}

 
//StartStreaming
SSOIPTXEXPORT void StartStreaming(int stethoscopeOption,bool lungs)
{
	try
	{
		ssoiptx.StartStreaming(stethoscopeOption,lungs);
	}
	catch (...)
	{
		
	}
}

SSOIPTXEXPORT void StartRecording() {
	ssoiptx.StartRecording();
}

SSOIPTXEXPORT void StopRecording() {
	ssoiptx.StopRecording();
}

SSOIPTXEXPORT void StopStreaming()
{
	try
	{
		ssoiptx.StopStreaming();
	}
	catch (...)
	{

	}
	 
}



//Retrives the list of recorded devices connected;
SSOIPTXEXPORT int GetRecordingDeviceList(int  id[], string  deviceNamelist[], string  deviceType)
{

	ssoiptx.RecordingDevicesList();
	return	ssoiptx.GetRecordingList(id, deviceNamelist, deviceType);

}


SSOIPTXEXPORT void RegisterCallback(callbackFunc func)
{
	ssoiptx.RegisterCallback(func);
}


//Logfile with folder Path
void   SetTXLogFolder(const char* logfolder)
{
	ssoipRXLog.LogFilePath(logfolder);
	ssoiptx.SetLogfile(ssoipRXLog);

}