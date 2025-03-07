#include <ESP8266WiFi.h>
#include <Wire.h>
#include <SPI.h>
#include <BH1750.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_AHTX0.h>
#include <ScioSense_ENS160.h>
#include <PubSubClient.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>  

float temperatureC;
int temperatureF;
float pression;
float pressionSeaLevel;
float humidite;
int offset_temp = 0;
int offset_hum = 0;
unsigned long previousMillisMQTT = 0;
unsigned long previousMillisDB = 0;
unsigned long previousMillisReset = 0;  
float temperatureAhtC;
int temperatureAhtF;
float humiditeAht;
float lightIntensity; 
float lightIntensityWm2;
float co2_ppm;       
float tvoc_ppb;  

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme;
BH1750 lightMeter; 
Adafruit_AHTX0 aht;
ScioSense_ENS160 ens160(ENS160_I2CADDR_1); 

const char* latitude = "YOUR_LATITUDE";                        // 1. Latitude
const char* longitude = "YOUR_LONGITUDE";                      // 2. Longitude 

const char* APRScall = "YOUR_CALL";                            // 3. APRS callsign
const char* APRSpassword = "YOUR_PASSWORD";                    // 4. APRS password
const char* host = "euro.aprs2.net";                           // 5. APRS server
int port = 14580;                                              // 6. APRS port

const char* dbServerIP = "YOUR_SERVER";                        // 7. DB server
const int dbServerPort = 80;                                   // 8. DB port
const char* dbScriptPath = "/wx_station.php";                  // 9. DB script path
const char* tableName = "wx_holesovice";                       // 10 DB table name

const char* mqttServer = "YOUR_SERVER";                        // 11. MQTT server
const int mqttPort = YOUR_PORT;                                // 12. MQTT port
const char* mqttTopic = "YOU_TOPIC";                           // 13. MQTT topic

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

float seaLevelPressure(float pressure, float temperature, float altitude) {
    const float L = 0.0065;
    const float T0 = 273.15;
    float tempKelvin = temperature + T0;
    float seaLevelPressure = pressure * pow((1 - ((L * altitude) / (tempKelvin + (L * altitude)))), -5.257);
    return seaLevelPressure;
}

void setup() {
    Serial.begin(115200);
    Serial.println();
    Serial.println("APRS WX station - by OK1KKY");

    Wire.begin(D2, D1); // SDA, SCL
    lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE);

    if (!aht.begin()) {
      Serial.println("Could not find a valid AHT21 sensor!");
      while (1) delay(10); // Nekonečný cyklus, pokud senzor není detekován
    }

    bool status = bme.begin(0x76);
    if (!status) {
        Serial.println("Could not find a valid BME280 sensor!");
        while (1);
    }

    // Initialize ENS160 sensor
    ens160.begin();
    if (!ens160.available()) {
        Serial.println("Could not find a valid ENS160 sensor!");
        while (1);
    }
    
    // Set ENS160 to standard operating mode
    ens160.setMode(ENS160_OPMODE_STD);

    WiFiManager wifiManager;
    wifiManager.autoConnect("WX-Station-AP"); 

    Serial.println("Connected to WiFi");
    Serial.print("IP address is: ");
    Serial.println(WiFi.localIP());

    BME280_Read();
    lightIntensity = lightMeter.readLightLevel();
    lightIntensityWm2 = lightIntensity * 0.0079;
    ahtRead();
    ENS160_read();

    temperatureF = (temperatureC * 1.8) + 32;
    temperatureAhtF = (temperatureAhtC * 1.8) + 32;
    APRS_Send();
    sendDataToDB();

    mqttClient.setServer(mqttServer, mqttPort);

    connectToMQTT();
}

void loop() {
    BME280_Read();
    lightIntensity = lightMeter.readLightLevel();
    lightIntensityWm2 = lightIntensity * 0.0079;
    ahtRead();
    ENS160_read();

    unsigned long currentMillis = millis();

    // Sending data to MQTT every minute
    if (currentMillis - previousMillisMQTT >= 60000) {
        previousMillisMQTT = currentMillis;

        publishToMQTT();
    }

    // Sending data to server and APRS every 10 minutes
    if (currentMillis - previousMillisDB >= 600000) { // 600000 ms = 10 minutes
        previousMillisDB = currentMillis;
        APRS_Send();
        sendDataToDB();
    }

    // Reset ESP každých 12 hodin (43 200 000 ms)
    if (currentMillis - previousMillisReset >= 43200000 || currentMillis < previousMillisReset) {
        previousMillisReset = currentMillis;
        Serial.println("12 hours passed, restarting ESP...");
        ESP.restart();
    }

    mqttClient.loop();
}

void ahtRead() {
  sensors_event_t humidity, temp;

  // Získání dat ze senzoru
  aht.getEvent(&humidity, &temp);

  // Uložení hodnot do globálních proměnných
  temperatureAhtC = temp.temperature;
  temperatureAhtF = (temperatureAhtC * 1.8) + 32;
  humiditeAht = humidity.relative_humidity;
}

void ENS160_read() {
    if (ens160.available()) {
        ens160.set_envdata(temperatureC, humidite); // Set environment data for ENS160
        ens160.measure(true);
        co2_ppm = ens160.geteCO2();  // Corrected method to get eCO2 (equivalent CO2 in ppm)
        tvoc_ppb = ens160.getTVOC(); // TVOC value from ENS160 (total volatile organic compounds, in ppb)
    }
}

void APRS_Send() {
    WiFiClient client;
    Serial.printf("\n[Connecting to %s:%d....\n", host, port);
    if (client.connect(host, port)) {
        Serial.println("[Connected]");
        Serial.println("Logging in to the APRS server");
        char login[60];
        char sentence[150];

        BME280_Read();
        temperatureF = (temperatureC * 1.8) + 32;
        temperatureAhtF = (temperatureAhtC * 1.8) + 32;

        sprintf(login, "user %s pass %s vers WX_Station 0.1 filter m/1", APRScall, APRSpassword);
        sprintf(sentence, "%s>APRS,TCPIP*:@090247z%s/%s_.../...t%03dh%02db%05dL%03d WX-Station https://www.ok1kky.cz", APRScall, latitude, longitude, temperatureAhtF, (int)humiditeAht, (int)(pressionSeaLevel * 10), (int)lightIntensityWm2);
        client.println(login);
        Serial.println(sentence);
        Serial.println("[Response:]");

        while (client.connected() || client.available()) {
            if (client.available()) {
                String line = client.readStringUntil('\n');
                Serial.println(line);
                delay(3000);
                client.println(sentence);
                Serial.println(sentence);
                delay(1000);
                client.stop();
            }
        }
        client.stop();
        Serial.println("\n[Disconnected]");
    } else {
        Serial.println("[Connection failed!]");
        client.stop();
    }
}

void sendDataToDB() {
    WiFiClient client;
    if (client.connect(dbServerIP, dbServerPort)) {
        String url = String(dbScriptPath) + "?station=" + String(tableName) +
                     "&temperature=" + String(temperatureAhtC) +
                     "&pressure=" + String(pressionSeaLevel) +
                     "&humidity=" + String(humiditeAht) +
                     "&light=" + String(lightIntensityWm2) +
                     "&co2=" + String(co2_ppm) +
                     "&tvoc=" + String(tvoc_ppb);
                     
        client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                     "Host: " + dbServerIP + "\r\n" +
                     "Connection: close\r\n\r\n");
        delay(1000);
        while (client.available()) {
            String line = client.readStringUntil('\n');
            Serial.println(line);
        }
        client.stop();
    } else {
        Serial.println("[Failed to connect to DB server]");
    }
}

void BME280_Read() {
    float altitude = 230;
    temperatureC = bme.readTemperature() + offset_temp;
    temperatureF = (temperatureC * 1.8) + 32;
    pression = bme.readPressure();
    humidite = bme.readHumidity() + offset_hum;

    pressionSeaLevel = seaLevelPressure(pression, temperatureC, altitude) / 100.0F;
}

void connectToMQTT() {
    Serial.print("Connecting to an MQTT server...");
    while (!mqttClient.connected()) {
        if (mqttClient.connect(APRScall)) {
            Serial.println(" connected to the MQTT broker");
        } else {
            Serial.print(" failed, rc=");
            Serial.print(mqttClient.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
}

void publishToMQTT() {
    if (!mqttClient.connected()) {
        connectToMQTT();
    }

    StaticJsonDocument<200> jsonDoc;
    jsonDoc["temperature"] = String(temperatureAhtC).toFloat();
    jsonDoc["humidity"] = String(humiditeAht, 2).toFloat();
    jsonDoc["pressure"] = String(pressionSeaLevel, 2).toFloat();
    jsonDoc["light"] = String(lightIntensityWm2, 2).toFloat();
    jsonDoc["co2"] = String(co2_ppm, 2).toFloat();
    jsonDoc["tvoc"] = String(tvoc_ppb, 2).toFloat();

    char jsonBuffer[256];
    serializeJson(jsonDoc, jsonBuffer);

    // Odeslání JSON zprávy do MQTT
    if (mqttClient.publish(mqttTopic, jsonBuffer)) {
        Serial.print("MQTT: ");
        Serial.println(jsonBuffer);
    } else {
        Serial.println("Failed to send to MQTT");
    }
}

