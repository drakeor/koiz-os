format elf
use32

include '../libc/ccall.inc'

section '.text' executable


section '.bss'
    ; This boot-record is hard coded to a 
    ;  (4096 * 1024) size ramdisk and heavily references the following linux
    ; commands:
    ; 
    ; dd if=/dev/zero of=dosfs bs=4096 count=1024
    ; mkdosfs -F 16 dosfs -S 512 -s 1
    ;
    ; We recall that 4096 * 1024 is the maximum size of our test ramdisk
    ; and coincidently the minimum size for FAT16.

    ; Make this accessable
    public ramdisk_fat16_bootrecord

    ; REMEMBER EVERYTHING BELOW SHOULD END UP AS LITTLE ENDIAN!!
    ramdisk_fat16_bootrecord:

        ; Offset 0x0 - (3 Bytes) 
        ;           This will disassemble to JMP SHORT 3C NOP
        ;           This jumps over the disk format information.
        ;           This essentially isn't getting called so we're just making
        ;           it doesn't actually matter
        db 0xEB
        db 0x3C
        db 0x90

        ; Offset 0x3 - 8 bytes
        ;           OEM Identitifer. Since I'm using mkfs.dos as a reference
        ;           I'm simply making this mkfs.dos which is
        db "mkfs.dos"

        ; Offset 0xB - 2 bytes
        ;           This is the number of bytes per sector in powers of two
        ;           Most commonly, this is 512 (0x0200)
        ;           NOTE that we need to make this little-endian below!!
        db 0x00
        db 0x20

        ; Offset 0xD - 1 byte
        ;           This is the number of sectors per cluster in powers of two.
        ;           Old MS-DOS supported a max cluster size of 4KB while modern
        ;           OS's can support up to 128KB clusters with 512 bytes/sector
        ;           We're setting ours to 1 to use minimum size clusters.
        ;           as we need to fit this into our ramdisk
        db 0x01

        ; Offset 0xE - 2 bytes
        ;           This is the count of reserved logical sectors
        ;           This is normally 1 and mkfs.dos made this 1 as well
        ;           so might as well stick with 1.
        ;           REMEMBER this is little-endian!
        db 0x01
        db 0x00

        ; Offset 0x10 - 1 byte
        ;           Number of file allocation tables (FAT)
        ;           This is apparently almost always 2 so that's what we'll
        ;           use
        db 0x02

        ; Offset 0x11 - 2 bytes
        ;               Maximum number of FAT12 and FAT16 root entries
        ;               mkfs.dos made it 0x0200 (512) so that's what we'll use
        ;               240 is also the max number for <S-DOS/PC DOS hard disks
        db 0x00
        db 0x02

        ; Offset 0x13 - 2 bytes
        ;               Total logical sectors
        ;               This is set at 0x0200 (512) for our hardcoded value
        db 0x00
        db 0x20

        ; Offset 0x15 - 1 byte
        ;               This is the media descriptor.
        ;               This is a long list I won't replicate here
        ;               We're using 0xF8 which means fixed disk
        db 0xF8

        ; Offset 0x16 - 2 bytes
        ;               Logical sectors per file allocation table
        ;               mkfs.dos set this at 0x0020 so we will too
        db 0x20
        db 0x00

        ; Offset 0x18 - 2 bytes
        ;               Physical sectors per track. Mostly unused for drives
        ;               now but we set it to 0x0020 (32) because mkfs.dos does
        db 0x20
        db 0x00

        ; Offset 0x1A - 2 bytes
        ;               Number of heads for disks. We set this to 0x0040 (64)
        ;               Because that's what mkfs.dos sets it to
        db 0x40
        db 0x00

        ; Offset 0x1C - 4 bytes
        ;               Number of hidden sectors. We don't have any so this is
        ;               zero
        dd 0x00

        ; Offset 0x20 - 4 bytes
        ;               Large sector count. Usually set if there's more than
        ;               65535 sectors in the volume. Obviously, ours is small
        ;               so it'll just be 0
        dd 0x00

        ; Offset 0x24 - 1 byte
        ;               This is the drive number. Usually useless, but the value
        ;               is identical to the value returned by Interrupt 0x13.
        ;               We has 0x0080 for hard disks
        db 0x80

        ; Offset 0x25 - 1 byte
        ;               Reserved flag for Windows NT
        db 0x00

        ; Offset 0x26 - 1 byte
        ;               Signature that must be 0x28 or 0x29
        db 0x29

        ; Offset 0x27 - 4 bytes
        ;               Volume ID serial number for tracking volumes between
        ;               computers
        dd 0x42069420

        ; Offset 0x2B - 11 bytes
        ;               Volume label string padded with spaces
        db "FAT16KOIZOS"

        ; Offset 0x36 - 8 bytes
        ;               System identifer string. Never trust the contents of
        ;               this string
        db "FAT16   "

        ; Offset 0x3E - 448 bytes
        ;               This is the boot code. We won't have anything in it
        ;               for now so just reserve some space
        rb 448
        ; Offset 0x1FE - 2 bytes
        ;               Magic number 0xAA55
        db 0x55
        db 0xAA