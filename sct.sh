#!/bin/bash

ksyms="/proc/kallsyms"
sysmap="/usr/lib/debug/boot/System.map-6.1.0-29-amd64"
str="start_cpu0"




grab() {

    symbol="$1"
    file="$2"
    if [ "$file" = "" ]; then
        >&2 echo "Error"
        exit 1
    fi

    regex="[[:space:]]${symbol}$"

    grep -E $regex $ksyms | cut -d" " -f1 | head -1

}

grabks() {
    grab $1 $ksyms
}

grabsm() {
    grab $1 $sysmap

}

offset() {
    k=$(grabks $str)
    s=$(grabsm $str)
    fk="Ox${k}"
    fs="Ox${s}"
    dec=$(($fk - $fs))

    printf "%x" $dec
}

syscalltable() {
    tgt="$1"
    if [ "$tgt" = "" ]; then
        >&2 echo "Error"
        exit 1
    fi

    o=$(offset)
    fo="Ox${o}"
    addr=$(grabsm $tgt)
    faddr="0x${addr}"
    dec=$(($faddr + $fo))

    printf "0x%x" $dec
}


if [ "$1" = "" ]; then
    target="sys_call_table"
else
    target="$1"
fi

taddr=$(syscalltable $target)
echo "$taddr"
exit 0
