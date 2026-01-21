# Zone Controller Schematic Netlist & Pin Map

## Project: OpenNeuro Zone Controller (PTP/TSN)
**Revision**: v0.1-Alpha
**Date**: 2026-02-02

## 1. Stackup Strategy
**Layer Count**: 4
- **L1 (Top)**: Signal / RF (High Speed)
- **L2**: GND Plane (Solid)
- **L3**: Power Plane (3V3, 1V8, 1V2, 5V0)
- **L4 (Bottom)**: Signal / Low Speed

## 2. PTP Timestamping Circuit (PHY to MAC)

### Components
- **U1**: RK3588 (SoC)
- **U2**: RTL8211FD (Gigabit PHY)
- **X1**: 25MHz Crystal (PHY Ref)

### Netlist: RGMII + PTP Signals
| Net Name | U1 (RK3588) Pin | U2 (RTL8211) Pin | Description |
| :--- | :--- | :--- | :--- |
| **RGMII_TX_CLK** | M22 (GMAC1_TXCLK) | 35 (TXC) | Transmit Clock (125MHz) |
| **RGMII_TX_CTL** | M23 (GMAC1_TXEN) | 36 (TXCTL) | Transmit Control |
| **RGMII_TXD0** | L22 | 37 (TXD0) | Data Bit 0 |
| **RGMII_TXD1** | L23 | 38 (TXD1) | Data Bit 1 |
| **RGMII_TXD2** | K22 | 39 (TXD2) | Data Bit 2 |
| **RGMII_TXD3** | K23 | 40 (TXD3) | Data Bit 3 |
| **RGMII_RX_CLK** | N22 (GMAC1_RXCLK) | 28 (RXC) | Receive Clock (Source Sync) |
| **RGMII_RX_CTL** | N23 (GMAC1_RXDV) | 27 (RXCTL) | Receive Control |
| **RGMII_RXD[0-3]** | P22, P23, R22, R23 | 23, 24, 25, 26 | Receive Data |
| **MDC** | J22 (GMAC1_MDC) | 18 (MDC) | Management Clock |
| **MDIO** | J23 (GMAC1_MDIO) | 19 (MDIO) | Management Data |
| **PTP_PPS_OUT** | T20 (GPIO2_C3) | TP1 (Test Point) | PPS Output for Measurement |
| **PTP_TRIG_IN** | T21 (GPIO2_C4) | J3 (Header) | External Trigger Input |

## 3. Power Distribution Network (PDN)

### Regulators
- **U3 (PMIC)**: RK806-1
  - **Buck1**: 0.8V (Core) -> U1_VDD_LOG
  - **Buck2**: 1.8V (IO) -> U1_VCC_1V8, U2_DVDD_1V8
  - **LDO1**: 3.3V (Analog) -> U2_AVDD_3V3

### Decoupling
- **PHY (U2)**:
  - VDD_1V0: 10uF + 0.1uF @ Pin 32, 48
  - AVDD_3V3: 10uF + 0.1uF @ Pin 12, 29
- **SoC (U1 - GMAC domain)**:
  - VCC_1V8: 0.1uF @ Pin N20

## 4. Clocking
- **X1**: 25.000MHz +/- 10ppm CRYSTAL (SMD 3225)
  - Pin 1 -> U2_XTAL_IN
  - Pin 2 -> U2_XTAL_OUT
  - Load Caps: 12pF (C10, C11)

## 5. Layout Guidelines (Constraints)
- **RGMII**: Match length within +/- 50 mils. Series term 22R on TX lines close to U1? (RK3588 has internal bias, verify config). Use 33R if needed.
- **Microstrip**: 50 Ohm impedance for single-ended RGMII traces.
- **Reference**: Solid GND on L2 implies minimal layer transitions.
