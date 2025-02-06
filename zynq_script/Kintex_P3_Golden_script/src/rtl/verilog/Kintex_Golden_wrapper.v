//Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
//--------------------------------------------------------------------------------
//Tool Version: Vivado v.2020.1 (lin64) Build 2902540 Wed May 27 19:54:35 MDT 2020
//Date        : Tue Dec  3 10:39:47 2024
//Host        : vivadolab running 64-bit Ubuntu 20.04.3 LTS
//Command     : generate_target Kintex_Golden_wrapper.bd
//Design      : Kintex_Golden_wrapper
//Purpose     : IP block netlist
//--------------------------------------------------------------------------------
`timescale 1 ps / 1 ps

module Kintex_Golden_wrapper
   (diff_clock_rtl_1_clk_n,
    diff_clock_rtl_1_clk_p,
    pcie_7x_mgt_rtl_0_rxn,
    pcie_7x_mgt_rtl_0_rxp,
    pcie_7x_mgt_rtl_0_txn,
    pcie_7x_mgt_rtl_0_txp,
    uart_rtl_0_rxd,
    uart_rtl_0_txd);
  input [0:0]diff_clock_rtl_1_clk_n;
  input [0:0]diff_clock_rtl_1_clk_p;
  input [0:0]pcie_7x_mgt_rtl_0_rxn;
  input [0:0]pcie_7x_mgt_rtl_0_rxp;
  output [0:0]pcie_7x_mgt_rtl_0_txn;
  output [0:0]pcie_7x_mgt_rtl_0_txp;
  input uart_rtl_0_rxd;
  output uart_rtl_0_txd;

  wire [0:0]diff_clock_rtl_1_clk_n;
  wire [0:0]diff_clock_rtl_1_clk_p;
  wire [0:0]pcie_7x_mgt_rtl_0_rxn;
  wire [0:0]pcie_7x_mgt_rtl_0_rxp;
  wire [0:0]pcie_7x_mgt_rtl_0_txn;
  wire [0:0]pcie_7x_mgt_rtl_0_txp;
  wire uart_rtl_0_rxd;
  wire uart_rtl_0_txd;

  Kintex_Golden Kintex_Golden_i
       (.diff_clock_rtl_1_clk_n(diff_clock_rtl_1_clk_n),
        .diff_clock_rtl_1_clk_p(diff_clock_rtl_1_clk_p),
        .pcie_7x_mgt_rtl_0_rxn(pcie_7x_mgt_rtl_0_rxn),
        .pcie_7x_mgt_rtl_0_rxp(pcie_7x_mgt_rtl_0_rxp),
        .pcie_7x_mgt_rtl_0_txn(pcie_7x_mgt_rtl_0_txn),
        .pcie_7x_mgt_rtl_0_txp(pcie_7x_mgt_rtl_0_txp),
        .uart_rtl_0_rxd(uart_rtl_0_rxd),
        .uart_rtl_0_txd(uart_rtl_0_txd));
endmodule
