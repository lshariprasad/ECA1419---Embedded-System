# Reflection – Experiment 8

In this experiment, I explored clock-driven concurrent process modeling using SystemC. I learned that an `SC_THREAD` represents an independently scheduled process within a SystemC module. By making each thread sensitive to `clk.pos()` and using `wait()`, the process suspends and resumes in response to positive clock edges.

I also learned the role of `sc_clock`, `sc_time`, and `sc_start()` in controlling simulation timing. The model uses two independent threads driven by the same clock, with each thread updating its own signal. This helped me understand how concurrent hardware behavior is represented through a discrete-event simulation kernel.

Alongside the SystemC source, I assembled a separate 74LS74 and 74LS86 sequential-logic demonstration in Proteus and inspected clock and logic signals using the VSM Logic Analyzer. This provided a visual hardware-level comparison with the concepts being modeled in SystemC.

**Important note:** The Proteus circuit is not executing SystemC code. The actual SystemC implementation is in `code/main.cpp`; its successful compilation and 5 ns runtime should be recorded after running the provided build instructions.
