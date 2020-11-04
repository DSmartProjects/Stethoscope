#pragma once
#include<iostream>
#include<fstream>
using namespace std;
#pragma warning(disable : 4996)
class LogfileUtils
{
	  
	 
public:
	LogfileUtils();
	~LogfileUtils();

	void LogFilePath(const char* logilePath);
	void LogMessage(const char* message, bool append=true);
	void LogCurrentDateTime();
	void LogData(const char*fileName, const char*data,  bool endlog=true, bool append=true);

	string FolderPath;
	string Logfile;
};

