#pragma once
#include<iostream>
using namespace std;
#define SSOIPTXEXPORT extern "C"   __declspec(dllexport)

 
SSOIPTXEXPORT void GenerateRecordingDeviceFile();
SSOIPTXEXPORT void ReadTXConfigurationFile();

SSOIPTXEXPORT void StartStreaming(int stethoscopeOption,bool lungs);
SSOIPTXEXPORT void StopStreaming();

SSOIPTXEXPORT int GetRecordingDeviceList(int  id[], string  deviceNamelist[], string  deviceType);

typedef void(__stdcall *callbackFunc)(const char*); 
SSOIPTXEXPORT void RegisterCallback(callbackFunc func);
//Logfile with folder path ex: c:\\rxLog.txt
SSOIPTXEXPORT void   SetTXLogFolder(const char* logfilder);
