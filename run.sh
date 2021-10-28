# I just use this for X-Server
export LIBGL_ALWAYS_INDIRECT=Yes

if [ -z "$DISPLAY" ]; then
export DISPLAY=$(cat /etc/resolv.conf | grep nameserver | awk '{print $2; exit;}'):0
export DISPLAY=127.0.0.1:0.0
fi

#export DISPLAY=$(cat /etc/resolv.conf | grep nameserver | awk '{print $2; exit;}'):0

# Build kernel
cd kernel
make clean
make
if [ $? -ne 0 ]
then
    echo "Kernel Build Failed! Terminating.."
    exit 1 
fi
cd ..

# Add log director
mkdir -p logs

# Run in QEMU
echo "waiting for gdb debugger"
#qemu-system-x86_64 -fda bin/koizos-img.bin -device rtl8139 -serial file:logs/serial.log -s -S &
#qemu-system-x86_64 -fda bin/koizos-grub.bin -device rtl8139 -serial file:logs/serial.log -s -S &
qemu-system-i386 -cdrom bin/koizos.iso -serial file:logs/serial.log -s -S &
# qemu-system-x86_64 -fda bin/koizos-img.bin -device rtl8139 -serial stdio -s -S &
pid=$!
gdb -x ./gdbcmds
kill $pid

#qemu-system-x86_64 -fda bin/koizos-img.bin -serial pty
#qemu-system-x86_64 -fda bin/koizos-img.bin -chardev stdio,id=char0,logfile=logs/serial.log,signal=off \
#  -serial chardev:char0
