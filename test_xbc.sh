#!/bin/bash

ope_arr=("a" "b" "c" "d" \
    "e" "f" "g" "h" \
    "i" "j" "k" "l"
    "m")

            # a-d
expect_arr=(3 0 "Hello World!" 8 \
    # e-h
    0x15 "G" 0x0 0x4 \
    # i-l
    0xFF 0x10x1 0x20 0x37 
    0x3)

prefix="./ws/"
exit_code=0

echo "Starting test."
echo 

for i in {0..12}; do
    res=$(${prefix}${ope_arr[i]})
    expect=${expect_arr[i]}

    echo -n "[${prefix}${ope_arr[i]}] : "
    if [[ $res = $expect ]]; then
        echo -e "\e[1;32mPASSED\e[0m : got $res."
    else
        echo -e "\e[1;31mFAILED\e[0m : expected $expect, but got $res."
        exit_code=1
    fi
done

echo

if [ $exit_code -eq 1 ];then
    echo -e "\e[1;31mTesting Failed\e[0m"
else
    echo -e "\e[1;32mAll Passed\e[0m"
fi
