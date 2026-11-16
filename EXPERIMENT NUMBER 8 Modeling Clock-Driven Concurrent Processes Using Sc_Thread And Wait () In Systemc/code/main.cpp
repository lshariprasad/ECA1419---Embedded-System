#include <systemc>
#include <iostream>

using namespace sc_core;
using namespace std;

// Two independent clock-driven threads.
// Each thread wakes on the positive edge of clk and toggles its own signal.
SC_MODULE(ConcurrentProcesses)
{
    sc_in<bool> clk{"clk"};

    sc_signal<bool> process1_signal{"process1_signal"};
    sc_signal<bool> process2_signal{"process2_signal"};

    void process1()
    {
        while (true)
        {
            wait(); // Wait for the next event in this thread's static sensitivity list.
            bool next_value = !process1_signal.read();
            process1_signal.write(next_value);

            cout << sc_time_stamp()
                 << " | SC_THREAD 1 | process1_signal = "
                 << next_value << '\n';
        }
    }

    void process2()
    {
        while (true)
        {
            wait(); // Independent wait; this thread also wakes on clk.pos().
            bool next_value = !process2_signal.read();
            process2_signal.write(next_value);

            cout << sc_time_stamp()
                 << " | SC_THREAD 2 | process2_signal = "
                 << next_value << '\n';
        }
    }

    SC_CTOR(ConcurrentProcesses)
    {
        SC_THREAD(process1);
        sensitive << clk.pos();

        SC_THREAD(process2);
        sensitive << clk.pos();
    }
};

int sc_main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;

    // Clock period = 1 ns, duty cycle = 50%, first positive edge at 0.5 ns.
    sc_clock clk("clk",
                 sc_time(1, SC_NS),
                 0.5,
                 sc_time(500, SC_PS),
                 true);

    ConcurrentProcesses dut("dut");
    dut.clk(clk);

    // Record the clock and both output signals for waveform inspection.
    sc_trace_file* trace_file =
        sc_create_vcd_trace_file("systemc_waveform");

    sc_trace(trace_file, clk, "clock");
    sc_trace(trace_file, dut.process1_signal, "process1_signal");
    sc_trace(trace_file, dut.process2_signal, "process2_signal");

    cout << "============================================\n";
    cout << " Experiment 8: SystemC Concurrent Processes\n";
    cout << " Clock period       : 1 ns\n";
    cout << " Simulation duration: 5 ns\n";
    cout << "============================================\n";

    sc_start(5, SC_NS);

    cout << "--------------------------------------------\n";
    cout << "Simulation completed at " << sc_time_stamp() << '\n';

    sc_close_vcd_trace_file(trace_file);
    return 0;
}
