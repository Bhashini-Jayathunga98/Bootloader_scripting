#!/bin/bash

# Bootloader for Zynq---------------------------------------------------------------------------------------------------

# Set current project directory
current_project_directory="/home/bhashini/Desktop/Bootloader_Scripting/Scripting/zynq_script"

# Paths to the TCL scripts
tcl_file="$current_project_directory/scripts/Create_Vitis_new.tcl"

/tools/Xilinx/Vitis/2020.1/bin/xsct "$tcl_file"