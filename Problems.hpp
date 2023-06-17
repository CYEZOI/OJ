#ifndef PROBLEMS_HPP
#define PROBLEMS_HPP

#include "Result.hpp"
#include "Problem.hpp"

class PROBLEMS
{
private:
    static RESULT JSONToSamples(std::string JSONData, std::vector<SAMPLE> &Samples);
    static RESULT JSONToUnjudgedTestGroups(std::string JSONData, std::vector<TEST_GROUP_DATA> &UnjudgedTestGroups);
    static RESULT SamplesToJSON(std::vector<SAMPLE> Samples, std::string &JSONData);
    static RESULT UnjudgedTestGroupsToJSON(std::vector<TEST_GROUP_DATA> UnjudgedTestGroups, std::string &JSONData);

public:
    static RESULT GetProblem(std::string PID, PROBLEM &Problem);
    static RESULT AddProblem(PROBLEM Problem);
};

#endif
