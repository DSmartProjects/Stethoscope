#include "stdafx.h"
#include "LogfileUtils.h"
 #include <ctime>
LogfileUtils::LogfileUtils()
{
	Logfile = "RXLog.txt";
	FolderPath = "";
}


LogfileUtils::~LogfileUtils()
{
	 
}

/// set the log file path
void LogfileUtils::LogFilePath(const char * logfileFolder)
{
 
	FolderPath = logfileFolder;
	Logfile = logfileFolder;
	Logfile += "\\RXLog.txt";
	 
	time_t now = time(0);
	// convert now to string form
	char* dt = ctime(&now);
	LogMessage(dt, false);
}



/// open file and append message, close the file.
void LogfileUtils::LogMessage(const char * message, bool append)
{
	try
	{
		ofstream logfile;
		if (append)
			logfile.open(Logfile, ios::app);
		else
			logfile.open(Logfile, ios::out);


		logfile << message << endl;

		logfile.close();
	}
	catch (...)
	{

	}

}

void LogfileUtils::LogCurrentDateTime()
{
	time_t now = time(0);
	// convert now to string form
	char* dt = ctime(&now);
	LogMessage(dt, false);
}

void LogfileUtils::LogData(const char * fileName, const char * data,   bool endlog, bool append)
{
	ofstream logfile;
	try
	{
		
		if (append)
			logfile.open(fileName, ios::app);
		else
			logfile.open(fileName, ios::out);


		logfile << data << endl;

		if(endlog)
		logfile.close();
	}
	catch (...)
	{
		if(logfile.is_open())
			logfile.close();
	}
}
