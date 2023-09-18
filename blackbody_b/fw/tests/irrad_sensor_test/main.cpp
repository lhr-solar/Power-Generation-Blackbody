/**
 * @file main.cpp
 * @author Matthew Yu (matthewjkyu@gmail.com)
 * @brief Blackbody A/B Irradiance sensor test. Verify that:
 *      - Liveliness - verify that irradiance measurements can be taken from each irradiance sensor.
 *      - Replication - verify that irradiance measurements can be taken at various operating frequencies and determine message dropout metrics.
 *      - Accuracy - verify that the sensors are accurate within a known range of lighting conditions, with the appropriate calibration function.
 * @version 0.1.0
 * @date 2023-09-16
 *
 * @copyright Copyright (c) 2023
 *
 * @note Pinout:
 *  - D1  | HEARTBEAT LED
 *  - D0  | TRACKING LED
 *  - D3  | ERROR LED
 * 
 *  - D2  | CAN_TX
 *  - D10 | CAN_RX
 *  - D4  | I2C_SDA to irradiance sensor
 *  - D5  | I2C_SCL to irradiance sensor
 *  - D6  | INT from irradiance sensor
 *  - D11 | SPI_MISO to RTDs
 *  - D12 | SPI_MOSI to RTDs
 *  - D13 | SPI_SCLK to RTDs
 *  - A0  | SPI_CS_3 to RTDs
 *  - A1  | SPI_CS_7 to RTDs
 *  - A2  | SPI_CS_6 to RTDs
 *  - A3  | SPI_CS_2 to RTDs
 *  - A4  | SPI_CS_1 to RTDs
 *  - A5  | SPI_CS_5 to RTDs
 *  - A6  | SPI_CS_0 to RTDs
 *  - A7  | SPI_CS_4 to RTDs
 */
#include "mbed.h"
#include "inc/tsl2591.hpp"

DigitalOut led_heartbeat(D1);
DigitalOut led_tracking(D0);
DigitalOut led_error(D3);

I2C i2c1(D4, D5);
InterruptIn sensor_int(D6);
TSL2591 irradiance_sensor(&i2c1, &sensor_int, TSL2591_ADDR);

Ticker ticker_heartbeat;
EventQueue queue(32 * EVENTS_EVENT_SIZE);

/**
 * @brief Interrupt triggered by the heartbeat ticker to call an event to send a
 * pre-formed CAN message. 
 */
void handler_heartbeat(void);

/**
 * @brief Event to measure a sensor.
 */
void event_measure_sensor(void);

int main()
{
    ThisThread::sleep_for(3000ms);
    
    led_heartbeat = 0;
    led_tracking = 0;
    led_error = 0;

    if (!irradiance_sensor.setup()) {
        led_error = 1;
        while (1);
    }
    led_tracking = 1;

    ticker_heartbeat.attach(&handler_heartbeat, 250ms);
    queue.dispatch_forever();
}

void handler_heartbeat(void) { 
    led_heartbeat = !led_heartbeat;
    queue.call(&event_measure_sensor);
}

void event_measure_sensor(void) {
    // Measure sensor
    uint16_t ch0_raw;
    uint16_t ch1_raw;
    irradiance_sensor.sample(&ch0_raw, &ch1_raw);

    // Perform calibration and filter function
    float ch0_irradiance = ch0_raw / (264.1 * 100);
    float ch1_irradiance = ch1_raw / (34.9 * 100);
    
    // Output to screen
    printf("CH0: %.3f w/m^2\tCH1: %.3f w/m^2\n", ch0_irradiance, ch1_irradiance);
}