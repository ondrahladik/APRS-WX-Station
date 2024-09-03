# Version B  

This version is an extended WX station where BME280, BH1750 and ENS160+AHT21 modules are connected to ESP8266. Take this version more as a demonstration of how the WX station can be expanded with other modules, but it is still a fully functional version.

- BME280 - only the pressure value is sent.
- BH1750 - the value converted to W/m2 is sent.
- AHT21 - the value of temperature and humidity is sent.
- ENS160 - the CO2 value is sent. (It is not sent to the APRS.is network)

![version](https://github.com/ondrahladik/APRS-WX-Station/blob/main/VersionB/WiringDiagram.png)