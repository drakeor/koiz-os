# I just use this for X-Server
export LIBGL_ALWAYS_INDIRECT=Yes
export DISPLAY=localhost:0.0

# Build bootsector (Obsolete, replaced by bootimage rust crate)
#cd bootsector
#./build.sh
#cd ..

# Build kernel
cd kernel
./build.sh
cd ..

# Run in QEMU
qemu-system-x86_64 -drive format=raw,file=kernel/target/x86_64-koiz_os/debug/bootimage-koiz_kernel.bin