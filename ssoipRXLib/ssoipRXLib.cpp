// ssoipRXLib.cpp : Defines the exported functions for the DLL application.

/*
Rivision History
Date              Author             Description
01-24-18         Sujit Halder        Implemented SSOIPRX SDK.
*/

#include "stdafx.h"
#include"ssoipRXLib.h"
#include"LogfileUtils.h"
#include"ssoipRX.h"
#include<iostream>
#include<fstream>
#include<map>
using namespace std;

LogfileUtils ssoipRXLog;
ssoipRX ssoiprx;

//Logfile with folder Path
 void   SetLogFolder(const char* logfolder)
{
	 ssoipRXLog.LogFilePath(logfolder);
	 ssoiprx.SetLogfile(ssoipRXLog);

}

 //Setup configuration, alternative option is to read confguration from file.
 SSOIPRXEXPORT void SetupConfiguration(const char  *ip, short port, const char * cutoff, const char * username, const char * password)
 {
	 try
	 {
	    ssoiprx.Setup(ip, port, cutoff, username, password);
		 
	 }
	 catch (...)
	 {
		  
	 }
	
 }

 //Connect to TX module, anl error message will be logged. monitor the logfile
 SSOIPRXEXPORT void   Connect( bool islungs)
 {	 
	 try
	 {
		 ssoiprx.Connect(islungs);
	 }
	 catch (...)
	 {

	 }
 }
  

 //Disconnect the connection with TX
 SSOIPRXEXPORT void   Disconnect()
 {
	 try
	 {
		 ssoiprx.Disconnect();
	 }
	 catch (...)
	 {

	 }
 }

 //Start Recording
 SSOIPRXEXPORT void   Record()
 {
	 try
	 {
		 ssoiprx.Record();
	 }
	 catch (...)
	 {

	 }
	
 }

 SSOIPRXEXPORT void RegisterRXCallback(callbackFunc func)
 {
	 ssoiprx.RegisterCallback(func);
 }
 //Read data from configuration file-stethoscopeConfig
 SSOIPRXEXPORT void ReadConfigurationFile()
 {
	 try
	 {
		 ssoiprx.ReadConfigFile();
	 }
	 catch (...)
	 {

	 }
 }

 //log message to log file
 SSOIPRXEXPORT void   LogMessage(const char * msg ,bool append=true)
 {
	 try
	 {
		 ssoipRXLog.LogMessage(msg, append);
	 }
	 catch (...)
	 {

	 }
 }
