
UPGRADETOOL=$(pwd)/upgrade_tool
ROCKDEV_DIR=$(pwd)/images
LOADER=$ROCKDEV_DIR/MiniLoaderAll.bin
PARAMETER=$ROCKDEV_DIR/parameter_nouser.txt
UBOOT=$ROCKDEV_DIR/uboot-baidu.img
TRUST=$ROCKDEV_DIR/trust.img
BOOT=$ROCKDEV_DIR/boot.img
OEM=$ROCKDEV_DIR/oem.img
ROOTFS=$ROCKDEV_DIR/linaro-rootfs.img

if [ ! -n "$1" ]
then
echo "flash all images as default"
FLASH_TYPE=all
else
FLASH_TYPE="$1"
fi

if [ $FLASH_TYPE = all ]
then
	sudo $UPGRADETOOL ul $LOADER
	sudo $UPGRADETOOL di -p $PARAMETER
	sudo $UPGRADETOOL di -uboot $UBOOT
	sudo $UPGRADETOOL di -trust $TRUST
	sudo $UPGRADETOOL di -b $BOOT
	sudo $UPGRADETOOL di -oem $OEM
	sudo $UPGRADETOOL di -rootfs $ROOTFS
	sudo $UPGRADETOOL rd
fi

if [ $FLASH_TYPE = loader ]
then
	sudo $UPGRADETOOL ul $LOADER
fi

if [ $FLASH_TYPE = parameter ]
then
	sudo $UPGRADETOOL di -p $PARAMETER
fi

if [ $FLASH_TYPE = uboot ]
then
	sudo $UPGRADETOOL di -uboot $UBOOT
fi

if [ $FLASH_TYPE = trust ]
then
	sudo $UPGRADETOOL di -trust $TRUST
fi

if [ $FLASH_TYPE = boot ]
then
	sudo $UPGRADETOOL di -b $BOOT
	sudo $UPGRADETOOL rd
fi

if [ $FLASH_TYPE = oem ]
then
	sudo $UPGRADETOOL di -oem $OEM
fi

if [ $FLASH_TYPE = rootfs ]
then
	sudo $UPGRADETOOL di -rootfs $ROOTFS
	sudo $UPGRADETOOL rd
fi

