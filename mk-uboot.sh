#!/bin/bash

cd u-boot && ./make.sh evb-rk3399 && cd -

if [ $? -eq 0 ]; then
    echo "====Build uboot ok!===="
        cp u-boot/uboot.img .
        cp u-boot/trust.img .        
else
    echo "====Build uboot failed!===="
    exit 1
fi
