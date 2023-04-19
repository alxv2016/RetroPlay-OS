#!/bin/sh
# retroplay.pak

# NOTE: will launch "say" which will display message during boot, TODO show an image?
# NOTE: sleep XX indicates the duration timeout before proceeding to next set of actions.

# init lcd
cat /proc/ls
sleep 0.25

# init charger detection
if [ ! -f /sys/devices/gpiochip0/gpio/gpio59/direction ]; then
	echo 59 > /sys/class/gpio/export
	echo in > /sys/devices/gpiochip0/gpio/gpio59/direction
fi

# init backlight
pwmdir=/sys/class/pwm/pwmchip0
echo 0 > $pwmdir/export
echo 800 > $pwmdir/pwm0/period
echo 80 > $pwmdir/pwm0/duty_cycle
echo 1 > $pwmdir/pwm0/enable

export SDCARD_PATH=/mnt/SDCARD
export BIOS_PATH=/mnt/SDCARD/Bios
export ROMS_PATH=/mnt/SDCARD/Roms
export SAVES_PATH=/mnt/SDCARD/Saves
export USERDATA_PATH=/mnt/SDCARD/.userdata
export LOGS_PATH=/mnt/SDCARD/.userdata/logs
export CORES_PATH=/mnt/SDCARD/.system/cores
export RES_PATH=/mnt/SDCARD/.system/res
# used by bin/shutdown
export DATETIME_PATH=$USERDATA_PATH/.retroplay/datetime.txt

# killall tee # NOTE: killing tee is somehow responsible for audioserver crashes
rm -f "$SDCARD_PATH/update.log"

export LD_LIBRARY_PATH="/mnt/SDCARD/.system/lib:$LD_LIBRARY_PATH"
export PATH="/mnt/SDCARD/.system/bin:$PATH"

# NOTE: could cause performance issues on more demanding cores...maybe?
if [ -f /customer/lib/libpadsp.so ]; then
    LD_PRELOAD=as_preload.so audioserver.mod &
    export LD_PRELOAD=libpadsp.so
fi

# adjust lcd luma and saturation
lumon &

CHARGING=`/customer/app/axp_test | awk -F'[,: {}]+' '{print $7}'`
if [ "$CHARGING" == "3" ]; then
	batmon
else
	# Show welcome boot image unless device is charging
	/mnt/SDCARD/.system/bin/sys-img "boot.png"
	sleep 2
fi


keymon &

mkdir -p "$LOGS_PATH"
mkdir -p "$USERDATA_PATH/.mmenu"
mkdir -p "$USERDATA_PATH/.retroplay"

# init datetime
if [ -f "$DATETIME_PATH" ]; then
	DATETIME=`cat "$DATETIME_PATH"`
	date +'%F %T' -s "$DATETIME"
	DATETIME=`date +'%s'`
	DATETIME=$((DATETIME + 6 * 60 * 60))
	date -s "@$DATETIME"
fi

AUTO_PATH=$USERDATA_PATH/auto.sh
if [ -f "$AUTO_PATH" ]; then
	"$AUTO_PATH"
fi

cd $(dirname "$0")

EXEC_PATH=/tmp/retroplay_exec
touch "$EXEC_PATH"  && sync

MIYOO_VERSION=`/etc/fw_printenv miyoo_version`
export MIYOO_VERSION=${MIYOO_VERSION#miyoo_version=}

# Battery level debug info
ls /customer/app > "$USERDATA_PATH/.retroplay/app_contents.txt"
/customer/app/axp_test > "$USERDATA_PATH/.retroplay/axp_result.txt"

CPU_PATH=/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor
while [ -f "$EXEC_PATH" ]; do
	echo ondemand > "$CPU_PATH"

	./retroplay &> "$LOGS_PATH/retroplay.txt"
	
	echo `date +'%F %T'` > "$DATETIME_PATH"
	echo performance > "$CPU_PATH"
	sync

	NEXT="/tmp/next"
	if [ -f $NEXT ]; then
		CMD=`cat $NEXT`
		eval $CMD
		rm -f $NEXT
		if [ -f "/tmp/using-swap" ]; then
			swapoff $USERDATA_PATH/swapfile
			rm -f "/tmp/using-swap"
		fi
		
		echo `date +'%F %T'` > "$DATETIME_PATH"
		sync
	fi
done

# just in case
shutdown
