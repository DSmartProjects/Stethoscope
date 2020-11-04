#pragma once

#define SSOIPRXEXPORT extern "C"   __declspec(dllexport)

//Logfile with folder path ex: c:\\rxLog.txt
SSOIPRXEXPORT void   SetLogFolder(const char* logfilder);

//read configuration File
SSOIPRXEXPORT void ReadConfigurationFile();

//setup configuration
SSOIPRXEXPORT void   SetupConfiguration(const char*ip, short port, const char* cutoff, const char* username, const char*password);
//Log message to logfile
SSOIPRXEXPORT void   LogMessage(const char* msg, bool append);

//Connect to TX module
SSOIPRXEXPORT void   Connect(bool islungs=false);

//Disconnect from TX 
SSOIPRXEXPORT void   Disconnect();

//Record the received sound
SSOIPRXEXPORT void   Record();
typedef void(__stdcall *callbackFunc)(const char*);
SSOIPRXEXPORT void RegisterRXCallback(callbackFunc func);

 