#pragma once
#include<iostream>
#include<fstream>
using namespace std;

class LogfileUtils
{
	  
	 
public:
	LogfileUtils();
	~LogfileUtils();

	void LogFilePath(const char* logilePath);
	void LogMessage(const char* message, bool append=true);
	void LogCurrentDateTime();
	void LogData(const char*fileName, string data, bool append = true);

	string FolderPath;
	string Logfile;
};

