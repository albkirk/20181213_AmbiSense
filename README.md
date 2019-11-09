# **AmbiSense**  -  Ambient Sensor

This project builds a device to measure some ambient metrics, such: **Temperature** [ -40-80 °C ± 0.5 °C ], **Humidity** [ 20-90% RH ± 2% RH ] and **Light** [ 0 - 100% ] and reports it using MQTT protocol.

##
## --Hardware--
This device is based on Espressif **ESP8266** MCU.

Temperature and Humidity are measured using the **DHT22** module.

Light is measured using **LDR** based Voltage divider circuit attached to ESP ADC pin.

As it is battery powered, the MCU is kept in sleepmode as mush possible and is only waked to measure the ambient and to report those values. Currently, a single 18650 Lithium Ion battey can last more than a month booting up on every 15 minutes. **Batt Charger** circuit included.

Simple **Push Button** is used to Start the MCU (RST to GND shunt).

##
## --Software--
Being based on my [GitHub albkirk ESP8266_KickStart](https://github.com/albkirk/ESP8266_KickStart), there's not much to add...
