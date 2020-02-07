/**********************************************************************************************
    --- Arduino Ehternet HTTP webclient ---
    Written by Stanley Skarshaug, 06.02.2020 - Norway.
    Free to use - if credit is given to author.

    Fast and simple web server API that can be used to controll a system nexa 433MHz relay.
    Does include the HTTP parser to check for variables in the HTTP request.
    The API reqire that a APIkey is given as a variable. Else it will not act.
    Does respont to client with a JSON.

    For the NexaCtrl library. Check https://github.com/calle-gunnarsson/NexaCtrl

**********************************************************************************************/

#include<SPI.h>
#include<Ethernet.h>
#include<NexaCtrl.h>

unsigned long controller_id =  13112270; 		// Remote controller id.
unsigned int device 		= 1; 				// Nexa remot, button 2


const int httpLEDPin		= 4;				// LED indicator for http activity. Both as a server and a client
const int errorLEDPin		= 5;        		// LED indicator for any system error 
const int nexaTxPin 		= 9; 				// 433MHz Transmitter
NexaCtrl nexaCtrl(nexaTxPin);

byte mac[] = { 0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02 };	// Custom MAC-address. Can be pretty much anything...
IPAddress ip(10,22,22,60);								// Static IP Address
IPAddress myDns(8,8,8,8);								// DNS Server - dns.google 
IPAddress myGateway(10,22,22,1);
EthernetServer server(80);                              // Start server in port 80.

String apiKey = "s3cr37";								// Key to access the API. Must be provided in order to use API

String readString;
bool ledStatus = 0;
bool apiAuth = 0;
bool firstLine = true;

void setup(){
	pinMode(httpLEDPin, OUTPUT);
	pinMode(errorLEDPin, OUTPUT);

	Serial.begin(9600);
	Serial.println("OK - Serial communication is established ");

	Ethernet.begin(mac, ip, myDns, myGateway);
	server.begin();

	if(Ethernet.localIP()){
		Serial.print("Arduino IP address: ");
		Serial.println(Ethernet.localIP());
		Serial.println("OK - Ethernet system is ready...");
	} else {
		Serial.println("ERROR! - Failed to configure Ethernet using static IP");
		digitalWrite(errorLEDPin, HIGH); 										// turn ON Error LED indicator of DHCP setup failed.
		while (true); 															// Stop program forever.
	}

	Serial.println("OK - All systems active");
	Serial.println();
}

void loop() {
	runApi();
}

/***********************************************************************************************************************/
/*                    Recieve data from a web client, act on instructions                                              */
/***********************************************************************************************************************/

void runApi(){
	EthernetClient client = server.available();
	// If a client connects, run the API logic.
	if (client) {
		Serial.println("new client: " + String(client.remoteIP()));
		digitalWrite(httpLEDPin, HIGH);
		bool currentLineIsBlank = true;
		while (client.connected()) {
			if (client.available()) {
				char c = client.read();
				//Serial.write(c);

				//Read the first line of the get request to figure out what the client wants from the API.
				//Stop reading the line if it's more than 100 characters long, and it's no longer on the first line.
				if(readString.length()<100 && firstLine){
					readString += c;
					
					// This API only cares about the first line of the HTTP request.
					if(c == '\n'){
						firstLine = false;
						Serial.println(readString);

						// Check if the correct apikey us supplied
						// If ok, run commands.
						// If not set apiAuth flag low.
						if(parseRequest(readString, "apikey") == apiKey){
							apiAuth = 1;
							runCommands(readString);
						} else {
							apiAuth = 0;
						}
						// Reset the readString to be ready for a new connection.
						readString = "";
					}
				}

				// Wait for the get request header to finish. It will end with a empty line containing a newline character.
				// This API does not care about the body of the request
				// Then respond to client
				if (c == '\n' && currentLineIsBlank) {
					if(apiAuth){
						returnStatus(client);
					} else {
						returnForbidden(client);
					}
					break;
				}
				if (c == '\n') {
					currentLineIsBlank = true;
				}
				else if (c != '\r') {
					currentLineIsBlank = false;
				}
			}
		}
		//delay for stability.
		delay(1);
		client.stop();
		Serial.println("client disconnected");
		digitalWrite(httpLEDPin, LOW);
		firstLine = true;
		Serial.println("*******************************************************************************************");
		Serial.println();
	}
}


String parseRequest(String requestString, String variableName){ 
    if(requestString.indexOf(variableName)>0){
        unsigned int varStarts = readString.indexOf(variableName) + variableName.length()+1; 
		unsigned int varEnds;

		if(readString.indexOf("&", varStarts)>0) {				
			varEnds = readString.indexOf("&", varStarts);
		} else {
			varEnds = readString.indexOf(" ", varStarts);
		}

		return requestString.substring(varStarts, varEnds);
	} else {
		return "-1";
	}
}

void runCommands(String readString){
	if(parseRequest(readString, "light") == "1"){
		nexaCtrl.DeviceOn(controller_id, device);
	} else if(parseRequest(readString, "light") == "0"){
		nexaCtrl.DeviceOff(controller_id, device);
	}
}

void returnStatus(EthernetClient client){
	client.println("HTTP/1.1 200 OK");
	client.println("Content-Type: application/json;charset=utf-8");
	client.println("Server: Arduino");
	client.println("Connection: close");
	client.println();
	// HTTP Responese body - this is a JSON with status info
	client.print("{");
	client.print("\"IOT-Device\": \"Nexa IOT Switch\"");
	client.println("}");
	
	client.println();
}
void returnForbidden(EthernetClient client){
	client.println("HTTP/1.1 403 Forbidden");
	client.println("Content-Type: Text/html");
	client.println("Server: Arduino");
	client.println("Connection: close");
	client.println();
	// HTTP Responese body - this is a JSON with status info
	client.println("403 Forbidden");
	client.println();
}