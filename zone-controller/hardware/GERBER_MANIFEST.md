# OpenNeuro Zone Controller - Gerber File Manifest
**Project**: OpenNeuro Zone Controller (PTP/TSN)  
**Revision**: v0.1-Alpha  
**Date**: 2026-02-03  
**PCB Fab**: JLCPCB (4-Layer Standard)

## Gerber Files for Manufacturing

### Standard Gerber RS-274X Files
| File Name | Layer | Description |
| :--- | :--- | :--- |
| `zone-controller-F_Cu.gbr` | Top Copper | L1 Signal layer (components, high-speed traces) |
| `zone-controller-In1_Cu.gbr` | Inner Layer 1 | L2 Ground plane (solid GND) |
| `zone-controller-In2_Cu.gbr` | Inner Layer 2 | L3 Power plane (3V3, 1V8, 1V0 splits) |
| `zone-controller-B_Cu.gbr` | Bottom Copper | L4 Signal layer (low-speed, bulk caps) |
| `zone-controller-F_Mask.gbr` | Top Soldermask | Green soldermask (top) |
| `zone-controller-B_Mask.gbr` | Bottom Soldermask | Green soldermask (bottom) |
| `zone-controller-F_Silkscreen.gbr` | Top Silkscreen | Component designators, logos |
| `zone-controller-B_Silkscreen.gbr` | Bottom Silkscreen | (Minimal, if any) |
| `zone-controller-Edge_Cuts.gbr` | Board Outline | PCB physical dimensions |

### Drill Files
| File Name | Format | Description |
| :--- | :--- | :--- |
| `zone-controller-PTH.drl` | Excellon | Plated through-holes (vias, component pins) |
| `zone-controller-NPTH.drl` | Excellon | Non-plated holes (mounting, mechanical) |

### Additional Files
| File Name | Description |
| :--- | :--- |
| `zone-controller-job.gbrjob` | Gerber Job file (metadata, layer stackup info) |
| `zone-controller-BOM.csv` | Bill of Materials (components list) |
| `zone-controller-CPL.csv` | Component Placement List (Pick & Place for SMT) |

## Fabrication Specifications
- **Board Size**: ~100mm x 80mm (TBD based on final layout)
- **Layers**: 4
- **Material**: FR-4 TG150
- **Thickness**: 1.6mm
- **Copper Weight**: 1oz outer, 0.5oz inner
- **Surface Finish**: ENIG (for RJ45 connector pads)
- **Min Trace/Space**: 5/5 mil
- **Min Drill**: 0.3mm

## Assembly Notes
- **Critical Components**: RK3588 SoC, RTL8211FD PHY require precise placement per layout strategy.
- **Impedance Control**: Verify 50Ω single-ended and 100Ω differential with fab house stackup calculator.
- **Testing**: PTP PPS output on TP1, RGMII signals accessible via test points for validation.

---
*Note: This is a manifest for reference. Actual Gerber generation would be performed in KiCad via "Plot" function after PCB layout completion.*
