#!/bin/bash
ope_arr=("a" "b")
expect_arr=(3 0)
prefix="./ws/"

echo "Starting test."
echo 

for i in {0..1}; do
    res=$(${prefix}${ope_arr[i]})
    expect=${expect_arr[i]}

    echo [${prefix}${ope_arr[i]}]
    if [ $res -eq $expect ]; then
        echo -e "\e[1;32mPASSED\e[0m : got $res."
    else
        echo -e "\e[1;31mFAILED\e[0m : expected $expect, but got $res."
        exit_code=1
    fi
    echo
done
