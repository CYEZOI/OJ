#!/bin/bash
echo -e "\033[33m"
echo "OJ  Copyright (C) 2023  langningchen"
echo "This program comes with ABSOLUTELY NO WARRANTY."
echo "This is free software, and you are welcome to redistribute it under certain conditions."
echo -e "\033[0m"

# Clean
for mount_point in $(sudo mount | awk '{print $3}')
do
    if [[ "$mount_point" =~ "/home/Judger/Run/"* ]]
    then
        sudo umount "$mount_point"
    fi
done
sudo rm -rf /home/Judger/Run
sudo mkdir /home/Judger/Run
sudo rm -f $(sudo find "/home/Judger/" -name "*.log") Log.log

# Build
cmake -B build
cmake --build build -j 8

# Run
if test -f "./build/OJ"
then
    sudo service mysql start >/dev/null 2>&1
    sudo ./build/OJ
fi
