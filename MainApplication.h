// MainApplication.h

#ifndef _MAINAPPLICATION_h
#define _MAINAPPLICATION_h

#include <ESP8266WiFi.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager
#include <WiFiClient.h>
#include <ESP8266mDNS.h>
#include <StreamString.h>
#include <Ticker.h>
#include <FS.h>


#include "HttpServer.h"
#include "JsonConfiguration.h"

#include "IComponent.h"

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

class MainApplication : public IComponent
{
public:
	MainApplication();
	virtual ~MainApplication();

	virtual void setup(void);
	virtual void handle(void);
private:
	Ticker _ticker;
	WiFiManager _wifiManager;

	bool runUpdate(Stream& in, uint32_t size, int command);

};

#endif

