#! /bin/sh
#export PATH=$PATH:/home/bnxd/rk3399/gcc/gcc-linaro-5.5.0-2017.10-x86_64_arm-linux-gnueabihf/bin
#export PATH=$PATH:/home/bnxd/rk3399/gcc/gcc-linaro-6.3.1-2017.05-x86_64_aarch64-linux-gnu/bin
export PATH=$PATH:../prebuilts/gcc/linux-x86/aarch64/gcc-linaro-6.3.1-2017.05-x86_64_aarch64-linux-gnu/bin

cp makefile_baidu makefile

#make clean
rm bin/*
make all
make burnintest

#./copy.sh
#cp bin /home/bnxd/rk3399/nfs/app -rf
#cp lib /home/bnxd/rk3399/nfs/app -rf
#cp bin /mnt/hgfs/share/ -rf
#cp lib /mnt/hgfs/share/ -rf
