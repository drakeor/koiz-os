# I just use this for X-Server
export LIBGL_ALWAYS_INDIRECT=Yes
#export DISPLAY=127.0.0.1:0.0

export DISPLAY=$(cat /etc/resolv.conf | grep nameserver | awk '{print $2; exit;}'):0.0

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

# Add log director
mkdir -p logs

# Run in QEMU
qemu-system-x86_64 -fda bin/koizos-img.bin -serial pty
#qemu-system-x86_64 -fda bin/koizos-img.bin -chardev stdio,id=char0,logfile=logs/serial.log,signal=off \
#  -serial chardev:char0
