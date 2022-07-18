// **** Project code definition here ...
#include <ambient.h>

#define SW_PIN 4                            // Base of Transistor connected to PIN GPIO4, acting as Switch



// **** Project code functions here ...
void custom_ambient_data() {
    digitalWrite(SW_PIN, HIGH);   // Set ADC Switch to HIGH to sample Lux (BC547 Saturated)
    delay(100);
    telnet_println("ADC Switch in Lux position");
    ambient_data();      // TIMER bigger than zero on div or dog bites!!
    digitalWrite(SW_PIN, LOW);   // Set ADC Switch to LOW to sample Voltage (BC547 Cut)
    telnet_println("ADC Switch in VOLTAGE position");
}


void project_hw() {
    // Output GPIOs
        pinMode(SW_PIN, OUTPUT);                  // Initialize the SW_PIN pin as an output
        digitalWrite(SW_PIN, LOW);                // Set ADC Switch to LOW to sample Voltage (BC547 Cut)

    // Input GPIOs

}


void project_setup() {
  // Start Ambient devices
      ambient_setup();
      TIMER = (ulong)config.SLEEPTime;            // TIMER value (Recommended 15 minutes) to get Ambient data.
      custom_ambient_data(); 
}

void project_loop() {
  // Ambient handing
      if (TIMER >0) if ((millis() - 3500) % (TIMER * 60000) < 5) custom_ambient_data();

}
