/**
 * @file blackbody_a_state_machine.h
 * @author Matthew Yu (matthewjkyu@gmail.com)
 * @brief Manages the state machine for the Blackbody A. Documentation at SYSTEM_DESIGN.md. 
 * @version 0.1.0
 * @date 09-13-23
 */
#pragma once
#include "mbed.h"

class StateMachine {
    /**
     * A state machine 
     */
    public:
        enum State {
            STATE_STOP = 0,
            STATE_RUN = 1,
            STATE_ERROR = 2
        };

        /**
         * @brief Construct a new Blackbody A State Machine object
         * 
         * @param frequency Frequency that the state machine runs at.
         */
        StateMachine(const uint32_t frequency);

        /**
         * @brief Start operation of the state machine.
         */
        void start(void);

        /**
         * @brief Stop operation of the state machine.
         */
        void stop(void);

        /**
         * @brief Get the current state object
         * 
         * @return enum State 
         */
        enum State getState(void) const;

        /**
         * @brief Set the State object
         * 
         * @param state 
         */
        void setState(const enum State state);

        // TODO: debug state
        // void printMachine(void);

    protected:
    private:
        Ticker _ticker;
        uint32_t _tick_freq;

        /**
         * @brief Current state of the state machine.
         */
        enum State _current_state = STATE_STOP;

        void _step(void);
};
