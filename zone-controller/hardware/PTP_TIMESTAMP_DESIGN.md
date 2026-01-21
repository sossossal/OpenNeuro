# PTP Hardware Timestamping Circuit Design

**Date**: 2026-02-06  
**Status**: Draft for Schematic Entry  
**Target**: Zone Controller (Sheet 4)

## 1. Overview
The PTP hardware timestamping is achieved by capturing the exact moment the Start of Frame (SOF) delimiter passes the MII/RGMII interface between the MAC (RK3588) and the PHY (RTL8211FD).

## 2. Key Components

### 2.1 PHY Transceiver
*   **Part**: Realtek RTL8211FD-CG
*   **Package**: QFN-40
*   **Interface**: RGMII (Reduced Gigabit Media Independent Interface)

### 2.2 Timestamp Unit (TSU) Signals
Since RK3588 has internal MAC with PTP support, we focus on signal integrity for the PTP pins.

| Pin Name | RK3588 Pin | Function | Connection |
|----------|------------|----------|------------|
| ETH0_PTP_CLK | Pin_X1 | PTP Reference Clock Input | 50MHz XO (±20ppm) |
| ETH0_PTP_PPS | Pin_X2 | Pulse Per Second Output | To GPIO Header & LED |
| ETH0_PTP_TRIG| Pin_X3 | External Trigger Input | To GPIO Header |

## 3. Circuit Description

### 3.1 Clock Source
*   Use a **50.000 MHz Crystal Oscillator (XO)**.
*   Stability: **±10 ppm** or better (Critical for holdover performance).
*   Power: 3.3V Ultra-Low Noise LDO (e.g., LP5907).

### 3.2 PPS Output
*   **PPS LED**: Green LED connected via MOSFET buffer to visually indicate Sync status.
*   **SMA Connector**: Edge-launch SMA connector for connecting oscilloscope/logic analyzer for delay measurement.
*   **Protection**: ESD Protection diode (TVS) on the SMA line.

### 3.3 RGMII Delay Strat
*   RTL8211FD supports internal TX/RX delays.
*   **Configuration**: Set `TXDLY` and `RXDLY` strapping pins to `0` (Disable internal delay) initially.
*   **Reason**: We will control delays via Linux Device Tree (dts) for finer tuning `rgmii-id` mode.

## 4. Layout Guidelines (PCB)
*   **Length Matching**: RGMII Data (TD[0:3], RD[0:3]) matched to Clock (TXC, RXC) within **50 mils**.
*   **Impedance**: 50 Ohm single-ended.
*   **Isolation**: Keep the 50MHz PTP clock away from DC-DC converters.

## 5. Next Steps
*   Import component `RTL8211` into KiCad.
*   Wire Sheet 4 according to this spec.
