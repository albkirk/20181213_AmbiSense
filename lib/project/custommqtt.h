// Function to insert customized MQTT Configuration actions

void  mqtt_custom(String reqparam, String reqvalue, JsonObject& data) {
  if ( reqparam == "Temperatura") mqtt_publish(mqtt_pathtele(), "Temperatura", String(getTemperature()));
  if ( reqparam == "Humidade") mqtt_publish(mqtt_pathtele(), "Humidade", String(getHumidity()));
  if ( reqparam == "Lux") mqtt_publish(mqtt_pathtele(), "Lux", String(getLux()));

}
