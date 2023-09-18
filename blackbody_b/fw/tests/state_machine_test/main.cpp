/**
 * @file main.cpp
 * @author Matthew Yu (matthewjkyu@gmail.com)
 * @brief Blackbody B state machine test. Verify that given inputs result in the
 * correct state and outputs. 
 * @version 0.1.0
 * @date 2023-09-17
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

static Ticker ticker_heartbeat;
static Ticker ticker_sample_irrad;
static EventQueue queue(32 * EVENTS_EVENT_SIZE);
static enum State current_state;
bool is_error;
bool set_mode;
uint16_t sample_frequency;
Error_t sys_error;

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
 * @brief Event to toggle the heartbeat LED and send a heartbeat CAN message. 
 */
void event_heartbeat(void);

/**
 * @brief Event to measure irradiance sensor and output the result over CAN.
 */
void event_measure_irradiance_sensor(void);

/**
 * @brief Event to update the state machine and manage any sensor tickers.
 */
void event_update_state_machine(void);

/**
 * @brief Event to change our state to STATE_ERROR and output via CAN.
 */
void event_process_error(void);

/**
 * @brief Testbench to emulate inputs set by event_process_can_message.
 * 
 * @param count Current cycle count (1Hz).
 */
void testbench(char count) {
    switch (count) {
        case 5:
            // Transition into RUN state.
            printf("\nGoto RUN mode.\n");
            set_mode = true;
            queue.call(&event_update_state_machine);
            break;
        case 10:
            // Transition into STOP state.
            printf("\nGoto STOP mode.\n");
            set_mode = false;
            queue.call(&event_update_state_machine);
            break;
        case 15:
            // Transition into ERROR state.
            printf("\nGoto ERROR mode.\n");
            is_error = true;
            sys_error = ERROR_DEBUG;
            queue.call(&event_process_error);
            break;
        case 20:
            // Transition back into STOP state.
            printf("\nGoto STOP mode.\n");
            is_error = false;
            queue.call(&event_update_state_machine);
            break;
        case 25:
            // Set new frequency.
            sample_frequency = 2;
            printf("\nSet sample frequency = %i Hz.\n", sample_frequency);

            // Transition back into RUN state.
            printf("\nGoto RUN mode.\n");
            set_mode = true;
            queue.call(&event_update_state_machine);
            break;
        default:
            break;
    }
}

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

    ticker_heartbeat.attach(&handler_heartbeat, 1000ms);

    queue.dispatch_forever();
}

void handler_heartbeat(void) { 
    led_heartbeat = !led_heartbeat;
    queue.call(&event_heartbeat);
}

void handler_measure_irradiance_sensor(void) {
    queue.call(&event_measure_irradiance_sensor);
}

void event_heartbeat(void) {
    static char counter = 0;
    ++counter;

    printf(
        "Cycle %i:\tSTATE=%i,\tIS_ERROR=%i,\tSET_MODE=%i\n", 
        counter,
        current_state,
        is_error,
        set_mode
    );
    testbench(counter);
}

void event_measure_irradiance_sensor(void) {
    printf("\tSampling irradiance sensor.\n");
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
    printf("\tError: %i\n", _error);
}
