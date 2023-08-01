#!/usr/bin/fish
clear
for mount_point in (mount | awk '{print $3}')
    if string match -q --regex "/home/Judger/Run/*" "$mount_point"
        umount "$mount_point"
    end
end
rm -f (find "/home/Judger/" -name "*.log") Log.log ./build/main
service mysql start >/dev/null
cmake -B build
cd ./build
make
cd ../
if test -f "./build/main"
    clear
    ./build/main
    cat Log.log
end
