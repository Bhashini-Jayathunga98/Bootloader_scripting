library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity generic_regs32 is
	generic (
		-- Users to add parameters here

		-- User parameters ends
		-- Do not modify the parameters beyond this line


		-- Parameters of Axi Slave Bus Interface S00_AXI
		C_S00_AXI_DATA_WIDTH	: integer	:= 32;
		C_S00_AXI_ADDR_WIDTH	: integer	:= 7
	);
	port (
		-- Users to add ports here
		regout0 : out std_logic_vector(31 downto 0);
		regout1 : out std_logic_vector(31 downto 0);
		regout2 : out std_logic_vector(31 downto 0);
		regout3 : out std_logic_vector(31 downto 0);
		regout4 : out std_logic_vector(31 downto 0);
		regout5 : out std_logic_vector(31 downto 0);
		regout6 : out std_logic_vector(31 downto 0);
		regout7 : out std_logic_vector(31 downto 0);
		regout8 : out std_logic_vector(31 downto 0);
		regout9 : out std_logic_vector(31 downto 0);
		regout10 : out std_logic_vector(31 downto 0);
		regout11 : out std_logic_vector(31 downto 0);
		regout12 : out std_logic_vector(31 downto 0);
		regout13 : out std_logic_vector(31 downto 0);
		regout14 : out std_logic_vector(31 downto 0);
		regout15 : out std_logic_vector(31 downto 0);

		regin0 : in std_logic_vector(31 downto 0);
		regin1 : in std_logic_vector(31 downto 0);
		regin2 : in std_logic_vector(31 downto 0);
		regin3 : in std_logic_vector(31 downto 0);
		regin4 : in std_logic_vector(31 downto 0);
		regin5 : in std_logic_vector(31 downto 0);
		regin6 : in std_logic_vector(31 downto 0);
		regin7 : in std_logic_vector(31 downto 0);
		regin8 : in std_logic_vector(31 downto 0);
		regin9 : in std_logic_vector(31 downto 0);
		regin10 : in std_logic_vector(31 downto 0);
		regin11 : in std_logic_vector(31 downto 0);
		regin12 : in std_logic_vector(31 downto 0);
		regin13 : in std_logic_vector(31 downto 0);
		regin14 : in std_logic_vector(31 downto 0);
		regin15 : in std_logic_vector(31 downto 0);
		
		-- User ports ends
		-- Do not modify the ports beyond this line


		-- Ports of Axi Slave Bus Interface S00_AXI
		s00_axi_aclk	: in std_logic;
		s00_axi_aresetn	: in std_logic;
		s00_axi_awaddr	: in std_logic_vector(C_S00_AXI_ADDR_WIDTH-1 downto 0);
		s00_axi_awprot	: in std_logic_vector(2 downto 0);
		s00_axi_awvalid	: in std_logic;
		s00_axi_awready	: out std_logic;
		s00_axi_wdata	: in std_logic_vector(C_S00_AXI_DATA_WIDTH-1 downto 0);
		s00_axi_wstrb	: in std_logic_vector((C_S00_AXI_DATA_WIDTH/8)-1 downto 0);
		s00_axi_wvalid	: in std_logic;
		s00_axi_wready	: out std_logic;
		s00_axi_bresp	: out std_logic_vector(1 downto 0);
		s00_axi_bvalid	: out std_logic;
		s00_axi_bready	: in std_logic;
		s00_axi_araddr	: in std_logic_vector(C_S00_AXI_ADDR_WIDTH-1 downto 0);
		s00_axi_arprot	: in std_logic_vector(2 downto 0);
		s00_axi_arvalid	: in std_logic;
		s00_axi_arready	: out std_logic;
		s00_axi_rdata	: out std_logic_vector(C_S00_AXI_DATA_WIDTH-1 downto 0);
		s00_axi_rresp	: out std_logic_vector(1 downto 0);
		s00_axi_rvalid	: out std_logic;
		s00_axi_rready	: in std_logic
	);
end generic_regs32;

architecture arch_imp of generic_regs32 is

	-- component declaration
	component generic_regs32_v1_0_S00_AXI is
		generic (
		C_S_AXI_DATA_WIDTH	: integer	:= 32;
		C_S_AXI_ADDR_WIDTH	: integer	:= 7
		);
		port (
		S_AXI_ACLK	: in std_logic;
		S_AXI_ARESETN	: in std_logic;
		S_AXI_AWADDR	: in std_logic_vector(C_S_AXI_ADDR_WIDTH-1 downto 0);
		S_AXI_AWPROT	: in std_logic_vector(2 downto 0);
		S_AXI_AWVALID	: in std_logic;
		S_AXI_AWREADY	: out std_logic;
		S_AXI_WDATA	: in std_logic_vector(C_S_AXI_DATA_WIDTH-1 downto 0);
		S_AXI_WSTRB	: in std_logic_vector((C_S_AXI_DATA_WIDTH/8)-1 downto 0);
		S_AXI_WVALID	: in std_logic;
		S_AXI_WREADY	: out std_logic;
		S_AXI_BRESP	: out std_logic_vector(1 downto 0);
		S_AXI_BVALID	: out std_logic;
		S_AXI_BREADY	: in std_logic;
		S_AXI_ARADDR	: in std_logic_vector(C_S_AXI_ADDR_WIDTH-1 downto 0);
		S_AXI_ARPROT	: in std_logic_vector(2 downto 0);
		S_AXI_ARVALID	: in std_logic;
		S_AXI_ARREADY	: out std_logic;
		S_AXI_RDATA	: out std_logic_vector(C_S_AXI_DATA_WIDTH-1 downto 0);
		S_AXI_RRESP	: out std_logic_vector(1 downto 0);
		S_AXI_RVALID	: out std_logic;
		S_AXI_RREADY	: in std_logic;
		
		regout0 : out std_logic_vector(31 downto 0);
		regout1 : out std_logic_vector(31 downto 0);
		regout2 : out std_logic_vector(31 downto 0);
		regout3 : out std_logic_vector(31 downto 0);
		regout4 : out std_logic_vector(31 downto 0);
		regout5 : out std_logic_vector(31 downto 0);
		regout6 : out std_logic_vector(31 downto 0);
		regout7 : out std_logic_vector(31 downto 0);
		regout8 : out std_logic_vector(31 downto 0);
		regout9 : out std_logic_vector(31 downto 0);
		regout10 : out std_logic_vector(31 downto 0);
		regout11 : out std_logic_vector(31 downto 0);
		regout12 : out std_logic_vector(31 downto 0);
		regout13 : out std_logic_vector(31 downto 0);
		regout14 : out std_logic_vector(31 downto 0);
		regout15 : out std_logic_vector(31 downto 0);

		regin0 : in std_logic_vector(31 downto 0);
		regin1 : in std_logic_vector(31 downto 0);
		regin2 : in std_logic_vector(31 downto 0);
		regin3 : in std_logic_vector(31 downto 0);
		regin4 : in std_logic_vector(31 downto 0);
		regin5 : in std_logic_vector(31 downto 0);
		regin6 : in std_logic_vector(31 downto 0);
		regin7 : in std_logic_vector(31 downto 0);
		regin8 : in std_logic_vector(31 downto 0);
		regin9 : in std_logic_vector(31 downto 0);
		regin10 : in std_logic_vector(31 downto 0);
		regin11 : in std_logic_vector(31 downto 0);
		regin12 : in std_logic_vector(31 downto 0);
		regin13 : in std_logic_vector(31 downto 0);
		regin14 : in std_logic_vector(31 downto 0);
		regin15 : in std_logic_vector(31 downto 0)
		
		);
	end component generic_regs32_v1_0_S00_AXI;

begin

-- Instantiation of Axi Bus Interface S00_AXI
generic_regs32_v1_0_S00_AXI_inst : generic_regs32_v1_0_S00_AXI
	generic map (
		C_S_AXI_DATA_WIDTH	=> C_S00_AXI_DATA_WIDTH,
		C_S_AXI_ADDR_WIDTH	=> C_S00_AXI_ADDR_WIDTH
	)
	port map (
		S_AXI_ACLK	=> s00_axi_aclk,
		S_AXI_ARESETN	=> s00_axi_aresetn,
		S_AXI_AWADDR	=> s00_axi_awaddr,
		S_AXI_AWPROT	=> s00_axi_awprot,
		S_AXI_AWVALID	=> s00_axi_awvalid,
		S_AXI_AWREADY	=> s00_axi_awready,
		S_AXI_WDATA	=> s00_axi_wdata,
		S_AXI_WSTRB	=> s00_axi_wstrb,
		S_AXI_WVALID	=> s00_axi_wvalid,
		S_AXI_WREADY	=> s00_axi_wready,
		S_AXI_BRESP	=> s00_axi_bresp,
		S_AXI_BVALID	=> s00_axi_bvalid,
		S_AXI_BREADY	=> s00_axi_bready,
		S_AXI_ARADDR	=> s00_axi_araddr,
		S_AXI_ARPROT	=> s00_axi_arprot,
		S_AXI_ARVALID	=> s00_axi_arvalid,
		S_AXI_ARREADY	=> s00_axi_arready,
		S_AXI_RDATA	=> s00_axi_rdata,
		S_AXI_RRESP	=> s00_axi_rresp,
		S_AXI_RVALID	=> s00_axi_rvalid,
		S_AXI_RREADY	=> s00_axi_rready,
		
		regout0 => regout0 ,
		regout1 => regout1 ,
		regout2 => regout2 ,
		regout3 => regout3 ,
		regout4 => regout4 ,
		regout5 => regout5 ,
		regout6 => regout6 ,
		regout7 => regout7 ,
		regout8 => regout8 ,
		regout9 => regout9 ,
		regout10 => regout10 ,
		regout11 => regout11 ,
		regout12 => regout12 ,
		regout13 => regout13 ,
		regout14 => regout14 ,
		regout15 => regout15 ,

		regin0 => regin0 ,
		regin1 => regin1 ,
		regin2 => regin2 ,
		regin3 => regin3 ,
		regin4 => regin4 ,
		regin5 => regin5 ,
		regin6 => regin6 ,
		regin7 => regin7 ,
		regin8 => regin8 ,
		regin9 => regin9 ,
		regin10 => regin10 ,
		regin11 => regin11 ,
		regin12 => regin12 ,
		regin13 => regin13 ,
		regin14 => regin14 ,
		regin15 => regin15 
	);

	-- Add user logic here

	-- User logic ends

end arch_imp;
