#!/bin/bash

ope_arr=("a" "b" "c" "d" "e" "f" \
    "g" "h" "i" "j")

expect_arr=(3 0 "Hello World!" 8 0x15 "G" \
    0x0 0x4 0xFF 0x10x1)

prefix="./ws/"

echo "Starting test."
echo 

for i in {0..9}; do
    res=$(${prefix}${ope_arr[i]})
    expect=${expect_arr[i]}

    echo [${prefix}${ope_arr[i]}]
    if [[ $res = $expect ]]; then
        echo -e "\e[1;32mPASSED\e[0m : got $res."
    else
        echo -e "\e[1;31mFAILED\e[0m : expected $expect, but got $res."
        exit_code=1
    fi
    echo
done
