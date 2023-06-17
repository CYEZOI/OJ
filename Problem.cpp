#include "Problem.hpp"
#include "TestGroup.hpp"
#include "Utilities.hpp"
#include <string>
#include <vector>
#include <fstream>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>

bool PROBLEM::operator<(const PROBLEM &Compare) const
{
    return PID < Compare.PID;
}
