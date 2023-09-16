/**
 * @file state_machine.cpp
 * @author Matthew Yu (matthewjkyu@gmail.com)
 * @brief Manages the state machine for the Blackbody A. Documentation at SYSTEM_DESIGN.md. 
 * @version 0.1.0
 * @date 09-13-23
 */
#include "./state_machine.h"

StateMachine::StateMachine(const uint32_t frequency) {
    _tick_freq = frequency;
}

void StateMachine::start(void) {
    _ticker.attach(&_step, _tick_freq);
}

void StateMachine::stop(void) {
    _ticker.detach();
}

enum State StateMachine::getState(void) const {
    return _current_state;
}

void StateMachine::setState(const enum State state) {
    _current_state = state;
}

void StateMachine::_step(void) {

}
