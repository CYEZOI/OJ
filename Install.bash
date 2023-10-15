#!/bin/bash
echo -e "\033[33m"
echo "OJ  Copyright (C) 2023  langningchen"
echo "This program comes with ABSOLUTELY NO WARRANTY."
echo "This is free software, and you are welcome to redistribute it under certain conditions."
echo -e "\033[0m"

# Basic checks
if [ ! -f /etc/os-release ] || [ $(cat /etc/os-release | grep -c "Ubuntu") -eq 0 ]; then
    echo "This script only supports Ubuntu."
    exit
elif [ $(id -u) -ne 0 ]; then
    sudo $0 $(whoami) $HOME
    exit
elif [ $# -ne 2 ]; then
    echo "Usage: $0 <username> <home directory>"
    echo "       <username> is the username of the user you want to install OJ for."
    echo "       <home directory> is the home directory of the user you want to install OJ for."
    echo ""
    echo "Note: Please run this script as root or with sudo."
    echo "      The username must be a non-root user."
    echo "      The user and the home directory must exist."
    echo "      The home directory must be an absolute path."
    exit
fi
LastDir=$(pwd)
cd $2

# Change apt source
cp /etc/apt/sources.list /etc/apt/sources.list.bak
sed -i "s@http://.*security.ubuntu.com@http://mirrors.tuna.tsinghua.edu.cn@g" /etc/apt/sources.list
sed -i "s@http://.*archive.ubuntu.com@http://mirrors.tuna.tsinghua.edu.cn@g" /etc/apt/sources.list
apt update
apt upgrade -y

# Install packages
apt install fish dialog g++ make cmake git libcurl4-gnutls-dev mysql-client mysql-server libmysqlcppconn-dev -y
chsh --shell /usr/bin/fish $1

# Clone repo
# git clone https://github.com/langningchen/OJ.git --depth 1
git clone https://ghproxy.com/https://github.com/langningchen/OJ --depth 1

# Create judge user
groupadd judge
useradd Judger -g judge
passwd Judger -d >/dev/null
mkdir /home/Judger
chown -R Judger /home/Judger
chgrp -R judge /home/Judger
echo -e "Judger\tALL=(ALL:ALL) ALL" | tee -a /etc/sudoers >/dev/null
chsh Judger -s /usr/bin/fish
# chmod -R a+rw /home/langningc2009/Coding
chmod a+s /bin/chgrp
chmod a+s /bin/chown
mkdir /home/Judger/Run
chown -R Judger /home/Judger/Run
chgrp -R judge /home/Judger/Run

# config database
service mysql start
DatabaseUsername="OJUser"
DatabasePassword=$(cat /dev/urandom | tr -dc 'a-zA-Z0-9' | fold -w 32 | head -n 1)
mkdir /etc/OJ
echo "localhost" >/etc/OJ/DatabaseHost
echo "3306" >/etc/OJ/DatabasePort
echo "$DatabaseUsername" >/etc/OJ/DatabaseUsername
echo "$DatabasePassword" >/etc/OJ/DatabasePassword
echo "OJ" >/etc/OJ/DatabaseName
echo "CREATE DATABASE OJ;" >>./Database.sql
echo "CREATE USER $DatabaseUsername IDENTIFIED BY \"$DatabasePassword\";" >>./Database.sql
echo "GRANT ALL ON OJ.* TO $DatabaseUsername;" >>./Database.sql
echo "FLUSH PRIVILEGES;" >>./Database.sql
mysql <./Database.sql
rm -f ./Database.sql
mysql -u $DatabaseUsername -p$DatabasePassword OJ <./OJ/Database.sql
echo "INSERT INTO \`Problems\` (\`PID\`, \`Title\`, \`IOFilename\`, \`Description\`, \`Input\`, \`Output\`, \`Range\`, \`Hint\`, \`Samples\`, \`TestGroups\`) VALUES (1000, 'Add', 'Add', '***Add*** two numbers \$a\$ and \$b\$', 'two numbers in _one line_', 'one number', '', '\$\$1 \\\\leq a,b \\\\leq 1000\$\$', '[{\"Description\":\"\",\"Input\":\"1 2\",\"Output\":\"3\"}]', '[{\"TGID\":0,\"TestCases\":[{\"Answer\":\"3\",\"Input\":\"1 2\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":0,\"TimeLimit\":1000},{\"Answer\":\"5\",\"Input\":\"2 3\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":1,\"TimeLimit\":1000},{\"Answer\":\"50\",\"Input\":\"23 27\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":2,\"TimeLimit\":1000},{\"Answer\":\"29\",\"Input\":\"20 09\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":3,\"TimeLimit\":1000},{\"Answer\":\"18\",\"Input\":\"01 17\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":4,\"TimeLimit\":1000},{\"Answer\":\"67\",\"Input\":\"45 22\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":5,\"TimeLimit\":1000},{\"Answer\":\"198\",\"Input\":\"99 99\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":6,\"TimeLimit\":1000},{\"Answer\":\"1011\",\"Input\":\"465 546\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":7,\"TimeLimit\":1000},{\"Answer\":\"579\",\"Input\":\"123 456\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":8,\"TimeLimit\":1000},{\"Answer\":\"1356\",\"Input\":\"877 479\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":9,\"TimeLimit\":1000}]},{\"TGID\":1,\"TestCases\":[{\"Answer\":\"4294967296\",\"Input\":\"2147483648 2147483648\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":0,\"TimeLimit\":1000},{\"Answer\":\"12185866737\",\"Input\":\"6567867981 5617998756\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":1,\"TimeLimit\":1000},{\"Answer\":\"11111111100\",\"Input\":\"1234567890 9876543210\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":2,\"TimeLimit\":1000},{\"Answer\":\"3825938259\",\"Input\":\"1357924680 2468013579\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":3,\"TimeLimit\":1000},{\"Answer\":\"7800224355\",\"Input\":\"1122334455 6677889900\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":4,\"TimeLimit\":1000}]},{\"TGID\":2,\"TestCases\":[{\"Answer\":\"0\",\"Input\":\"1 -1\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":0,\"TimeLimit\":1000},{\"Answer\":\"-1\",\"Input\":\"2 -3\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":1,\"TimeLimit\":1000},{\"Answer\":\"-4\",\"Input\":\"23 -27\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":2,\"TimeLimit\":1000},{\"Answer\":\"11\",\"Input\":\"20 -09\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":3,\"TimeLimit\":1000},{\"Answer\":\"-16\",\"Input\":\"01 -17\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":4,\"TimeLimit\":1000},{\"Answer\":\"23\",\"Input\":\"45 -22\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":5,\"TimeLimit\":1000},{\"Answer\":\"0\",\"Input\":\"99 -99\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":6,\"TimeLimit\":1000},{\"Answer\":\"-81\",\"Input\":\"465 -546\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":7,\"TimeLimit\":1000},{\"Answer\":\"-333\",\"Input\":\"123 -456\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":8,\"TimeLimit\":1000},{\"Answer\":\"398\",\"Input\":\"877 -479\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":9,\"TimeLimit\":1000}]},{\"TGID\":3,\"TestCases\":[{\"Answer\":\"0\",\"Input\":\"2147483647 -2147483647\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":0,\"TimeLimit\":1000},{\"Answer\":\"949869225\",\"Input\":\"6567867981 -5617998756\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":1,\"TimeLimit\":1000},{\"Answer\":\"-8641975320\",\"Input\":\"1234567890 -9876543210\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":2,\"TimeLimit\":1000},{\"Answer\":\"-1110088899\",\"Input\":\"1357924680 -2468013579\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":3,\"TimeLimit\":1000},{\"Answer\":\"-5555555445\",\"Input\":\"1122334455 -6677889900\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":4,\"TimeLimit\":1000}]}]');" >./Initiate.sql

# Create admin account
dialog --title "Create admin account" --no-cancel --no-kill --inputbox "Please enter the username of the admin account." 20 60 2>./AdminUsername
AdminUsername=$(cat ./AdminUsername)
FirstTime=1
while [ $FirstTime -eq 1 ] || [ "$AdminPassword" != "$AdminPasswordAgain" ]; do
    if [ $FirstTime -eq 0 ]; then
        dialog --title "Create admin account" --no-cancel --no-kill --msgbox "The passwords you entered are not the same. Please try again." 20 60
    fi
    dialog --title "Create admin account" --no-cancel --no-kill --insecure --passwordbox "Please enter the password of the admin account." 20 60 2>./AdminPassword
    AdminPassword=$(cat ./AdminPassword)
    dialog --title "Create admin account" --no-cancel --no-kill --insecure --passwordbox "Please enter the password of the admin account again." 20 60 2>./AdminPasswordAgain
    AdminPasswordAgain=$(cat ./AdminPasswordAgain)
    FirstTime=0
done
dialog --title "Create admin account" --no-cancel --no-kill --inputbox "Please enter the nickname of the admin account." 20 60 2>./AdminNickname
AdminNickname=$(cat ./AdminNickname)
dialog --title "Create admin account" --no-cancel --no-kill --inputbox "Please enter the email of the admin account." 20 60 2>./AdminEmail
AdminEmail=$(cat ./AdminEmail)
echo "INSERT INTO Users (\`Username\`, \`Password\`, \`Nickname\`, \`EmailAddress\`, \`Role\`) VALUES ('$AdminUsername', '$AdminPassword', '$AdminNickname', '$AdminEmail', 0);" >>./Initiate.sql
rm -f ./AdminUsername ./AdminPassword ./AdminPasswordAgain ./AdminNickname ./AdminEmail

# Set email server
EmailOK=0
while [ $EmailOK -eq 0 ]; do
    EmailServer=""
    EmailPort=""
    EmailHint=""
    dialog --title "Set email server" --no-cancel --no-kill --menu "Please select the email server you want to use." 20 60 12 outlook "Outlook (Recommended)" hotmail "Hotmail" 163 "163" qq "QQ" gmail "Gmail" yahoo "Yahoo" sina "Sina" other "Other" 2>./EmailServer
    EmailServer=$(cat ./EmailServer)
    if [ "$EmailServer" = "outlook" ]; then
        EmailServer="smtp-mail.outlook.com"
        EmailPort="587"
        EmailHint="your outlook password"
    elif [ "$EmailServer" = "hotmail" ]; then
        EmailServer="smtp-mail.outlook.com"
        EmailPort="587"
        EmailHint="your hotmail password"
    elif [ "$EmailServer" = "163" ]; then
        EmailServer="smtp.163.com"
        EmailPort="465"
        EmailHint="your 163 authorization code"
    elif [ "$EmailServer" = "qq" ]; then
        EmailServer="smtp.qq.com"
        EmailPort="465"
        EmailHint="your qq authorization code"
    elif [ "$EmailServer" = "gmail" ]; then
        EmailServer="smtp.gmail.com"
        EmailPort="587"
        EmailHint="your gmail password"
    elif [ "$EmailServer" = "yahoo" ]; then
        EmailServer="smtp.mail.yahoo.com"
        EmailPort="587"
        EmailHint="your yahoo password"
    elif [ "$EmailServer" = "sina" ]; then
        EmailServer="smtp.sina.com"
        EmailPort="587"
        EmailHint="your sina password"
    elif [ "$EmailServer" = "other" ]; then
        dialog --title "Set email server" --no-cancel --no-kill --inputbox "Please enter the email server you want to use." 20 60 2>./EmailServer
        EmailServer=$(cat ./EmailServer)
        dialog --title "Set email server" --no-cancel --no-kill --inputbox "Please enter the port of the email server you want to use." 20 60 2>./EmailPort
        EmailPort=$(cat ./EmailPort)
        EmailHint="your password"
    fi
    dialog --title "Set email server" --no-cancel --no-kill --inputbox "Please enter the email address you want to use." 20 60 2>./EmailAddress
    EmailAddress=$(cat ./EmailAddress)
    dialog --title "Set email server" --no-cancel --no-kill --insecure --passwordbox "Please enter $EmailHint of the email address you want to use." 20 60 2>./EmailPassword
    EmailPassword=$(cat ./EmailPassword)
    dialog --title "Set email server" --no-cancel --no-kill --infobox "Testing email server..." 20 60
    echo "Subject: Test email" >./TestEmail.txt
    echo "From: $EmailAddress" >>./TestEmail.txt
    echo "To: $EmailAddress" >>./TestEmail.txt
    echo "" >>./TestEmail.txt
    echo "This is a test email. If you can receive this email, the email server you entered is correct." >>./TestEmail.txt
    curl --url "smtp://$EmailServer:$EmailPort" --ssl-reqd --mail-from "$EmailAddress" --mail-rcpt "$EmailAddress" --upload-file ./TestEmail.txt --user "$EmailAddress:$EmailPassword" --silent --output /dev/null
    if [ $? -ne 0 ]; then
        dialog --title "Set email server" --no-cancel --no-kill --msgbox "The email server you entered is not correct. Please try again." 20 60
    else
        EmailOK=1
    fi
done
dialog --clear
echo "INSERT INTO \`Settings\` (\`Key\`, \`Value\`) VALUES ('EmailServer', 'smtp://$EmailServer:$EmailPort');" >>./Initiate.sql
echo "INSERT INTO \`Settings\` (\`Key\`, \`Value\`) VALUES ('EmailUsername', '$EmailAddress');" >>./Initiate.sql
echo "INSERT INTO \`Settings\` (\`Key\`, \`Value\`) VALUES ('EmailPassword', '$EmailPassword');" >>./Initiate.sql
rm -f ./EmailServer ./EmailPort ./EmailAddress ./EmailPassword ./TestEmail.txt

PasswordSalt1=$(cat /dev/urandom | tr -dc 'a-zA-Z0-9!@#$%^&*()_+-=[]{}|;:,./<>?~' | fold -w 32 | head -n 1)
PasswordSalt2=$(cat /dev/urandom | tr -dc 'a-zA-Z0-9!@#$%^&*()_+-=[]{}|;:,./<>?~' | fold -w 32 | head -n 1)
echo "INSERT INTO \`Settings\` (\`Key\`, \`Value\`) VALUES ('PasswordSalt1', '$PasswordSalt1');" >>./Initiate.sql
echo "INSERT INTO \`Settings\` (\`Key\`, \`Value\`) VALUES ('PasswordSalt2', '$PasswordSalt2');" >>./Initiate.sql

echo "INSERT INTO \`Settings\` (\`Key\`, \`Value\`) VALUES ('Compiler', '/bin/g++');" >>./Initiate.sql
echo "INSERT INTO \`Settings\` (\`Key\`, \`Value\`) VALUES ('CompileOutputLimit', '134217728');" >>./Initiate.sql
echo "INSERT INTO \`Settings\` (\`Key\`, \`Value\`) VALUES ('CompileTimeLimit', '10000');" >>./Initiate.sql
echo "INSERT INTO \`Settings\` (\`Key\`, \`Value\`) VALUES ('JudgeUserGroupID', '1001');" >>./Initiate.sql
echo "INSERT INTO \`Settings\` (\`Key\`, \`Value\`) VALUES ('JudgeUserID', '1001');" >>./Initiate.sql
echo "INSERT INTO \`Settings\` (\`Key\`, \`Value\`) VALUES ('JudgeUsername', 'Judger');" >>./Initiate.sql
echo "INSERT INTO \`Settings\` (\`Key\`, \`Value\`) VALUES ('SocketPort', '80');" >>./Initiate.sql
echo "INSERT INTO \`Settings\` (\`Key\`, \`Value\`) VALUES ('SystemCallList', '-1,-1,0,-1,-1,-1,0,0,-1,-1,-1,-1,-1,-1,-1,0,0,-1,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,1,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,-1,0,0,0,-1,0,0,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0');" >>./Initiate.sql
mysql -u $DatabaseUsername -p$DatabasePassword OJ <./Initiate.sql
rm -f ./Initiate.sql

# Compile
cd ./OJ
cmake -B build
cd ./build
make
cd ../../

# End
echo "Installation complete."
echo "Your ($1's) default shell has been changed to the fish shell."
echo "Please start the server by running \"$(pwd)/OJ/Run.fish\" with the fish shell and root user."
echo "Before starting the server, please change the working dir to \"$(pwd)/OJ\"."
echo "You can check the log in \"$(pwd)/OJ/Log.log\"."
echo "After the server is started, you can visit the website at \"http://localhost\"."
echo "Please go to \"https://github.com/langningchen/OJ\" for more information or to report bugs."
echo "The database username is \"$DatabaseUsername\" and the password is \"$DatabasePassword\"."
echo "Please save the database username and password in a safe place and do not share it with others."
echo "We strongly recommend you to install the dbeaver database management tool at \"https://dbeaver.io/\"."
echo ""
echo "Thank you for using this OJ."
echo "Have a nice day!"
cd $LastDir
