#include "bme280.h"

#include "sensesp.h"
#include "i2c_tools.h"

float myAltitude=0; // LdB

// BME280 represents an ADAfruit (or compatible) BME280 temperature / pressure / humidity sensor.
BME280::BME280(uint8_t addr, String config_path) : // LdB
       Sensor(config_path), addr{addr} {
    className = "BME280";
    scan_i2c(); // make this optional?
    pAdafruitBME280 = new Adafruit_BME280();
    check_status(); // make this optional? No - but if you do, have to put the begin() here.
}

void BME280::check_status() {
  bool status = pAdafruitBME280->begin(addr);
  if (!status) {
        Serial.println("Could not find a valid BME280 sensor, check wiring, address, and sensor ID.");
        Serial.print("SensorID is: 0x"); Serial.println(pAdafruitBME280->sensorID(),16);
        Serial.println("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085");
        Serial.println("        ID of 0x56-0x58 represents a BMP 280,");
        Serial.println("        ID of 0x60 represents a BME 280.");
        Serial.println("        ID of 0x61 represents a BME 680.");
        while (1) delay(10);
    }
}


// BME280value reads and outputs the specified type of value of a BME280 sensor
BME280value::BME280value(BME280* pBME280, BME280ValType val_type, uint read_delay, String config_path) :
                   NumericSensor(config_path), pBME280{pBME280}, val_type{val_type}, read_delay{read_delay} {
      className = "BME280value";
      load_configuration();
}

void BME280value::setAltitude (float mA) {
  myAltitude = mA;
}
// BME280 outputs temp in Celsius. Need to convert to Kelvin before sending to Signal K.
// Pressure is output in Pascals, Humidity is output in relative humidity (0 - 100%)
void BME280value::enable() {
  app.onRepeat(read_delay, [this](){ 
      if (val_type == temperature) {
          output = pBME280->pAdafruitBME280->readTemperature() + 273.15; // Kelvin is Celsius + 273.15
      }
      if (val_type == pressure) {
          output = pBME280->pAdafruitBME280->readPressure();
      }  
      if (val_type == humidity) {
          output = pBME280->pAdafruitBME280->readHumidity();
      }
      //  LdB altitude in meters and current athm pressure
      
      if (val_type == sealevel_pressure) {
          output = pBME280->pAdafruitBME280->seaLevelForAltitude(myAltitude, 
                                                                 pBME280->pAdafruitBME280->readPressure());
      }
      
      notify();
 });
}

JsonObject& BME280value::get_configuration(JsonBuffer& buf) {
  JsonObject& root = buf.createObject();
  root["read_delay"] = read_delay;
  root["value"] = output;
  return root;
  };

  static const char SCHEMA[] PROGMEM = R"###({
    "type": "object",
    "properties": {
        "read_delay": { "title": "Read delay", "type": "number", "description": "The time, in milliseconds, between each read of the input" },
        "value": { "title": "Last value", "type" : "number", "readOnly": true }
    }
  })###";


  String BME280value::get_config_schema() {
  return FPSTR(SCHEMA);
}

bool BME280value::set_configuration(const JsonObject& config) {
  String expected[] = {"read_delay"};
  for (auto str : expected) {
    if (!config.containsKey(str)) {
      return false;
    }
  }
  read_delay = config["read_delay"];
  return true;
}



/*

**********************************************************

// BME280temp reads and outputs the temperature value of a BME280 sensor
BME280temp::BME280temp(BME280* bme, uint read_delay, String config_path) :
                   NumericSensor(config_path), bme{bme}, read_delay{read_delay} {
      className = "BME280temp";
      load_configuration();
}

// BME280 outputs temp in Celsius. Need to convert to Kelvin before sending to Signal K.
void BME280temp::enable() {
  app.onRepeat(read_delay, [this](){ 
      output = bme->bme280->readTemperature() + 273.15; // Kelvin is Celsius + 273.15
      notify();
 });
}

JsonObject& BME280temp::get_configuration(JsonBuffer& buf) {
  JsonObject& root = buf.createObject();
  root["read_delay"] = read_delay;
  root["value"] = output;
  return root;
  };

  static const char SCHEMA[] PROGMEM = R"###({
    "type": "object",
    "properties": {
        "read_delay": { "title": "Read delay", "type": "number", "description": "The time, in milliseconds, between each read of the input" },
        "value": { "title": "Last value", "type" : "number", "readOnly": true }
    }
  })###";


  String BME280temp::get_config_schema() {
  return FPSTR(SCHEMA);
}

bool BME280temp::set_configuration(const JsonObject& config) {
  String expected[] = {"read_delay"};
  for (auto str : expected) {
    if (!config.containsKey(str)) {
      return false;
    }
  }
  read_delay = config["read_delay"];
  return true;
}


// BME280pressure reads and outputs the barometric pressure value of a BME280 sensor
BME280pressure::BME280pressure(BME280* bme, uint read_delay, String config_path) :
                   NumericSensor(config_path), bme{bme}, read_delay{read_delay} {
      className = "BME280pressure";
      load_configuration();
}

void BME280pressure::enable() {
  app.onRepeat(read_delay, [this](){ 
      output = bme->bme280->readPressure();
      notify();
 });
}

JsonObject& BME280pressure::get_configuration(JsonBuffer& buf) {
  JsonObject& root = buf.createObject();
  root["read_delay"] = read_delay;
  root["value"] = output;
  return root;
  };

  static const char SCHEMA2[] PROGMEM = R"###({
    "type": "object",
    "properties": {
        "read_delay": { "title": "Read delay", "type": "number", "description": "The time, in milliseconds, between each read of the input" },
        "value": { "title": "Last value", "type" : "number", "readOnly": true }
    }
  })###";


  String BME280pressure::get_config_schema() {
  return FPSTR(SCHEMA2);
}

bool BME280pressure::set_configuration(const JsonObject& config) {
  String expected[] = {"read_delay"};
  for (auto str : expected) {
    if (!config.containsKey(str)) {
      return false;
    }
  }
  read_delay = config["read_delay"];
  return true;
}

// BME280humidity reads and outputs the pressure value of a BME280 sensor
BME280humidity::BME280humidity(BME280* bme, uint read_delay, String config_path) :
                   NumericSensor(config_path), bme{bme}, read_delay{read_delay} {
      className = "BME280humidity";
      load_configuration();
}

void BME280humidity::enable() {
  app.onRepeat(read_delay, [this](){ 
      output = bme->bme280->readHumidity();
      notify();
 });
}

JsonObject& BME280humidity::get_configuration(JsonBuffer& buf) {
  JsonObject& root = buf.createObject();
  root["read_delay"] = read_delay;
  root["value"] = output;
  return root;
  };

  static const char SCHEMA3[] PROGMEM = R"###({
    "type": "object",
    "properties": {
        "read_delay": { "title": "Read delay", "type": "number", "description": "The time, in milliseconds, between each read of the input" },
        "value": { "title": "Last value", "type" : "number", "readOnly": true }
    }
  })###";


  String BME280humidity::get_config_schema() {
  return FPSTR(SCHEMA3);
}

bool BME280humidity::set_configuration(const JsonObject& config) {
  String expected[] = {"read_delay"};
  for (auto str : expected) {
    if (!config.containsKey(str)) {
      return false;
    }
  }
  read_delay = config["read_delay"];
  return true;
}

*/
