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

    friend class API_PROCEED;

public:
    static RESULT AddProblem(PROBLEM Problem);
    static RESULT GetProblem(std::string PID, PROBLEM &Problem);
    static RESULT UpdateProblem(PROBLEM Problem);
    static RESULT DeleteProblem(std::string PID);
};

#endif
