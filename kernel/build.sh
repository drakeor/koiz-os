# Compile for our OS (32 bit)
cargo build --target i686-koiz_os.json -Z build-std

# Compile for our OS (64 bit) {NOT YET SUPPORTED}
# cargo build --target i686-koiz_os.json  -Z build-std

# Compile for linux
# cargo rustc -- -C link-arg=-nostartfiles