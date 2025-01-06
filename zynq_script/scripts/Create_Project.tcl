# Bootloader for Zynq--------------------------------------------------------------------------------------------------

# ----------------------------------------------------------------------
# Set the current project directory
# ----------------------------------------------------------------------
set current_project_dir "/home/bhashni/Desktop/Bootloader_scripting/Scripting/zynq_script/"
puts "Current Project Directory is: $current_project_dir"



# ----------------------------------------------------------------------
# Set the variables
# ----------------------------------------------------------------------
set project_name "Calisto_P2"
set project_dir "$current_project_dir/workdir/$project_name"
set output_files "$current_project_dir/output"
file mkdir $output_files -force
set part_name "xczu11eg-ffvc1760-2-i"
set bd_name_1 "design_1"
set bd_name_2 "design_2" 
set bd_wrapper "${bd_name_1}_wrapper"
set bin_output "$output_files/$bd_wrapper.bin"
set bd_1_tcl "$current_project_dir/src/bd/design_1.tcl"
set bd_2_tcl "$current_project_dir/src/bd/design_2.tcl"


# ----------------------------------------------------------------------
# Create a new project
# ----------------------------------------------------------------------
create_project $project_name $project_dir -part $part_name -force


# ----------------------------------------------------------------------
# Set the project properties
# ----------------------------------------------------------------------
set obj [current_project]
set_property -name "default_lib" -value "xil_defaultlib" -objects $obj
set_property -name "enable_vhdl_2008" -value "1" -objects $obj
set_property -name "ip_cache_permissions" -value "read write" -objects $obj
set_property -name "ip_output_repo" -value "$project_dir/$project_name.cache/ip" -objects $obj
set_property -name "mem.enable_memory_map_generation" -value "1" -objects $obj
set_property -name "part" -value "xczu11eg-ffvc1760-2-i" -objects $obj


# ----------------------------------------------------------------------
# Set the sources
# ----------------------------------------------------------------------

# Create 'sources_1' fileset 
if {[string equal [get_filesets -quiet sources_1] ""]} {
  create_fileset -srcset sources_1
}


# Set 'sources_1' fileset object
set obj [get_filesets sources_1]

# Import local files from the original project
set files [list \
 [file normalize "$current_project_dir/src/rtl/verilog/design_1_wrapper.v"]\
 [file normalize "$current_project_dir/src/rtl/verilog/design_2_wrapper.v"]\
]
set imported_files [import_files -fileset sources_1 $files]


# Set 'sources_1' fileset properties
set obj [get_filesets sources_1]
set_property -name "top" -value $bd_wrapper -objects $obj



# ----------------------------------------------------------------------
# Set the constraints
# ----------------------------------------------------------------------

# Create 'constrs_1' fileset
if {[string equal [get_filesets -quiet constrs_1] ""]} {
  create_fileset -constrset constrs_1
}

# Set 'constrs_1' fileset object
set obj [get_filesets constrs_1]

# Add constrs file and set constrs file properties
set file "[file normalize $current_project_dir/constraints/pcie.xdc]"
set file_imported [import_files -fileset constrs_1 [list $file]]

# Set 'constrs_1' fileset properties
set obj [get_filesets constrs_1]
set_property -name "target_part" -value "xczu11eg-ffvc1760-2-i" -objects $obj





# ----------------------------------------------------------------------
# Set the IP repository path
# ----------------------------------------------------------------------
set ip_repo_path "$current_project_dir/src/ip/generic_regs_repo"
set_property ip_repo_paths $ip_repo_path [current_project]
update_ip_catalog



# ----------------------------------------------------------------------
# Source the block design 1
# ----------------------------------------------------------------------
source $bd_1_tcl
cr_bd_design_1 ""
set_property GENERATE_SYNTH_CHECKPOINT "0" [get_files $bd_name_1.bd ] 
set_property REGISTERED_WITH_MANAGER "1" [get_files $bd_name_1.bd ]

# ----------------------------------------------------------------------
# Source the block design 2
# ----------------------------------------------------------------------
source $bd_2_tcl
cr_bd_design_2 ""
set_property GENERATE_SYNTH_CHECKPOINT "0" [get_files $bd_name_2.bd ] 
set_property REGISTERED_WITH_MANAGER "1" [get_files $bd_name_2.bd ]



# ----------------------------------------------------------------------
# Generate output products for the block design
# ----------------------------------------------------------------------
generate_target all [get_files ${bd_name_1}.bd]
make_wrapper -files [get_files ${bd_name_1}.bd] -top
generate_target all [get_files ${bd_name_2}.bd]
make_wrapper -files [get_files ${bd_name_2}.bd] -top
set_property top $bd_wrapper [get_filesets sources_1]
update_compile_order -fileset sources_1


# ----------------------------------------------------------------------
# Launch synthesis
# ----------------------------------------------------------------------

launch_runs synth_1 -jobs 10
wait_on_run synth_1

# ----------------------------------------------------------------------
# Launch implementation and generate bitstream
# ----------------------------------------------------------------------

launch_runs impl_1 -to_step write_bitstream -jobs 10
wait_on_run impl_1

# ----------------------------------------------------------------------
# Generating the binary file
# ----------------------------------------------------------------------

puts "Generating .bin file"
file copy -force "$current_project_dir/workdir/Calisto_P2/Calisto_P2.runs/impl_1/design_1_wrapper.bit" "$output_files"
write_cfgmem -force -format bin -size 32 -loadbit "up 0 $output_files/design_1_wrapper.bit" -file $bin_output
puts ".bin file created successfully at: $bin_output"

