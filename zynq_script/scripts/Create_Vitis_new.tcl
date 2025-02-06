# Generate Vitis Project for Zynq Bootloader------------------------------------

set project_dir "/home/bhashini/Desktop/Bootloader_Scripting/Scripting/zynq_script/vitis"
set workspace_path "$project_dir/output/zynq_ddrless_sw_upgrade_010100"
set output_files "$project_dir/output/output_files"
set work_dir "/home/bhashini/Desktop/Bootloader_Scripting/Scripting/zynq_script/vitis/workdir"
set xsa "/home/bhashini/Desktop/Bootloader_Scripting/Scripting/zynq_script/output/vivado/design_1_wrapper.xsa"
set platform_name "design_1_wrapper_010200_1"
set app_1 "fsbl_for_vivado_flash"
set app_2 "fsbl_sw_upgrade_010100"
set boot_bif_src "/home/bhashini/Desktop/Bootloader_Scripting/Scripting/zynq_script/vitis/src/fsbl_sw_upgrade_010100_system.bif"
set boot_bif "$project_dir/src/fsbl_sw_upgrade_010100_system.bif"
set boot_bin "$output_files/fsbl_sw_upgrade_010100_system.bin"


# Clean the workspace
file delete -force $workspace_path

# Set the workspace
setws $workspace_path

# Import the project to the workspace
importprojects $work_dir

# Generate the platform project
platform active $platform_name
platform config -updatehw $xsa
platform generate
puts "[platform list]"
puts "[bsp getlibs]"
puts "[app list]"

# Generate the application project 1
app build $app_1
sysproj build -name fsbl_for_vivado_flash_system 

# Generate the application project 2
app build $app_2
sysproj build -name fsbl_sw_upgrade_010100_system

#Generate the bootimage

puts "Running bootgen"
if {[catch {exec bootgen -arch zynqmp -image $boot_bif -o $boot_bin -w on} err]} {
    puts "ERROR: Bootgen failed: $err"
    exit 1
}

puts "INFO: boot.bin successfully generated at: $boot_bin"

# Copy output files to the main output directory
file copy -force $boot_bin /home/bhashini/Desktop/Bootloader_Scripting/Scripting/zynq_script/output/vitis/fsbl_sw_upgrade_010100_system.bin
file copy -force /home/bhashini/Desktop/Bootloader_Scripting/Scripting/zynq_script/vitis/output/zynq_ddrless_sw_upgrade_010100/fsbl_for_vivado_flash/Debug/fsbl_for_vivado_flash.elf /home/bhashini/Desktop/Bootloader_Scripting/Scripting/zynq_script/output/vitis/fsbl_for_vivado_flash.elf

