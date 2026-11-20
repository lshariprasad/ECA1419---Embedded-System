#!/usr/bin/env python3
"""Reference execution trace for the Exp 09 production-line sequence.

This is a Python behavioral reference model, NOT a SystemC/TLM runtime.
Use run_systemc.ps1 to build and execute the real SystemC TLM model.
"""
from pathlib import Path

PRODUCT_ID = 555
STAGES = [
    ("conveyor_1", "transporting"),
    ("processing_station_1", "processing"),
    ("buffer_1", "store"),
    ("buffer_1", "release"),
    ("conveyor_2", "transporting"),
    ("inspection_station_2", "processing"),
    ("buffer_2", "store"),
    ("buffer_2", "release"),
    ("conveyor_3", "transporting"),
    ("packaging_station_3", "processing"),
    ("buffer_3", "store"),
    ("buffer_3", "release"),
]

def make_trace():
    lines = [
        "==================================================",
        " EXPERIMENT 9: SYSTEMC TLM PRODUCTION LINE",
        "==================================================",
        "",
        "[TLM INITIATOR] Sending product data 555 at 1 ns",
        "",
        "[TLM TARGET] Received WRITE transaction at 1 ns",
        "[TLM TARGET] Generic payload data = 555",
        "[PRODUCTION LINE] Starting product journey",
    ]
    t = 1
    for name, action in STAGES:
        if action == "transporting":
            lines.append(f"  [{name}] transporting product {PRODUCT_ID} at {t} ns")
            t += 1
        elif action == "processing":
            lines.append(f"  [{name}] processing product {PRODUCT_ID} at {t} ns")
            t += 1
        elif action == "store":
            lines.append(f"  [{name}] stored product {PRODUCT_ID} (1/2) at {t} ns")
            t += 1
        else:
            lines.append(f"  [{name}] released product {PRODUCT_ID} (0/2) at {t} ns")
            # Each buffer release consumes the final wait for that stage.
            # Conveyor 2/3 begin at 4/7 ns in the C++ target model, so no increment here.
    lines += [
        "[PRODUCTION LINE] Product 555 completed all three stages at 10 ns",
        "[TLM TARGET] Response: TLM_OK_RESPONSE",
        "",
        "[TLM INITIATOR] Target response received: OK",
        "[TLM INITIATOR] Product 555 transaction completed at 10 ns",
        "",
        "Simulation finished at 10 ns",
        "==================================================",
    ]
    return "\n".join(lines) + "\n"

if __name__ == "__main__":
    trace = make_trace()
    out = Path(__file__).resolve().parents[2] / "output" / "reference_model_run_NOT_SYSTEMC.txt"
    out.write_text(trace, encoding="utf-8")
    print(trace, end="")
    print(f"Reference trace saved to: {out}")
