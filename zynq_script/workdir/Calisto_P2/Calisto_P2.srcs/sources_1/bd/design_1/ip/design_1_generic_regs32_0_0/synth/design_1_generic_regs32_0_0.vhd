-- (c) Copyright 1995-2025 Xilinx, Inc. All rights reserved.
-- 
-- This file contains confidential and proprietary information
-- of Xilinx, Inc. and is protected under U.S. and
-- international copyright and other intellectual property
-- laws.
-- 
-- DISCLAIMER
-- This disclaimer is not a license and does not grant any
-- rights to the materials distributed herewith. Except as
-- otherwise provided in a valid license issued to you by
-- Xilinx, and to the maximum extent permitted by applicable
-- law: (1) THESE MATERIALS ARE MADE AVAILABLE "AS IS" AND
-- WITH ALL FAULTS, AND XILINX HEREBY DISCLAIMS ALL WARRANTIES
-- AND CONDITIONS, EXPRESS, IMPLIED, OR STATUTORY, INCLUDING
-- BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, NON-
-- INFRINGEMENT, OR FITNESS FOR ANY PARTICULAR PURPOSE; and
-- (2) Xilinx shall not be liable (whether in contract or tort,
-- including negligence, or under any other theory of
-- liability) for any loss or damage of any kind or nature
-- related to, arising under or in connection with these
-- materials, including for any direct, or any indirect,
-- special, incidental, or consequential loss or damage
-- (including loss of data, profits, goodwill, or any type of
-- loss or damage suffered as a result of any action brought
-- by a third party) even if such damage or loss was
-- reasonably foreseeable or Xilinx had been advised of the
-- possibility of the same.
-- 
-- CRITICAL APPLICATIONS
-- Xilinx products are not designed or intended to be fail-
-- safe, or for use in any application requiring fail-safe
-- performance, such as life-support or safety devices or
-- systems, Class III medical devices, nuclear facilities,
-- applications related to the deployment of airbags, or any
-- other applications that could lead to death, personal
-- injury, or severe property or environmental damage
-- (individually and collectively, "Critical
-- Applications"). Customer assumes the sole risk and
-- liability of any use of Xilinx products in Critical
-- Applications, subject only to applicable laws and
-- regulations governing limitations on product liability.
-- 
-- THIS COPYRIGHT NOTICE AND DISCLAIMER MUST BE RETAINED AS
-- PART OF THIS FILE AT ALL TIMES.
-- 
-- DO NOT MODIFY THIS FILE.

-- IP VLNV: digital14:mansour:generic_regs32:1.0
-- IP Revision: 1

LIBRARY ieee;
USE ieee.std_logic_1164.ALL;
USE ieee.numeric_std.ALL;

ENTITY design_1_generic_regs32_0_0 IS
  PORT (
    regout0 : OUT STD_LOGIC_VECTOR(31 DOWNTO 0);
    regout1 : OUT STD_LOGIC_VECTOR(31 DOWNTO 0);
    regout2 : OUT STD_LOGIC_VECTOR(31 DOWNTO 0);
    regout3 : OUT STD_LOGIC_VECTOR(31 DOWNTO 0);
    regout4 : OUT STD_LOGIC_VECTOR(31 DOWNTO 0);
    regout5 : OUT STD_LOGIC_VECTOR(31 DOWNTO 0);
    regout6 : OUT STD_LOGIC_VECTOR(31 DOWNTO 0);
    regout7 : OUT STD_LOGIC_VECTOR(31 DOWNTO 0);
    regout8 : OUT STD_LOGIC_VECTOR(31 DOWNTO 0);
    regout9 : OUT STD_LOGIC_VECTOR(31 DOWNTO 0);
    regout10 : OUT STD_LOGIC_VECTOR(31 DOWNTO 0);
    regout11 : OUT STD_LOGIC_VECTOR(31 DOWNTO 0);
    regout12 : OUT STD_LOGIC_VECTOR(31 DOWNTO 0);
    regout13 : OUT STD_LOGIC_VECTOR(31 DOWNTO 0);
    regout14 : OUT STD_LOGIC_VECTOR(31 DOWNTO 0);
    regout15 : OUT STD_LOGIC_VECTOR(31 DOWNTO 0);
    regin0 : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
    regin1 : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
    regin2 : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
    regin3 : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
    regin4 : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
    regin5 : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
    regin6 : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
    regin7 : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
    regin8 : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
    regin9 : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
    regin10 : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
    regin11 : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
    regin12 : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
    regin13 : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
    regin14 : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
    regin15 : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
    s00_axi_aclk : IN STD_LOGIC;
    s00_axi_aresetn : IN STD_LOGIC;
    s00_axi_awaddr : IN STD_LOGIC_VECTOR(6 DOWNTO 0);
    s00_axi_awprot : IN STD_LOGIC_VECTOR(2 DOWNTO 0);
    s00_axi_awvalid : IN STD_LOGIC;
    s00_axi_awready : OUT STD_LOGIC;
    s00_axi_wdata : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
    s00_axi_wstrb : IN STD_LOGIC_VECTOR(3 DOWNTO 0);
    s00_axi_wvalid : IN STD_LOGIC;
    s00_axi_wready : OUT STD_LOGIC;
    s00_axi_bresp : OUT STD_LOGIC_VECTOR(1 DOWNTO 0);
    s00_axi_bvalid : OUT STD_LOGIC;
    s00_axi_bready : IN STD_LOGIC;
    s00_axi_araddr : IN STD_LOGIC_VECTOR(6 DOWNTO 0);
    s00_axi_arprot : IN STD_LOGIC_VECTOR(2 DOWNTO 0);
    s00_axi_arvalid : IN STD_LOGIC;
    s00_axi_arready : OUT STD_LOGIC;
    s00_axi_rdata : OUT STD_LOGIC_VECTOR(31 DOWNTO 0);
    s00_axi_rresp : OUT STD_LOGIC_VECTOR(1 DOWNTO 0);
    s00_axi_rvalid : OUT STD_LOGIC;
    s00_axi_rready : IN STD_LOGIC
  );
END design_1_generic_regs32_0_0;

ARCHITECTURE design_1_generic_regs32_0_0_arch OF design_1_generic_regs32_0_0 IS
  ATTRIBUTE DowngradeIPIdentifiedWarnings : STRING;
  ATTRIBUTE DowngradeIPIdentifiedWarnings OF design_1_generic_regs32_0_0_arch: ARCHITECTURE IS "yes";
  COMPONENT generic_regs32 IS
    GENERIC (
      C_S00_AXI_DATA_WIDTH : INTEGER;
      C_S00_AXI_ADDR_WIDTH : INTEGER
    );
    PORT (
      regout0 : OUT STD_LOGIC_VECTOR(31 DOWNTO 0);
      regout1 : OUT STD_LOGIC_VECTOR(31 DOWNTO 0);
      regout2 : OUT STD_LOGIC_VECTOR(31 DOWNTO 0);
      regout3 : OUT STD_LOGIC_VECTOR(31 DOWNTO 0);
      regout4 : OUT STD_LOGIC_VECTOR(31 DOWNTO 0);
      regout5 : OUT STD_LOGIC_VECTOR(31 DOWNTO 0);
      regout6 : OUT STD_LOGIC_VECTOR(31 DOWNTO 0);
      regout7 : OUT STD_LOGIC_VECTOR(31 DOWNTO 0);
      regout8 : OUT STD_LOGIC_VECTOR(31 DOWNTO 0);
      regout9 : OUT STD_LOGIC_VECTOR(31 DOWNTO 0);
      regout10 : OUT STD_LOGIC_VECTOR(31 DOWNTO 0);
      regout11 : OUT STD_LOGIC_VECTOR(31 DOWNTO 0);
      regout12 : OUT STD_LOGIC_VECTOR(31 DOWNTO 0);
      regout13 : OUT STD_LOGIC_VECTOR(31 DOWNTO 0);
      regout14 : OUT STD_LOGIC_VECTOR(31 DOWNTO 0);
      regout15 : OUT STD_LOGIC_VECTOR(31 DOWNTO 0);
      regin0 : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
      regin1 : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
      regin2 : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
      regin3 : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
      regin4 : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
      regin5 : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
      regin6 : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
      regin7 : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
      regin8 : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
      regin9 : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
      regin10 : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
      regin11 : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
      regin12 : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
      regin13 : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
      regin14 : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
      regin15 : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
      s00_axi_aclk : IN STD_LOGIC;
      s00_axi_aresetn : IN STD_LOGIC;
      s00_axi_awaddr : IN STD_LOGIC_VECTOR(6 DOWNTO 0);
      s00_axi_awprot : IN STD_LOGIC_VECTOR(2 DOWNTO 0);
      s00_axi_awvalid : IN STD_LOGIC;
      s00_axi_awready : OUT STD_LOGIC;
      s00_axi_wdata : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
      s00_axi_wstrb : IN STD_LOGIC_VECTOR(3 DOWNTO 0);
      s00_axi_wvalid : IN STD_LOGIC;
      s00_axi_wready : OUT STD_LOGIC;
      s00_axi_bresp : OUT STD_LOGIC_VECTOR(1 DOWNTO 0);
      s00_axi_bvalid : OUT STD_LOGIC;
      s00_axi_bready : IN STD_LOGIC;
      s00_axi_araddr : IN STD_LOGIC_VECTOR(6 DOWNTO 0);
      s00_axi_arprot : IN STD_LOGIC_VECTOR(2 DOWNTO 0);
      s00_axi_arvalid : IN STD_LOGIC;
      s00_axi_arready : OUT STD_LOGIC;
      s00_axi_rdata : OUT STD_LOGIC_VECTOR(31 DOWNTO 0);
      s00_axi_rresp : OUT STD_LOGIC_VECTOR(1 DOWNTO 0);
      s00_axi_rvalid : OUT STD_LOGIC;
      s00_axi_rready : IN STD_LOGIC
    );
  END COMPONENT generic_regs32;
  ATTRIBUTE X_CORE_INFO : STRING;
  ATTRIBUTE X_CORE_INFO OF design_1_generic_regs32_0_0_arch: ARCHITECTURE IS "generic_regs32,Vivado 2020.1";
  ATTRIBUTE CHECK_LICENSE_TYPE : STRING;
  ATTRIBUTE CHECK_LICENSE_TYPE OF design_1_generic_regs32_0_0_arch : ARCHITECTURE IS "design_1_generic_regs32_0_0,generic_regs32,{}";
  ATTRIBUTE IP_DEFINITION_SOURCE : STRING;
  ATTRIBUTE IP_DEFINITION_SOURCE OF design_1_generic_regs32_0_0_arch: ARCHITECTURE IS "package_project";
  ATTRIBUTE X_INTERFACE_INFO : STRING;
  ATTRIBUTE X_INTERFACE_PARAMETER : STRING;
  ATTRIBUTE X_INTERFACE_INFO OF s00_axi_rready: SIGNAL IS "xilinx.com:interface:aximm:1.0 s00_axi RREADY";
  ATTRIBUTE X_INTERFACE_INFO OF s00_axi_rvalid: SIGNAL IS "xilinx.com:interface:aximm:1.0 s00_axi RVALID";
  ATTRIBUTE X_INTERFACE_INFO OF s00_axi_rresp: SIGNAL IS "xilinx.com:interface:aximm:1.0 s00_axi RRESP";
  ATTRIBUTE X_INTERFACE_INFO OF s00_axi_rdata: SIGNAL IS "xilinx.com:interface:aximm:1.0 s00_axi RDATA";
  ATTRIBUTE X_INTERFACE_INFO OF s00_axi_arready: SIGNAL IS "xilinx.com:interface:aximm:1.0 s00_axi ARREADY";
  ATTRIBUTE X_INTERFACE_INFO OF s00_axi_arvalid: SIGNAL IS "xilinx.com:interface:aximm:1.0 s00_axi ARVALID";
  ATTRIBUTE X_INTERFACE_INFO OF s00_axi_arprot: SIGNAL IS "xilinx.com:interface:aximm:1.0 s00_axi ARPROT";
  ATTRIBUTE X_INTERFACE_INFO OF s00_axi_araddr: SIGNAL IS "xilinx.com:interface:aximm:1.0 s00_axi ARADDR";
  ATTRIBUTE X_INTERFACE_INFO OF s00_axi_bready: SIGNAL IS "xilinx.com:interface:aximm:1.0 s00_axi BREADY";
  ATTRIBUTE X_INTERFACE_INFO OF s00_axi_bvalid: SIGNAL IS "xilinx.com:interface:aximm:1.0 s00_axi BVALID";
  ATTRIBUTE X_INTERFACE_INFO OF s00_axi_bresp: SIGNAL IS "xilinx.com:interface:aximm:1.0 s00_axi BRESP";
  ATTRIBUTE X_INTERFACE_INFO OF s00_axi_wready: SIGNAL IS "xilinx.com:interface:aximm:1.0 s00_axi WREADY";
  ATTRIBUTE X_INTERFACE_INFO OF s00_axi_wvalid: SIGNAL IS "xilinx.com:interface:aximm:1.0 s00_axi WVALID";
  ATTRIBUTE X_INTERFACE_INFO OF s00_axi_wstrb: SIGNAL IS "xilinx.com:interface:aximm:1.0 s00_axi WSTRB";
  ATTRIBUTE X_INTERFACE_INFO OF s00_axi_wdata: SIGNAL IS "xilinx.com:interface:aximm:1.0 s00_axi WDATA";
  ATTRIBUTE X_INTERFACE_INFO OF s00_axi_awready: SIGNAL IS "xilinx.com:interface:aximm:1.0 s00_axi AWREADY";
  ATTRIBUTE X_INTERFACE_INFO OF s00_axi_awvalid: SIGNAL IS "xilinx.com:interface:aximm:1.0 s00_axi AWVALID";
  ATTRIBUTE X_INTERFACE_INFO OF s00_axi_awprot: SIGNAL IS "xilinx.com:interface:aximm:1.0 s00_axi AWPROT";
  ATTRIBUTE X_INTERFACE_PARAMETER OF s00_axi_awaddr: SIGNAL IS "XIL_INTERFACENAME s00_axi, DATA_WIDTH 32, PROTOCOL AXI4LITE, ID_WIDTH 0, ADDR_WIDTH 7, AWUSER_WIDTH 0, ARUSER_WIDTH 0, WUSER_WIDTH 0, RUSER_WIDTH 0, BUSER_WIDTH 0, READ_WRITE_MODE READ_WRITE, HAS_BURST 0, HAS_LOCK 0, HAS_PROT 1, HAS_CACHE 0, HAS_QOS 0, HAS_REGION 0, HAS_WSTRB 1, HAS_BRESP 1, HAS_RRESP 1, SUPPORTS_NARROW_BURST 0, NUM_READ_OUTSTANDING 1, NUM_WRITE_OUTSTANDING 1, MAX_BURST_LENGTH 1, PHASE 0.000, CLK_DOMAIN design_1_xdma_0_0_axi_aclk, NUM_READ_THREADS 1, NUM_WRITE_THREADS 1, RUSER_B" & 
"ITS_PER_BYTE 0, WUSER_BITS_PER_BYTE 0, INSERT_VIP 0";
  ATTRIBUTE X_INTERFACE_INFO OF s00_axi_awaddr: SIGNAL IS "xilinx.com:interface:aximm:1.0 s00_axi AWADDR";
  ATTRIBUTE X_INTERFACE_PARAMETER OF s00_axi_aresetn: SIGNAL IS "XIL_INTERFACENAME s00_axi_aresetn, POLARITY ACTIVE_LOW, INSERT_VIP 0";
  ATTRIBUTE X_INTERFACE_INFO OF s00_axi_aresetn: SIGNAL IS "xilinx.com:signal:reset:1.0 s00_axi_aresetn RST";
  ATTRIBUTE X_INTERFACE_PARAMETER OF s00_axi_aclk: SIGNAL IS "XIL_INTERFACENAME s00_axi_aclk, ASSOCIATED_BUSIF s00_axi, ASSOCIATED_RESET s00_axi_aresetn, FREQ_TOLERANCE_HZ 0, PHASE 0.000, CLK_DOMAIN design_1_xdma_0_0_axi_aclk, INSERT_VIP 0";
  ATTRIBUTE X_INTERFACE_INFO OF s00_axi_aclk: SIGNAL IS "xilinx.com:signal:clock:1.0 s00_axi_aclk CLK";
BEGIN
  U0 : generic_regs32
    GENERIC MAP (
      C_S00_AXI_DATA_WIDTH => 32,
      C_S00_AXI_ADDR_WIDTH => 7
    )
    PORT MAP (
      regout0 => regout0,
      regout1 => regout1,
      regout2 => regout2,
      regout3 => regout3,
      regout4 => regout4,
      regout5 => regout5,
      regout6 => regout6,
      regout7 => regout7,
      regout8 => regout8,
      regout9 => regout9,
      regout10 => regout10,
      regout11 => regout11,
      regout12 => regout12,
      regout13 => regout13,
      regout14 => regout14,
      regout15 => regout15,
      regin0 => regin0,
      regin1 => regin1,
      regin2 => regin2,
      regin3 => regin3,
      regin4 => regin4,
      regin5 => regin5,
      regin6 => regin6,
      regin7 => regin7,
      regin8 => regin8,
      regin9 => regin9,
      regin10 => regin10,
      regin11 => regin11,
      regin12 => regin12,
      regin13 => regin13,
      regin14 => regin14,
      regin15 => regin15,
      s00_axi_aclk => s00_axi_aclk,
      s00_axi_aresetn => s00_axi_aresetn,
      s00_axi_awaddr => s00_axi_awaddr,
      s00_axi_awprot => s00_axi_awprot,
      s00_axi_awvalid => s00_axi_awvalid,
      s00_axi_awready => s00_axi_awready,
      s00_axi_wdata => s00_axi_wdata,
      s00_axi_wstrb => s00_axi_wstrb,
      s00_axi_wvalid => s00_axi_wvalid,
      s00_axi_wready => s00_axi_wready,
      s00_axi_bresp => s00_axi_bresp,
      s00_axi_bvalid => s00_axi_bvalid,
      s00_axi_bready => s00_axi_bready,
      s00_axi_araddr => s00_axi_araddr,
      s00_axi_arprot => s00_axi_arprot,
      s00_axi_arvalid => s00_axi_arvalid,
      s00_axi_arready => s00_axi_arready,
      s00_axi_rdata => s00_axi_rdata,
      s00_axi_rresp => s00_axi_rresp,
      s00_axi_rvalid => s00_axi_rvalid,
      s00_axi_rready => s00_axi_rready
    );
END design_1_generic_regs32_0_0_arch;
