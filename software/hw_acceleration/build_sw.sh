# Check for at least two arguments
if [ "$#" -lt 2 ]; then
    echo "Usage: $0 <output_shared_library.so> <source_file1.cpp> [<source_file2.cpp> ...]"
    exit 1
fi

unset LD_LIBRARY_PATH
source /home/aleksi/xilinx/Vitis/2023.2/settings64.sh
source environment-setup-cortexa72-cortexa53-xilinx-linux
export ROOTFS=/home/aleksi/wheel_loader_kr260/linux_os/images/linux/
export SYSROOT=$SDKTARGETSYSROOT

# Extract the output shared library name and the source files
OUTPUT_SHARED_LIB="$1"
shift
SOURCE_FILES="$@"

# Compile the source files into a shared object
$CXX -fPIC -std=c++17 -Wall -O3 -fmessage-length=0 -shared -o "compiled_apps/$OUTPUT_SHARED_LIB" $SOURCE_FILES -I/home/aleksi/xilinx/Vitis_HLS/2023.2/include -Ivitis_include -I$SYSROOT/usr/include/opencv4 -I$SYSROOT/usr/include/xrt -LSYSROOT/usr/lib -lxrt_coreutil -pthread --sysroot=$SYSROOT
echo "Shared library created: $OUTPUT_SHARED_LIB"

