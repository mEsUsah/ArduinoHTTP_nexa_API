# Arduino NEXA IOT Web API
Use Arduino to controll NEXA 433MHz switches via an WEB API.

This build is based on the Arduino Uno with a ethernet shield, and a cheap 433Hz transmitter module.

### Prerequisites
* Arduino IDE with ethernet library (comes preinstalled) 
* Download the [NexaCtrl](https://github.com/calle-gunnarsson/NexaCtrl) library. 
* Install it in the Arduino IDE via Sketch => Include Library => Add -ZIP library...

### Electronics
![433Mzh Transmitter](https://github.com/mEsUsah/ArduinoHTTP_nexa_API/blob/master/README-files/433MHz-RF-Wireless-Transmitter-Pinout.png?raw=true)
* Cheap 433MHz transmitter from China 
* A 17cm ghetto style antenna (1/4 wavelenght antenna) soldered to the antenna pad on the module..
* W5100 based ethernet chield or module.
* Any "normal" arduino like the Uno, Mega, Mini or Nano.
* 2pcs of status LED's.
* 2pcs of correct resistors for the LED's.

# How the sketch works.
Define the api key and network configurations to fit your project. I use static IP on all my API deviced, since i want to find them at the same spot every time.
Send a HTTP request to the arduino with the correct API key, what remote you want the arduino to act like, what device id you want it to controll and what you want to do with it.
A 433Mzh packet sniffer is useful to obtain these ID's.

### Example HTTP request
```
http://10.22.22.60?apikey=s3cr37&light=1&controller=13112270&device=1
```
* apikey = s3cr37
* light = 1
* controller = 13112270
* device = 1

If the apikey is correct - this will give the command over 433Mzh HomeEasy protocol that remote controll with the ID of 13112270 want the device with the ID of 1 to go high/on/max. 