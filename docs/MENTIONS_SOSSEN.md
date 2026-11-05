# Sossen protocol references and attribution

The compact C++ Tuya 3.5 state machine and datapoint reader were written for this firmware. Their wire format, session negotiation, telemetry stream activation and Sossen register mapping use the following MIT-licensed references:

- TinyTuya, https://github.com/jasonacox/tinytuya — Jason Cox.
- sossen-ha, https://github.com/caveman2024/sossen-ha — caveman2024; community mapping by flrs-94 and Loudramin.
- SOSSEN Direct, https://github.com/ritonbrunis-lab/sossen-direct — ritonbrunis-lab.

No Python runtime or Home Assistant code is embedded in the ESP32 binary. No power-limit/protection control was ported. Cryptographic operations use the firmware's existing mbedTLS library. Test vectors use fictitious keys and IDs.

## MIT License

Copyright (c) 2024 Jason Cox

Copyright (c) 2026 caveman2024

Copyright (c) 2026 ritonbrunis-lab

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
