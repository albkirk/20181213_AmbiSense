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
#define SWVer "13.05"                                   // Major.Minor Software version (use String 01.00 - 99.99 format !)

// Battery & ESP Voltage
#define BattPowered true                                // Is the device battery powered?
#define LDO_Corr float(0.4)                             // Battery Voltage [volt] corrective Factor due to LDO/Diode voltage drop
#define Batt_L_Thrs 40                                  // Battery level threshold [0%-100%] (before slepping forever).

// GPIO to Function Assignment
//ADC_MODE(ADC_VCC);                                      // TO COMMENT IF  you will use the ADC
#define Internal_ADC false                              // will the ADC be used only to measure the interval voltage?
#define LED_esp 2                                       // ESP Led is connected to GPIO 2
#define DHTPIN 5                                        // GPIO Connected to DHT22 Data PIN. -1 means NO DHT used!
#define BUZZER 0                                        // (Active) Buzzer


struct strConfig {
  String DeviceName;
  String Location;
  String ClientID;
  byte ONTime;
  byte SLEEPTime;
  boolean DEEPSLEEP;
  boolean LED;
  boolean TELNET;
  boolean OTA;
  boolean WEB;
  boolean Remote_Allow;
  boolean STAMode;
  String ssid;
  String WiFiKey;
  boolean dhcp;
  byte IP[4];
  byte Netmask[4];
  byte Gateway[4];
  String NTPServerName;
  long TimeZone;
  long Update_Time_Via_NTP_Every;
  boolean isDayLightSaving;
  String MQTT_Server;
  long MQTT_Port;
  String MQTT_User;
  String MQTT_Password;
  String UPDATE_Server;
  long UPDATE_Port;
  String UPDATE_User;
  String UPDATE_Password;
  long Temp_Corr;
} config;


void config_defaults() {
    Serial.println("Setting config Default values");

    config.DeviceName = String("AmbiSense");              // Device Name
    config.Location = String("Hall");                     // Device Location
    config.ClientID = String("001001");                   // Client ID (used on MQTT)
    config.ONTime = 10;                                   // 0-255 seconds (Byte range)
    config.SLEEPTime = 15;                                // 0-255 minutes (Byte range)
    config.DEEPSLEEP = true;                              // 0 - Disabled, 1 - Enabled
    config.LED = true;                                    // 0 - OFF, 1 - ON
    config.TELNET = false;                                // 0 - Disabled, 1 - Enabled
    config.OTA = true;                                    // 0 - Disabled, 1 - Enabled
    config.WEB = false;                                   // 0 - Disabled, 1 - Enabled
    config.Remote_Allow = true;                           // 0 - Not Allow, 1 - Allow remote operation
    config.STAMode = true;                                // 0 - AP or AP+STA Mode, 1 - Station only Mode
    config.ssid = String("ThomsonCasaN");                 // Wireless LAN SSID (STA mode)
    config.WiFiKey = String("12345678");                  // Wireless LAN Key (STA mode)
    config.dhcp = true;                                   // 0 - Static IP, 1 - DHCP
    config.IP[0] = 192; config.IP[1] = 168; config.IP[2] = 1; config.IP[3] = 10;
    config.Netmask[0] = 255; config.Netmask[1] = 255; config.Netmask[2] = 255; config.Netmask[3] = 0;
    config.Gateway[0] = 192; config.Gateway[1] = 168; config.Gateway[2] = 1; config.Gateway[3] = 254;
    config.NTPServerName = String("pt.pool.ntp.org");     // NTP Server
    config.Update_Time_Via_NTP_Every = 1200;              // Time in minutes to re-sync the clock
    config.TimeZone = 0;                                  // -12 to 13. See Page_NTPSettings.h why using -120 to 130 on the code.
    config.isDayLightSaving = 1;                          // 0 - Disabled, 1 - Enabled
    config.MQTT_Server = String("iothubna.hopto.org");    // MQTT Broker Server (URL or IP)
    config.MQTT_Port = 1883;                              // MQTT Broker TCP port
    config.MQTT_User = String("admin");                   // MQTT Broker username
    config.MQTT_Password = String("admin");               // MQTT Broker password
    config.UPDATE_Server = String("iothubna.hopto.org");  // UPDATE Server (URL or IP)
    config.UPDATE_Port = 1880;                            // UPDATE Server TCP port
    config.UPDATE_User = String("user");                  // UPDATE Server username
    config.UPDATE_Password = String("1q2w3e4r");          // UPDATE Server password
    config.Temp_Corr = 0;     // Sensor Temperature Correction Factor, typically due to electronic self heat.
}


#include <storage.h>
#include <global.h>
#include <wifi.h>
#include <telnet.h>
#include <ntp.h>
#include <web.h>
#include <ota.h>
//#include <mqtt.h>


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


void setup() {
// Output GPIOs
  pinMode(LED_esp, OUTPUT);
  digitalWrite(LED_esp, HIGH);                      // initialize LED off
  pinMode(ADC_SW_PIN, OUTPUT);                      // Initialize the ADC_SW_PIN pin as an output
  digitalWrite(ADC_SW_PIN, LOW);                    // Set ADC Switch to LOW to sample Voltage (BC547 Cut)


// Input GPIOs


  Serial.begin(115200);
  WiFi.mode(WIFI_OFF);

  Serial.println(" ");
  Serial.println("Hello World!");
  Serial.println("My ID is " + ChipID + " and I'm running version " + SWVer);
  Serial.println(ESP.getResetReason());


  // Check Battery Level
      Batt_Level = getVoltage();

  // Start DHT device
      if (DHTPIN>=0) dht_val.begin();

  // Start Storage service and read stored configuration
      storage_setup();

  // Start WiFi service (Station or/and as Access Point)
      wifi_setup();

  // Start TELNET service
      if (config.TELNET) telnet_setup();

  // Start NTP service
      ntp_setup();

  // Start OTA service
      if (config.OTA) ota_setup();

  // Start ESP Web Service
      if (config.WEB) web_setup();

  // Start MQTT service
      int mqtt_status = mqtt_setup();

      if (mqtt_status == MQTT_CONNECTED) {
          if (ESP.getResetReason() != "Deep-Sleep Wake") {
              mqtt_publish(mqtt_pathtele(), "Boot", ESP.getResetReason());
              mqtt_publish(mqtt_pathtele(), "ChipID", ChipID);
              mqtt_publish(mqtt_pathtele(), "Brand", BRANDName);
              mqtt_publish(mqtt_pathtele(), "Model", MODELName);
              mqtt_publish(mqtt_pathtele(), "SWVer", SWVer);
          }
          if (BattPowered) {
              mqtt_publish(mqtt_pathtele(), "BatLevel", String(Batt_Level));
              if (Batt_Level > Batt_L_Thrs) mqtt_publish(mqtt_pathtele(), "Status", "Battery");
              else mqtt_publish(mqtt_pathtele(), "Status", "LOW Battery");
          }
          else mqtt_publish(mqtt_pathtele(), "Status", "Mains");
          mqtt_publish(mqtt_pathtele(), "RSSI", String(getRSSI()));
          mqtt_publish(mqtt_pathtele(), "IP", WiFi.localIP().toString());
      }

  // LOW Battery handling
      if (Batt_Level < Batt_L_Thrs) {
        mqtt_disconnect();
        telnet_println("Going to sleep forever. Please, recharge the battery ! ! ! ");
        delay(100);
        ESP.deepSleep(0);                   // Sleep forever
      }


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

  // LED handling usefull if you need to identify the unit from many
      digitalWrite(LED_esp, boolean(!config.LED));  // Values is reversed due to Pull-UP configuration

  // WiFi handling
      wifi_loop();

  // TELNET handling
      if (config.TELNET) telnet_loop();

  // NTP handling
      ntp_loop();

  // OTA request handling
      if (config.OTA) ota_loop();

  // ESP Web Server requests handling
      if (config.WEB) web_loop();

  // MQTT handling
      mqtt_loop();

  // DeepSleep handling
  if (config.DEEPSLEEP && !config.TELNET && !config.OTA && !config.WEB && (millis()/1000) > (ulong(config.ONTime) + ONTime_Offset + Extend_time)) {
    mqtt_publish(mqtt_pathtele(), "Status", "DeepSleep");
    mqtt_disconnect();
    telnet_println("Going to sleep until next event... zzZz :) ");
    delay(100);
    ESP.deepSleep(config.SLEEPTime * 60 * 1000000);   // time in minutes converted to microseconds
  }

  // **** Normal LOOP Skecth code here ...



}  // end of loop()
