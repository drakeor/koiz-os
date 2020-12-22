mkdir -p ../bin

echo "building bootsector"
fasm.x64 bootsect.asm ../bin/bootsect.bin