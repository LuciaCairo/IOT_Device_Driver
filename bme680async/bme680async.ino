#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL2tTzPkrqk"
#define BLYNK_TEMPLATE_NAME "Pines de Colors"
#define BLYNK_AUTH_TOKEN "Ryyc2jucHGlQaGnzBrUk0lK-Oco-Pdva"

#include "Zanshin_BME680.h"  // Include the BME680 Sensor library
#include <WiFi.h>
#include <HTTPClient.h> // para telegram
#include <BlynkSimpleEsp32.h>


const uint32_t SERIAL_SPEED{115200}; 
const String telegramToken = "7147191029:AAGREYneL5uhnpeu1JXxo2arBKRF3z2PDZ4";
const String chatID = "1557986943";
const String apiUrl = "http://10.65.2.194:3000" // Ro's IP

BME680_Class BME680; 
float altitude(const int32_t press, const float seaLevel = 1013.25);
float altitude(const int32_t press, const float seaLevel) {
  /*!
  @brief     This converts a pressure measurement into a height in meters
  @details   The corrected sea-level pressure can be passed into the function if it is known,
             otherwise the standard atmospheric pressure of 1013.25hPa is used (see
             https://en.wikipedia.org/wiki/Atmospheric_pressure) for details.
  @param[in] press    Pressure reading from BME680
  @param[in] seaLevel Sea-Level pressure in millibars
  @return    floating point altitude in meters.
  */
  static float Altitude;
  Altitude =
      44330.0 * (1.0 - pow(((float)press / 100.0) / seaLevel, 0.1903));  // Convert into meters
  return (Altitude);
}  // of method altitude()

const char* ssid = "LABORATORIO-B"; //"A15 de Luci";
const char* password = ""; //"luciicai";

// Declaring a global variabl for sensor data
double sensorVal, sensorVal1, sensorVal2, sensorVal3 ; 

// This function creates the timer object. It's part of Blynk library 
BlynkTimer timer; 

void reportDataTimer() {
  // This function describes what will happen with each timer tick
  // e.g. writing sensor value to datastream V5
  
  Blynk.virtualWrite(V2, sensorVal);  
  Blynk.virtualWrite(V0, sensorVal1);  
  Blynk.virtualWrite(V3, sensorVal2);  
  Blynk.virtualWrite(V1, sensorVal3);

  // Send data to blockchain API
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = apiUrl + "/api";
    String payload = '{\"temperature\": ' + String(sensorVal) + ', \"humidity\": ' + String(sensorVal1) + ', \"pressure\": ' + String(sensorVal2) + ', \"airQuality\": ' + String(sensorVal3) + '}';
    Serial.println("Payload: " + payload);
    http.begin(url);
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST(payload);
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(response);
    } else {
      Serial.println("Error in HTTP request");
    }    
    http.end();
  } else {
    Serial.println("Error in WiFi connection");
  }
}

void getDataTimer() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = apiUrl + "/api";
    http.begin(url);
    int httpResponseCode = http.GET();
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(response);
    } else {
      Serial.println("Error in HTTP request");
    }
    if (httpResponseCode == 200) {
      // Parse JSON
      DynamicJsonDocument doc(1024);
      deserializeJson(doc, http.getString());
      bool temperature = doc["needsHeat"];

      if (needsHeat) {
        digitalWrite(2, HIGH);
        digitalWrite(4, LOW);
      } else {
        digitalWrite(4, HIGH);
        digitalWrite(2, LOW);
      } 
    }
    http.end();
  } else {
    Serial.println("Error in WiFi connection");
  }
}


// Esta función se ejecuta cada vez que la app Blynk envía un valor al pin virtual
BLYNK_WRITE(V4) {
  pinMode(4, OUTPUT); // led rojo 
  pinMode(2, OUTPUT); // led verde
  int valorRecibido = param.asInt();
  

  if (valorRecibido  == 0) { // La temperatura esta por arriba de 30 grados      
    Serial.print(F("Apagando estufa ..."));
    //digitalWrite(4, HIGH);
    digitalWrite(4, LOW);
  }   
  if (valorRecibido  == 1) { // La temperatura esta debajo de 28 grados    
    Serial.print(F("Prendiendo estufa ..."));
    digitalWrite(4, HIGH);
    //digitalWrite(4, LOW);
  }  
   
}


void setup() {

  pinMode(4, OUTPUT); // led salida de rele
  //digitalWrite(8, HIGH);

  // Para blynk --------------------------------------
  Serial.begin(115200);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password );
  timer.setInterval(30000L, reportDataTimer); 
  timer.setInterval(50000L, getDataTimer); 


  // ------------------------------------------------

  pinMode(5, OUTPUT); // led rojo 
  pinMode(19, OUTPUT); // led verde
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
    digitalWrite(5, HIGH);    
  }
  Serial.println("Connected to WiFi");         
  digitalWrite(5, LOW);    
  digitalWrite(19, HIGH);    

  /*!
  @brief    Arduino method called once at startup to initialize the system
  @details  This is an Arduino IDE method which is called first upon boot or restart. It is only
            called one time and then control goes to the main "loop()" method, from which
            control never returns
  @return   void
  */
  Serial.begin(SERIAL_SPEED);  // Start serial port at Baud rate
#ifdef _AVR_ATmega32U4_      // If this is a 32U4 processor, then wait 3 seconds to init USB port
  delay(3000);
#endif
  Serial.print(F("Starting I2CDemo example program for BME680\n"));
  Serial.print(F("- Initializing BME680 sensor\n"));
  while (!BME680.begin(I2C_STANDARD_MODE)) {  // Start BME680 using I2C, use first device found
    Serial.print(F("-  Unable to find BME680. Trying again in 5 seconds.\n"));
    delay(5000);
  }  // of loop until device is located
  Serial.print(F("- Setting 16x oversampling for all sensors\n"));
  BME680.setOversampling(TemperatureSensor, Oversample16);  // Use enumerated type values
  BME680.setOversampling(HumiditySensor, Oversample16);     // Use enumerated type values
  BME680.setOversampling(PressureSensor, Oversample16);     // Use enumerated type values
  Serial.print(F("- Setting IIR filter to a value of 4 samples\n"));
  BME680.setIIRFilter(IIR4);  // Use enumerated type values
  Serial.print(F("- Setting gas measurement to 320\xC2\xB0\x43 for 150ms\n"));  // " C" symbols
  BME680.setGas(320, 150);  // 320 c for 150 milliseconds
}  // of method setup()
void loop() {

  /*!
  @brief    Arduino method for the main program loop
  @details  This is the main program for the Arduino IDE, it is an infinite loop and keeps on
            repeating. The "sprintf()" function is to pretty-print the values, since floating
            point is not supported on the Arduino, split the values into those before and those
            after the decimal point.
  @return   void
  */
  static int32_t  temp, humidity, pressure, gas;  // BME readings
  static char     buf[16];                        // sprintf text buffer
  static float    alt;                            // Temporary variable
  static uint16_t loopCounter = 0;                // Display iterations
  if (loopCounter % 25 == 0) {                    // Show header @25 loops
    Serial.print(F("\nLoop Temp\xC2\xB0\x43 Humid% Press hPa   Alt m Air m"));
    Serial.print(F("\xE2\x84\xA6\n==== ====== ====== ========= ======= ======\n"));  // " C" symbol
  }                                                     // if-then time to show headers
  BME680.getSensorData(temp, humidity, pressure, gas);  // Get readings
  if (loopCounter++ != 0) {                             // Ignore first reading, might be incorrect
    sprintf(buf, "%4d %3d.%02d", (loopCounter - 1) % 9999,  // Clamp to 9999,
            (int8_t)(temp / 100), (uint8_t)(temp % 100));   // Temp in decidegrees
    Serial.print(buf);
    sprintf(buf, "%3d.%03d", (int8_t)(humidity / 1000),
            (uint16_t)(humidity % 1000));  // Humidity milli-pct
    Serial.print(buf);
    sprintf(buf, "%7d.%02d", (int16_t)(pressure / 100),
            (uint8_t)(pressure % 100));  // Pressure Pascals
    Serial.print(buf);
    alt = altitude(pressure);                                                // temp altitude
    sprintf(buf, "%5d.%02d", (int16_t)(alt), ((uint8_t)(alt * 100) % 100));  // Altitude meters
    Serial.print(buf);
    sprintf(buf, "%4d.%02d\n", (int16_t)(gas / 100), (uint8_t)(gas % 100));  // Resistance milliohms
    Serial.print(buf);
    delay(30000);  // Wait 30s
  }                // of ignore first reading

  // BYNKKK ----------------------------
  sensorVal = double(int(temp) / 100.0); 
  sensorVal1 = double(int(humidity) / 1000.0); 
  sensorVal2 = double(int(pressure) / 100.0); 
  sensorVal3 = double(int(gas) / 100.0); 

  
  // Runs all Blynk stuff
  Blynk.run(); 
  
  // runs BlynkTimer
  timer.run(); 
  // ---------------------------------
  

}  // of method loop()