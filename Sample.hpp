#ifndef SAMPLE_HPP
#define SAMPLE_HPP

#include <string>

class SAMPLE
{
private:
    std::string Input;
    std::string Output;
    std::string Description;

    friend class API_PROCEED;
    friend class PROBLEM;
    friend class PROBLEMS;
};

#endif
