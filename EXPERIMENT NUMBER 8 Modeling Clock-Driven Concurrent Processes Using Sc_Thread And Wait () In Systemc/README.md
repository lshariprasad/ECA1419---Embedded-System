# Experiment 8 – Modeling Clock-Driven Concurrent Processes Using `SC_THREAD` and `wait()` in SystemC

<img width="1919" height="1024" alt="Screenshot 2026-11-16 143749" src="https://github.com/user-attachments/assets/649d3a1d-85a6-4cac-8141-c82f76d6d406" />

<img width="1209" height="918" alt="Screenshot 2026-11-16 143704" src="https://github.com/user-attachments/assets/8b415a18-527c-48c7-a6d6-f62d06243651" />




![Experiment](https://img.shields.io/badge/Experiment-08-blue)
![Platform](https://img.shields.io/badge/Platform-SystemC%203.0.2-green)
![Language](https://img.shields.io/badge/Language-C%2B%2B17-orange)
![Simulation](https://img.shields.io/badge/Simulation-5%20ns-purple)

## Aim

To model and simulate independent clock-driven concurrent processes in SystemC using `SC_THREAD`, `wait()`, `sc_clock`, and positive-edge sensitivity (`clk.pos()`).

## Objectives

1. Study the fundamentals of SystemC and its use in modeling concurrent hardware and embedded-system behavior.
2. Understand `SC_THREAD` and independent concurrent processes within a SystemC module.
3. Implement clock-driven concurrent processes using `SC_THREAD` and `wait()` to control process execution over simulation time.

## Software and Tools

- Windows with MSYS2 UCRT64
- GCC / G++ (MSYS2)
- SystemC 3.0.2
- CMake and Ninja
- Visual Studio Code
- Proteus 8 Professional (for the separate logic-circuit demonstration and waveform capture)

## Experiment Overview

The SystemC model contains one `sc_clock` and two independent `SC_THREAD` processes. Both threads are statically sensitive to the positive edge of the clock using `sensitive << clk.pos();`. Each thread starts by calling `wait()`, suspends, and resumes at its next positive clock edge. On every activation, it toggles its own Boolean signal.

A 1 ns clock period and a 5 ns simulation interval are configured. The program also writes a VCD waveform file (`systemc_waveform.vcd`) for viewing signal transitions in a waveform viewer.

> **Important distinction:** Proteus does not execute SystemC C++ constructs such as `SC_THREAD` or `wait()`. The Proteus 74LS74/XOR circuit included here is a separate hardware-level sequential-logic demonstration with a Logic Analyzer. The actual `SC_THREAD` / `wait()` experiment is implemented by `code/main.cpp` and must be built and run with the SystemC library.

## Repository Contents

```text
Exp_08_SystemC_SC_THREAD/
├── README.md
├── reflection.md
├── code/
│   ├── main.cpp
│   └── CMakeLists.txt
├── proteus/
│   ├── proteus_circuit_overview.png
│   └── proteus_circuit_simulation.png
└── output/
    ├── logic_analyzer_four_signals.png
    ├── logic_analyzer_capture_detail.png
    ├── logic_analyzer_with_circuit.png
    ├── proteus_simulation_log.png
    ├── logic_analyzer_initial_capture.png
    ├── logic_analyzer_setup_view.png
    ├── logic_analyzer_idle_view.png
    ├── logic_analyzer_blank_setup.png
    ├── vscode_workspace_view.png
    └── vscode_project_folder_view.png
```

## SystemC Program Description

### Clock configuration

```cpp
sc_clock clk("clk",
             sc_time(1, SC_NS),
             0.5,
             sc_time(500, SC_PS),
             true);
```

- Clock period: 1 ns
- Duty cycle: 50%
- First positive edge: 0.5 ns
- Simulation run: 5 ns

### Concurrent process registration

```cpp
SC_THREAD(process1);
sensitive << clk.pos();

SC_THREAD(process2);
sensitive << clk.pos();
```

Each `SC_THREAD` executes independently. Both are activated by the positive edge of the same clock.

### Waiting for a clock event

Within each thread:

```cpp
while (true)
{
    wait();
    // Work performed after the next positive clock edge.
}
```

`wait()` suspends the current thread. Because static sensitivity is configured with `clk.pos()`, the thread resumes when the clock has a positive edge.

## Build and Run (MSYS2 UCRT64 + PowerShell)

### 1. Install prerequisites

The tested toolchain in this workspace was:

- GCC/G++ 16.2.0
- SystemC 3.0.2
- CMake 4.4.3
- Ninja (installed with the MSYS2 CMake package)

Install packages from the **MSYS2 UCRT64** terminal:

```bash
pacman -S --needed mingw-w64-ucrt-x86_64-gcc
pacman -S --needed mingw-w64-ucrt-x86_64-systemc
pacman -S --needed mingw-w64-ucrt-x86_64-cmake
```

### 2. Ensure the UCRT64 tools are on PATH

In PowerShell:

```powershell
$toolsPath = "C:\msys64\ucrt64\bin"
$userPath = [Environment]::GetEnvironmentVariable("Path", "User")

if (($userPath -split ';') -notcontains $toolsPath) {
    [Environment]::SetEnvironmentVariable("Path", "$userPath;$toolsPath", "User")
}

$env:Path += ";$toolsPath"
```

### 3. Configure the project

From the experiment directory:

```powershell
cmake -S code -B build -G Ninja `
  -DCMAKE_MAKE_PROGRAM="C:/msys64/ucrt64/bin/ninja.exe" `
  -DCMAKE_CXX_COMPILER="C:/msys64/ucrt64/bin/g++.exe" `
  -DCMAKE_PREFIX_PATH="C:/msys64/ucrt64"
```

### 4. Build

```powershell
cmake --build build
```

### 5. Run from the build directory

```powershell
cd build
.\Experiment_08.exe
```

The executable should print timestamped messages from both concurrent threads and report that simulation completed at 5 ns. It should also generate `systemc_waveform.vcd` in the current working directory.

## Expected Behavior

With a 1 ns clock period and the first positive edge at 0.5 ns, positive edges occur at 0.5 ns intervals offset by whole nanoseconds: 0.5, 1.5, 2.5, 3.5, and 4.5 ns during the 5 ns run. Both threads are scheduled at each positive edge and toggle their corresponding signals.

The two thread messages may appear in either process order at the same simulation timestamp; SystemC scheduling order should not be interpreted as real parallel CPU execution. The model represents concurrent hardware processes under a discrete-event simulation kernel.

## Proteus Hardware-Level Demonstration

The Proteus schematic uses:
- 74LS74 dual D flip-flop
- 74LS86 XOR gate
- Clock source
- LEDs and current-limiting resistors
- Reset push button and pull-up resistor
- VSM Logic Analyzer

The analyzer channels shown in the capture are intended to monitor:
- A0 – Clock
- A1 – U1:A Q output
- A2 – U1:B Q output
- A3 – XOR output

Refer to the `proteus/` and `output/` folders for the supplied screenshots. The Proteus circuit is supplementary hardware-level evidence, not a SystemC runtime result.

## Result

The SystemC source is structured to demonstrate two independent `SC_THREAD` processes, each waiting for the positive edge of a common `sc_clock`, and a 5 ns simulation interval. Build and runtime success should be confirmed on the target computer by following the commands above. The supplied screenshots document the Proteus circuit and Logic Analyzer work; they do not document a completed SystemC executable run.

## Learning Outcomes

- Understood how SystemC represents concurrent hardware behavior.
- Learned how `SC_THREAD` registers independently scheduled processes.
- Used `sensitive << clk.pos()` to configure positive-edge activation.
- Used `wait()` to suspend and resume a thread on clock events.
- Configured simulation time using `sc_time` and `sc_start`.
- Generated VCD traces for waveform inspection.

## References

- [Accellera Systems Initiative – SystemC](https://systemc.org/)
- [SystemC GitHub Repository](https://github.com/accellera-official/systemc)
- [MSYS2](https://www.msys2.org/)

