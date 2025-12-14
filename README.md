# Cortex-M Signal Pipeline

This project demonstrates a bare-metal Cortex-M signal pipeline with
interrupt-driven sampling, lock-free buffering, and framed data transmission,
paired with a Go backend for stream parsing and processing.

The primary goal is to showcase low-level firmware concepts and firmware–backend
integration patterns, not production-grade emulation fidelity.

---

## Project Structure

- `firmware/` — Bare-metal Cortex-M firmware (no libc)
- `backend/` — Go application for stream parsing and validation
- `docs/` — Design notes and protocol description

---

## Firmware Overview

The firmware implements:

- Minimal startup code and linker script
- SysTick-driven periodic sampling (ISR)
- Lock-free ring buffer (ISR producer, main-loop consumer)
- Framed binary protocol: `[magic][count][seq][samples...]`
- Little-endian wire format

This architecture mirrors real embedded data acquisition systems
(e.g. ECG / EEG frontends).

### Protocol Data Unit (PDU)

The protocol uses a fixed-size 72-byte frame structure. Multi-byte integers are transmitted in **Little-Endian** byte order (LSB first).

```text
  Offset:  0      2      4                   8                                   72
           +------+------+-------------------+------------------------------------+
  Field:   | MAGIC| COUNT|    SEQUENCE ID    |         PAYLOAD (SAMPLES)          |
           +------+------+-------------------+------------------------------------+
  Type:    | u16  | u16  |       u32         |          uint16_t [32]             |
           +------+------+-------------------+------------------------------------+
  Value:   |0xABCD|  32  |   Frame Counter   |      ADC Data (64 bytes total)     |
           +------+------+-------------------+------------------------------------+
```

### Wire Representation (Example)

Since the pipeline is Little-Endian, `0xABCD` is transmitted as `CD AB`.

**Example Frame:**
- Magic: `0xABCD`
- Count: `32` samples (`0x0020`)
- Seq:   `1` (`0x00000001`)

```text
 Byte Stream:
 [ 0xCD 0xAB ]  [ 0x20 0x00 ]  [ 0x01 0x00 0x00 0x00 ]  [ 0xFF 0x0F ... ]
 ^-- Magic LSB  ^-- Count LSB  ^-- Seq LSB (Byte 0)     ^-- Sample[0] LSB
     ^-- Magic MSB   ^-- Count MSB       ^-- Seq MSB (Byte 3)
```

---

## Building the Firmware

Requires an `arm-none-eabi-gcc` toolchain.

```bash
arm-none-eabi-gcc \
-O2 -g -nostdlib \
-mcpu=cortex-m3 -mthumb \
-T firmware/linker.ld \
-o firmware/firmware.elf \
firmware/startup.s \
firmware/main.c \
firmware/protocol.c \
firmware/ring_buffer.c \
firmware/sensor.c \
firmware/systick.c
```

--- 

## Running in QEMU (Debug / Development Only)

The firmware can be executed under QEMU for instruction-level debugging
and interrupt verification.

Example (board-dependent):

```
qemu-system-arm \
  -M mps2-an385 \
  -kernel firmware/firmware.elf \
  -nographic
```

> [!NOTE]
> UART peripheral emulation for Cortex-M targets in QEMU is limited and
> board-specific. While firmware UART TX logic executes correctly, reliable
> end-to-end serial forwarding to the host is not guaranteed under emulation.
> **A real Cortex-M development board is required for fully accurate UART I/O**.

QEMU is used here primarily for:
- validating startup code
- verifying ISR execution
- debugging memory and control flow via GDB

## Go Backend
The Go backend demonstrates:
Binary stream parsing
Frame synchronization (magic + length)
Sequence tracking and validation
In real hardware scenarios, it would consume UART data via:
1. /dev/ttyUSB*
2. TCP serial bridge
3. USB CDC ACM

To run: `go run backend/main.go`
