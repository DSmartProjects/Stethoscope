#include "stdafx.h"
#include "ssoipRX.h"
#include<map>
ssoipRX::ssoipRX()
{
	NotifyClient = NULL;
}


ssoipRX::~ssoipRX()
{
}

void ssoipRX::Recorded_files()
{
}

//setup as per the cutoff value
void ssoipRX::Cuttoff_frequency_change(const char *cutoff)
{
	 
	if (player && cutoff != NULL)
	{
		double fq = 0.0;
		

		if (strcmp(cutoff, "No Filter")==0)
		{
			player->setFilterParameters(0.0);
		}
		else if (strcmp(cutoff, "Dual")==0)
		{
			player->setFilterType("double lpf");
			player->setFilterParameters(250.0);
		}
		else if (strcmp(cutoff,"Lungs")==0)
		{
			player->setFilterType("lpf");
			player->setFilterParameters(-250.0);
			logfile.LogMessage("Filter-Lungs: -250.0");
		}
		else
		{
			fq = atof(cutoff);
			player->setFilterType("lpf");
			player->setFilterParameters(fq);

			logfile.LogMessage(cutoff);
		}
	}
}

//start recording 
void ssoipRX::Record()
{
	// Here we call the player to record the stream in a file
	//  - file management will need to be added 
	// to organize multiple files
	if (player)
	{
		if (!player->isRecording())
		{ 
			string filename = logfile.FolderPath+"\\record.wav";
			player->record(filename.c_str());
		   logfile.LogMessage(	"Stop Recording");
		}
		else
		{
			player->stopRecording();
		//	manageNewRecording("testRecord.wav");
			logfile.LogMessage("Record");
		}
	}
}

//Connect to TX module
void ssoipRX::Connect(bool islungs)
{

	char client[1024];
   //License string not in use
	int sz = SoipPlayerFactory::getLicensedString("Customer Product Name", client, 1023);
	if (sz > 0)
	{
		logfile.LogMessage(client);
	}
	//"streamUsingTCP" 
	playStream(false, true, false, islungs);
}

//stop Streaming
void ssoipRX::Disconnect()
{
	stopStream();
	
}

//Read Configfile
void ssoipRX::ReadConfigFile()
{
	try
	{
		string configFile = logfile.FolderPath;
		configFile += "\\ST_MCC_Config.txt";
		if(logfile.FolderPath.length()==0)
			configFile += "ST_MCC_Config.txt";

		//if (NotifyClient != NULL)
		//	NotifyClient(configFile.c_str());

		ifstream configfile(configFile);
		if (!configfile)
		{
			logfile.LogMessage("Error: StethoscopeConfig File is missing.");
			return;
		}
		
		char line[256];
		
		while (configfile)
		{
			configfile.getline(line, 256);
			string strline = line;
			if (strline.length() <= 0)
				continue;

			int separatorPos = strline.find(':');

			string key = strline.substr(0, separatorPos);
			string value = strline.substr(separatorPos + 1, strline.length());
			configdata[key] = value;
			 
		}
		if(configdata["CUTOFF"].length() ==0)
			configdata["CUTOFF"]= "No Filter";
		if (configdata["LOGFILE"].length() == 0)
			configdata["LOGFILE"] = "rnk_RX_logfile.txt";
		if (configdata["FILTER-CUTOFF-LUNGS"].length() == 0)
			configdata["FILTER-CUTOFF-LUNGS"] = "-250";

		configdata["RECORDINGFILE"] = "RXtestRecord.wav";
		
		configfile.close();
	}
	catch (...)
	{
		
	}
}

//Start Streaming
void ssoipRX::playStream(bool catchAfterTransport, bool renderLocalSound, bool useTcpIp, bool islungs)
{
	logfile.LogCurrentDateTime();

	if (player && player->isPlaying())
	{
		stopStream();
		
		return;
	}

	
	player = SoipPlayerFactory::create();
	if (!player)
	{
		logfile.LogMessage("Error: Failed to Create Player object.");
		return;
	}

	// provide the parameters to API and the status listener
	// to get teh API to hand in the transport just set the seconds of buffer
	int seconds4Buffer = 0;
	if (catchAfterTransport)
		seconds4Buffer = 10;
	if(!islungs)
	Cuttoff_frequency_change(configdata["CUTOFF"].c_str());
	else
	Cuttoff_frequency_change(configdata["FILTER-CUTOFF-LUNGS"].c_str());


	const char *username = configdata["USERNAME"].c_str();
	const char *password = configdata["PASSWORD"].c_str();

	if (username !=NULL && password !=NULL)
	{
		if (username == NULL)
		{
			// no encryption - check with the TX side to have the same setting
			player->setCredentials(NULL, NULL);
		}
		else
		{
			player->setCredentials(username ,  password );
		}
	}
	else
	{
		//Demo purpose
		player->setCredentials("rnk", "test");
	}
	
	const char*ip = configdata["IP"].c_str();
	short port = atoi(configdata["PORT"].c_str());

	string logFileName = "logs.txt";
	string logfilepath = logfile.FolderPath;
	logfilepath += "\\";
	logfilepath += logFileName;

	player->init(ip, port , useTcpIp, seconds4Buffer, renderLocalSound, qosMeasurementIntervalMS, logfilepath.c_str(), *(this), soipSender);

	bool bstatus = player->start();
	if (bstatus)
	{
		if (catchAfterTransport)
		{
			// get the buffer
			AbstractRingBuffer* ringBuffer = player->getBufferAfterTransport();

			rr = SoipPlayerFactory::newRecorderInterceptor();
			if (rr)
			{
				char recordingfile[256];
				strcpy(recordingfile, configdata["RECORDINGFILE"].c_str());
				rr->setParams(recordingfile, ringBuffer);
				rr->start();
			}
		}

	}


	logfile.LogMessage(islungs?"LUNGS":"CHEST");
}

//Stop Streaming
void ssoipRX::stopStream()
{
	if (player && player->isPlaying())
	{
		player->stop();
		if (rr) rr->stop();
		 
		delete(player);
		player = NULL;

	}
	else if(player != NULL)
	{
		 
		delete(player);
		player = NULL;
	}
}

 

bool ssoipRX::SoipSenderCommandCallback(const char* command, void* result, int* resultSize, void* userData)
{
	int test = 0;
	// logfile.LogMessage("SoipSenderCommandCallback");
	return false;
}

bool ssoipRX::SoipSenderCommandCallback(const char * command, void * result, int * resultSize)
{
	logfile.LogMessage("SoipSenderCommandCallback");
	return false;
}

void ssoipRX::onStatus(const char * one, const char * two)
{
	if (one != NULL && strcmp(one, "error") == 0)
	{
		logfile.LogMessage(one);
		if (NotifyClient != NULL)
			NotifyClient(one);
		else
			logfile.LogMessage("one null");
	}

	if (two != NULL)
	{
		logfile.LogMessage(two);
		if (NotifyClient != NULL)
			NotifyClient(two);
		else
			logfile.LogMessage("two null");
	}
}


void ssoipRX::onQoS(const QoSInfo & info)
{
	logfile.LogMessage("info");
	 
}

// Register call back function
void ssoipRX::RegisterCallback(callbackFunc func)
{
	NotifyClient = func;
}

void ssoipRX::ManageNewRecording(const char * filename)
{
	 
}

 