#include <ESP8266WiFi.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_AHTX0.h> 
#include <PubSubClient.h>
#include <BH1750.h>
#include <ScioSense_ENS160.h>
#include <WiFiManager.h>  

float temperatureC;
int temperatureF;
float pression;
float pressionSeaLevel;
int humidite;
int offset_temp = -2.5;
int offset_hum = 0;
unsigned long previousMillisMQTT = 0;
unsigned long previousMillisDB = 0;
float lightIntensity;
float co2_ppm;
float tvoc_ppb;

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme;
Adafruit_AHTX0 aht; 
BH1750 lightMeter;
ScioSense_ENS160 ens160(ENS160_I2CADDR_1);

const char* latitude = "YOUR_LATITUDE";                        // 1. Latitude
const char* longitude = "YOUR_LONGITUDE";                      // 2. Longitude 

const char* call = "YOUR_CALL";                                // 3. Callsign without SSID
const char* APRScall = "YOUR_CALL-13";                         // 4. Callsign with SSID
const char* APRSpassword = "YOUR_PASSWORD";                    // 5. APRS password
const char* host = "euro.aprs2.net";                           // 6. APRS server
int port = 14580;

const char* dbServerIP = "YOUR_SERVER";                        // 7. DB server
const int dbServerPort = YOUR_PORT;                            // 8. DB port
const char* dbScriptPath = "/wx_station.php";                  // 9. DB script path

const char* mqttServer = "YOUR_SERVER";                        // 10. MQTT server
const int mqttPort = YOUR_PORT;                                // 11. MQTT port
const char* mqttTopic = "YOU_TOPIC";                           // 12. MQTT topic

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

    // WiFiManager
    WiFiManager wifiManager;

    // Uncomment to reset saved WiFi settings
    // wifiManager.resetSettings();

    if (!wifiManager.autoConnect("WX-Station-AP")) {
        Serial.println("Failed to connect to WiFi, resetting...");
        delay(3000);
        ESP.restart();
    }

    Serial.println("Connected to WiFi!");
    Serial.print("IP address is: ");
    Serial.println(WiFi.localIP());

    bool status = bme.begin(0x76);
    if (!status) {
        Serial.println("Could not find a valid BME280 sensor!");
        while (1);
    }

    // Initialize AHT21 sensor
    if (!aht.begin()) {
        Serial.println("Could not find a valid AHT21 sensor!");
        while (1);
    }

    // Initialize BH1750 sensor
    Wire.begin();
    lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE);

    // Initialize ENS160 sensor
    ens160.begin();
    if (!ens160.available()) {
        Serial.println("Could not find a valid ENS160 sensor!");
        while (1);
    }
    
    // Set ENS160 to standard operating mode
    ens160.setMode(ENS160_OPMODE_STD);

    BME280_Read();
    AHT21_Read();
    lightIntensity = lightMeter.readLightLevel(); 
    ENS160_read(); 

    temperatureF = (temperatureC * 1.8) + 32;

    APRS_Send();
    sendDataToDB();

    mqttClient.setServer(mqttServer, mqttPort);
    connectToMQTT();
}

void loop() {
    BME280_Read();
    AHT21_Read(); 
    lightIntensity = lightMeter.readLightLevel(); 
    ENS160_read(); 

    unsigned long currentMillis = millis();

    // Sending data to MQTT every minute
    if (currentMillis - previousMillisMQTT >= 60000) {
        previousMillisMQTT = currentMillis;
        publishToMQTT();
    }

    // Sending data to server and APRS every 10 minutes
    if (currentMillis - previousMillisDB >= 600000) {
        previousMillisDB = currentMillis;
        APRS_Send();
        sendDataToDB();
    }

    mqttClient.loop();
}

void AHT21_Read() {
    sensors_event_t humidity, temp;
    aht.getEvent(&humidity, &temp); 
    temperatureC = temp.temperature + offset_temp;
    humidite = humidity.relative_humidity + offset_hum;
}

void ENS160_read() {
    if (ens160.available()) {
        ens160.set_envdata(temperatureC, humidite); 
        ens160.measure(true);
        co2_ppm = ens160.geteCO2();  
        tvoc_ppb = ens160.getTVOC(); 
    }
}

void APRS_Send() {
    WiFiClient client;
    Serial.printf("\n[Connecting to %s:%d....\n", host, port);
    if (client.connect(host, port)) {
        Serial.println("[Connected]");
        Serial.println("Logging in to the APRS server");
        char login[60];
        char sentence[200]; 

        sprintf(login, "user %s pass %s vers WX_Station 0.1 filter m/1", APRScall, APRSpassword);
        sprintf(sentence, "%s>APRS,TCPIP*:@090247z%s/%s_.../...t%03dh%02db%05dL%d WX-Station https://www.ok1kky.cz", APRScall, latitude, longitude, temperatureF, humidite, (int)(pressionSeaLevel * 10), (int)lightIntensity);
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
        String url = String(dbScriptPath) + "?temperature=" + String(temperatureC) + "&pressure=" + String(pressionSeaLevel) + "&humidity=" + String(humidite) + "&light=" + String(lightIntensity) + "&co2_ppm=" + String(co2_ppm) + "&tvoc_ppb=" + String(tvoc_ppb); 
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
    float altitude = 230;                                      // 13. Your altitude
    pression = bme.readPressure();
    pressionSeaLevel = seaLevelPressure(pression, temperatureC, altitude) / 100.0F;
}

void connectToMQTT() {
    Serial.print("Connecting to an MQTT server...");
    while (!mqttClient.connected()) {
        if (mqttClient.connect("ESP8266Client")) {
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

    char msg[150];
    sprintf(msg, "Temp: %06.1f C, Humi: %06.1f %%, Pres: %06.1f hPa, Ligh: %06.1f W/m2, eCO2: %06.1f ppm, TVOC: %06.1f ppb", temperatureC, (float)humidite, pressionSeaLevel, (float)lightIntensity, co2_ppm, tvoc_ppb);
    mqttClient.publish(mqttTopic, msg);
    Serial.println("Sent to MQTT: ");
    Serial.println(msg);
}
