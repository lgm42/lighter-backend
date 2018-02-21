// 
// 
// 
#include <FS.h>

#include "Constants.h"
#include "StatusHandler.h"
#include "HttpServer.h"
#include "JsonConfiguration.h"

const String HttpServer::RestKeyword = "/rest/";
const String HttpServer::LedKeyword = "led/";
const String HttpServer::ModeKeyword = "mode";
const String HttpServer::StatusKeyword = "status";
const String HttpServer::ParamsKeyword = "params";
const String HttpServer::ParamKeyword = "param/";

const String HttpServer::HostnameKeyword = "hostname";
const String HttpServer::FtpLoginKeyword = "ftp-login";
const String HttpServer::FtpPasswdKeyword = "ftp-passwd";
const String HttpServer::TimeOffsetKeyword = "time-offset";
const String HttpServer::TimeStartKeyword = "time-start";
const String HttpServer::TimeEndKeyword = "time-end";
const String HttpServer::StartDurationKeyword = "start-duration";
const String HttpServer::EndDurationKeyword = "end-duration";
const String HttpServer::ValueWhenLowKeyword = "value-low";
const String HttpServer::ValueWhenHighKeyword = "value-high";

HttpServer::HttpServer() : _timeClient(_ntpUDP)
{
}

HttpServer::~HttpServer()
{
}

String HttpServer::getContentType(String filename)
{
	if (_webServer.hasArg("download")) return "application/octet-stream";
	else if (filename.endsWith(".htm")) return "text/html";
	else if (filename.endsWith(".html")) return "text/html";
	else if (filename.endsWith(".css")) return "text/css";
	else if (filename.endsWith(".js")) return "application/javascript";
	else if (filename.endsWith(".png")) return "image/png";
	else if (filename.endsWith(".gif")) return "image/gif";
	else if (filename.endsWith(".jpg")) return "image/jpeg";
	else if (filename.endsWith(".ico")) return "image/x-icon";
	else if (filename.endsWith(".xml")) return "text/xml";
	else if (filename.endsWith(".pdf")) return "application/x-pdf";
	else if (filename.endsWith(".zip")) return "application/x-zip";
	else if (filename.endsWith(".gz")) return "application/x-gzip";
	return "text/plain";
}

bool HttpServer::handleFileRead(String path)
{
	HTTPServer.webServer().sendHeader("Access-Control-Allow-Origin", "*");
	//we look for /rest/
	if (path.startsWith(RestKeyword))
	{
		String instruction = path.substring(RestKeyword.length());
		if (instruction.startsWith(LedKeyword))
		{
			if (Status.mode() != StatusHandler::kAutomatic)
			{
				String value = instruction.substring(LedKeyword.length());
				int val = value.toInt();
				Status.currentValue(val);
				HTTPServer.sendOk();
			}
			else
			{
				HTTPServer.sendKo("Unable to drive led in automatic mode");
			}
		}
		else if (instruction.startsWith(StatusKeyword))
		{
			String value = instruction.substring(StatusKeyword.length());
			Serial.println("sending status");
			HTTPServer.sendOkAnswerWithParams(Status.toJson());
		}
		else if (instruction.startsWith(ModeKeyword))
		{
			String value = instruction.substring(ModeKeyword.length());
			if (value == "/auto")
			{
				Serial.println("going to automatic mode");
				Status.mode(StatusHandler::kAutomatic);
				HTTPServer.sendOk();
			}
			else if (value == "/manual")
			{
				Serial.println("going to manual mode");
				Status.mode(StatusHandler::kManual);
				HTTPServer.sendOk();
			}
			else if (value == "/forcedManual")
			{
				Serial.println("going to forced manual mode");
				Status.mode(StatusHandler::kForcedManual);
				HTTPServer.sendOk();
			}
			else
				HTTPServer.sendKo("unknown mode");
		}
		else if (instruction.startsWith(ParamsKeyword))
		{
			HTTPServer.sendOkAnswerWithParams(Configuration.toJson());
		}
		else if (instruction.startsWith(ParamKeyword))
		{
			int idx = instruction.indexOf('/', ParamKeyword.length());
			Serial.println("idx");
			Serial.println(idx);
			Serial.println("ParamKeyword.length()");
			Serial.println(ParamKeyword.length());
			Serial.println("instruction");
			Serial.println(instruction);

			String paramName = instruction.substring(ParamKeyword.length(), idx);
			
			String paramValue = instruction.substring(idx + 1);

			Serial.println("paramName");
			Serial.println(paramName);
			Serial.println("paramValue");
			Serial.println(paramValue);


			if (paramName == HostnameKeyword)
			{
				if (paramValue != "")
				{
					Configuration._hostname = paramValue;
					Configuration.saveConfig();
				}
				HTTPServer.sendOkAnswerWithParams("\"" + (String)Configuration._hostname + "\"");
			} 
			else if (paramName == FtpLoginKeyword)
			{
				if (paramValue != "")
				{
					Configuration._ftpLogin = paramValue;
					Configuration.saveConfig();
				}
				HTTPServer.sendOkAnswerWithParams("\"" + (String)Configuration._ftpLogin + "\"");
			} 
			else if (paramName == FtpPasswdKeyword)
			{
				if (paramValue != "")
				{
					Configuration._ftpPasswd = paramValue;
					Configuration.saveConfig();
				}
				HTTPServer.sendOkAnswerWithParams("\"" + (String)Configuration._ftpPasswd + "\"");
			} 
			else if (paramName == TimeOffsetKeyword)
			{
				if (paramValue != "")
				{
					Configuration._timeOffset = paramValue.toInt();
					Configuration.saveConfig();
					HTTPServer.timeClient().setTimeOffset(Configuration._timeOffset);
				}
				HTTPServer.sendOkAnswerWithParams("\"" + (String)Configuration._timeOffset + "\"");
			} 
			else if (paramName == TimeStartKeyword)
			{
				if (paramValue != "")
				{
					Configuration._timeStart = paramValue.toInt();;
					Configuration.saveConfig();
				}
				HTTPServer.sendOkAnswerWithParams("\"" + (String)Configuration._timeStart + "\"");
			} 
			else if (paramName == TimeEndKeyword)
			{
				if (paramValue != "")
				{
					Configuration._timeEnd = paramValue.toInt();;
					Configuration.saveConfig();
				}
				HTTPServer.sendOkAnswerWithParams("\"" + (String)Configuration._timeEnd + "\"");
			} 
			else if (paramName == StartDurationKeyword)
			{
				if (paramValue != "")
				{
					Configuration._startDuration = paramValue.toInt();;
					Configuration.saveConfig();
				}
				HTTPServer.sendOkAnswerWithParams("\"" + (String)Configuration._startDuration + "\"");
			} 
			else if (paramName == EndDurationKeyword)
			{
				if (paramValue != "")
				{
					Configuration._endDuration = paramValue.toInt();;
					Configuration.saveConfig();
				}
				HTTPServer.sendOkAnswerWithParams("\"" + (String)Configuration._endDuration + "\"");
			} 
			else
			{
				HTTPServer.sendKo("unknown param name");
			}
		}
		else
		{
			HTTPServer.sendKo("unknown rest command");
			
		}
		return true;
	}
	else
	{	
		if (path.endsWith("/")) path += "index.html";
		String contentType = HTTPServer.getContentType(path);
		String pathWithGz = path + ".gz";
		if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)) {
			if (SPIFFS.exists(pathWithGz))
				path += ".gz";
			File file = SPIFFS.open(path, "r");
			HTTPServer.webServer().streamFile(file, contentType);
			file.close();
			return true;
		}
	}

	return false;
}

void HttpServer::sendOk()
{
	_webServer.send(200, "application/json", "{\"result\":true}");
}

void HttpServer::sendOkAnswerWithParams(const String & params)
{
	String data("{\"result\":true, \"data\":");
	data += params;
	data += "}";
	_webServer.send(200, "application/json", data);
}

void HttpServer::sendKo(const String & message)
{
	String data("{\"result\":false, \"message\":\"");
	data += message;
	data += "\"}";
	_webServer.send(400, "application/json", data);
}

void HttpServer::setup(void)
{
	MDNS.begin(Configuration._hostname.c_str()); 
	MDNS.addService("http", "tcp", 80);

	_ftpServer.begin(Configuration._ftpLogin, Configuration._ftpPasswd);
	MDNS.addService("ftp", "tcp", 21);

	_webServer.begin();

	_timeClient.begin();
	_timeClient.setTimeOffset(Configuration._timeOffset);

	//called when the url is not defined here
	//use it to load content from SPIFFS
	_webServer.onNotFound([]() {
		if (!HTTPServer.handleFileRead(HTTPServer.webServer().uri()))
			HTTPServer.webServer().send(404, "text/plain", "FileNotFound");
	});
}

void HttpServer::handle(void)
{
	_webServer.handleClient();
	_ftpServer.handleFTP();
	_timeClient.update();
}

ESP8266WebServer & HttpServer::webServer() 
{
	return _webServer;
}

NTPClient & HttpServer::timeClient()
{
	return _timeClient;
}

#if !defined(NO_GLOBAL_INSTANCES) 
HttpServer HTTPServer;
#endif