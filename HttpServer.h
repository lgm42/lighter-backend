// HttpServer.h

#ifndef _WEBSERVER_h
#define _WEBSERVER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include <ESP8266WebServer.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ESP8266mDNS.h>
#include <ESP8266FtpServer.h>

#include "IComponent.h"

class HttpServer : public IComponent
{
  public:
	  HttpServer();
	  virtual ~HttpServer();

	virtual void setup(void);
	virtual void handle(void);

	String getContentType(String filename);

	ESP8266WebServer & webServer();

	NTPClient & timeClient();
private:
	ESP8266WebServer _webServer;
	FtpServer _ftpServer;
	WiFiUDP _ntpUDP;
	NTPClient _timeClient;

	bool handleFileRead(String path);

	void sendOk();
	void sendKo(const String & message);
	void sendOkAnswerWithParams(const String & params);

	static const String RestKeyword;
	static const String LedKeyword;
	static const String ModeKeyword;
	static const String StatusKeyword;
	static const String ParamKeyword;
	static const String ParamsKeyword;

	static const String HostnameKeyword;
	static const String FtpLoginKeyword;
	static const String FtpPasswdKeyword;
	static const String TimeOffsetKeyword;

	static const String TimeStartKeyword;
	static const String TimeEndKeyword;
	static const String StartDurationKeyword;
	static const String EndDurationKeyword;
	static const String ValueWhenLowKeyword;
	static const String ValueWhenHighKeyword;

};

#if !defined(NO_GLOBAL_INSTANCES)
extern HttpServer HTTPServer;
#endif

#endif

