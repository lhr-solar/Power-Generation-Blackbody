/**
 * @file main.cpp
 * @author Matthew Yu (matthewjkyu@gmail.com) & Krish Parikh (krishparikh@utexas.edu)
 * @brief Main program for Blackbody A. Measures temperature and light readings
 * across the solar array. Documentation at SYSTEM_DESIGN.md.
 * @version 0.1.0
 * @date 09-13-23
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
#include "MAX31865_BitBangEnabled.h"  //RTD Sensor
#include "TSL2591.hpp"  
#include <cstdio>

#define __LOOPBACK__      0
#define NUM_IRRAD_SENSORS 1
#define NUM_TEMP_SENSORS 7
#define CAN_HEARTBEAT   0x620
#define CAN_SET_MODE    0x621
#define CAN_BB_FAULT    0x622
#define CAN_ACK_FAULT   0x623
#define CAN_RTD_CONF    0x624
#define CAN_IRR_CONF    0x625
#define CAN_RTD_MEAS    0x626
#define CAN_IRR_MEAS    0x627

#define debug 0

enum State {
    STATE_STOP = 0,
    STATE_RUN = 1,
    STATE_ERROR = 2
};

DigitalOut led_heartbeat(D1);
DigitalOut led_tracking(D0);
DigitalOut led_error(D3);
CAN can(D10, D2);

enum State current_state;
bool is_error;
bool set_mode;
bool ack_fault;

static I2C i2c1(I2C_SDA, I2C_SCL);
static TSL2591 irrad(&i2c1, TSL2591_ADDR);
typedef struct IrradianceSensors {
    uint8_t active_sensors_packed;
    TSL2591* sensors[NUM_IRRAD_SENSORS] = {&irrad}; // TODO: fix this init DONE
    uint16_t raw_sensor_vals[NUM_IRRAD_SENSORS];
    uint16_t sample_frequency;
} IrradianceSensors;

MAX31865_RTD rtd0(MAX31865_RTD::RTD_PT100, D12, D11, D13, A6);
MAX31865_RTD rtd1(MAX31865_RTD::RTD_PT100, D12, D11, D13, A4);  // hates irrad
MAX31865_RTD rtd2(MAX31865_RTD::RTD_PT100, D12, D11, D13, A3); 
MAX31865_RTD rtd3(MAX31865_RTD::RTD_PT100, D12, D11, D13, A0); 
// MAX31865_RTD rtd4(MAX31865_RTD::RTD_PT100, D12, D11, D13, A7);  // hates print
MAX31865_RTD rtd5(MAX31865_RTD::RTD_PT100, D12, D11, D13, A5);  // hates irrad
MAX31865_RTD rtd6(MAX31865_RTD::RTD_PT100, D12, D11, D13, A2); 
MAX31865_RTD rtd7(MAX31865_RTD::RTD_PT100, D12, D11, D13, A1); 
typedef struct TemperatureSensors {
    uint8_t active_sensors_packed;
    MAX31865_RTD* sensors[7] = {&rtd0, &rtd1, &rtd2, &rtd3, &rtd5, &rtd6, &rtd7}; // TODO: fix this init DONE
    float raw_sensor_vals[NUM_TEMP_SENSORS];
    float temps[NUM_TEMP_SENSORS];
    uint16_t sample_frequency;
} TemperatureSensors;

IrradianceSensors irradiance_sensors;
TemperatureSensors temperature_sensors;


/**
 * @brief Event to toggle the heartbeat LED and send a heartbeat CAN message. 
 */
void event_heartbeat(void);

/**
 * @brief Event to measure irradiance sensors and output the result over CAN.
 */
void event_measure_irradiance_sensors(void);

/**
 * @brief Event to measure temperature sensors and output the result over CAN.
 */
void event_measure_temp_sensors(void);

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

void measure_RTD(MAX31865_RTD*, uint8_t);

void cycle();

int main() {
    // #ifdef __LOOPBACK__
    //     can.mode(CAN::LocalTest);
    //     printf("CAN Local Test\n");
    // #endif
    if (debug) printf("Begin\n");
    led_tracking = 1;
    led_error = 0;
    current_state = STATE_RUN;
    is_error = false;
    set_mode = true;
    ack_fault = false;
    irradiance_sensors.active_sensors_packed = 1;
    temperature_sensors.active_sensors_packed = 255;
    irradiance_sensors.sample_frequency = 10;
    temperature_sensors.sample_frequency = 2;

    for (uint8_t idx = 0; idx < NUM_TEMP_SENSORS; ++idx) {
        temperature_sensors.sensors[idx]->configure( true, true, false, false, MAX31865_FAULT_DETECTION_NONE,
                true, true, 0x0000, 0x7fff );
    }

    while (1) {
        event_process_can_message();
        if (current_state == STATE_RUN) cycle();
    }

}

void event_heartbeat(void) {
    led_heartbeat = !led_heartbeat;
    static char counter = 0;
    if (debug) printf("Heartbeat, State: %d\n", current_state);
    CANMessage message(CAN_HEARTBEAT, &counter, 1);
    ++counter;
    can.write(message);
}

void cycle() {
                                                    // Time 0
    event_heartbeat();                              // Heartbeat
    measure_RTD(temperature_sensors.sensors[0], 0); // RTD0
    event_measure_irradiance_sensors();             // Irrad
    osDelay(62);                                    // Time 62
    measure_RTD(temperature_sensors.sensors[1], 1); // RTD1
    osDelay(38);                                    // Time 100
    event_measure_irradiance_sensors();             // Irrad
    osDelay(25);                                    // Time 125
    measure_RTD(temperature_sensors.sensors[2], 2); // RTD2
    osDelay(62);                                    // Time 187
    measure_RTD(temperature_sensors.sensors[3], 3); // RTD3
    osDelay(13);                                    // Time 200
    event_measure_irradiance_sensors();             // Irrad
    osDelay(50);                                    // Time 250
    // measure_RTD(temperature_sensors.sensors[4], 4); // RTD4
    osDelay(50);                                    // Time 300
    event_measure_irradiance_sensors();             // Irrad
    osDelay(12);                                    // Time 312
    measure_RTD(temperature_sensors.sensors[5], 5); // RTD5
    osDelay(63);                                    // Time 375
    measure_RTD(temperature_sensors.sensors[6], 6); // RTD6
    osDelay(25);                                    // Time 400
    event_measure_irradiance_sensors();             // Irrad
    osDelay(37);                                    // Time 437
    // measure_RTD(temperature_sensors.sensors[7], 7); // RTD7
    osDelay(63);                                    // Time 500
    measure_RTD(temperature_sensors.sensors[0], 0); // RTD0
    event_measure_irradiance_sensors();             // Irrad
    osDelay(62);                                    // Time 562
    measure_RTD(temperature_sensors.sensors[1], 1); // RTD1
    osDelay(38);                                    // Time 600
    event_measure_irradiance_sensors();             // Irrad
    osDelay(25);                                    // Time 625
    measure_RTD(temperature_sensors.sensors[2], 2); // RTD2
    osDelay(62);                                    // Time 687
    measure_RTD(temperature_sensors.sensors[3], 3); // RTD3
    osDelay(13);                                    // Time 700
    event_measure_irradiance_sensors();             // Irrad
    osDelay(50);                                    // Time 750
    // measure_RTD(temperature_sensors.sensors[4], 4); // RTD4
    osDelay(50);                                    // Time 800
    event_measure_irradiance_sensors();             // Irrad
    osDelay(12);                                    // Time 812
    measure_RTD(temperature_sensors.sensors[5], 5); // RTD5
    osDelay(63);                                    // Time 875
    measure_RTD(temperature_sensors.sensors[6], 6); // RTD6
    osDelay(25);                                    // Time 900
    event_measure_irradiance_sensors();             // Irrad
    osDelay(37);                                    // Time 937
    // measure_RTD(temperature_sensors.sensors[7], 7); // RTD7
    osDelay(63);                                    // Time 1000
}

void measure_RTD(MAX31865_RTD* sensor, uint8_t idx) {
    float tempbuffer;
    float temperature;
    if (temperature_sensors.active_sensors_packed >> idx & 0x1) {
        sensor->read_all();
        tempbuffer = sensor->temperature();
        if(tempbuffer > -300.0 && tempbuffer < 150000.0){
            temperature = tempbuffer;
        }
        if (debug) printf("Sensor %d: %f C\n", idx, temperature);
    }

    struct data {
        uint8_t idx;
        float value;
    } data = {
        .idx = idx,
        .value = temperature
    };

    if (can.write(CANMessage(CAN_RTD_MEAS, (uint8_t*) &data, 5))) {
        #ifdef __LOOPBACK__
            printf("Temperature message sent by %i\n", idx);
            event_process_can_message();
        #endif
    }
}

void event_measure_irradiance_sensors(void) {
    /**
     * @brief For every active sensor in active_sensors_packed, perform the
     * measurement and load into a buffer.
     * 
     * Then convert the value into a calibrated W/m^2 and post on CAN.
     */
     if (debug) printf("Measure Irrad\n");
    for (uint8_t idx = 0; idx < NUM_IRRAD_SENSORS; ++idx) {
        // Sensor is active if the bit associated with the idx is 1
        if (irradiance_sensors.active_sensors_packed >> idx & 0x1) {
            // TODO: Measure sensor DONE
            irrad.getALS();
            irrad.calcLux();
            uint16_t ch0counts = irrad.full;
            uint16_t ch1counts = irrad.ir;

            // This particular metric comes from Re, Irradiance responsivity
            // from Figure TSL2591 – 9. 
            double ch0irradiance = (double) ch0counts / 6024; // uW/cm^2
            double ch1irradiance = (double) ch1counts / 1003; // uW/cm^2
            double avgIrradiance = (ch0irradiance + ch1irradiance) / 2; // uW/cm^2
            avgIrradiance *= 1000.0; // 1000 uW/cm^2
            avgIrradiance /= 100.0;  // 1000 W/m^2
            float irradiance = avgIrradiance;

            // TODO: Preprocess data and filter DONE
            struct data {
                uint8_t idx;
                float value;
            } data = {
                .idx = 0,
                .value = irradiance
            };
            if (debug) printf("\tCH0: %.3f w/m^2\tCH1: %.3f w/m^2\n", ch0irradiance, ch1irradiance);
            // Output on CAN
            if (can.write(CANMessage(CAN_IRR_MEAS, (uint8_t*)&data, 5))) {
                #ifdef __LOOPBACK__
                    printf("Irradiance message sent by %i\n", idx);
                    event_process_can_message();
                #endif
            }
        }
    }
}

void event_process_can_message(void) {
    // Read message
    if (debug) {
        printf("CAN Receive\n");
    }
    CANMessage message;
    if (can.read(message, 0)) {
        #ifdef __LOOPBACK__
            printf("Message Recieved: ID %i, %d\n", message.id, message.data[0]);
        #endif
    }
    uint32_t can_id = message.id;
    switch (can_id) {
        case CAN_SET_MODE:
            // TODO: change state machine mode. DONE
            if (message.data[0] == 0x01) {
                set_mode = true;
            } else {
                set_mode = false;
            }
            event_update_state_machine();
            break;
        case CAN_ACK_FAULT:
            // TODO: ack fault and exit error state. DONE
            if (message.data[0] == 0x01) {
                ack_fault = true;
            } else {
                ack_fault = false;
            }
            event_update_state_machine();
            break;
        case CAN_RTD_CONF:
            // TODO: Adjust ticker period, active sensors. DONE
            temperature_sensors.active_sensors_packed = message.data[0];
            temperature_sensors.sample_frequency = message.data[1]*8 + message.data[2];
            break;
        case CAN_IRR_CONF:
            // TODO: Adjust ticker period, active sensors. DONE
            irradiance_sensors.active_sensors_packed = message.data[0];
            irradiance_sensors.sample_frequency = message.data[1]*8 + message.data[2];
            break;
        default:
            // Ignore any other CAN messages.
            break;
    }
}

void event_update_state_machine(void) {
    switch (current_state) {
        case STATE_STOP:
            // Check if CAN SET_MODE is RUN
            if (set_mode) current_state = STATE_RUN;
            // Check if error occurred
            if (is_error)  current_state = STATE_ERROR;
            break;
        case STATE_RUN:
            // Check if CAN SET_MODE is RUN
            if (!set_mode) current_state = STATE_STOP;
            // Check if error occurred
            if (is_error)  current_state = STATE_ERROR;
            break;
        case STATE_ERROR:
            if (ack_fault) current_state = STATE_STOP;
            break;
    }

    switch (current_state) {
        case STATE_STOP:
            // Turn off tracking LED
            // Turn off error LED
            // Disable measurement tasks
            led_tracking = 0;
            led_error = 0;
            break;
        case STATE_RUN:
            // Turn on tracking LED
            // Turn off error LED
            // Enable measurement tasks
            led_tracking = 1;
            led_error = 0;
            break;
        case STATE_ERROR:
            // Turn on error LED
            // Turn off tracking LED
            // Disable measurement tasks
            led_error = 1;
            led_tracking = 0;
            break;
    }
}

void event_process_error(void) {
    // figure out what error
    // output on CAN
    uint16_t error = 0; // TODO: error class
    can.write(CANMessage(CAN_BB_FAULT, (uint8_t*)&error, 2));

    // update state machine
    is_error = true;
    event_update_state_machine();
}