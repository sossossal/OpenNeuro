# OpenNeuro Zone Controller - PCB Layout Strategy
**Revision**: v0.1 | **Date**: 2026-02-03

## 1. Stackup Design (4-Layer JLCPCB Standard)
To minimize cost while ensuring signal integrity for Gigabit Ethernet.

| Layer | Type | Copper | Description |
| :--- | :--- | :--- | :--- |
| **L1 (Top)** | Signal | 1oz | High-speed signals (RGMII, DIFF pairs), RF, Components |
| **L2 (In 1)**| Plane | 0.5oz | **Ground (GND)** Reference. Solid plane. No splits under HS traces. |
| **L3 (In 2)**| Plane | 0.5oz | **Power (PWR)**. Split planes: 3V3 (Main), 1V8, 1V0. |
| **L4 (Bot)** | Signal | 1oz | Low-speed signals, strapping resistors, bulk capacitors. |

## 2. Impedance Control
*Targeting JLC7628 Stackup*

*   **Single-Ended (RGMII, CLK)**: 50 Ω ±10%
    *   Trace Width: ~5-6 mil (Check calculator for specific prepreg)
    *   Reference: L2 GND.
*   **Differential (MDI - RJ45)**: 100 Ω ±10%
    *   Trace Width: ~4-5 mil
    *   Spacing: ~6-8 mil

## 3. Component Placement Strategy

### A. Ethernet PHY (RTL8211FD)
*   **Placement**: Place < 25mm from RK3588 SoC MAC pins if possible.
*   **Orientation**: Align RGMII pins towards SoC to avoid crossing.
*   **Decoupling**: Place 0.1uF caps immediately at power pins (Avdd, RegOut).

### B. Magnetics / RJ45
*   **Isolation**: Keep > 20mm separation between PHY and RJ45 if using discrete magnetics.
*   **Void**: Remove L2/L3 copper under the magnetics/RJ45 area to improve isolation (ESD).

## 4. Routing Guidelines

### RGMII Bus (125MHz DDR)
*   **Length Matching**: Match all Data (TXD[0:3]), CTL (TX_CTL), and CLK (TX_CLK) within **±50 mil**.
*   **Skew**: PCB skew should be minimal. RGMII v2.0 requires internal delay (ID), ensure RTL8211FD `TX_DELAY` and `RX_DELAY` straps are configured correctly (or software enabled) so PCB doesn't need huge serpentine traces.
*   **Series Termination**: Place 22Ω resistors on TX lines *close to source* (SoC for TX, PHY for RX_CLK) to reduce ringing.

### Power & Thermal
*   **RK3588**: Add thermal vias pad for heat dissipation.
*   **Vias**: Use minimum 0.3mm/0.6mm vias for power stitching.

## 5. PTP Specifics
*   **PPS Output**: Route `PTP_PPS` signal to a dedicated SMA or Test Point with strict isolation. Keep away from DC-DC switchers.
*   **Oscillator**: The 25MHz crystal for PHY is critical for PTP accuracy. Use a tight loop for Crystal -> PHY. Place Ground Via fence around Crystal.
