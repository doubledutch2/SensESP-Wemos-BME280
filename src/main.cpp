// BME280 example. This is a very basic example, with no transforms other than SKOutputNumber.
// It shows how to get the values from a BME280 sensor, but you should look at other examples
// for a more thorough explanation of how to use the various transforms and config paths.

#include <Arduino.h>

//#define SERIAL_DEBUG_DISABLED

#define USE_LIB_WEBSOCKET true

#include "sensesp_app.h"
#include "sensors/bme280.h"
#include "signalk/signalk_output.h"

ReactESP app([] () {
  #ifndef SERIAL_DEBUG_DISABLED
  Serial.begin(115200);

  // A small arbitrary delay is required to let the
  // serial port catch up

  delay(100);
  Debug.setSerialEnabled(true);
  #endif

  boolean disableStdSensors = true;
  sensesp_app = new SensESPApp(disableStdSensors);

  // Uses the same read delay for all of the outputs below. Note that you can use different read delays for each reading if you want.
  uint bme_read_delay = 30000;

  // Create a pointer to a BME280, which represents a BME280 sensor, that will be used by the temp, barometric pressure, and humidity sensors.
  // Default address is 0x77, but some chips allow you to use 0x76 - see datasheet.
  // To change the mode, sampling, filter, or standby duration, pass those parameters to setSampling(). See
  // https://github.com/adafruit/Adafruit_BME280_Library/blob/master/Adafruit_BME280.h for details.
  auto* pBME = new BME280(0x77);
  pBME->pAdafruitBME280->setSampling();
  float myAltitude = 126; //  In Meters (sealevel = 0), used to calculate sealevel atm pressure

  // Get the temperature
  // If you don't want to be able to change the read delay at runtime, you don't need to provide the last parameter (the config path).
  auto* pBME280temp = new BME280value(pBME, temperature, bme_read_delay, "/BME280temp/read_delay");
        pBME280temp->connectTo(new SKOutputNumber("environment.outside.temp"));   // connect the output of BME280temp() to a SignalK Output as a number

  // Get the barometric pressure
  auto* pBMEpressure = new BME280value(pBME, pressure, bme_read_delay, "/BME280pressure/read_delay");
        pBMEpressure->connectTo(new SKOutputNumber("environment.inside.engineroom.pressure"));   // connect the output of BME280pressure() to a SignalK Output as a number

  // Get the sealevel barometric pressure
  auto* pBMESLpressure = new BME280value(pBME, sealevel_pressure, bme_read_delay, "/BME280pressure/read_delay");
        pBMESLpressure->setAltitude(myAltitude);
        pBMESLpressure->connectTo(new SKOutputNumber("environment.outside.pressure"));   // connect the output of BME280pressure() to a SignalK Output as a number

  // Get the humidity
  auto* pBMEhumidity = new BME280value(pBME, humidity, bme_read_delay, "/BME280humidity/read_delay");
        pBMEhumidity->connectTo(new SKOutputNumber("environment.outside.humidity"));   // connect the output of BME280humidity() to a SignalK Output as a number
  
  // Set everything in motion
  sensesp_app->enable();
});
