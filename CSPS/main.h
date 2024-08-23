#include "KCORES_CSPS.h"
#include "esphome.h"

byte PSaddr = 0x5f;
byte ROMaddr = 0x57;
bool ChkSum = true;

CSPS PowerSupply(PSaddr, ROMaddr, ChkSum);

class CSPStext : public PollingComponent {
  public:
  	TextSensor *spn = new TextSensor();
  	TextSensor *mfg = new TextSensor();
    TextSensor *mfr = new TextSensor();
  	TextSensor *name = new TextSensor();
    TextSensor *okn = new TextSensor();
  	TextSensor *ct = new TextSensor();
    CSPStext(): PollingComponent(60000) { }
    void setup() override {
      ESP_LOGD("Power Supply", "Spare Part No: %s", PowerSupply.getSPN().c_str());
      ESP_LOGD("Power Supply", "Manufacture Date: %s", PowerSupply.getMFG().c_str());
      ESP_LOGD("Power Supply", "Manufacturer: %s", PowerSupply.getMFR().c_str());
      ESP_LOGD("Power Supply", "Power Name: %s", PowerSupply.getName().c_str());
      ESP_LOGD("Power Supply", "Option Kit No: %s", PowerSupply.getOKN().c_str());
      ESP_LOGD("Power Supply", "CT Date Codes: %s", PowerSupply.getCT().c_str());
    }
    void update() override {
      name->publish_state(PowerSupply.getName().c_str());
      mfr->publish_state(PowerSupply.getMFR().c_str());
      mfg->publish_state(PowerSupply.getMFG().c_str());
      ct->publish_state(PowerSupply.getCT().c_str());
      spn->publish_state(PowerSupply.getSPN().c_str());
      okn->publish_state(PowerSupply.getOKN().c_str());

    }
};   
class CSPSOther : public PollingComponent {
  public:
    Sensor *rt = new Sensor();
    Sensor *pao = new Sensor();
    CSPSOther(): PollingComponent(3000) { }
    void setup() override {
    }
    void update() override {
      rt->publish_state(PowerSupply.getRunTime());
      pao->publish_state(PowerSupply.getPeakAmpsOut());
    }
};    
class CSPSPowerIn : public PollingComponent {
  public:
    Sensor *power_in = new Sensor();
    Sensor *current_in = new Sensor();
    Sensor *voltage_in = new Sensor();
    CSPSPowerIn(): PollingComponent(1250) { }
    void setup() override {
    }
    void update() override {
      power_in->publish_state(PowerSupply.getInputPower());
      current_in->publish_state(PowerSupply.getInputCurrent());
      voltage_in->publish_state(PowerSupply.getInputVoltage());
    }
};
class CSPSPowerOut : public PollingComponent {
  public:
    Sensor *power_out = new Sensor();
    Sensor *current_out = new Sensor();
    Sensor *voltage_out = new Sensor();
    CSPSPowerOut(): PollingComponent(750) { }
    void setup() override {
    }
    void update() override {
      power_out->publish_state(PowerSupply.getOutputPower());
      current_out->publish_state(PowerSupply.getOutputCurrent());
      voltage_out->publish_state(PowerSupply.getOutputVoltage());
    }
};
class CSPSFan : public PollingComponent {
  public:
    Sensor *fan_speed = new Sensor();
    CSPSFan(): PollingComponent(1000) { }
    void setup() override {
    }
    void update() override {
      fan_speed->publish_state(PowerSupply.getFanRPM());
    }
    void set_fan_rpm(float state) {
      PowerSupply.setFanRPM(state);
    }
};
class CSPSTemp : public PollingComponent {
  public:
    Sensor *temp1 = new Sensor();
    Sensor *temp2 = new Sensor();
    CSPSTemp(): PollingComponent(2500) { }
    void setup() override {
    }
    void update() override {
      temp1->publish_state(PowerSupply.getTemp1());
      temp2->publish_state(PowerSupply.getTemp2());
    }
};
