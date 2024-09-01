# APRS-WX-Station

In this project for sending data to the APRS.IS network, the code base is from [APRS-WX-ESP8266](https://github.com/9A4AM/APRS-WX-ESP8266) from 9A4AM  

The WX station sends data to three different servers.
- APRS.is
- DB server
- MQTT broker

![diagram](https://github.com/ondrahladik/APRS-WX-Station/blob/main/Image/Diagram.png)  

The entire project is divided into two versions:

### Version A
This is the version of the basic WX station, where only the BME280 module is connected to the ESP8266, which sends data to all three servers.

### Version B