#ifndef _bme280_H_
#define _bme280_H_

#include <Wire.h>
#include <Adafruit_BME280.h>

#include "sensors/sensor.h"

// The BME280 classes are based on the ADAfruit_BME280 library.

// BME280 represents an ADAfruit (or compatible) BME280 temperature / pressure / humidity sensor.
class BME280 : public Sensor {
  public:
    BME280(uint8_t addr = 0x77, String config_path = "");
    Adafruit_BME280* pAdafruitBME280;

  private:
    uint8_t addr;
    void check_status();
      
};


// Pass one of these in the constructor to BME280value() to tell which type of value you want to output
// LdB enum BME280ValType { temperature, pressure, humidity };
enum BME280ValType { temperature, pressure, humidity,sealevel_pressure };

// BME280value reads and outputs the specified value of a BME280 sensor.
class BME280value : public NumericSensor {
  public:
    BME280value(BME280* pBME280, BME280ValType val_type, uint read_delay = 500, String config_path="");
    void enable() override final;
    void setAltitude(float mA);
    BME280* pBME280;

  private:
    
    BME280ValType val_type;
    uint read_delay;
    virtual JsonObject& get_configuration(JsonBuffer& buf) override;
    virtual bool set_configuration(const JsonObject& config) override;
    virtual String get_config_schema() override;

};

/*

// BME280pressure reads and outputs the barometric pressure value of a BME280 sensor. 
class BME280pressure : public NumericSensor {
  public:
    BME280pressure(BME280* bme, uint read_delay = 500, String config_path="");
    void enable() override final;

  private:
    BME280* bme;
    uint read_delay;
    virtual JsonObject& get_configuration(JsonBuffer& buf) override;
    virtual bool set_configuration(const JsonObject& config) override;
    virtual String get_config_schema() override;

};

// BME280humidity reads and outputs the humidity value of a BME280 sensor. 
class BME280humidity : public NumericSensor {
  public:
    BME280humidity(BME280* bme, uint read_delay = 500, String config_path="");
    void enable() override final;

  private:
    BME280* bme;
    uint read_delay;
    virtual JsonObject& get_configuration(JsonBuffer& buf) override;
    virtual bool set_configuration(const JsonObject& config) override;
    virtual String get_config_schema() override;

};

*/

#endif
