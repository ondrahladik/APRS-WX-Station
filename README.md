# APRS-WX-Station

In this project for sending data to the APRS.IS network, the code base is from [APRS-WX-ESP8266](https://github.com/9A4AM/APRS-WX-ESP8266) from 9A4AM  

The WX station sends data to three different servers.
- APRS.is
- DB server
- MQTT broker

![diagram](https://github.com/ondrahladik/APRS-WX-Station/blob/main/Diagram.png)  

The entire project is divided into two versions:

### Version A
This is the version of the basic WX station, where only the BME280 module is connected to the ESP8266, which sends data to all three servers.

- BME280 - all acquired data is sent to all three servers

### Version B
This version is an extended WX station where BME280, BH1750 and ENS160+AHT21 modules are connected to ESP8266. Take this version more as a demonstration of how the WX station can be expanded with other modules, but it is still a fully functional version.

- BME280 - only the pressure value is sent.
- BH1750 - the value converted to W/m2 is sent.
- AHT21 - the value of temperature and humidity is sent.
- ENS160 - the CO2 value is sent. (It is not sent to the APRS.is network)

# Installation
Be the first to check out this project's [wiki](https://github.com/ondrahladik/APRS-WX-Station/wiki) page.  

Use the Arduino IDE to upload the codes (APRS-WX-Station.ino) to your ESP8266. Before compiling and uploading, make sure you have all the necessary libraries installed.  

After successfully uploading and connecting your WX station's power, ESP will start broadcasting WiFI named WX-Station-AP, no password is needed to connect.   

When you connect to the AP, just open a browser and enter the IP address `192.168.4.1`, on which the WiFi manager runs, thanks to which you connect the WX station to the internet.  

And now, if you have correctly performed the necessary steps to configure the station, then you are all done. Congratulations