/**
 * @file tsl2591.cpp
 * @author Matthew Yu (matthewjkyu@gmail.com)
 * @brief Drivers for TSL2591 AMS light sensor.
 * @version 0.1.0
 * @date 2023-09-16
 * 
 * @copyright Copyright (c) 2023
 */
#include "tsl2591.hpp"

#define TSL2591_ID          (0x50)

#define TSL2591_CMD_BIT     (0xA0)

#define TSL2591_EN_NPIEN    (0x80)
#define TSL2591_EN_SAI      (0x40)
#define TSL2591_EN_AIEN     (0x10)
#define TSL2591_EN_AEN      (0x02)
#define TSL2591_EN_PON      (0x01)
#define TSL2591_EN_POFF     (0x00)

#define TSL2591_LUX_DF      (408.0F)
#define TSL2591_LUX_COEFB   (1.64F)  // CH0 coefficient 
#define TSL2591_LUX_COEFC   (0.59F)  // CH1 coefficient A
#define TSL2591_LUX_COEFD   (0.86F)  // CH2 coefficient B

enum {
    TSL2591_REG_ENABLE          = 0x00,
    TSL2591_REG_CONTROL         = 0x01,
    TSL2591_REG_THRES_AILTL     = 0x04,
    TSL2591_REG_THRES_AILTH     = 0x05,
    TSL2591_REG_THRES_AIHTL     = 0x06,
    TSL2591_REG_THRES_AIHTH     = 0x07,
    TSL2591_REG_THRES_NPAILTL   = 0x08,
    TSL2591_REG_THRES_NPAILTH   = 0x09,
    TSL2591_REG_THRES_NPAIHTL   = 0x0A,
    TSL2591_REG_THRES_NPAIHTH   = 0x0B,
    TSL2591_REG_PERSIST         = 0x0C,
    TSL2591_REG_PID             = 0x11,
    TSL2591_REG_ID              = 0x12,
    TSL2591_REG_STATUS          = 0x13,
    TSL2591_REG_CHAN0_L         = 0x14,
    TSL2591_REG_CHAN0_H         = 0x15,
    TSL2591_REG_CHAN1_L         = 0x16,
    TSL2591_REG_CHAN1_H         = 0x17,
} TSL2591_registers;

TSL2591::TSL2591(I2C* tsl2591_i2c, InterruptIn* tsl2591_int, uint8_t addr, TSL2591Gain_t gain, TSL2591IntegrationTime_t integ):
 _i2c(tsl2591_i2c), _int(tsl2591_int), _addr(addr << 1)
{
    _gain = gain;
    _integ = integ;
}

bool TSL2591::setup(void) {
    char write[] = { (TSL2591_CMD_BIT|TSL2591_REG_ID) };
    if(_i2c->write(_addr, write, 1, 0) == 0) {
        char read[1];
        _i2c->read(_addr, read, 1, 0);
        if(read[0] == TSL2591_ID) {
            set_gain(_gain);
            set_integration_time(_integ);
            disable();
            return true;
        }
    }
    return false;
}

void TSL2591::set_gain(TSL2591Gain_t gain) {
    enable();
    _gain = gain;
    char write[] = {(TSL2591_CMD_BIT|TSL2591_REG_CONTROL), static_cast<char>((_integ|_gain))};
    _i2c->write(_addr, write, 2, 0);
    disable();
}

void TSL2591::set_integration_time(TSL2591IntegrationTime_t integ) {
    enable();
    _integ = integ;
    char write[] = {(TSL2591_CMD_BIT|TSL2591_REG_CONTROL), static_cast<char>((_integ|_gain))};
    _i2c->write(_addr, write, 2, 0);
    disable();
}

void TSL2591::sample(uint16_t* ch0_counts, uint16_t* ch1_counts) {
    enable();
    for(uint8_t t=0; t<=_integ+1; t++) {
        ThisThread::sleep_for(100ms);
    }

    // Channel 1
    char write1[] = {(TSL2591_CMD_BIT|TSL2591_REG_CHAN1_L)};
    _i2c->write(_addr, write1, 1, 0);
    char read1[2];
    _i2c->read(_addr, read1, 2, 0);

    // Channel 0
    char write2[] = {(TSL2591_CMD_BIT|TSL2591_REG_CHAN0_L)};
    _i2c->write(_addr, write2, 1, 0);
    char read2[2];
    _i2c->read(_addr, read2, 2, 0);

    disable();

    *ch1_counts = (*(uint16_t*)read1);
    *ch0_counts = (*(uint16_t*)read2);
}

void TSL2591::enable(void) {
    char write[] = {(TSL2591_CMD_BIT|TSL2591_REG_ENABLE), (TSL2591_EN_PON|TSL2591_EN_AEN|TSL2591_EN_AIEN|TSL2591_EN_NPIEN)};
    _i2c->write(_addr, write, 2, 0);
}

void TSL2591::disable(void) {
    char write[] = {(TSL2591_CMD_BIT|TSL2591_REG_ENABLE), (TSL2591_EN_POFF)};
    _i2c->write(_addr, write, 2, 0);
}
