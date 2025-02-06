#!/bin/bash

# Bootloader for Kintex---------------------------------------------------------------------------------------------------

current_project_directory="/home/bhashni/Desktop/Bootloader_scripting/Scripting/Kintex_P3_Golden_script"

# Paths to the TCL scripts
tcl_file="$current_project_directory/scripts/Create_Project.tcl"

/tools/Xilinx/Vivado/2020.1/bin/vivado -mode batch -source "$tcl_file"