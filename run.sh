# I just use this for X-Server
export LIBGL_ALWAYS_INDIRECT=Yes
export DISPLAY=localhost:0.0

# Build bootsector (Obsolete, replaced by bootimage rust crate)
cd bootsector
./build.sh
cd ..

# Build kernel
cd kernel
./build.sh
cd ..

# Combine them
cat bin/bootsect.bin bin/kernel.bin > bin/koizos-img.bin

# Run in QEMU
qemu-system-x86_64 -fda bin/koizos-img.bin