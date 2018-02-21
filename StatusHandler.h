// StatusHandler.h

#ifndef _StatusHandler_h
#define _StatusHandler_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

class StatusHandler 
{
public:
	typedef enum  
	{
		kAutomatic = 0,
		kManual = 1,
		kForcedManual = 2
	} LightMode;

	StatusHandler();
	virtual ~StatusHandler();

	virtual void setup();

	String toJson();

	void currentValue(const int value);
	int currentValue() const;

	void mode(const StatusHandler::LightMode mode);
	StatusHandler::LightMode mode() const;

private:
	StatusHandler::LightMode _mode;
	int _currentValue;
};

#if !defined(NO_GLOBAL_INSTANCES)
extern StatusHandler Status;
#endif

#endif

