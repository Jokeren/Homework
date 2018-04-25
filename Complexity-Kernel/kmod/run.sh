#!/bin/bash

rustup override add nightly
make RELEASE=1
echo kz21xyz | sudo -S rm -f /tmp/kz21.output
echo kz21xyz | sudo -S insmod target/kernel/kmod.ko
echo "Install success!"
echo "Let module run 2 minutes!"
sleep 120s
echo "Execution success!"
echo kz21xyz | sudo -S rmmod kmod
echo "Remove success!"
echo "Please read dmesg to verify the calculation time!"
dmesg | grep "Lib->Calculation" | tail -n 1
