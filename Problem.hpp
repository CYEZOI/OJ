#ifndef PROBLEM_HPP
#define PROBLEM_HPP

#include "Sample.hpp"
#include "TestGroupData.hpp"

class PROBLEM
{
private:
    std::string PID;
    std::string Title;
    std::string Description;
    std::string Input;
    std::string Output;
    std::vector<SAMPLE> Samples;
    std::vector<TEST_GROUP_DATA> TestGroups;
    std::string Range;
    std::string Hint;
    std::string WorkDir;
    std::string IOFilename;

    friend class PROBLEM_LIST;
    friend class WEB_DATA_PROCEED;
    friend class SUBMISSION;
    friend class PROBLEMS;
    friend class SUBMISSIONS;
    friend class TEST_CASE;

public:
    bool
    operator<(const PROBLEM &Compare) const;
};

#endif
