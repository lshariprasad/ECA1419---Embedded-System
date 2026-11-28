# 🔌 ECA14 – Embedded Systems Laboratory

![Course](https://img.shields.io/badge/Course-ECA14-blue)
![Experiments](https://img.shields.io/badge/Experiments-17-orange)
![Progress](https://img.shields.io/badge/Completed-6%2F17-brightgreen)
![Platform](https://img.shields.io/badge/Platform-Arduino%20%7C%20LPC2148%20%7C%20ESP32%20%7C%20SystemC-green)
![Language](https://img.shields.io/badge/Language-Embedded%20C%20%7C%20C%2B%2B%20%7C%20Python-yellow)

OBE (Outcome Based Education) lab record for **ECA14 – Embedded Systems**, B.Tech Electrical and Electronics Engineering, SIMATS Engineering, Saveetha Institute of Medical and Technical Sciences.

**Pre-requisite:** ECA10 – Microprocessor and Microcontroller

---

## 📌 About This Repository

This repository tracks all 17 experiments in the ECA14 Embedded Systems lab. The files currently committed cover Experiments 1–4 and 8–9 (six experiment folders). Experiments 5–7 and 10–17 remain listed in the tracker but their experiment folders and implementation files have not yet been added here. The lab spans Arduino sensor systems, LPC2148 programming, SystemC modeling, Python test automation and FreeRTOS.

---

## 📊 Progress Tracker

**Overall Progress: 6 / 17 completed** (based on the six experiment folders currently committed)

| Exp No | Experiment Name | Platform / Tools | Status |
|:------:|-----------------|------------------|:------:|
| 1 | Arduino Uno-Based Smart Home Safety and Occupancy Monitoring System Using Gas, PIR and Ultrasonic Sensors | Arduino Uno, Proteus | ✅ Completed |
| 2 | Formal Design of an Embedded Controller Using State Machine (Statechart) Modeling – Traffic Light Controller | Arduino Uno, Proteus, FSM | ✅ Completed |
| 3 | I²C-Based Health Monitoring Node for Pulse and Temperature Acquisition | Arduino Uno, LM75AD, Pulse Sensor, Proteus | ✅ Completed |
| 4 | Industrial HVAC Control Firmware: ADC Temperature Acquisition and Timer-Interrupt PWM Fan Control | Arduino Uno, Timer1, Proteus | ✅ Completed |
| 5 | Program to Read the Switch and Display in the LEDs Using LPC2148 Kit | LPC2148, Embedded C | ✅ Completed |
| 6 | Structured Embedded C: Modular Header/Source Driver for Seven-Segment Display Using LPC2148 Kit | LPC2148, Embedded C | ✅ Completed |
| 7 | Serial Transmission and Reception Using On-Chip UART in LPC2148 Kit | LPC2148, UART0 | ✅ Completed |
| 8 | Modeling Clock-Driven Concurrent Processes Using SC_THREAD and wait() in SystemC | SystemC (C++) | ✅ Completed  |
| 9 | Transaction Level Modeling (TLM) and SystemC-Based Production Line Simulator | SystemC TLM-2.0 | ✅ Completed |
| 10 | Python Scripting for Embedded Test Automation (PySerial) and Scripted Firmware Flashing | ESP32, Python, PySerial, esptool | ✅ Completed |
| 11 | RTOS Task Creation, Task States and Priority Assignment Using FreeRTOS | FreeRTOS, ESP32 | ✅ Completed |
| 12 | Task Synchronization and Shared Data Protection Using Binary Semaphore, Counting Semaphore and Mutex Using FreeRTOS | FreeRTOS, ESP32 | ✅ Completed |
| 13 | Inter-Task Communication Using Message Queues, Mailboxes and Pipes (Stream Buffers) – Producer–Consumer Using FreeRTOS | FreeRTOS, ESP32 | ✅ Completed |
| 14 | RTOS Timer Functions and Event Groups Using FreeRTOS | FreeRTOS, ESP32 | ✅ Completed |
| 15 | Interrupt Service Routines in an RTOS Environment: Deferred Processing and Interrupt Priority Handling | FreeRTOS, ESP32 | ✅ Completed |
| 16 | Priority Inversion and Priority Inheritance Protocol for Resource Sharing Using FreeRTOS | FreeRTOS, ESP32 | ✅ Completed |
| 17 | Memory Management in RTOS: Heap Schemes, Stack High-Water Mark and Memory Pools | FreeRTOS, ESP32 | ✅ Completed |

**Legend:** ✅ Completed &nbsp;|&nbsp; 🔄 In Progress &nbsp;|&nbsp; ⬜ Pending

---

## 🧪 Experiment Details

### Part A – Arduino Sensor Systems and Modeling (Exp 1–4)

| Exp | Key Concepts | What Was Learned |
|:---:|--------------|------------------|
| 1 | MQ-2/MQ-6 gas sensor, PIR, ultrasonic sensor, threshold alerts | Combining multiple sensors in one circuit with threshold-based alert logic |
| 2 | FSM, Statechart, RED/GREEN/YELLOW states, timer events | Modeling a controller as states and timed transitions, then converting it to Embedded C |
| 3 | I²C bus, LM75AD temperature sensor, pulse counting, BPM | Acquiring temperature over I²C and calculating BPM from pulses |
| 4 | ADC sampling, Timer1 interrupt, PWM, fan driver | Temperature-dependent fan speed control with interrupt-driven PWM |

### Part B – LPC2148 (ARM7) Bare-Metal Programming (Exp 5–7)

| Exp | Key Concepts | What Was Learned |
|:---:|--------------|------------------|
| 5 | GPIO, IOPIN0, IOSET0, IOCLR0, data shifting | Configuring pins as input and output to read a switch and drive LEDs |
| 6 | Modular driver, `.h` / `.c` files, lookup table, seven-segment | Writing a reusable display driver that counts from 0 to 9 |
| 7 | UART0, baud rate, 8N1 format | Serial transmission and reception with a terminal |

### Part C – System-Level Modeling with SystemC (Exp 8–9)

| Exp | Key Concepts | What Was Learned |
|:---:|--------------|------------------|
| 8 | `sc_clock`, `SC_THREAD`, `wait()`, `clk.pos()` | Modeling clock-driven concurrent processes over simulation time |
| 9 | TLM, Initiator/Target sockets, generic payload, `b_transport()` | Transaction-level communication between modules (data 555 transfer) |

### Part D – Test Automation (Exp 10)

| Exp | Key Concepts | What Was Learned |
|:---:|--------------|------------------|
| 10 | PySerial, PASS/FAIL test cases, esptool | Automating serial testing of an ESP32 and flashing firmware with a script |

### Part E – Real-Time Operating System with FreeRTOS (Exp 11–17)

| Exp | Key Concepts | What Was Learned |
|:---:|--------------|------------------|
| 11 | Task creation, Running/Ready/Blocked/Suspended states, priorities | How priority and delay affect scheduling and execution counts |
| 12 | Binary semaphore, counting semaphore, mutex | Task synchronization and shared data protection |
| 13 | Queues, mailboxes, stream buffers, producer–consumer | FIFO message passing versus byte-stream communication |
| 14 | Software timers, timer callbacks, event groups | Periodic timers and wait-for-both synchronization with event bits |
| 15 | ISR, task notification, deferred processing, interrupt priority | Keeping ISRs short and moving heavy work into tasks |
| 16 | Priority inversion, priority inheritance, mutex | Priority change from 1 to 3 to fix inversion |
| 17 | Heap schemes, stack high-water mark, memory pools | Dynamic allocation, stack usage measurement and pool exhaustion and reuse |

---

## 🛠️ Tools and Technologies

| Category | Tools |
|----------|-------|
| Simulation | Proteus Design Suite |
| Microcontrollers | Arduino Uno (ATmega328P), LPC2148 (ARM7TDMI-S), ESP32 |
| IDEs / Compilers | Arduino IDE, Keil µVision, Flash Magic |
| System-Level Modeling | SystemC, TLM-2.0 |
| Scripting | Python 3, PySerial, esptool |
| RTOS | FreeRTOS |
| Languages | Embedded C, C++, Python |

---

## 📁 Repository Structure

The experiment folders currently committed to this repository are:

- `Experment No - 1 - Arduino Uno-Based Smart Home Safety and Occupancy Monitoring System Using Gas, PIR and Ultrasonic Sensors/`
- `EXPERIMENT NUMBER - 2 - Formal Design Of An Embedded Controller Using State Machine (Statechart) Modeling – Traffic Light Controller/`
- `EXPERMENT NO 3 I2c Based Health Monitoring Node For Pulse And Temperature Acquisition/`
- `EXPERMENT NO 4 Industrial Hvac Control Firmware Adc Temperature Acquisition/`
- `EXPERIMENT NUMBER 8 Modeling Clock-Driven Concurrent Processes Using Sc_Thread And Wait () In Systemc/`
- `EXPERIMENT NO 9 TRANSACTION LEVEL MODELING AND SYSTEMC BASED PRODUCTION LINE SIMULATOR/`
- `output/` – shared output material

The contents vary by experiment. Open the README or evidence index inside an experiment folder for its actual source files, circuit references, simulation material and run notes. Experiments 5–7 and 10–17 are planned in the tracker but are not yet represented by committed experiment folders.

---

## ▶️ How to Run

**Arduino / Proteus (Exp 1–4)**
1. Open the `.pdsprj` file in Proteus.
2. Compile the `.ino` file in Arduino IDE and export the compiled `.hex`.
3. Load the `.hex` into the Arduino Uno in Proteus and start the simulation.

**LPC2148 (Exp 5–7)**
1. Open the project in Keil µVision and build to generate the `.hex`.
2. Flash the `.hex` to the LPC2148 kit using Flash Magic.
3. For Exp 7, open a serial terminal at the configured baud rate (8N1).

**SystemC (Exp 8–9)**
```bash
g++ -I$SYSTEMC_HOME/include -L$SYSTEMC_HOME/lib-linux64 main.cpp -lsystemc -o sim
./sim
```

**Python and ESP32 (Exp 10)**
```bash
pip install pyserial esptool
python test_automation.py
```

**FreeRTOS (Exp 11–17)**
1. Open the `.ino` file in Arduino IDE with the ESP32 board package installed.
2. Select the correct board and COM port, then upload.
3. Open the Serial Monitor to observe task output.

---

## 📝 Assessment Method

Every experiment is evaluated with a **Laboratory Performance Assessment Rubric** on four levels:

| Level | Score |
|-------|:-----:|
| Excellent | 4 |
| Good | 3 |
| Satisfactory | 2 |
| Needs Improvement | 1 |

Criteria cover circuit or hardware configuration, program development, implementation, simulation or execution, and result analysis or viva. Each experiment also includes a short **Reflection** and **Faculty Signature**.

---

## 🎯 Course Outcomes

- Design and simulate sensor-based embedded systems using Arduino and Proteus.
- Program ARM7 (LPC2148) peripherals such as GPIO and UART in Embedded C.
- Model concurrent hardware and system behavior using SystemC and TLM.
- Automate embedded testing and firmware flashing with Python.
- Build multitasking applications using FreeRTOS, including synchronization, communication, timers, interrupts and memory management.

---

## 👤 Author

**Hari Prasad L S**
B.Tech Electrical and Electronics Engineering
SIMATS Engineering, Saveetha Institute of Medical and Technical Sciences, Chennai

🔗 GitHub: [github.com/lshariprasad](https://github.com/lshariprasad)

---

## 📄 License

This repository is for academic and learning purposes. Feel free to refer to the code, but please write your own for your submissions.

---

⭐ If this repository helped you, consider giving it a star!
