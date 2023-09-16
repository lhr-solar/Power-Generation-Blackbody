# TESTING

## Electrical Tests

1. Verify 3V3, 5V0, 9V0 power testpoints report correctly.
2. Verify that 5V0 power led turns on.

## Firmware Tests

1. LED test
   1. Liveliness - verify that Error, Tracking, Heartbeat, CAN_TX/CAN_RX LEDs turn
   on. 
2. CAN tests
   1. Loopback - HEARTBEAT CAN message can be sent and received in loopback configuration.
   2. Secondary device - HEARTBEAT CAN message can be sent and received between
      two devices.
3. Irradiance sensor tests
   1. Liveliness - verify that irradiance measurements can be taken from each
      irradiance sensor.  
   2. Replication - verify that irradiance measurements can be taken at various
      operating frequencies and determine message dropout metrics. 
   3. Accuracy - verify that the sensors are accurate within a known range of
      lighting conditions, with the appropriate calibration function. 
4. Temperature sensor tests
   1. Liveliness - verify that temperature measurements can be taken from each
      temperature sensor.  
   2. Replication - verify that temperature measurements can be taken at various
      operating frequencies and determine message dropout metrics. 
   3. Accuracy - verify that the sensors are accurate within a known range of
      temperature conditions, with the appropriate calibration function. 
5. System operation tests
   1. Verify that the state machine transitions as expected given all possible inputs.
   2. Verify that mock inputs properly trigger event generators.
   3. Verify that events associated with each event generator execute as expected
      1. CanEG event task: process input CAN messages (SET_MODE, ACK_FAULT,
         RTD_CONF, IRR_CONF) 
      2. StateMachineEG event task: transitions state and enables required EGs
      3. TempEG event task: performs RTD measurement and outputs result to CAN
      4. IrradEG event task: performs irradiance measurement and outputs result
         to CAN 
      5. Heartbeat event task: outputs heartbeat CAN message
