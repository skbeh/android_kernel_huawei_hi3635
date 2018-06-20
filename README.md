# android_kernel_huawei_hi3635
Stock, stable, upstreamed kernel for devices using hi3635 board (P8, H7).
No heavy modifications.

# Modifications
* Upstream to 3.10.108
* Disable ZRAM (01100a6)
* Disable KSM (01100a6)
* Update governor_maliondemand.c (43b28df)
* Optimize kernel for size (136d80a, 6a47ecd)
* Removing some Huawei modifications to allow SD card be stable (574b3a0)
* Enable wifi bonding (6dc58a8)
* Enable F2FS (94efc06)
