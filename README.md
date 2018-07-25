# android_kernel_huawei_hi3635
A custom and upstreamed kernel for devices using the HI3635 board (Compatible with Huawei P8, Huawei Mate S, Huawei Mediapad M2, Huawei Mediapad X2 and Honor 7 devices).

# Modifications
* Upstream to 3.10.108
* Disable ZRAM (01100a6)
* Disable KSM (01100a6)
* Update governor_maliondemand.c (43b28df)
* Optimize kernel for size (136d80a, 6a47ecd)
* Removing some Huawei modifications to allow SD card be stable (574b3a0)
* Enable wifi bonding (6dc58a8)
* Enable F2FS (94efc06)
* Higher volume
* Boeffla wakelock blocker
* Clean up Huawei crap
>>>>>>> nougat-stable
