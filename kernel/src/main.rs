#![no_std]
#![no_main]

static HELLO: &[u8] = b"Koiz OS!";

// Panic Handler
use core::panic::PanicInfo;
#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    loop {}
}

// Make sure rust doesn't mangle the name
// This function is also never allowed to return (-> !)
#[no_mangle]
pub extern "C" fn _start() {
    let vga_buffer = 0xb8000 as *mut u8;

    for (i, &byte) in HELLO.iter().enumerate() {
        unsafe {
            *vga_buffer.offset(i as isize * 2) = byte;
            *vga_buffer.offset(i as isize * 2 + 1) = 0xb;
        }
    }

    
    loop {}
}
