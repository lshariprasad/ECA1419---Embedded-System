# SystemC TLM project

`main.cpp` contains an initiator/target TLM-2.0 model with a 32-bit generic payload carrying product ID `555`. The target validates the request, advances the product through three conveyor/station/buffer stages with blocking waits, and returns `TLM_OK_RESPONSE`.

## Windows build and run

From the project root in PowerShell:

```powershell
Set-ExecutionPolicy -Scope Process Bypass
.\code\systemc_tlm\run_systemc.ps1
```

The script expects MSYS2 UCRT64 under `C:\msys64\ucrt64` and writes the real execution output to `output\systemc_tlm_run.txt`.

The separate `code/reference_model/production_line_reference.py` script is only a behavioral reference; it does not use SystemC or TLM.
