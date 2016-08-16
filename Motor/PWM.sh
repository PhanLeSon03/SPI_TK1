#sdo su
cd /sys/class/pwm/pwmchip0
echo 0 > export
cd pwm0
echo 1000000 > period
echo 100000 > duty
echo 1 > enable
