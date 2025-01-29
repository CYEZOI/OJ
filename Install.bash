#!/bin/bash

TerminalWidth=$(tput cols)
PasswordSalt1=$(cat /dev/urandom | tr -dc 'a-zA-Z0-9' | fold -w 32 | head -n 1)
PasswordSalt2=$(cat /dev/urandom | tr -dc 'a-zA-Z0-9' | fold -w 32 | head -n 1)
function BeginSection() {
    echo -e "╭$(printf '─%.0s' $(seq 1 $((TerminalWidth - 2))))╮\033[3G $1 "
}
function EndSection() {
    echo -e "╰$(printf '─%.0s' $(seq 1 $((TerminalWidth - 2))))╯"
    echo ""
}
function Output() {
    echo -e "│$(printf ' %.0s' $(seq 1 $((TerminalWidth - 2))))│\033[3G$1"
    echo -e "╰$(printf '─%.0s' $(seq 1 $((TerminalWidth - 2))))╯"
    echo -n -e "\033[1A"
    if [ "$2" == "-n" ]; then
        echo -n -e "\033[1A\033[$((${#1} - 13))C"
    fi
}
function Error() {
    Output "\033[31m✗ $1\033[0m"
}
function Success() {
    Output "\033[32m✓ $1\033[0m"
}
function Warning() {
    Output "\033[33m⚠ $1\033[0m"
}
function Wait() {
    Output "\033[34m○ $1\033[0m"
    echo -e "\033[2A"
}
function Confirm() {
    Output "\033[35m? $1 (Y/n) \033[0m" -n
    read -n 1 -p "" ConfirmData
    echo -e "\033[?25l"
    if [ "$ConfirmData" == "Y" ] || [ "$ConfirmData" == "y" ]; then
        return 1
        elif [ "$ConfirmData" == "" ]; then
        echo -n -e "\033[1A"
        return 1
        elif [ "$ConfirmData" == "N" ] || [ "$ConfirmData" == "n" ]; then
        return 0
    else
        Confirm "$1"
        return $?
    fi
}
function Input() {
    Output "\033[36m? $1 \033[0m" -n
    if [ "$2" == "-p" ]; then
        read -s -p "" InputData
    else
        read -p "" InputData
    fi
    echo -e "\033[?25l"
    if [ "$2" != "-p" ]; then
        echo -n -e "\033[1A"
    fi
    export InputResult=$InputData
}

function BasicChecks() {
    BeginSection "Basic checks"
    if [ ! -f /etc/os-release ] || [ $(cat /etc/os-release | grep -c "Ubuntu") -eq 0 ]; then
        Error "This script only supports Ubuntu."
        EndSection
        exit
        elif [ $(id -u) -ne 0 ]; then
        Error "Please run with root privilege."
        EndSection
        exit
    else
        Success "All checks passed."
    fi
    EndSection
}
function InstallPackages() {
    BeginSection "Install packages"
    Wait "Updating package list..."
    apt-get update >/dev/null 2>&1
    if [ $? -ne 0 ]; then
        Error "Failed to update package list."
        EndSection
        exit
    else
        Success "Package list updated."
    fi
    
    Wait "Upgrading packages..."
    apt-get upgrade -y >/dev/null 2>&1
    if [ $? -ne 0 ]; then
        Error "Failed to upgrade packages."
        EndSection
        exit
    else
        Success "Packages upgraded."
    fi
    
    Wait "Installing packages..."
    apt-get install g++ make cmake git libcurl4-gnutls-dev mysql-client mysql-server libmysqlcppconn-dev mount libssl-dev libminizip-dev -y >/dev/null 2>&1
    if [ $? -ne 0 ]; then
        Error "Failed to install packages."
        EndSection
        exit
    else
        Success "Packages installed."
    fi
    EndSection
}
function CloneRepo() {
    BeginSection "Clone repo"
    if [ -d ./OJ ]; then
        git config --global safe.directory '*' >/dev/null 2>&1
        if [ $(git -C ./OJ remote -v | grep -c "CYEZOI/OJ.git") -gt 0 ]; then
            Wait "Updating repo..."
            git -C ./OJ pull --depth 1  >/dev/null 2>&1
            if [ $? -ne 0 ]; then
                Error "Failed to update repo."
                EndSection
                exit
            else
                Success "Repo updated."
            fi
            EndSection
            return
        else
            Error "Folder \"OJ\" already exists and is not a git repo or the url is incorrect."
            EndSection
            exit
        fi
    fi
    Wait "Cloning repo..."
    git clone https://github.com/CYEZOI/OJ.git --depth 1 >/dev/null 2>&1
    if [ $? -ne 0 ]; then
        Error "Failed to clone repo."
        EndSection
        exit
    else
        Success "Repo cloned."
    fi
    EndSection
}
function CreateJudgeUser() {
    BeginSection "Create judge user"
    if [ $(cat /etc/passwd | grep -c "Judger") -ne 0 ]; then
        Warning "Judge user already exists."
        EndSection
        return
    fi
    Wait "Creating judge user..."
    groupadd judge
    useradd Judger -g judge
    passwd Judger -d >/dev/null 2>&1
    mkdir /home/Judger
    chown -R Judger /home/Judger
    chgrp -R judge /home/Judger
    echo -e "Judger\tALL=(ALL:ALL) ALL" | tee -a /etc/sudoers >/dev/null 2>&1
    chmod a+s /bin/chgrp
    chmod a+s /bin/chown
    mkdir /home/Judger/Run
    chown -R Judger /home/Judger/Run
    chgrp -R judge /home/Judger/Run
    mkdir /home/Judger/IOData
    chown -R Judger /home/Judger/IOData
    chgrp -R judge /home/Judger/IOData
    if [ $? -ne 0 ]; then
        Error "Failed to create judge user."
        EndSection
        exit
    else
        Success "Judge user created."
    fi
    EndSection
}
function ConfigDatabase() {
    BeginSection "Config database"
    usermod -d /var/lib/mysql/ mysql >/dev/null 2>&1
    service mysql start >/dev/null 2>&1
    if [ $? -ne 0 ]; then
        Error "Failed to start mysql service."
        EndSection
        exit
    else
        Success "Mysql service started."
    fi
    if [ -d /etc/OJ ]; then
        if [ -f /etc/OJ/DatabaseHost ] && [ -f /etc/OJ/DatabasePort ] && [ -f /etc/OJ/DatabaseUsername ] && [ -f /etc/OJ/DatabasePassword ] && [ -f /etc/OJ/DatabaseName ]; then
            echo "[client]" > /etc/OJ/DatabaseConfig.cnf
            echo "user = \"$(cat /etc/OJ/DatabaseUsername)\"" >> /etc/OJ/DatabaseConfig.cnf
            echo "password = \"$(cat /etc/OJ/DatabasePassword)\"" >> /etc/OJ/DatabaseConfig.cnf
            echo "host = \"$(cat /etc/OJ/DatabaseHost)\"" >> /etc/OJ/DatabaseConfig.cnf
            echo "port = \"$(cat /etc/OJ/DatabasePort)\"" >> /etc/OJ/DatabaseConfig.cnf
            echo "database = \"$(cat /etc/OJ/DatabaseName)\"" >> /etc/OJ/DatabaseConfig.cnf
            
            mysql --defaults-extra-file=/etc/OJ/DatabaseConfig.cnf -e "SELECT 1;" >/dev/null 2>&1
            if [ $? -ne 0 ]; then
                Error "Failed to login to database with the config files in \"/etc/OJ\"."
                Confirm "Do you want to reconfigure the database?"
                if [ $? -eq 1 ]; then
                    rm -rf /etc/OJ >/dev/null 2>&1
                else
                    EndSection
                    exit
                fi
            else
                Warning "Database already configured."
                EndSection
                return
            fi
        else
            Error "Folder \"/etc/OJ\" already exists and is not a valid database config folder."
            Confirm "Do you want to remove the folder \"/etc/OJ\" and reconfigure the database?"
            if [ $? -eq 1 ]; then
                rm -rf /etc/OJ >/dev/null 2>&1
            else
                EndSection
                exit
            fi
        fi
    fi
    DatabaseUsername="OJUser"
    DatabasePassword=$(cat /dev/urandom | tr -dc 'a-zA-Z0-9' | fold -w 32 | head -n 1)
    MYSQL_PWD=$DatabasePassword
    mkdir /etc/OJ
    echo "localhost" >/etc/OJ/DatabaseHost
    echo "3306" >/etc/OJ/DatabasePort
    echo "$DatabaseUsername" >/etc/OJ/DatabaseUsername
    echo "$DatabasePassword" >/etc/OJ/DatabasePassword
    echo "OJ" >/etc/OJ/DatabaseName
    Success "Database configure files created."
    
    echo "CREATE DATABASE OJ;" >./ConfigDatabase.sql
    echo "CREATE USER $DatabaseUsername IDENTIFIED BY \"$DatabasePassword\";" >>./ConfigDatabase.sql
    echo "GRANT ALL ON OJ.* TO $DatabaseUsername;" >>./ConfigDatabase.sql
    echo "FLUSH PRIVILEGES;" >>./ConfigDatabase.sql
    mysql <./ConfigDatabase.sql
    if [ $? -ne 0 ]; then
        Error "Failed to create database \"OJ\" and user \"$DatabaseUsername\" using file \"./ConfigDatabase.sql\"."
        Warning "Maybe either the database or the user already exists."
        Confirm "Do you want to remove the database and user (MAY CAUSE DATA LOSS)?"
        if [ $? -eq 1 ]; then
            echo "DROP DATABASE OJ IF EXISTS;" >./RemoveDatabase.sql
            echo "DROP USER $DatabaseUsername IF EXISTS;" >>./RemoveDatabase.sql
            mysql <./RemoveDatabase.sql
            if [ $? -ne 0 ]; then
                Error "Failed to remove database \"OJ\" or user \"$DatabaseUsername\"."
            else
                Success "Database removed, please rerun this script."
            fi
        fi
        EndSection
        exit
    else
        Success "Database created."
    fi
    
    echo "[client]" > /etc/OJ/DatabaseConfig.cnf
    echo "user = \"$DatabaseUsername\"" >> /etc/OJ/DatabaseConfig.cnf
    echo "password = \"$DatabasePassword\"" >> /etc/OJ/DatabaseConfig.cnf
    echo "host = \"localhost\"" >> /etc/OJ/DatabaseConfig.cnf
    echo "port = \"3306\"" >> /etc/OJ/DatabaseConfig.cnf
    echo "database = \"OJ\"" >> /etc/OJ/DatabaseConfig.cnf
    EndSection
}
function CreateTables() {
    BeginSection "Create tables"
    Wait "Creating tables..."
    mysql $(cat /etc/OJ/DatabaseName) <./OJ/Database.sql
    if [ $? -ne 0 ]; then
        Error "Failed to import database using file \"./OJ/Database.sql\"."
        EndSection
        exit
    else
        Success "Database imported."
    fi
    EndSection
}
function CreateAdminAccount() {
    BeginSection "Create admin account"
    if [ $(mysql -e "SELECT COUNT(*) FROM Users WHERE Role = 0;" $(cat /etc/OJ/DatabaseName) | grep -c "0") -eq 1 ]; then
        Input "Please enter the username of the admin account."
        AdminUsername=$InputResult
        while [ 1 ]; do
            Input "Please enter the password of the admin account." -p
            AdminPassword=$InputResult
            Input "Please enter the password of the admin account again." -p
            AdminPasswordAgain=$InputResult
            if [ "$AdminPassword" != "$AdminPasswordAgain" ]; then
                Warning "The passwords you entered are not the same. Please try again."
            else
                break
            fi
        done
        Input "Please enter the nickname of the admin account."
        AdminNickname=$InputResult
        Input "Please enter the email of the admin account."
        AdminEmail=$InputResult
        HashedPassword=$(echo -n "$PasswordSalt1$AdminPassword$PasswordSalt2" | sha256sum | awk '{print $1}')
        echo "INSERT INTO Users (\`Username\`, \`Password\`, \`Nickname\`, \`EmailAddress\`, \`Role\`) VALUES ('$AdminUsername', '$HashedPassword', '$AdminNickname', '$AdminEmail', 0);" >./CreateAdminAccount.sql
        mysql $(cat /etc/OJ/DatabaseName) <./CreateAdminAccount.sql
        if [ $? -ne 0 ]; then
            Error "Failed to create admin account."
            EndSection
            exit
        else
            Success "Admin account created."
        fi
    else
        Warning "Admin account already exists."
    fi
    EndSection
}
function CreateDefaultProblem() {
    BeginSection "Create default problem"
    if [ $(mysql -e "SELECT COUNT(*) FROM Problems;" $(cat /etc/OJ/DatabaseName) | grep -c "0") -eq 1 ]; then
        echo "INSERT INTO \`Problems\` (\`PID\`, \`Title\`, \`IOFilename\`, \`Description\`, \`Input\`, \`Output\`, \`Range\`, \`Hint\`, \`Samples\`, \`TestGroups\`) VALUES (1000, 'Add', 'Add', '***Add*** two numbers \$a\$ and \$b\$', 'two numbers in _one line_', 'one number', '\$\$-2^{32} < a,b < 2^{32}\$\$', '', '[{\"Description\":\"\",\"Input\":\"1 2\",\"Output\":\"3\"}]', '[{\"TGID\":0,\"TestCases\":[{\"Answer\":\"3\",\"Input\":\"1 2\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":0,\"TimeLimit\":1000},{\"Answer\":\"5\",\"Input\":\"2 3\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":1,\"TimeLimit\":1000},{\"Answer\":\"50\",\"Input\":\"23 27\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":2,\"TimeLimit\":1000},{\"Answer\":\"29\",\"Input\":\"20 09\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":3,\"TimeLimit\":1000},{\"Answer\":\"18\",\"Input\":\"01 17\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":4,\"TimeLimit\":1000},{\"Answer\":\"67\",\"Input\":\"45 22\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":5,\"TimeLimit\":1000},{\"Answer\":\"198\",\"Input\":\"99 99\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":6,\"TimeLimit\":1000},{\"Answer\":\"1011\",\"Input\":\"465 546\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":7,\"TimeLimit\":1000},{\"Answer\":\"579\",\"Input\":\"123 456\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":8,\"TimeLimit\":1000},{\"Answer\":\"1356\",\"Input\":\"877 479\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":9,\"TimeLimit\":1000}]},{\"TGID\":1,\"TestCases\":[{\"Answer\":\"4294967296\",\"Input\":\"2147483648 2147483648\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":0,\"TimeLimit\":1000},{\"Answer\":\"12185866737\",\"Input\":\"6567867981 5617998756\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":1,\"TimeLimit\":1000},{\"Answer\":\"11111111100\",\"Input\":\"1234567890 9876543210\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":2,\"TimeLimit\":1000},{\"Answer\":\"3825938259\",\"Input\":\"1357924680 2468013579\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":3,\"TimeLimit\":1000},{\"Answer\":\"7800224355\",\"Input\":\"1122334455 6677889900\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":4,\"TimeLimit\":1000}]},{\"TGID\":2,\"TestCases\":[{\"Answer\":\"0\",\"Input\":\"1 -1\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":0,\"TimeLimit\":1000},{\"Answer\":\"-1\",\"Input\":\"2 -3\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":1,\"TimeLimit\":1000},{\"Answer\":\"-4\",\"Input\":\"23 -27\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":2,\"TimeLimit\":1000},{\"Answer\":\"11\",\"Input\":\"20 -09\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":3,\"TimeLimit\":1000},{\"Answer\":\"-16\",\"Input\":\"01 -17\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":4,\"TimeLimit\":1000},{\"Answer\":\"23\",\"Input\":\"45 -22\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":5,\"TimeLimit\":1000},{\"Answer\":\"0\",\"Input\":\"99 -99\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":6,\"TimeLimit\":1000},{\"Answer\":\"-81\",\"Input\":\"465 -546\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":7,\"TimeLimit\":1000},{\"Answer\":\"-333\",\"Input\":\"123 -456\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":8,\"TimeLimit\":1000},{\"Answer\":\"398\",\"Input\":\"877 -479\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":9,\"TimeLimit\":1000}]},{\"TGID\":3,\"TestCases\":[{\"Answer\":\"0\",\"Input\":\"2147483647 -2147483647\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":0,\"TimeLimit\":1000},{\"Answer\":\"949869225\",\"Input\":\"6567867981 -5617998756\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":1,\"TimeLimit\":1000},{\"Answer\":\"-8641975320\",\"Input\":\"1234567890 -9876543210\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":2,\"TimeLimit\":1000},{\"Answer\":\"-1110088899\",\"Input\":\"1357924680 -2468013579\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":3,\"TimeLimit\":1000},{\"Answer\":\"-5555555445\",\"Input\":\"1122334455 -6677889900\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":4,\"TimeLimit\":1000}]}]');" >./CreateDefaultProblem.sql
        mysql $(cat /etc/OJ/DatabaseName) <./CreateDefaultProblem.sql
        if [ $? -ne 0 ]; then
            Error "Failed to create default problem."
            EndSection
            exit
        else
            Success "Default problem created."
        fi
    else
        Warning "Problems already exist, skipping."
    fi
    EndSection
}
function ConfigureSettings() {
    BeginSection "Configure settings"
    if [ $(mysql -e "SELECT COUNT(*) FROM Settings;" $(cat /etc/OJ/DatabaseName) | grep -c "0") -eq 1 ]; then
        Warning "As the server use the email to send verification code when registering or resetting password, you need to set the email server."
        Warning "You can use the email server provided by Microsoft, Google, or other email service providers."
        Input "Please enter the smtp server. (eg. smtp-mail.outlook.com)"
        EmailServer=$InputResult
        Input "Please enter the port of the email server. (Most of the time it is 587)"
        EmailPort=$InputResult
        Input "Please enter the email address you want to use."
        EmailAddress=$InputResult
        Input "Please enter password or authentication code of the email address you want to use." -p
        EmailPassword=$InputResult
        echo "INSERT INTO \`Settings\` (\`Key\`, \`Value\`) VALUES ('EmailServer', 'smtp://$EmailServer:$EmailPort');" >./ConfigureSettings.sql
        echo "INSERT INTO \`Settings\` (\`Key\`, \`Value\`) VALUES ('EmailUsername', '$EmailAddress');" >>./ConfigureSettings.sql
        echo "INSERT INTO \`Settings\` (\`Key\`, \`Value\`) VALUES ('EmailPassword', '$EmailPassword');" >>./ConfigureSettings.sql
        echo "INSERT INTO \`Settings\` (\`Key\`, \`Value\`) VALUES ('PasswordSalt1', '$PasswordSalt1');" >>./ConfigureSettings.sql
        echo "INSERT INTO \`Settings\` (\`Key\`, \`Value\`) VALUES ('PasswordSalt2', '$PasswordSalt2');" >>./ConfigureSettings.sql
        echo "INSERT INTO \`Settings\` (\`Key\`, \`Value\`) VALUES ('Compiler', '/bin/g++');" >>./ConfigureSettings.sql
        echo "INSERT INTO \`Settings\` (\`Key\`, \`Value\`) VALUES ('CompileOutputLimit', '134217728');" >>./ConfigureSettings.sql
        echo "INSERT INTO \`Settings\` (\`Key\`, \`Value\`) VALUES ('CompileTimeLimit', '10000');" >>./ConfigureSettings.sql
        echo "INSERT INTO \`Settings\` (\`Key\`, \`Value\`) VALUES ('JudgeUserGroupID', '1001');" >>./ConfigureSettings.sql
        echo "INSERT INTO \`Settings\` (\`Key\`, \`Value\`) VALUES ('JudgeUserID', '1001');" >>./ConfigureSettings.sql
        echo "INSERT INTO \`Settings\` (\`Key\`, \`Value\`) VALUES ('JudgeUsername', 'Judger');" >>./ConfigureSettings.sql
        echo "INSERT INTO \`Settings\` (\`Key\`, \`Value\`) VALUES ('SocketPort', '80');" >>./ConfigureSettings.sql
        echo "INSERT INTO \`Settings\` (\`Key\`, \`Value\`) VALUES ('SystemCallList', '-1,-1,0,-1,-1,-1,0,0,-1,-1,-1,-1,-1,-1,-1,0,0,-1,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,1,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,-1,0,0,0,-1,0,0,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0');" >>./ConfigureSettings.sql
        mysql $(cat /etc/OJ/DatabaseName) <./ConfigureSettings.sql
        if [ $? -ne 0 ]; then
            Error "Failed to configure settings."
            EndSection
            exit
        else
            Success "Settings configured."
        fi
    else
        Warning "Settings already exist, skipping."
    fi
    EndSection
}

echo -e "\033[?25l"
echo -e "\033[33m"
echo "OJ  Copyright (C) 2024  langningchen"
echo "This program comes with ABSOLUTELY NO WARRANTY."
echo "This is free software, and you are welcome to redistribute it under certain conditions."
echo -e "\033[0m"

BasicChecks
InstallPackages
CloneRepo
CreateJudgeUser
ConfigDatabase
CreateTables
CreateAdminAccount
CreateDefaultProblem
ConfigureSettings

echo -e "\033[4;5;32mInstallation complete\033[0m"
echo -e "\033[33mThe server is installed in the folder \"./OJ\". You can start the server by running \"./OJ/Run.bash\".\033[0m"
echo -e "Please go to \"https://github.com/CYEZOI/OJ\" for more information or to report bugs."
echo "We strongly recommend you to install the dbeaver database management tool at \"https://dbeaver.io/\"."
echo ""
echo "Thank you for installing this OJ."
echo "Have a nice day!"
echo -e "\033[?25h"
function Wait() {
    Output "\033[34m○ $1\033[0m"
    echo -e "\033[2A"
}
function Confirm() {
    Output "\033[35m? $1 (Y/n) \033[0m" -n
    read -n 1 -p "" ConfirmData
    echo -e "\033[?25l"
    if [ "$ConfirmData" == "Y" ] || [ "$ConfirmData" == "y" ]; then
        return 1
        elif [ "$ConfirmData" == "" ]; then
        echo -n -e "\033[1A"
        return 1
        elif [ "$ConfirmData" == "N" ] || [ "$ConfirmData" == "n" ]; then
        return 0
    else
        Confirm "$1"
        return $?
    fi
}
function Input() {
    Output "\033[36m? $1 \033[0m" -n
    if [ "$2" == "-p" ]; then
        read -s -p "" InputData
    else
        read -p "" InputData
    fi
    echo -e "\033[?25l"
    if [ "$2" != "-p" ]; then
        echo -n -e "\033[1A"
    fi
    export InputResult=$InputData
}

function BasicChecks() {
    BeginSection "Basic checks"
    if [ ! -f /etc/os-release ] || [ $(cat /etc/os-release | grep -c "Ubuntu") -eq 0 ]; then
        Error "This script only supports Ubuntu."
        EndSection
        exit
        elif [ $(id -u) -ne 0 ]; then
        Error "Please run with root privilege."
        EndSection
        exit
    else
        Success "All checks passed."
    fi
    EndSection
}
function InstallPackages() {
    BeginSection "Install packages"
    Wait "Updating package list..."
    apt-get update >/dev/null 2>&1
    if [ $? -ne 0 ]; then
        Error "Failed to update package list."
        EndSection
        exit
    else
        Success "Package list updated."
    fi
    
    Wait "Upgrading packages..."
    apt-get upgrade -y >/dev/null 2>&1
    if [ $? -ne 0 ]; then
        Error "Failed to upgrade packages."
        EndSection
        exit
    else
        Success "Packages upgraded."
    fi
    
    Wait "Installing packages..."
    apt-get install g++ make cmake git libcurl4-gnutls-dev mysql-client mysql-server libmysqlcppconn-dev mount libssl-dev libminizip-dev -y >/dev/null 2>&1
    if [ $? -ne 0 ]; then
        Error "Failed to install packages."
        EndSection
        exit
    else
        Success "Packages installed."
    fi
    EndSection
}
function CloneRepo() {
    BeginSection "Clone repo"
    if [ -d ./OJ ]; then
        git config --global safe.directory '*' >/dev/null 2>&1
        if [ $(git -C ./OJ remote -v | grep -c "CYEZOI/OJ.git") -gt 0 ]; then
            Wait "Updating repo..."
            git -C ./OJ pull --depth 1  >/dev/null 2>&1
            if [ $? -ne 0 ]; then
                Error "Failed to update repo."
                EndSection
                exit
            else
                Success "Repo updated."
            fi
            EndSection
            return
        else
            Error "Folder \"OJ\" already exists and is not a git repo or the url is incorrect."
            EndSection
            exit
        fi
    fi
    Wait "Cloning repo..."
    git clone https://github.com/CYEZOI/OJ.git --depth 1 >/dev/null 2>&1
    if [ $? -ne 0 ]; then
        Error "Failed to clone repo."
        EndSection
        exit
    else
        Success "Repo cloned."
    fi
    EndSection
}
function CreateJudgeUser() {
    BeginSection "Create judge user"
    if [ $(cat /etc/passwd | grep -c "Judger") -ne 0 ]; then
        Warning "Judge user already exists."
        EndSection
        return
    fi
    Wait "Creating judge user..."
    groupadd judge
    useradd Judger -g judge
    passwd Judger -d >/dev/null 2>&1
    mkdir /home/Judger
    chown -R Judger /home/Judger
    chgrp -R judge /home/Judger
    echo -e "Judger\tALL=(ALL:ALL) ALL" | tee -a /etc/sudoers >/dev/null 2>&1
    chmod a+s /bin/chgrp
    chmod a+s /bin/chown
    mkdir /home/Judger/Run
    chown -R Judger /home/Judger/Run
    chgrp -R judge /home/Judger/Run
    mkdir /home/Judger/IOData
    chown -R Judger /home/Judger/IOData
    chgrp -R judge /home/Judger/IOData
    if [ $? -ne 0 ]; then
        Error "Failed to create judge user."
        EndSection
        exit
    else
        Success "Judge user created."
    fi
    EndSection
}
function ConfigDatabase() {
    BeginSection "Config database"
    usermod -d /var/lib/mysql/ mysql >/dev/null 2>&1
    service mysql start >/dev/null 2>&1
    if [ $? -ne 0 ]; then
        Error "Failed to start mysql service."
        EndSection
        exit
    else
        Success "Mysql service started."
    fi
    if [ -d /etc/OJ ]; then
        if [ -f /etc/OJ/DatabaseHost ] && [ -f /etc/OJ/DatabasePort ] && [ -f /etc/OJ/DatabaseUsername ] && [ -f /etc/OJ/DatabasePassword ] && [ -f /etc/OJ/DatabaseName ]; then
            echo "[client]" > /etc/OJ/DatabaseConfig.cnf
            echo "user = \"$(cat /etc/OJ/DatabaseUsername)\"" >> /etc/OJ/DatabaseConfig.cnf
            echo "password = \"$(cat /etc/OJ/DatabasePassword)\"" >> /etc/OJ/DatabaseConfig.cnf
            echo "host = \"$(cat /etc/OJ/DatabaseHost)\"" >> /etc/OJ/DatabaseConfig.cnf
            echo "port = \"$(cat /etc/OJ/DatabasePort)\"" >> /etc/OJ/DatabaseConfig.cnf
            echo "database = \"$(cat /etc/OJ/DatabaseName)\"" >> /etc/OJ/DatabaseConfig.cnf
            
            mysql --defaults-extra-file=/etc/OJ/DatabaseConfig.cnf -e "SELECT 1;" >/dev/null 2>&1
            if [ $? -ne 0 ]; then
                Error "Failed to login to database with the config files in \"/etc/OJ\"."
                Confirm "Do you want to reconfigure the database?"
                if [ $? -eq 1 ]; then
                    rm -rf /etc/OJ >/dev/null 2>&1
                else
                    EndSection
                    exit
                fi
            else
                Warning "Database already configured."
                EndSection
                return
            fi
        else
            Error "Folder \"/etc/OJ\" already exists and is not a valid database config folder."
            Confirm "Do you want to remove the folder \"/etc/OJ\" and reconfigure the database?"
            if [ $? -eq 1 ]; then
                rm -rf /etc/OJ >/dev/null 2>&1
            else
                EndSection
                exit
            fi
        fi
    fi
    DatabaseUsername="OJUser"
    DatabasePassword=$(cat /dev/urandom | tr -dc 'a-zA-Z0-9' | fold -w 32 | head -n 1)
    MYSQL_PWD=$DatabasePassword
    mkdir /etc/OJ
    echo "localhost" >/etc/OJ/DatabaseHost
    echo "3306" >/etc/OJ/DatabasePort
    echo "$DatabaseUsername" >/etc/OJ/DatabaseUsername
    echo "$DatabasePassword" >/etc/OJ/DatabasePassword
    echo "OJ" >/etc/OJ/DatabaseName
    Success "Database configure files created."
    
    echo "CREATE DATABASE OJ;" >./ConfigDatabase.sql
    echo "CREATE USER $DatabaseUsername IDENTIFIED BY \"$DatabasePassword\";" >>./ConfigDatabase.sql
    echo "GRANT ALL ON OJ.* TO $DatabaseUsername;" >>./ConfigDatabase.sql
    echo "FLUSH PRIVILEGES;" >>./ConfigDatabase.sql
    mysql <./ConfigDatabase.sql
    if [ $? -ne 0 ]; then
        Error "Failed to create database \"OJ\" and user \"$DatabaseUsername\" using file \"./ConfigDatabase.sql\"."
        Warning "Maybe either the database or the user already exists."
        Confirm "Do you want to remove the database and user (MAY CAUSE DATA LOSS)?"
        if [ $? -eq 1 ]; then
            echo "DROP DATABASE OJ IF EXISTS;" >./RemoveDatabase.sql
            echo "DROP USER $DatabaseUsername IF EXISTS;" >>./RemoveDatabase.sql
            mysql <./RemoveDatabase.sql
            if [ $? -ne 0 ]; then
                Error "Failed to remove database \"OJ\" or user \"$DatabaseUsername\"."
            else
                Success "Database removed, please rerun this script."
            fi
        fi
        EndSection
        exit
    else
        Success "Database created."
    fi
    
    echo "[client]" > /etc/OJ/DatabaseConfig.cnf
    echo "user = \"$DatabaseUsername\"" >> /etc/OJ/DatabaseConfig.cnf
    echo "password = \"$DatabasePassword\"" >> /etc/OJ/DatabaseConfig.cnf
    echo "host = \"localhost\"" >> /etc/OJ/DatabaseConfig.cnf
    echo "port = \"3306\"" >> /etc/OJ/DatabaseConfig.cnf
    echo "database = \"OJ\"" >> /etc/OJ/DatabaseConfig.cnf
    EndSection
}
function CreateTables() {
    BeginSection "Create tables"
    Wait "Creating tables..."
    mysql $(cat /etc/OJ/DatabaseName) <./OJ/Database.sql
    if [ $? -ne 0 ]; then
        Error "Failed to import database using file \"./OJ/Database.sql\"."
        EndSection
        exit
    else
        Success "Database imported."
    fi
    EndSection
}
function CreateAdminAccount() {
    BeginSection "Create admin account"
    if [ $(mysql -e "SELECT COUNT(*) FROM Users WHERE Role = 0;" $(cat /etc/OJ/DatabaseName) | grep -c "0") -eq 1 ]; then
        Input "Please enter the username of the admin account."
        AdminUsername=$InputResult
        while [ 1 ]; do
            Input "Please enter the password of the admin account." -p
            AdminPassword=$InputResult
            Input "Please enter the password of the admin account again." -p
            AdminPasswordAgain=$InputResult
            if [ "$AdminPassword" != "$AdminPasswordAgain" ]; then
                Warning "The passwords you entered are not the same. Please try again."
            else
                break
            fi
        done
        Input "Please enter the nickname of the admin account."
        AdminNickname=$InputResult
        Input "Please enter the email of the admin account."
        AdminEmail=$InputResult
        HashedPassword=$(echo -n "$PasswordSalt1$AdminPassword$PasswordSalt2" | sha256sum | awk '{print $1}')
        echo "INSERT INTO Users (\`Username\`, \`Password\`, \`Nickname\`, \`EmailAddress\`, \`Role\`) VALUES ('$AdminUsername', '$HashedPassword', '$AdminNickname', '$AdminEmail', 0);" >./CreateAdminAccount.sql
        mysql $(cat /etc/OJ/DatabaseName) <./CreateAdminAccount.sql
        if [ $? -ne 0 ]; then
            Error "Failed to create admin account."
            EndSection
            exit
        else
            Success "Admin account created."
        fi
    else
        Warning "Admin account already exists."
    fi
    EndSection
}
function CreateDefaultProblem() {
    BeginSection "Create default problem"
    if [ $(mysql -e "SELECT COUNT(*) FROM Problems;" $(cat /etc/OJ/DatabaseName) | grep -c "0") -eq 1 ]; then
        echo "INSERT INTO \`Problems\` (\`PID\`, \`Title\`, \`IOFilename\`, \`Description\`, \`Input\`, \`Output\`, \`Range\`, \`Hint\`, \`Samples\`, \`TestGroups\`) VALUES (1000, 'Add', 'Add', '***Add*** two numbers \$a\$ and \$b\$', 'two numbers in _one line_', 'one number', '\$\$-2^{32} < a,b < 2^{32}\$\$', '', '[{\"Description\":\"\",\"Input\":\"1 2\",\"Output\":\"3\"}]', '[{\"TGID\":0,\"TestCases\":[{\"Answer\":\"3\",\"Input\":\"1 2\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":0,\"TimeLimit\":1000},{\"Answer\":\"5\",\"Input\":\"2 3\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":1,\"TimeLimit\":1000},{\"Answer\":\"50\",\"Input\":\"23 27\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":2,\"TimeLimit\":1000},{\"Answer\":\"29\",\"Input\":\"20 09\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":3,\"TimeLimit\":1000},{\"Answer\":\"18\",\"Input\":\"01 17\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":4,\"TimeLimit\":1000},{\"Answer\":\"67\",\"Input\":\"45 22\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":5,\"TimeLimit\":1000},{\"Answer\":\"198\",\"Input\":\"99 99\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":6,\"TimeLimit\":1000},{\"Answer\":\"1011\",\"Input\":\"465 546\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":7,\"TimeLimit\":1000},{\"Answer\":\"579\",\"Input\":\"123 456\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":8,\"TimeLimit\":1000},{\"Answer\":\"1356\",\"Input\":\"877 479\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":9,\"TimeLimit\":1000}]},{\"TGID\":1,\"TestCases\":[{\"Answer\":\"4294967296\",\"Input\":\"2147483648 2147483648\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":0,\"TimeLimit\":1000},{\"Answer\":\"12185866737\",\"Input\":\"6567867981 5617998756\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":1,\"TimeLimit\":1000},{\"Answer\":\"11111111100\",\"Input\":\"1234567890 9876543210\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":2,\"TimeLimit\":1000},{\"Answer\":\"3825938259\",\"Input\":\"1357924680 2468013579\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":3,\"TimeLimit\":1000},{\"Answer\":\"7800224355\",\"Input\":\"1122334455 6677889900\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":4,\"TimeLimit\":1000}]},{\"TGID\":2,\"TestCases\":[{\"Answer\":\"0\",\"Input\":\"1 -1\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":0,\"TimeLimit\":1000},{\"Answer\":\"-1\",\"Input\":\"2 -3\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":1,\"TimeLimit\":1000},{\"Answer\":\"-4\",\"Input\":\"23 -27\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":2,\"TimeLimit\":1000},{\"Answer\":\"11\",\"Input\":\"20 -09\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":3,\"TimeLimit\":1000},{\"Answer\":\"-16\",\"Input\":\"01 -17\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":4,\"TimeLimit\":1000},{\"Answer\":\"23\",\"Input\":\"45 -22\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":5,\"TimeLimit\":1000},{\"Answer\":\"0\",\"Input\":\"99 -99\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":6,\"TimeLimit\":1000},{\"Answer\":\"-81\",\"Input\":\"465 -546\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":7,\"TimeLimit\":1000},{\"Answer\":\"-333\",\"Input\":\"123 -456\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":8,\"TimeLimit\":1000},{\"Answer\":\"398\",\"Input\":\"877 -479\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":9,\"TimeLimit\":1000}]},{\"TGID\":3,\"TestCases\":[{\"Answer\":\"0\",\"Input\":\"2147483647 -2147483647\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":0,\"TimeLimit\":1000},{\"Answer\":\"949869225\",\"Input\":\"6567867981 -5617998756\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":1,\"TimeLimit\":1000},{\"Answer\":\"-8641975320\",\"Input\":\"1234567890 -9876543210\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":2,\"TimeLimit\":1000},{\"Answer\":\"-1110088899\",\"Input\":\"1357924680 -2468013579\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":3,\"TimeLimit\":1000},{\"Answer\":\"-5555555445\",\"Input\":\"1122334455 -6677889900\",\"MemoryLimit\":1073741824,\"Score\":100,\"TCID\":4,\"TimeLimit\":1000}]}]');" >./CreateDefaultProblem.sql
        mysql $(cat /etc/OJ/DatabaseName) <./CreateDefaultProblem.sql
        if [ $? -ne 0 ]; then
            Error "Failed to create default problem."
            EndSection
            exit
        else
            Success "Default problem created."
        fi
    else
        Warning "Problems already exist, skipping."
    fi
    EndSection
}
function ConfigureSettings() {
    BeginSection "Configure settings"
    if [ $(mysql -e "SELECT COUNT(*) FROM Settings;" $(cat /etc/OJ/DatabaseName) | grep -c "0") -eq 1 ]; then
        Warning "As the server use the email to send verification code when registering or resetting password, you need to set the email server."
        Warning "You can use the email server provided by Microsoft, Google, or other email service providers."
        Input "Please enter the smtp server. (eg. smtp-mail.outlook.com)"
        EmailServer=$InputResult
        Input "Please enter the port of the email server. (Most of the time it is 587)"
        EmailPort=$InputResult
        Input "Please enter the email address you want to use."
        EmailAddress=$InputResult
        Input "Please enter password or authentication code of the email address you want to use." -p
        EmailPassword=$InputResult
        echo "INSERT INTO \`Settings\` (\`Key\`, \`Value\`) VALUES ('EmailServer', 'smtp://$EmailServer:$EmailPort');" >./ConfigureSettings.sql
        echo "INSERT INTO \`Settings\` (\`Key\`, \`Value\`) VALUES ('EmailUsername', '$EmailAddress');" >>./ConfigureSettings.sql
        echo "INSERT INTO \`Settings\` (\`Key\`, \`Value\`) VALUES ('EmailPassword', '$EmailPassword');" >>./ConfigureSettings.sql
        echo "INSERT INTO \`Settings\` (\`Key\`, \`Value\`) VALUES ('PasswordSalt1', '$PasswordSalt1');" >>./ConfigureSettings.sql
        echo "INSERT INTO \`Settings\` (\`Key\`, \`Value\`) VALUES ('PasswordSalt2', '$PasswordSalt2');" >>./ConfigureSettings.sql
        echo "INSERT INTO \`Settings\` (\`Key\`, \`Value\`) VALUES ('Compiler', '/bin/g++');" >>./ConfigureSettings.sql
        echo "INSERT INTO \`Settings\` (\`Key\`, \`Value\`) VALUES ('CompileOutputLimit', '134217728');" >>./ConfigureSettings.sql
        echo "INSERT INTO \`Settings\` (\`Key\`, \`Value\`) VALUES ('CompileTimeLimit', '10000');" >>./ConfigureSettings.sql
        echo "INSERT INTO \`Settings\` (\`Key\`, \`Value\`) VALUES ('JudgeUserGroupID', '1001');" >>./ConfigureSettings.sql
        echo "INSERT INTO \`Settings\` (\`Key\`, \`Value\`) VALUES ('JudgeUserID', '1001');" >>./ConfigureSettings.sql
        echo "INSERT INTO \`Settings\` (\`Key\`, \`Value\`) VALUES ('JudgeUsername', 'Judger');" >>./ConfigureSettings.sql
        echo "INSERT INTO \`Settings\` (\`Key\`, \`Value\`) VALUES ('SocketPort', '80');" >>./ConfigureSettings.sql
        echo "INSERT INTO \`Settings\` (\`Key\`, \`Value\`) VALUES ('SystemCallList', '-1,-1,0,-1,-1,-1,0,0,-1,-1,-1,-1,-1,-1,-1,0,0,-1,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,1,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,-1,0,0,0,-1,0,0,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0');" >>./ConfigureSettings.sql
        mysql $(cat /etc/OJ/DatabaseName) <./ConfigureSettings.sql
        if [ $? -ne 0 ]; then
            Error "Failed to configure settings."
            EndSection
            exit
        else
            Success "Settings configured."
        fi
    else
        Warning "Settings already exist, skipping."
    fi
    EndSection
}

echo -e "\033[?25l"
echo -e "\033[33m"
echo "OJ  Copyright (C) 2024  langningchen"
echo "This program comes with ABSOLUTELY NO WARRANTY."
echo "This is free software, and you are welcome to redistribute it under certain conditions."
echo -e "\033[0m"

BasicChecks
InstallPackages
CloneRepo
CreateJudgeUser
ConfigDatabase
CreateTables
CreateAdminAccount
CreateDefaultProblem
ConfigureSettings

echo -e "\033[4;5;32mInstallation complete\033[0m"
echo -e "\033[33mThe server is installed in the folder \"./OJ\". You can start the server by running \"./OJ/Run.bash\".\033[0m"
echo -e "Please go to \"https://github.com/CYEZOI/OJ\" for more information or to report bugs."
echo "We strongly recommend you to install the dbeaver database management tool at \"https://dbeaver.io/\"."
echo ""
echo "Thank you for installing this OJ."
echo "Have a nice day!"
echo -e "\033[?25h"
