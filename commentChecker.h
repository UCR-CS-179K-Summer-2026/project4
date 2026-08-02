#ifndef COMMENT_CHECKER_H
#define COMMENT_CHECKER_H

#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <vector>
#include "ParsedSource.h"

class commentChecker : public Detector {
//    private:


    public: 
        int analyzeSource(const ParsedSource& parsedSource) override;
};
#endif