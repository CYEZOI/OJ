#!/usr/bin/fish
clear
for mount_point in (mount | awk '{print $3}')
    if string match -q --regex "/home/Judger/Run/*" "$mount_point"
        umount "$mount_point"
    end
end
rm -f (find "/home/Judger/" -name "*.log")
rm Log.log
service mysql start
echo "Cleaned"
cd ./build
make
cd ../
while true;
    if [ (echo (netstat -anp | grep "127.0.0.1:80")) = "" ]
        break
    end
end
echo "Start running program..."
./build/main
