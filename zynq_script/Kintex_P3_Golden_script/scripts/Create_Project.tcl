
# Bootloader for Kintex---------------------------------------------------------------------------------------------------

# ----------------------------------------------------------------------
# Set the current project directory
# ----------------------------------------------------------------------
set current_project_dir "/home/bhashni/Desktop/Bootloader_scripting/Scripting/Kintex_P3_Golden_script/"
puts "Current Project Directory is: $current_project_dir"



# ----------------------------------------------------------------------
# Set the variables
# ----------------------------------------------------------------------
set project_name "Kintex_P3_Vivado"
set project_dir "$current_project_dir/workdir/$project_name"
set output_files "$current_project_dir/output"
set part_name "xcku11p-ffva1156-2-i"
set bd_name "Kintex_Golden"
set bd_wrapper "${bd_name}_wrapper"
set xsa_output "$output_files/$bd_wrapper.xsa"
set bin_output "$output_files/$bd_wrapper.bin"
set bd_tcl "$current_project_dir/src/bd/Kintex_Golden.tcl"



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
set_property -name "part" -value "xcku11p-ffva1156-2-i" -objects $obj


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
 [file normalize "$current_project_dir/src/rtl/verilog/Kintex_Golden_wrapper.v"]\
 [file normalize "$current_project_dir/vitis/output/kintex_upgrade_010100.elf"]\
]
set imported_files [import_files -fileset sources_1 $files]

# Set 'sources_1' fileset file properties for local files
set file "kintex_upgrade_010100.elf"
set file_obj [get_files -of_objects [get_filesets sources_1] [list "*$file"]]
set_property -name "scoped_to_cells" -value "microblaze_0" -objects $file_obj
set_property -name "scoped_to_ref" -value "Kintex_Golden" -objects $file_obj
set_property -name "used_in" -value "implementation" -objects $file_obj
set_property -name "used_in_simulation" -value "0" -objects $file_obj

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
set file "[file normalize $current_project_dir/constraints/Kintex_PCIe_QSPI.xdc]"
set file_imported [import_files -fileset constrs_1 [list $file]]

# Set 'constrs_1' fileset properties
set obj [get_filesets constrs_1]
set_property -name "target_part" -value "xcku11p-ffva1156-2-i" -objects $obj





# ----------------------------------------------------------------------
# Set the IP repository path
# ----------------------------------------------------------------------
set ip_repo_path "$current_project_dir/src/ip/generic_regs"
set_property ip_repo_paths $ip_repo_path [current_project]
update_ip_catalog



# ----------------------------------------------------------------------
# Source the block design
# ----------------------------------------------------------------------
source $bd_tcl


# ----------------------------------------------------------------------
# Create block design
# ----------------------------------------------------------------------
cr_bd_Kintex_Golden ""
set_property GENERATE_SYNTH_CHECKPOINT "0" [get_files $bd_name.bd ] 
set_property REGISTERED_WITH_MANAGER "1" [get_files $bd_name.bd ] 



# ----------------------------------------------------------------------
# Generate output products for the block design
# ----------------------------------------------------------------------
generate_target all [get_files ${bd_name}.bd]
make_wrapper -files [get_files ${bd_name}.bd] -top
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
# Generating the output products
# ----------------------------------------------------------------------


# Generate the bin file
file copy -force "$current_project_dir/workdir/Kintex_P3_Vivado/Kintex_P3_Vivado.runs/impl_1/Kintex_Golden_wrapper.bit" "$output_files"
write_cfgmem -force -format bin -interface spix4 -size 32 -loadbit "up 0 $output_files/$bd_wrapper.bit" -file $bin_output

