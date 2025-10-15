#!/bin/bash

if ! ip link show wlan1 &> /dev/null; then
    echo "La interfaz no existe"
    exit
fi

sudo service hostapd start
sudo ip addr add 192.168.10.1/22 dev wlan0
sudo service dnsmasq start

sudo iptables -t nat -A POSTROUTING -o wlan1 -j MASQUERADE
