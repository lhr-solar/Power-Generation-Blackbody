#include <iostream>
#include <cstdint>
#include <mbed.h>  // Include the appropriate library for your platform

constexpr bool HIGH = true;
constexpr bool LOW = false;

class BitBangSPI {
public:
    BitBangSPI(PinName mosi, PinName miso, PinName sclk, PinName cs)
        : in(mosi), out(miso), clk(sclk), chipselect(cs) {}

    uint8_t write(uint8_t data) {
        uint8_t receivedData = 0;

        for (int i = 7; i >= 0; --i) {
            // Set in (Master Out Slave In) line
            in = (data & (1 << i)) != 0;

            // Rising edge on the clock
            clk = HIGH;

            // Read out (Master In Slave Out) line
            receivedData |= (out.read() << i);

            // Falling edge on the clock
            clk = LOW;
        }

        return receivedData;
    }

    void select() {
        // Optional: Set chipselect (Chip Select) line low to enable the slave device
        chipselect = LOW;
    }

    void deselect() {
        // Optional: Set chipselect (Chip Select) line high to disable the slave device
        chipselect = HIGH;
    }

private:
    DigitalOut in;
    DigitalIn out;
    DigitalOut clk;
    DigitalOut chipselect;
};



