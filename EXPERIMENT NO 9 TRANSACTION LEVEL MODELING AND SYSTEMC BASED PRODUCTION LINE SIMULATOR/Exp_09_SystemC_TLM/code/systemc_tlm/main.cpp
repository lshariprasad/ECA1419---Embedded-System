#include <systemc>
#include <tlm>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>

#include <cstdint>
#include <cstring>
#include <iostream>

using namespace sc_core;
using namespace tlm;

// -----------------------------------------------------------------------------
// Production-line components
// -----------------------------------------------------------------------------

class Conveyor : public sc_module {
public:
    explicit Conveyor(sc_module_name name) : sc_module(name) {}

    void transfer(std::uint32_t productId) const {
        std::cout << "  [" << name() << "] transporting product "
                  << productId << " at " << sc_time_stamp() << '\n';
    }
};

class ProcessingStation : public sc_module {
public:
    explicit ProcessingStation(sc_module_name name) : sc_module(name) {}

    void process(std::uint32_t productId) const {
        std::cout << "  [" << name() << "] processing product "
                  << productId << " at " << sc_time_stamp() << '\n';
    }
};

class ProductBuffer : public sc_module {
public:
    ProductBuffer(sc_module_name name, unsigned int capacity)
        : sc_module(name), capacity_(capacity), occupied_(0) {}

    bool store(std::uint32_t productId) {
        if (occupied_ >= capacity_) {
            std::cout << "  [" << name() << "] FULL - product "
                      << productId << " cannot enter\n";
            return false;
        }

        ++occupied_;
        std::cout << "  [" << name() << "] stored product " << productId
                  << " (" << occupied_ << '/' << capacity_ << ") at "
                  << sc_time_stamp() << '\n';
        return true;
    }

    void release(std::uint32_t productId) {
        if (occupied_ > 0) {
            --occupied_;
        }
        std::cout << "  [" << name() << "] released product " << productId
                  << " (" << occupied_ << '/' << capacity_ << ") at "
                  << sc_time_stamp() << '\n';
    }

private:
    unsigned int capacity_;
    unsigned int occupied_;
};

// -----------------------------------------------------------------------------
// TLM target: receives a generic payload through b_transport() and moves the
// product through three conveyors, processing stations and buffers.
// -----------------------------------------------------------------------------

class ProductionLineTarget : public sc_module {
public:
    tlm_utils::simple_target_socket<ProductionLineTarget> socket;

    Conveyor conveyor1;
    Conveyor conveyor2;
    Conveyor conveyor3;

    ProcessingStation station1;
    ProcessingStation station2;
    ProcessingStation station3;

    ProductBuffer buffer1;
    ProductBuffer buffer2;
    ProductBuffer buffer3;

    explicit ProductionLineTarget(sc_module_name name)
        : sc_module(name),
          socket("socket"),
          conveyor1("conveyor_1"),
          conveyor2("conveyor_2"),
          conveyor3("conveyor_3"),
          station1("processing_station_1"),
          station2("inspection_station_2"),
          station3("packaging_station_3"),
          buffer1("buffer_1", 2),
          buffer2("buffer_2", 2),
          buffer3("buffer_3", 2) {
        socket.register_b_transport(this, &ProductionLineTarget::b_transport);
    }

    void b_transport(tlm_generic_payload& trans, sc_time& delay) {
        if (trans.get_command() != TLM_WRITE_COMMAND) {
            trans.set_response_status(TLM_COMMAND_ERROR_RESPONSE);
            return;
        }

        if (trans.get_address() != 0) {
            trans.set_response_status(TLM_ADDRESS_ERROR_RESPONSE);
            return;
        }

        if (trans.get_data_ptr() == nullptr ||
            trans.get_data_length() != sizeof(std::uint32_t) ||
            trans.get_streaming_width() < sizeof(std::uint32_t)) {
            trans.set_response_status(TLM_BURST_ERROR_RESPONSE);
            return;
        }

        // Honour any delay annotated by the initiator.
        if (delay != SC_ZERO_TIME) {
            wait(delay);
            delay = SC_ZERO_TIME;
        }

        std::uint32_t productId = 0;
        std::memcpy(&productId, trans.get_data_ptr(), sizeof(productId));

        std::cout << "\n[TLM TARGET] Received WRITE transaction at "
                  << sc_time_stamp() << '\n'
                  << "[TLM TARGET] Generic payload data = " << productId
                  << "\n[PRODUCTION LINE] Starting product journey\n";

        // Stage 1: initial processing.
        conveyor1.transfer(productId);
        wait(1, SC_NS);
        station1.process(productId);
        wait(1, SC_NS);
        if (!buffer1.store(productId)) {
            trans.set_response_status(TLM_GENERIC_ERROR_RESPONSE);
            return;
        }
        wait(1, SC_NS);
        buffer1.release(productId);

        // Stage 2: inspection.
        conveyor2.transfer(productId);
        wait(1, SC_NS);
        station2.process(productId);
        wait(1, SC_NS);
        if (!buffer2.store(productId)) {
            trans.set_response_status(TLM_GENERIC_ERROR_RESPONSE);
            return;
        }
        wait(1, SC_NS);
        buffer2.release(productId);

        // Stage 3: packaging.
        conveyor3.transfer(productId);
        wait(1, SC_NS);
        station3.process(productId);
        wait(1, SC_NS);
        if (!buffer3.store(productId)) {
            trans.set_response_status(TLM_GENERIC_ERROR_RESPONSE);
            return;
        }
        wait(1, SC_NS);
        buffer3.release(productId);

        trans.set_response_status(TLM_OK_RESPONSE);
        std::cout << "[PRODUCTION LINE] Product " << productId
                  << " completed all three stages at " << sc_time_stamp()
                  << "\n[TLM TARGET] Response: TLM_OK_RESPONSE\n";
    }
};

// -----------------------------------------------------------------------------
// TLM initiator: creates a generic payload containing the required value 555.
// -----------------------------------------------------------------------------

class ProductionLineInitiator : public sc_module {
public:
    tlm_utils::simple_initiator_socket<ProductionLineInitiator> socket;

    SC_HAS_PROCESS(ProductionLineInitiator);

    explicit ProductionLineInitiator(sc_module_name name)
        : sc_module(name), socket("socket") {
        SC_THREAD(run_transaction);
    }

private:
    void run_transaction() {
        wait(1, SC_NS);

        std::uint32_t productId = 555;
        tlm_generic_payload payload;
        sc_time annotatedDelay = SC_ZERO_TIME;

        payload.set_command(TLM_WRITE_COMMAND);
        payload.set_address(0);
        payload.set_data_ptr(reinterpret_cast<unsigned char*>(&productId));
        payload.set_data_length(sizeof(productId));
        payload.set_streaming_width(sizeof(productId));
        payload.set_byte_enable_ptr(nullptr);
        payload.set_dmi_allowed(false);
        payload.set_response_status(TLM_INCOMPLETE_RESPONSE);

        std::cout << "\n[TLM INITIATOR] Sending product data 555 at "
                  << sc_time_stamp() << '\n';

        socket->b_transport(payload, annotatedDelay);

        if (payload.is_response_ok()) {
            std::cout << "\n[TLM INITIATOR] Target response received: OK\n"
                      << "[TLM INITIATOR] Product 555 transaction completed at "
                      << sc_time_stamp() << '\n';
        } else {
            std::cout << "\n[TLM INITIATOR] Transaction failed: "
                      << payload.get_response_string() << '\n';
        }

        sc_stop();
    }
};

int sc_main(int, char**) {
    std::cout << "==================================================\n"
              << " EXPERIMENT 9: SYSTEMC TLM PRODUCTION LINE\n"
              << "==================================================\n";

    ProductionLineTarget target("production_line_target");
    ProductionLineInitiator initiator("production_line_initiator");

    // Bind initiator socket to target socket.
    initiator.socket.bind(target.socket);

    sc_start(20, SC_NS);

    std::cout << "\nSimulation finished at " << sc_time_stamp() << '\n'
              << "==================================================\n";
    return 0;
}
