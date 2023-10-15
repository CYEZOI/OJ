#!/usr/bin/fish
echo -e "\033[33m"
echo "OJ  Copyright (C) 2023  langningchen"
echo "This program comes with ABSOLUTELY NO WARRANTY."
echo "This is free software, and you are welcome to redistribute it under certain conditions."
echo -e "\033[0m"

# Clean
for mount_point in (mount | awk '{print $3}')
    if string match -q --regex "/home/Judger/Run/*" "$mount_point"
        umount "$mount_point"
    end
end
rm -rf /home/Judger/Run
mkdir /home/Judger/Run
rm -f (find "/home/Judger/" -name "*.log") Log.log ./build/main

# Build
cmake -B build
cd ./build
make
cd ../

# Run
if test -f "./build/main"
    service mysql start >/dev/null
    ./build/main
    cat Log.log
end
