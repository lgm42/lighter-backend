// 
// 
// 
#include <FS.h>
#include <ArduinoJson.h>

#include "JsonConfiguration.h"

JsonConfiguration::JsonConfiguration()
{
}

JsonConfiguration::~JsonConfiguration()
{
}

void JsonConfiguration::setup(void)
{
	File configFile = SPIFFS.open("/config.json", "r");
	if (!configFile) 
	{
		Serial.println("Failed to open config file");
		restoreDefault();
	}

	size_t size = configFile.size();
	if (size > 1024) 
	{
		Serial.println("Config file size is too large");
		restoreDefault();
	}
	
	// Allocate a buffer to store contents of the file.
	std::unique_ptr<char[]> buf(new char[size]);

	// We don't use String here because ArduinoJson library requires the input
	// buffer to be mutable. If you don't use ArduinoJson, you may as well
	// use configFile.readString instead.
	configFile.readBytes(buf.get(), size);

	StaticJsonBuffer<1000> jsonBuffer;
	JsonObject& json = jsonBuffer.parseObject(buf.get());

	if (!json.success()) 
	{
		Serial.println("Failed to parse config file");
		restoreDefault();
	}
	
	_hostname = (const char*)json["hostname"];
	_ftpLogin = (const char*)json["ftp.login"];
	_ftpPasswd = (const char*)json["ftp.passwd"];
	_automaticMode = json["automaticMode"];
	_timeOffset = json["timeOffset"];

	_timeStart = json["timeStart"];
	_timeEnd = json["timeEnd"];
	_startDuration = json["startDuration"];
	_endDuration = json["endDuration"];

	if ((_hostname.length() == 0) || (_ftpLogin.length() == 0)
		|| (_ftpPasswd.length() == 0))
	{
		Serial.println("Invalid configuration values, restoring default values");
		restoreDefault();
	}

	Serial.printf("hostname : %s\n", _hostname.c_str());
	Serial.printf("ftpLogin : %s\n", _ftpLogin.c_str());
	Serial.printf("ftpPasswd : %s\n", _ftpPasswd.c_str());
	Serial.printf("automaticMode : %d\n", _automaticMode);
}

bool JsonConfiguration::saveConfig()
{
	StaticJsonBuffer<1000> jsonBuffer;
	JsonObject& json = jsonBuffer.createObject();
	json["hostname"] = _hostname;
	json["ftp.login"] = _ftpLogin;
	json["ftp.passwd"] = _ftpPasswd;
	json["automaticMode"] = _automaticMode;
	json["timeOffset"] = _timeOffset;
	
	json["timeStart"]		 = _timeStart 		;				 
	json["timeEnd"]			 = _timeEnd 		;				 
	json["startDuration"]	 = 	_startDuration ;				 
	json["endDuration"]		 = _endDuration 	;				 
	
	File configFile = SPIFFS.open("/config.json", "w");
	if (!configFile) 
	{
		Serial.println("Failed to open config file for writing");
		return false;
	}

	json.printTo(configFile);
	
	return true;
}

void JsonConfiguration::restoreDefault()
{
	_hostname = "lighter";
	_ftpLogin = "lighter";
	_ftpPasswd = "lighter";
	_automaticMode = true;
	_timeOffset = 3600;

	_timeStart = 8 * 3600; //8h00
	_timeEnd = 18 * 3600; //18h00
	_startDuration = 15 * 60; //15 min
	_endDuration = 15 * 60; //15 min

	saveConfig();
	Serial.println("configuration restored.");
}

String JsonConfiguration::toJson()
{
	String response = "{\"hostname\":\"" + Configuration._hostname + "\", 					\
	\"ftp-login\":\"" + Configuration._ftpLogin + "\",					\
	\"ftp-passwd\":\"" + Configuration._ftpPasswd + "\",					\
	\"time-offset\":\"" + Configuration._timeOffset + "\",					\
	\"time-start\":\"" + Configuration._timeStart + "\",					\
	\"time-end\":\"" + Configuration._timeEnd + "\",					\
	\"start-duration\":\"" + Configuration._startDuration + "\",					\
	\"end-duration\":\"" + Configuration._endDuration + "\"}";
	return response;
}

#if !defined(NO_GLOBAL_INSTANCES) 
JsonConfiguration Configuration;
#endif