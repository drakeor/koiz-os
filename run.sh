# I just use this for X-Server
export LIBGL_ALWAYS_INDIRECT=Yes
export DISPLAY=localhost:0.0

# Build bootsector
cd bootsector
./build.sh
cd ..

# Run in QEMU
qemu-system-x86_64 bin/bootsect.bin