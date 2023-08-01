#pragma once

#include "Logger.hpp"
#include "Result.hpp"
#include "configor/json.hpp"

class SETTINGS
{
private:
    std::string DatabaseHost;
    int DatabasePort;
    std::string DatabaseUsername;
    std::string DatabasePassword;
    std::string DatabaseName;

    friend class API_PROCEED;
    friend class DATABASE;

public:
    RESULT Init();
    static RESULT GetSettings(std::string Key, std::string &Value);
    static RESULT GetSettings(std::string Key, int &Value);
    static RESULT GetSettings(configor::json &Value);
    static RESULT SetSettings(std::string Key, std::string Value);
    static RESULT SetSettings(std::string Key, int Value);
    static RESULT SetSettings(configor::json Value);
};

extern SETTINGS Settings;


/*
sudo groupadd judge
sudo useradd Judger -g judge
sudo passwd Judger -d
sudo mkdir /home/Judger
sudo chown -R Judger /home/Judger
sudo chgrp -R judge /home/Judger
echo -e "Judger\tALL=(ALL:ALL) ALL" | sudo tee -a /etc/sudoers > /dev/null
sudo chsh Judger -s /usr/bin/fish
sudo chmod -R a+rw /home/langningc2009/Coding

sudo chmod a+s /bin/chgrp
sudo chmod a+s /bin/chown
*/
