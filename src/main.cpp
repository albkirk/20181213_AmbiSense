/*
 * * Main File to trigger all C code using "arduino" style.
 * * It contains all necessary function calls on setup and loop functions
 * * HOW TO USE THIS TEMPLATE:
 * * -- Adjust the parameter below to your project.
 * *    Parameters on struct "config" will be store on memory.
 * *    Don't forget to customize the Mem read and write actions on "lib/project/custostore.h"
 * * -- Use the "// **** Normal code ..." zones to add you own definition, functions, setup and loop code
 * * -- You can also add you own MQTT actions on "lib/project/customqtt.h"
 * * -- Suggest to use "lib/project/" to add your own h files
 */
#include <ESP8266WiFi.h>

// HARWARE & SOFTWARE Version
#define BRANDName "AlBros_Team"                         // Hardware brand name
#define MODELName "AmbiSense"                           // Hardware model name
#define SWVer "14.01"                                   // Major.Minor Software version (use String 01.00 - 99.99 format !)

// Battery & ESP Voltage
#define BattPowered true                                // Is the device battery powered?
#define LDO_Corr float(0.6)                             // Battery Voltage [volt] corrective Factor due to LDO/Diode voltage drop
#define Batt_L_Thrs 15                                  // Battery level threshold [0%-100%] (before slepping forever).

// GPIO to Function Assignment
#define Using_ADC true                                  // will this device use the ADC? (if not it will measure the interval voltage)
#define LED_esp 2                                       // ESP Led is connected to GPIO 2
#define DHTPIN 5                                        // GPIO Connected to DHT22 Data PIN. -1 means NO DHT used!
#define BUZZER 0                                        // (Active) Buzzer


struct __attribute__((__packed__)) strConfig {
  char DeviceName[16];
  char Location[16];
  char ClientID[8];
  byte ONTime;
  byte SLEEPTime;
  boolean DEEPSLEEP;
  boolean LED;
  boolean TELNET;
  boolean OTA;
  boolean WEB;
  boolean Remote_Allow;
  boolean STAMode;
  char ssid[32];
  char WiFiKey[32];
  boolean dhcp;
  byte IP[4];
  byte Netmask[4];
  byte Gateway[4];
  char NTPServerName[128];
  long TimeZone;
  unsigned long Update_Time_Via_NTP_Every;
  boolean isDayLightSaving;
  char MQTT_Server[128];
  long MQTT_Port;
  char MQTT_User[16];
  char MQTT_Password[16];
  char UPDATE_Server[128];
  long UPDATE_Port;
  char UPDATE_User[16];
  char UPDATE_Password[16];
  long Temp_Corr;
} config;


void config_defaults() {
    Serial.println("Setting config Default values");

    strcpy(config.DeviceName, "AmbiSense");               // Device Name
    strcpy(config.Location, "Hall");                      // Device Location
    strcpy(config.ClientID, "001001");                    // Client ID (used on MQTT)
    config.ONTime = 10;                                   // 0-255 seconds (Byte range)
    config.SLEEPTime = 15;                                // 0-255 minutes (Byte range)
    config.DEEPSLEEP = true;                              // 0 - Disabled, 1 - Enabled
    config.LED = true;                                    // 0 - OFF, 1 - ON
    config.TELNET = false;                                // 0 - Disabled, 1 - Enabled
    config.OTA = true;                                    // 0 - Disabled, 1 - Enabled
    config.WEB = false;                                   // 0 - Disabled, 1 - Enabled
    config.Remote_Allow = true;                           // 0 - Not Allow, 1 - Allow remote operation
    config.STAMode = true;                                // 0 - AP or AP+STA Mode, 1 - Station only Mode
    strcpy(config.ssid, "WiFiCasaN");                     // Wireless LAN SSID (STA mode)
    strcpy(config.WiFiKey, "12345678");                   // Wireless LAN Key (STA mode)
    config.dhcp = true;                                   // 0 - Static IP, 1 - DHCP
    config.IP[0] = 192; config.IP[1] = 168; config.IP[2] = 1; config.IP[3] = 10;
    config.Netmask[0] = 255; config.Netmask[1] = 255; config.Netmask[2] = 255; config.Netmask[3] = 0;
    config.Gateway[0] = 192; config.Gateway[1] = 168; config.Gateway[2] = 1; config.Gateway[3] = 254;
    strcpy(config.NTPServerName, "pt.pool.ntp.org");         // NTP Server
    config.Update_Time_Via_NTP_Every = 1200;              // Time in minutes to re-sync the clock
    config.TimeZone = 0;                                  // -12 to 13. See Page_NTPSettings.h why using -120 to 130 on the code.
    config.isDayLightSaving = 1;                          // 0 - Disabled, 1 - Enabled
    strcpy(config.MQTT_Server, "iothubna.hopto.org");    // MQTT Broker Server (URL or IP)
    config.MQTT_Port = 1883;                              // MQTT Broker TCP port
    strcpy(config.MQTT_User, "admin");                   // MQTT Broker username
    strcpy(config.MQTT_Password, "admin");               // MQTT Broker password
    strcpy(config.UPDATE_Server, "iothubna.hopto.org");  // UPDATE Server (URL or IP)
    config.UPDATE_Port = 1880;                            // UPDATE Server TCP port
    strcpy(config.UPDATE_User, "user");                  // UPDATE Server username
    strcpy(config.UPDATE_Password, "1q2w3e4r");          // UPDATE Server password
    config.Temp_Corr = 0;     // Sensor Temperature Correction Factor, typically due to electronic self heat.
}


#include <storage.h>
#include <hw8266.h>
#include <mywifi.h>
#include <telnet.h>
#include <ntp.h>
//#include <web.h>
#include <ota.h>
//#include <mqtt.h>
//#include <global.h>


// **** Normal code definition here ...
#define ADC_SW_PIN 4                              // Base of Transistor connected to PIN GPIO4, acting as Switch
float Temperature = 0.0;                          // Variable
float Humidity = 0.0;                             // Variable
float Lux = 0.0;                                  // Variable


// **** Normal code functions here ...
float getLux (int Nmeasures = Number_of_measures, float Max_val = 910, float Min_val = 55) {
    // 910 and 55 are empiric values extract while testing the circut
    digitalWrite(ADC_SW_PIN, HIGH);   // Set ADC Switch to HIGH to sample Lux (BC547 Saturated)
    delay(100);
    telnet_println("ADC Switch in Lux position");
    float lux = 0.0;
    for(int i = 0; i < Nmeasures; i++) {
        lux += (Max_val - (float)analogRead(A0)) / (Max_val - Min_val) * 100;
        //telnet_println("Sample-LUX: " + String(lux));
        delay(25);
    }
	  lux = lux / Nmeasures;
    if ( lux < 0 )   lux = 0.0;
    if ( lux > 100 ) lux = 100.0;
    telnet_println("LUX: " + String(lux));
    digitalWrite(ADC_SW_PIN, LOW);   // Set ADC Switch to LOW to sample Voltage (BC547 Cut)
    telnet_println("ADC Switch in VOLTAGE position");
    return lux;
}


#include <mqtt.h>
#include <global.h>


void setup() {
 // Start Serial interface
  Serial.begin(74880);                      //This odd baud speed will show ESP8266 boot diagnostics too.
  //Serial.begin(115200);                   // For faster communication use 115200

  Serial.println(" ");
  Serial.println("Hello World!");
  Serial.println("My ID is " + ChipID + " and I'm running version " + SWVer);
  Serial.println("Reset reason: " + ESP.getResetReason());


 // Output GPIOs
  pinMode(ADC_SW_PIN, OUTPUT);                      // Initialize the ADC_SW_PIN pin as an output
  digitalWrite(ADC_SW_PIN, LOW);                    // Set ADC Switch to LOW to sample Voltage (BC547 Cut)


 // Input GPIOs

 // Start Hardware services, like: ESP_LED. DHT, internal ADC,...
    hw_setup();

  // Start Storage service and read stored configuration
      storage_setup();

  // Start WiFi service (Station or/and as Access Point)
      wifi_setup();

  // Start NTP service
      ntp_setup();

  // Start TELNET service
      if (config.TELNET) telnet_setup();

  // Start OTA service
      if (config.OTA) ota_setup();

  // Start ESP Web Service
      //if (config.WEB) web_setup();

  // Start MQTT service
      mqtt_setup();

  //  LOW Battery check
      if (BattPowered) LOW_Batt_check();    // Must be execute after mqtt_setup. If LOW Batt, it will DeepSleep forever!


  // **** Normal SETUP Sketch code here...
      Temperature = getTemperature();
      Humidity = getHumidity();
      Lux = getLux();
      telnet_print("Temperature: " + String(Temperature) + " C\t");
      telnet_print("Humidity: " + String(Humidity) + " %\t");
      telnet_print("Lux: " + String(Lux) + " %\t");
      telnet_println("");
      mqtt_publish(mqtt_pathtele(), "Temperatura", String(Temperature));
      mqtt_publish(mqtt_pathtele(), "Humidade", String(Humidity));
      mqtt_publish(mqtt_pathtele(), "Lux", String(Lux));

  // Last bit of code before leave setup
      ONTime_Offset = millis()/1000 + 0.1;  //  100ms after finishing the SETUP function it starts the "ONTime" countdown.
                                            //  it should be good enough to receive the MQTT "ExtendONTime" msg
} // end of setup()


void loop() {
  // allow background process to run.
      yield();

  // Hardware handling, namely the ESP_LED
      hw_loop();

  // WiFi handling
      wifi_loop();

  // NTP handling
      ntp_loop();

  // TELNET handling
      if (config.TELNET) telnet_loop();

  // OTA request handling
      if (config.OTA) ota_loop();

  // ESP Web Server requests handling
      //if (config.WEB) web_loop();

  // MQTT handling
      mqtt_loop();

  // DeepSleep handling
      deepsleep_loop();

  // **** Normal LOOP Skecth code here ...


}  // end of loop()
