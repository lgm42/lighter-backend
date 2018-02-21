// JsonConfiguration.h

#ifndef _JSONCONFIGURATION_h
#define _JSONCONFIGURATION_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

class JsonConfiguration 
{
public:
	JsonConfiguration();
	virtual ~JsonConfiguration();

	virtual void setup();

	bool saveConfig();

	void restoreDefault();

	String _hostname;
	String _ftpLogin;
	String _ftpPasswd;
	int _timeOffset;

	int _timeStart;
	int _timeEnd;
	int _startDuration;
	int _endDuration;
	
	bool _automaticMode;

	String toJson();
private:

};

#if !defined(NO_GLOBAL_INSTANCES)
extern JsonConfiguration Configuration;
#endif

#endif

