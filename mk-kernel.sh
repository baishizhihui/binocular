#!/bin/bash

cd kernel && make ARCH=arm64 rockchip_linux_defconfig && make ARCH=arm64 rk3399-binocular-818-linux.img -j12 && cd -

if [ $? -eq 0 ]; then
    echo "====Build kernel ok!===="
        cp kernel/boot.img .
	#cp boot.img /mnt/hgfs/share/
	  
else
    echo "====Build kernel failed!===="
    exit 1
fi
