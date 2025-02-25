//Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
//--------------------------------------------------------------------------------
//Tool Version: Vivado v.2020.1 (lin64) Build 2902540 Wed May 27 19:54:35 MDT 2020
//Date        : Mon Feb  5 17:03:12 2024
//Host        : board7 running 64-bit Ubuntu 20.04.5 LTS
//Command     : generate_target design_1_wrapper.bd
//Design      : design_1_wrapper
//Purpose     : IP block netlist
//--------------------------------------------------------------------------------
`timescale 1 ps / 1 ps

module design_1_wrapper
   (PCI_CLK_clk_n,
    PCI_CLK_clk_p,
    pcie_mgt_0_rxn,
    pcie_mgt_0_rxp,
    pcie_mgt_0_txn,
    pcie_mgt_0_txp);
  input [0:0]PCI_CLK_clk_n;
  input [0:0]PCI_CLK_clk_p;
  input [0:0]pcie_mgt_0_rxn;
  input [0:0]pcie_mgt_0_rxp;
  output [0:0]pcie_mgt_0_txn;
  output [0:0]pcie_mgt_0_txp;

  wire [0:0]PCI_CLK_clk_n;
  wire [0:0]PCI_CLK_clk_p;
  wire [0:0]pcie_mgt_0_rxn;
  wire [0:0]pcie_mgt_0_rxp;
  wire [0:0]pcie_mgt_0_txn;
  wire [0:0]pcie_mgt_0_txp;

  design_1 design_1_i
       (.PCI_CLK_clk_n(PCI_CLK_clk_n),
        .PCI_CLK_clk_p(PCI_CLK_clk_p),
        .pcie_mgt_0_rxn(pcie_mgt_0_rxn),
        .pcie_mgt_0_rxp(pcie_mgt_0_rxp),
        .pcie_mgt_0_txn(pcie_mgt_0_txn),
        .pcie_mgt_0_txp(pcie_mgt_0_txp));
endmodule
