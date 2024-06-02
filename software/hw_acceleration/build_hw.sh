# Check for at least two arguments
if [ "$#" -lt 2 ]; then
    echo "Usage: $0 kernel_name hls.cfg"
    exit 1
fi

source /home/aleksi/xilinx/Vitis/2023.2/settings64.sh

# Extract the output shared library name and the source files
kernel_name="$1"
config_file="$2"

rm -rf ./compiled_kernels/$kernel_name
mkdir ./compiled_kernels/$kernel_name

# Compile the PL hardware
v++ -c --mode hls --platform /home/aleksi/wheel_loader_kr260/block_design_wrapper.xsa --config $config_file --work_dir hw
v++ -l -t hw --platform /home/aleksi/wheel_loader_kr260/block_design_wrapper.xsa "./hw/$kernel_name.xo" -o "./compiled_kernels/$kernel_name/$kernel_name.xclbin"

cp ../pl.dtbo ./compiled_kernels/$kernel_name
cp ../shell.json ./compiled_kernels/$kernel_name

echo "Created FPGA binaries"
