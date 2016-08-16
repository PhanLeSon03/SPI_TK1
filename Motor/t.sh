cat /sys/kernel/debug/gpio
echo 57 > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio57/direction
set a=10
while($a>0)
do
    echo 1 > /sys/class/gpio/gpio57/value
    usleep 10000
    echo 0 > /sys/class/gpio/gpio57/value
    usleep 10000
    @ a = $a - 1
done
end
