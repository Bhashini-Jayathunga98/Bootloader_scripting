# PCIe
set_property PACKAGE_PIN AF5 [get_ports {diff_clock_rtl_1_clk_n[0]}]
set_property PACKAGE_PIN AF6 [get_ports {diff_clock_rtl_1_clk_p[0]}]

set_property PACKAGE_PIN AP2 [get_ports {pcie_7x_mgt_rtl_0_rxp[0]}]
set_property PACKAGE_PIN AP1 [get_ports {pcie_7x_mgt_rtl_0_rxn[0]}]
set_property PACKAGE_PIN AN4 [get_ports {pcie_7x_mgt_rtl_0_txp[0]}]
set_property PACKAGE_PIN AN3 [get_ports {pcie_7x_mgt_rtl_0_txn[0]}]

# UART
set_property PACKAGE_PIN E27 [get_ports uart_rtl_0_rxd]
set_property IOSTANDARD LVCMOS18 [get_ports uart_rtl_0_rxd]
set_property PACKAGE_PIN F27 [get_ports uart_rtl_0_txd]
set_property IOSTANDARD LVCMOS18 [get_ports uart_rtl_0_txd]

set_property CONFIG_MODE SPIx4 [current_design]

set_property BITSTREAM.CONFIG.SPI_32BIT_ADDR YES [current_design]
set_property BITSTREAM.CONFIG.SPI_BUSWIDTH 4 [current_design]

set_property BITSTREAM.CONFIG.NEXT_CONFIG_ADDR 32'h00000000 [current_design]

#set_property BITSTREAM.CONFIG.TIMER_CFG 32'h1E65FBB0 [current_design]

set_property BITSTREAM.CONFIG.NEXT_CONFIG_REBOOT DISABLE [current_design]

set_property BITSTREAM.CONFIG.CONFIGRATE 51.0 [current_design]

set_property BITSTREAM.CONFIG.SPI_FALL_EDGE YES [current_design]



set_property BITSTREAM.CONFIG.TIMER_CFG 32'h06B00940 [current_design]







