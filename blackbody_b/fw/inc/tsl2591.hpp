/**
 * @file tsl2591.h
 * @author Matthew Yu (matthewjkyu@gmail.com)
 * @brief Drivers for TSL2591 AMS light sensor.
 * @version 0.1.0
 * @date 2023-09-16
 * 
 * @copyright Copyright (c) 2023
 */
#pragma once
#include "mbed.h"
#include <cstdint>

#define TSL2591_ADDR        (0x29)

/**
 * @brief Gain scaling relative to 1x gain.
 * - LOW:   1x
 * - MED:   25x
 * - HIGH:  428x
 * - MAX:   9876x
 */
typedef enum {
    TSL2591_GAIN_LOW    = 0x00,
    TSL2591_GAIN_MED    = 0x01,
    TSL2591_GAIN_HIGH   = 0x02,
    TSL2591_GAIN_MAX    = 0x03,
} TSL2591Gain_t;

/**
 * @brief Integration time of both photodiode channels.
 */
typedef enum {
    TSL2591_INT_TIME_100MS  = 0x00,
    TSL2591_INT_TIME_200MS  = 0x01,
    TSL2591_INT_TIME_300MS  = 0x02,
    TSL2591_INT_TIME_400MS  = 0x03,
    TSL2591_INT_TIME_500MS  = 0x04,
    TSL2591_INT_TIME_600MS  = 0x05,
} TSL2591IntegrationTime_t;


/**
 * @brief TSL2591 is a high sensitivity light to digital converter with a I2C
 * interface. Measures both irradiance and lux.
 */
class TSL2591
{
    public:
        /**
         * @brief Construct a new TSL2591 object
         * 
         * @param tsl2591_i2c I2C instance peripheral tied to the sensor.
         * @param tsl2591_int Interrupt pin tied to the sensor.
         * @param address I2C address of the sensor. Default 0x29.
         * @param gain Gain of the sensor. Default TSL2591_GAIN_LOW.
         * @param integ Integration time of the sensor. Default TSL2591_INT_TIME_100MS.
         */
        TSL2591(I2C* tsl2591_i2c, InterruptIn* tsl2591_int, uint8_t addr=TSL2591_ADDR, TSL2591Gain_t gain=TSL2591_GAIN_LOW, TSL2591IntegrationTime_t integ=TSL2591_INT_TIME_100MS);

        /**
         * @brief Sets up the sensor.
         * 
         * @return true If good response.
         * @return false Bad response from sensor.
         */
        bool setup(void);

        /**
         * @brief Set the gain of the sensor.
         * 
         * @param gain TSL2591Gain_t gain.
         */
        void set_gain(TSL2591Gain_t gain);
        
        /**
         * @brief Set the integration time of the sensor.
         * 
         * @param integ TSL2591Gain_t integration time.
         */
        void set_integration_time(TSL2591IntegrationTime_t integ);
        
        /**
         * @brief Sample the sensor and parse the results.
         * 
         * @param ch0_counts Raw counts for channel 0.
         * @param ch1_counts Raw counts for channel 1.
         */
        void sample(uint16_t* ch0_counts, uint16_t* ch1_counts);
    
    private:
        /**
         * @brief Enable device operation.
         */
        void enable(void);

        /**
         * @brief Disable device operation.
         */
        void disable(void);

        /**
         * @brief Reference to I2C object.
         */
        I2C*                        _i2c;
        
        /**
         * @brief I2C address of sensor (typically TSL2591_ADDR unless board has
         * I2C mux support). 
         */
        uint8_t                     _addr;

        /**
         * @brief Reference to pin used for interrupt.
         */
        InterruptIn*                _int;

        /**
         * @brief Gain of sensor.
         */
        TSL2591Gain_t               _gain;

        /**
         * @brief Integration time of sensor.
         */
        TSL2591IntegrationTime_t    _integ;
};
