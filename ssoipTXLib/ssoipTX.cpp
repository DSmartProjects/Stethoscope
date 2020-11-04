
/*Rivision History 
------------------------------------------------
Date        Author                Description
1-26-18   Sujit Halder             Created.
*/

#include "stdafx.h"
#include "ssoipTX.h"
#include<fstream>
#include"LogfileUtils.h"
#include<string>
using namespace std;
  
ssoipTX::ssoipTX()
{
	NotifyClient = NULL;
}


ssoipTX::~ssoipTX()
{
}

//return alll recording devices
int ssoipTX::GetRecordingList(int id[], string deviceNamelist[], string deviceType)
{
	map<string, vector<RecordingDeviceInfo>>::iterator itr;
	if (configdata["ALLRECORDING-DEVICE"].length() > 0 && strcmp(configdata["ALLRECORDING-DEVICE"].c_str(), "true") == 0)
		itr = AllRecordingDeivce.begin();
	else
		itr = AllRecordingDeivce.find(deviceType);
	int count = 0;
	if (itr != AllRecordingDeivce.end())
	{
		vector<RecordingDeviceInfo>	recordingDevice = itr->second;
		count = recordingDevice.size();
		for (int i = 0; i < count; i++)
		{
			RecordingDeviceInfo deviceinfo = recordingDevice[i];
			id[i] = deviceinfo.id;
			deviceNamelist[i] = deviceinfo.deviceName;
			deviceType = deviceinfo.deviceType;
			string device = std::to_string(deviceinfo.id);
			device += ":";
			device += deviceinfo.deviceName;
			device += deviceinfo.deviceType;

			//logfile.LogData("RecordingDevices.log", device);
		}
	}

	return count;
}

//Register call back function
void ssoipTX::RegisterCallback(callbackFunc func)
{
	NotifyClient = func;
}

//Retrive all recording devices.
int ssoipTX::RecordingDevicesList()
{
	AllRecordingDeivce.clear();

	int sizeLimit = 32;
	int idList[32];
	char* deviceNameList[32];
	char* deviceTypeList[32];

	char deviceTable[32][256];
	char deviceTypeTable[32][256];
	for (int i = 0; i< 32; i++)
	{
		deviceNameList[i] = deviceTable[i];
		deviceTypeList[i] = deviceTypeTable[i];
	}

	int	rdevs = GetRecordingDevices(idList, deviceNameList, deviceTypeList, sizeLimit);
	
	for (int i = 0; i < rdevs; i++)
	{
		RecordingDeviceInfo deviceinfo;
		deviceinfo.deviceName = deviceNameList[i];
		deviceinfo.deviceType = deviceTypeList[i];
		deviceinfo.id = idList[i];

	map<string,vector<RecordingDeviceInfo>>::iterator itr= AllRecordingDeivce.find(deviceTypeList[i]);

	if (itr == AllRecordingDeivce.end())
	{
		vector<RecordingDeviceInfo> device;
		device.push_back(deviceinfo);
		AllRecordingDeivce[deviceTypeList[i]] = device;

	}
	else
	{
		vector<RecordingDeviceInfo> *device = &itr->second;
		device->push_back(deviceinfo);
	}

	}


	return 0;
}

//ReadConfig file
void ssoipTX::ReadConfigFile()
{
	try
	{
		string configFile = logfile.FolderPath;
		configFile += "\\ST_sbcConfig.txt";
		if (logfile.FolderPath.length() == 0)
			configFile = "ST_sbcConfig.txt";
		ifstream configfile(configFile);
		if (!configfile)
		{
			logfile.LogMessage("Error: Config File is missing.");
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
		if (configdata["CUTOFFFILTER"].length() == 0)
			configdata["CUTOFFFILTER"] = "0";
		if (configdata["LOGFILE"].length() == 0)
			configdata["LOGFILE"] = "rnk_TX_logfile.txt";
		if (configdata["DEVICETYPE"].length() == 0)
			configdata["DEVICETYPE"] = "Windows WASAPI";

		configdata["RECORDINGFILE"] = "RXtestRecord.wav";

		configfile.close();
	}
	catch (...)
	{

	}
}

//Recording deviceList in a generated file.
void ssoipTX::GenerateRecordingDeviceFile()
{
	RecordingDevicesList();
	string filewithfolder = "RecordingDevices.txt";
	if (logfile.FolderPath.length() > 0)
	{
		filewithfolder = logfile.FolderPath;
		filewithfolder += "\\RecordingDevices.txt";

	}
	logfile.LogData(filewithfolder.c_str(), "ID: DEVICE NAME : DEVICE TYPE",false);
	logfile.LogData(filewithfolder.c_str(),"------------------------------------------------------------");
	int count = 0;
	map<string, vector<RecordingDeviceInfo>>::iterator itr;
	

		itr = AllRecordingDeivce.find("Windows WASAPI");
		if (itr != AllRecordingDeivce.end())
		{
			vector<RecordingDeviceInfo>	recordingDevice = itr->second;
			count = recordingDevice.size();
			for (int i = 0; i < count; i++)
			{
				RecordingDeviceInfo deviceinfo = recordingDevice[i];

				string device = std::to_string(deviceinfo.id);
				device += ":";
				device += deviceinfo.deviceName;
				device += ":";
				device += deviceinfo.deviceType;
				
					try
					{
						logfile.LogData(filewithfolder.c_str(), device);
					}
					catch (...)
					{

					}
			}
		}
		else
		{
			if (NotifyClient != NULL)
				NotifyClient("No device found.");
			 
		}

}

//Start Streaming
void ssoipTX::StartStreaming(int stethoscopeOption,bool lungs)
{
	if (stream)
	{
		StopStreaming();
	//	return;
	}

	int deviceId =GetDeviceID(stethoscopeOption);
	int port = atoi(configdata["PORT"].c_str());
	double filter = 0.0;
	string cutoff = configdata["CUTOFFFILTER"];
	try {
		if (!lungs && strcmpi(cutoff.c_str(), "0") != 0)
		{
			filter = atof(cutoff.c_str());
			/*if (strcmpi(cutoff.c_str(), "Lungs") == 0)
			{
				filter = -250.00;
			}
			else {
				filter = atof(cutoff.c_str());
			}*/
		}
		else if(lungs)
		{
			string cutofflungs = configdata["CUTOFFFILTERLUNGS"];
			if(strcmpi(cutoff.c_str(), "0") == 0)
				filter = -250.00;
			else
			filter = atof(cutofflungs.c_str());
		}
	}
	catch (...)
	{
		logfile.LogMessage("cutoff Filter failed to set" );
	}
	
	//set fileter
	::SetFilterProperties(filter);

	double fq = atof(configdata["FREQUENCYHEART"].c_str());
	if (lungs)
	{
		fq = atof(configdata["FREQUENCYLUNGS"].c_str());
	}
	 
	float gain = (float)atof(configdata["GAIN"].c_str());
	if (gain == 0.0)
		gain = 1.0;

	string currentCodec = ""; //configdata["CODEC"];
	if (currentCodec == "PCM")
		currentCodec = "";

	const char* username = configdata["USERNAME"].c_str();
	const char* password = configdata["PASSWORD"].c_str();
	//set credential
	::SetCredentials(username, password);
	 
	bool localMonitoring = false;
	string logFileName = "logfile.txt";
	string logfilepath = logfile.FolderPath;
	logfilepath += "\\";
	logfilepath += logFileName;
	this->stream = ::StartStreaming(deviceId, fq , currentCodec.c_str(), port, gain, localMonitoring, logfilepath.c_str(), url, 1024, SoipStreamStateCallback, this);
	if (this->stream)
	{ 
		StreamMessage = "Ready for streaming at :";
		StreamMessage += url;
		logfile.LogMessage(StreamMessage.c_str());
		if (NotifyClient != NULL)
		{
			NotifyClient(StreamMessage.c_str());
		}
	}
	else
	{
		StreamMessage = "Failed to start stream";
		logfile.LogMessage(StreamMessage.c_str());
		if (NotifyClient != NULL)
		{
			NotifyClient(StreamMessage.c_str());
		}
	}

	Logconfiguration(deviceId, port, fq, gain, username, currentCodec, filter, lungs);
}

void ssoipTX::Logconfiguration(int id, int port,double freq, float gain,string user, string currentCodec,double filter, bool islungs)
{
	try
	{
		string stethoscopeconfigLog = logfile.FolderPath;
		stethoscopeconfigLog += "\\";
		stethoscopeconfigLog +="StethoscopeConfigLog.txt";
		string data = "DeviceID: ";
		data += std::to_string(id);
		 
		logfile.LogData(stethoscopeconfigLog.c_str(), data, false);
		data = "Frequency: ";
		data+= std::to_string(freq);
		logfile.LogData(stethoscopeconfigLog.c_str(), data, true);
		data = "gain: ";
		data += std::to_string(gain);
		logfile.LogData(stethoscopeconfigLog.c_str(), data, true);
		data = "Filter: ";
		data += std::to_string(filter);
		logfile.LogData(stethoscopeconfigLog.c_str(), data, true);

		data = "Port: ";
		data += std::to_string(port);
		logfile.LogData(stethoscopeconfigLog.c_str(), data, true);
		data = "User: ";
		data += user;
		logfile.LogData(stethoscopeconfigLog.c_str(), data, true);

		data = "codec: ";
		data += currentCodec;
		 
		logfile.LogData(stethoscopeconfigLog.c_str(), data, true);

		data = "mode: ";
		data += islungs?"Lungs":"Chest";

		logfile.LogData(stethoscopeconfigLog.c_str(), data, true);
		
		 
	}
	catch (...)
	{

	}
}
void ssoipTX::StopStreaming()
{
	try
	{
		if (stream)
		{
			::StopStreaming(stream);
			stream = NULL;
			logfile.LogMessage("Streaming Stopped.");
			if (NotifyClient != NULL)
			{
				NotifyClient("Streaming Stopped.");
			}
		}
	}
	catch (...)
	{
		logfile.LogMessage("RX connected.");
	}
}

bool ssoipTX::SoipStreamStateCallback(bool sending, SoipRecorder * soipRecorder, void * userData)
{
	ssoipTX* pssoip = (ssoipTX*)userData;
	
	return pssoip->SoipStreamStateCallback(sending, soipRecorder);
}

bool ssoipTX::SoipStreamStateCallback(bool sending, SoipRecorder * soipRecorderin)
{
	if (sending)
	{ 
		 this->soipRecorder = soipRecorderin;
		StreamMessage = "Streaming at :";
		StreamMessage += url;
		logfile.LogMessage(StreamMessage.c_str());
		if (NotifyClient != NULL)
		{
			NotifyClient(StreamMessage.c_str());
		}
		
	}
	else 
	{
		 
		StreamMessage = "Ready for streaming at :";
		StreamMessage += url;
		logfile.LogMessage(StreamMessage.c_str());
		if (NotifyClient != NULL)
		{
			NotifyClient(StreamMessage.c_str());
		}
	}

	return true;
}

void ssoipTX::StartRecording()
{
	if (soipRecorder)
		{
			if (!soipRecorder->isRecording())
			{
				string stethoscopeconfigLog = logfile.FolderPath;
				stethoscopeconfigLog += "\\";
				stethoscopeconfigLog += "Heartbeat.wav";
				soipRecorder->record("Heartbeat.wav");
			}
		}
}
void ssoipTX::StopRecording()
{
	if (soipRecorder)
		{
			if (soipRecorder->isRecording())
			{
				soipRecorder->stopRecording();
			}
		}

}

int ssoipTX::GetDeviceID(int index)
{
	int ID = -1;
	switch (index)
	{
	case 0:
		ID = atoi(configdata["RECORDING-DEVICE-ID"].c_str());
		//ID =atoi(configdata["STETHOSCOPE-CHEST"].c_str());
		break;
	case 1:
		ID = atoi(configdata["STETHOSCOPE-LUNGS1"].c_str());
		break;
	case 2:
		ID = atoi(configdata["STETHOSCOPE-LUNGS2"].c_str());
		break;
	case 3:
		ID = atoi(configdata["STETHOSCOPE-LUNGS3"].c_str());
		break;
	case 4:
		ID = atoi(configdata["STETHOSCOPE-LUNGS4"].c_str());
		break;
	case 5:
		ID = atoi(configdata["STETHOSCOPE-LUNGS5"].c_str());
		break;
	case 6:
		ID = atoi(configdata["STETHOSCOPE-LUNGS6"].c_str());
		break;
	case 7:
		ID = atoi(configdata["STETHOSCOPE-LUNGS7"].c_str());
		break;
		case 8:
			ID = atoi(configdata["STETHOSCOPE-LUNGS8"].c_str());
			break;
		case 9:
			ID = atoi(configdata["STETHOSCOPE-LUNGS9"].c_str());
			break;
		case 10:
			ID = atoi(configdata["STETHOSCOPE-LUNGS10"].c_str());
			break;
		case 11:
			ID = atoi(configdata["STETHOSCOPE-LUNGS11"].c_str());
			break;
		case 12:
			ID = atoi(configdata["STETHOSCOPE-LUNGS12"].c_str());
			break;
		case 13:
			ID = atoi(configdata["STETHOSCOPE-LUNGS13"].c_str());
			break;
		case 14:
			ID = atoi(configdata["STETHOSCOPE-LUNGS14"].c_str());
			break;
		case 15:
			ID = atoi(configdata["STETHOSCOPE-LUNGS15"].c_str());
			break;
		case 16:
			ID = atoi(configdata["STETHOSCOPE-LUNGS16"].c_str());
			break;

	}
	return ID;
}

