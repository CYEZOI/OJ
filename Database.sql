DROP TABLE IF EXISTS `Users`;

CREATE TABLE `Users` (
    `UID` int UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY UNIQUE,
    `Username` varchar(32) NOT NULL UNIQUE,
    `Password` varchar(32) NOT NULL,
    `Nickname` varchar(32),
    `Email` text NOT NULL,
    `Privilege` int UNSIGNED NOT NULL DEFAULT 0
);

DROP TABLE IF EXISTS `Tokens`;

CREATE TABLE `Tokens` (
    `TID` int UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY UNIQUE,
    `TokenValue` varchar(32) NOT NULL UNIQUE,
    `UID` int UNSIGNED NOT NULL,
    `CreateTime` DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP
);

DROP TABLE IF EXISTS `Problems`;

CREATE TABLE `Problems` (
    `PID` int UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY UNIQUE,
    `Title` varchar(32) NOT NULL UNIQUE,
    `Description` text NOT NULL,
    `Input` text,
    `Output` text,
    `Range` text,
    `Hint` text,
    `Samples` json,
    `TestCases` json NOT NULL
);

DROP TABLE IF EXISTS `EmailVerificationCodes`;

CREATE TABLE `EmailVerificationCodes`(
    `EmailAddress` varchar(64) NOT NULL UNIQUE,
    `VerificationCode` varchar(32) NOT NULL,
    `CreateTime` DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP
);

DROP TABLE IF EXISTS `Submissions`;

CREATE TABLE `Submissions`(
    `SID` int UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY UNIQUE,
    `PID` int UNSIGNED NOT NULL,
    `UID` int UNSIGNED NOT NULL,
    `Code` text NOT NULL,
    `Result` int UNSIGNED NOT NULL DEFAULT 0,
    `Description` text,
    `Time` int UNSIGNED NOT NULL DEFAULT 0,
    `TimeSum` int UNSIGNED NOT NULL DEFAULT 0,
    `Memory` int UNSIGNED NOT NULL DEFAULT 0,
    `Score` int UNSIGNED NOT NULL DEFAULT 0,
    `EnableO2` bool NOT NULL DEFAULT false,
    `TestGroups` json,
    `CreateTime` DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP
);

DROP TABLE IF EXISTS `Problems`;

CREATE TABLE `Problems`(
    `PID` int UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY UNIQUE,
    `Title` varchar(32) NOT NULL UNIQUE,
    `IOFilename` text NOT NULL,
    `Description` text NOT NULL,
    `Input` text,
    `Output` text NOT NULL,
    `Range` text,
    `Hint` text,
    `Samples` json,
    `TestGroups` json NOT NULL
);

DROP TABLE IF EXISTS `Settings`;

CREATE TABLE `Settings`(
    `Key` varchar(32) NOT NULL UNIQUE,
    `Value` text NOT NULL
);

DROP TABLE IF EXISTS `Files`;

CREATE TABLE `Files`(
    `FID` int NOT NULL AUTO_INCREMENT PRIMARY KEY UNIQUE,
    `UID` int NOT NULL,
    `Filename` text NOT NULL,
    `FileType` text NOT NULL,
    `FileToken` varchar(32) NOT NULL UNIQUE,
    `CreateTime` DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP
);
