# Reflection – Experiment 9

This experiment combines a hardware-level production-line illustration in Proteus with a separate SystemC Transaction Level Modeling (TLM) model. The production-line concept contains three conveyor stages, processing/inspection/packaging stations, buffers, a controller, indicators, an LCD and an alarm.

The SystemC source defines an initiator and target connected by TLM sockets. The initiator creates a generic payload containing product identifier **555** and sends a WRITE transaction through `b_transport()`. The target validates the payload and models the product travelling through three conveyors, stations and buffers. SystemC waits represent stage delays.

The latest supplied Proteus running capture shows the simulator status as `ANIMATING`, the LCD text `SYSTEM RUNNING` / `SPEED: NORMAL`, and several illuminated status LEDs. A second screenshot gives a full view of the schematic. This is visual evidence of an active Proteus simulation view, but it does not document every button, sensor, buffer-full or fault test. Proteus circuit behavior and SystemC TLM communication are separate parts of this experiment; the Proteus circuit does not execute SystemC sockets.

## Verification record

- [ ] Arduino sketch compiles for Arduino UNO.
- [x] Latest Proteus screenshot shows an active/animating simulation view and production-line status on the LCD.
- [ ] START, STOP and RESET buttons are individually tested.
- [ ] Each product sensor and buffer/fault indicator is individually tested.
- [ ] SystemC TLM project configures and compiles on Windows/MSYS2.
- [ ] Actual SystemC output confirms initiator sends `555`.
- [ ] Target receives and processes the payload through all three stages.
- [ ] Target returns `TLM_OK_RESPONSE`.

**SystemC evidence status:** The included Python trace and timeline image are explicitly marked as reference previews, not genuine SystemC runtime evidence. Run `code/systemc_tlm/run_systemc.ps1` to create the actual `output/systemc_tlm_run.txt`.

**Proteus evidence status:** The two latest supplied screenshots are included in `proteus/` and `output/`. They show the simulation in an active/animating state and the LCD status, but no separate error-panel screenshot or complete test matrix was supplied.

**What I learned:** __________________________________________________

**Problems faced and corrections:** ___________________________________

**Faculty Signature:** __________________________
