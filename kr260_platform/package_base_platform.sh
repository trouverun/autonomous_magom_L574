rm -rf ./base_platform
mkdir base_platform
cp ./dtg_output/dtg_output/kria_kr260/psu_cortexa53_0/device_tree_domain/bsp/pl.dtbo ./base_platform/pl.dtbo
cp ./block_design_wrapper.bit ./base_platform/pl.bit.bin
touch base_platform/shell.json
echo "{
    \"shell_type\" : \"XRT_FLAT\",
    \"num_slots\": \"1\"
}" >> base_platform/shell.json
