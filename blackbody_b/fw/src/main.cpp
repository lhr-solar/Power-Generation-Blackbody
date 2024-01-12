/**
 * @file main.cpp
 * @author Matthew Yu (matthewjkyu@gmail.com)
 * @brief Blackbody B Irradiance main.
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

#define CAN_HEARTBEAT   0x620
#define CAN_SET_MODE    0x621
#define CAN_BB_FAULT    0x622
#define CAN_ACK_FAULT   0x623
#define CAN_RTD_CONF    0x624
#define CAN_IRR_CONF    0x625
#define CAN_RTD_MEAS    0x626
#define CAN_IRR_MEAS    0x627

enum State {
    STATE_STOP = 0,
    STATE_RUN = 1,
    STATE_ERROR = 2
};

typedef enum Error_t {
    ERROR_NONE,
    ERROR_DEBUG,
    ERROR_IRRAD_SETUP
} Error_t;

static DigitalOut led_heartbeat(D1);
static DigitalOut led_tracking(D0);
static DigitalOut led_error(D3);

static CAN can(D10, D2);

static I2C i2c1(D4, D5);
static InterruptIn sensor_int(D6);
static TSL2591 irradiance_sensor(&i2c1, &sensor_int, TSL2591_ADDR);

static Ticker ticker_heartbeat;
static Ticker ticker_sample_irrad;
static EventQueue queue(32 * EVENTS_EVENT_SIZE);

static enum State current_state;
static bool is_error;
static bool set_mode;
static uint16_t sample_frequency;
static Error_t sys_error;

/**
 * @brief Interrupt triggered by the heartbeat ticker to call event
 * event_heartbeat.
 */
void handler_heartbeat(void);

/**
 * @brief Interrupt triggered by an irradiance sensor ticker to call event
 * event_measure_irradiance_sensor.
 */
void handler_measure_irradiance_sensor(void);

/**
 * @brief Interrupt triggered by a CAN RX IRQ to call event
 * event_process_can_message.
 */
void handler_can(void);

/**
 * @brief Event to toggle the heartbeat LED and send a heartbeat CAN message. 
 */
void event_heartbeat(void);

/**
 * @brief Event to measure irradiance sensor and output the result over CAN.
 */
void event_measure_irradiance_sensor(void);

/**
 * @brief Event to process incoming CAN messages.
 */
void event_process_can_message(void);

/**
 * @brief Event to update the state machine and manage any sensor tickers.
 */
void event_update_state_machine(void);

/**
 * @brief Event to change our state to STATE_ERROR and output via CAN.
 */
void event_process_error(void);

int main() {
    ThisThread::sleep_for(3000ms);
    
    led_heartbeat = 0;
    led_tracking = 0;
    led_error = 0;
    current_state = STATE_STOP;
    is_error = false;
    set_mode = false;
    sample_frequency = 1;
    sys_error = ERROR_NONE;

    if (!irradiance_sensor.setup()) {
        is_error = 1;
        sys_error = ERROR_IRRAD_SETUP;
        queue.call(&event_process_error);
    }

    // Force start
    set_mode = true;
    queue.call(&event_update_state_machine);

    ticker_heartbeat.attach(&handler_heartbeat, 1000ms);
    can.attach(&handler_can, CAN::RxIrq);

    queue.dispatch_forever();
}

void handler_heartbeat(void) { 
    led_heartbeat = !led_heartbeat;
    queue.call(&event_heartbeat);
}

void handler_measure_irradiance_sensor(void) {
    queue.call(&event_measure_irradiance_sensor);
}

void handler_can(void) {
    queue.call(&event_process_can_message);
}

void event_heartbeat(void) {
    static char counter = 0;
    CANMessage message(CAN_HEARTBEAT, &counter, 1);
    ++counter;
    can.write(message);

    printf(
        "Cycle %i:\tSTATE=%i,\tIS_ERROR=%i,\tSET_MODE=%i\n", 
        counter,
        current_state,
        is_error,
        set_mode
    );
}

void event_measure_irradiance_sensor(void) {
    printf("\tSampling irradiance sensor.\n");
    // Measure sensor
    uint16_t ch0_raw;
    uint16_t ch1_raw;
    irradiance_sensor.sample(&ch0_raw, &ch1_raw);

    // TODO: Perform calibration and filter function
    float ch0_irradiance = ch0_raw / (264.1 * 100);
    float ch1_irradiance = ch1_raw / (34.9 * 100);
    
    // Output to screen
    printf("\tCH0: %.3f w/m^2\tCH1: %.3f w/m^2\n", ch0_irradiance, ch1_irradiance);

    // Output on CAN
    can.write(CANMessage(CAN_IRR_MEAS, (uint8_t*)&ch0_irradiance, 4));
}

void event_process_can_message(void) {
    // Read message
    CANMessage msg;
    if (can.read(msg)) {
        switch (msg.id) {
            case CAN_SET_MODE:
                // TODO: verify mode is set properly.
                set_mode = msg.data[0];
                printf("\tGot set_mode=%i\n", set_mode);
                queue.call(&event_update_state_machine);
                break;
            case CAN_ACK_FAULT:
                is_error = false;
                sys_error = ERROR_NONE;
                set_mode = false;
                queue.call(&event_update_state_machine);
                break;
            case CAN_IRR_CONF:
                // TODO: verify sample frequency is set properly.
                sample_frequency = (uint16_t) (msg.data[0]) << 8 | (uint16_t) (msg.data[1]);
                printf("\tGot sample frequency=%i\n", sample_frequency);
                break;
            default:
                // Ignore any other CAN messages.
                break;
        }
    } else {
        is_error = true;
        queue.call(&event_process_error);
    }
}

void event_update_state_machine(void) {
    switch (current_state) {
        case STATE_STOP:
            if (set_mode)   current_state = STATE_RUN;
            if (is_error)   current_state = STATE_ERROR;
            break;
        case STATE_RUN:
            if (!set_mode)  current_state = STATE_STOP;
            if (is_error)   current_state = STATE_ERROR;
            break;
        case STATE_ERROR:
            if (!is_error)  current_state = STATE_STOP;
            break;
    }

    printf("\tNew state: %i\n", current_state);
    
    switch (current_state) {
        case STATE_STOP:
            ticker_sample_irrad.detach();
            led_tracking = 0;
            led_error = 0;
            break;
        case STATE_RUN:
            ticker_sample_irrad.attach(
                handler_measure_irradiance_sensor, 
                (1000ms / sample_frequency)
            );
            led_tracking = 1;
            led_error = 0;
            break;
        case STATE_ERROR:
            ticker_sample_irrad.detach();
            led_error = 1;
            led_tracking = 0;
            break;
    }
}

void event_process_error(void) {
    is_error = true;
    queue.call(&event_update_state_machine);

    uint16_t _error = sys_error;
    can.write(CANMessage(CAN_BB_FAULT, (uint8_t*)&_error, 2));
    printf("\tError: %i\n", _error);
}
