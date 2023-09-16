/**
 * @file main.cpp
 * @author Matthew Yu (matthewjkyu@gmail.com)
 * @brief Blackbody A Irradiance sensor test. Verify that:
 *      - Liveliness - verify that irradiance measurements can be taken from each irradiance sensor.
 *      - Replication - verify that irradiance measurements can be taken at various operating frequencies and determine message dropout metrics.
 *      - Accuracy - verify that the sensors are accurate within a known range of lighting conditions, with the appropriate calibration function.
 * @version 0.1.0
 * @date 09-16-23
 *
 * @note Pinout:
 *  - D1  | HEARTBEAT LED
 *  - D0  | TRACKING LED
 *  - D3  | ERROR LED
 * 
 *  - D2  | CAN_TX
 *  - D10 | CAN_RX
 *  - D4  | I2C_SDA to Blackbody C
 *  - D5  | I2C_SCL to Blackbody C
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

DigitalOut led_heartbeat(D1);
DigitalOut led_tracking(D0);
DigitalOut led_error(D3);

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
    led_heartbeat = 0;
    led_tracking = 1;
    led_error = 0;

    ticker_heartbeat.attach(&handler_heartbeat, 1000ms);

    queue.dispatch_forever();
}

void handler_heartbeat(void) { 
    led_heartbeat = !led_heartbeat;
    queue.call(&event_measure_sensor);
}

void event_measure_sensor(void) {
    // Measure sensor
    // Perform calibration and filter function
    // Output to screen
}