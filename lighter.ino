/*
To upload through terminal you can use: curl -F "image=@firmware.bin" esp8266-webupdate.local/update
*/

#include "JsonConfiguration.h"
#include "HttpServer.h"
#include "Constants.h"
#include "MainApplication.h"

MainApplication app;

void setup(void) 
{
  Serial.println("setup ...");
	app.setup();
  Serial.println("setup completed.");
}

void loop(void) 
{
	app.handle();
}
