# Experiment 4 — Simulation / Result Report

## Objective
To develop embedded firmware for HVAC control, acquire temperature through the Arduino ADC, and implement timer-interrupt PWM fan control.

## Circuit configuration
- Controller: Arduino UNO / ATmega328P
- Temperature sensor: LM35 on A0
- Fan driver: IRLZ44N MOSFET
- PWM output: D9 / OC1A
- Timer: Timer1 Fast PWM mode 14, 1 kHz
- Fan supply: external supply matched to fan rating (12 V in the current schematic)

## Procedure
1. Compile the Arduino sketch and load the generated HEX into the Proteus Arduino component.
2. Set the LM35 model to each selected temperature setpoint.
3. Run the simulation and allow the filtered reading / fan controller to settle.
4. Record ADC, filtered temperature, fan stage and PWM duty from Serial Monitor / simulation instrumentation.
5. Test both rising and falling temperature transitions to verify hysteresis.

## Results
Enter measured results in `test_results.csv`. Attach screenshots of the running simulation and Serial Monitor here.

## Discussion
Describe ADC conversion, Timer1 interrupt scheduling, PWM output, fan-stage transitions and any observed deviations. Distinguish expected values from measured values.

## Conclusion
Complete this section only after the circuit has been simulated and the observations recorded.

## Limitations / safety
This is an educational prototype, not a certified industrial HVAC safety controller. Confirm power, current, thermal, MOSFET and motor-driver ratings before real hardware operation.
