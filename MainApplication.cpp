// 
// 
// 
#define DEBUG_UPDATER

#include "Constants.h"
#include "StatusHandler.h"

#include "MainApplication.h"

void tick()
{
	//every second we check time only in automatic mode
	if (Status.mode() == StatusHandler::kAutomatic)
	{
		int time = HTTPServer.timeClient().getEpochTime();
		//we get time from midnight
		time = time % 86400;
		/*Serial.println("time");
		Serial.println(time);*/
		
		int lowerBoundTime = 0;
		int higherBoundTime = 1;
		int lowerBoundValue = 0;
		int higherBoundValue = 1;

		int newValue = -1;

		if (time < Configuration._timeStart)
		{
			//off before light
			newValue = 0;
		}
		else if (time < Configuration._timeStart + Configuration._startDuration)
		{
			//sunrise
			lowerBoundTime = Configuration._timeStart;
			lowerBoundValue = 0;
			higherBoundTime = Configuration._timeStart + Configuration._startDuration;
			higherBoundValue = 100;
		}
		else if (time < Configuration._timeEnd)
		{
			//lighted
			newValue = 100;
		}
		else if (time < Configuration._timeEnd + Configuration._endDuration)
		{
			//sunset
			lowerBoundTime = Configuration._timeEnd;
			lowerBoundValue = 100;
			higherBoundTime = Configuration._timeEnd + Configuration._endDuration;
			higherBoundValue = 0;
		}
		else
		{
			//off after light
			newValue = 0;
		}

		if (newValue == -1)
		{
			//newValue hasn't been set 
			float implication = ((float)time - lowerBoundTime) / (higherBoundTime - lowerBoundTime);
			newValue = (int)(implication * higherBoundValue + (1.0 - implication) * lowerBoundValue);
		}

		/*Serial.println("new Value");
		Serial.println(newValue);*/
		Status.currentValue(newValue);
	}
}

//gets called when WiFiManager enters configuration mode
void configModeCallback(WiFiManager *myWiFiManager) {
	Serial.println("Entered config mode");
	Serial.println(WiFi.softAPIP());
	//if you used auto generated SSID, print it
	Serial.println(myWiFiManager->getConfigPortalSSID());
}

MainApplication::MainApplication()
{
	Serial.begin(115200);
}

MainApplication::~MainApplication()
{

}

void MainApplication::setup(void)
{
	delay(500);
	pinMode(Constants::LedPinNumber, OUTPUT);
	//_ticker.attach(0.2, tick);

	if (!SPIFFS.begin())
	{
		Serial.println("failed to initialize SPIFFS");
	}

	//we look if a file named lighter.ino.bin is on the spiffs
	//if yes we update the rom and delete the file
	String updateFilename = "/lighter.ino.bin";
	Serial.println("Checking update file " + updateFilename);
	if (SPIFFS.exists(updateFilename))
	{
		Serial.println("update file found");
		File updateFile = SPIFFS.open(updateFilename, "r");
		Serial.println("running update");
		runUpdate(updateFile, updateFile.size(), U_FLASH);	
		Serial.println("update finished");
		updateFile.close();
		Serial.println("removing update file");
		SPIFFS.remove(updateFilename);
		Serial.println("restarting");
		ESP.reset();
	}
	
	Configuration.setup();
	Status.setup();
	_wifiManager.setDebugOutput(true);
	//reset settings - for testing
	//_wifiManager.resetSettings();

	//set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
	_wifiManager.setAPCallback(configModeCallback);
	//fetches ssid and pass and tries to connect
	//if it does not connect it starts an access point with the specified name
	//here  "AutoConnectAP"
	//and goes into a blocking loop awaiting configuration
	if (!_wifiManager.autoConnect()) {
		Serial.println("failed to connect and hit timeout");
		//reset and try again, or maybe put it to deep sleep
		ESP.reset();
		delay(1000);
	}
	//if you get here you have connected to the WiFi
	Serial.println("connected.");

	HTTPServer.setup();
	
	analogWrite(Constants::LedPinNumber, 255);
	//Configuration.restoreDefault();

	//install timer
	_ticker.attach(1, tick);
}

void MainApplication::handle(void)
{
	HTTPServer.handle();
}

/**
 * write Update to flash
 * @param in Stream&
 * @param size uint32_t
 * @param md5 String
 * @return true if Update ok
 */
bool MainApplication::runUpdate(Stream& in, uint32_t size, int command)
{

    StreamString error;

    if(!Update.begin(size, command)) {
        Update.printError(error);
        error.trim(); // remove line ending
        Serial.printf("Update.begin failed! (%s)\n", error.c_str());
        return false;
    }

    if(Update.writeStream(in) != size) {
        Update.printError(error);
        error.trim(); // remove line ending
        Serial.printf("Update.writeStream failed! (%s)\n", error.c_str());
        return false;
    }

    if(!Update.end()) {
        Update.printError(error);
        error.trim(); // remove line ending
        Serial.printf("Update.end failed! (%s)\n", error.c_str());
        return false;
    }

    return true;
}