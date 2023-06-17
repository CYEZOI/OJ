#!/usr/bin/fish
clear
for mount_point in (mount | awk '{print $3}')
    if string match -q --regex "/home/Judger/Run/*" "$mount_point"
        umount "$mount_point"
    end
end
rm -f (find "/home/Judger/" -name "*.log")
echo "Cleaned"
cd ./build
make
cd ../
while true;
    if [ (echo (netstat -anp | grep ":80")) = "" ]
        break
    end
end
echo "Start running program..."
./build/main
