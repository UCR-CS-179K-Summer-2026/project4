/*
    - First start with function parameters. If the same set of parameters is used in multiple functions (>= 3), flag it.
    - To look for data clumps, we can extract the parameters of each function and store them in a map where the key is
      a sorted vector of parameter types and names, and the value is a list of function names that use that set of 
      parameters. After processing all functions, we can check the map for any entries that have 3 or more function 
      names associated with them, indicating a data clump.
    - To look for data clunps in local variables, we can extract the local variable declarations within each function and store them in a similar map. 
      One thing to consider will be to ignore variables that are declared within loops or conditional blocks, as they may not be part of a data clump.
      Another thing to consider is to identify a block of variables that go together. For example, if we have a set of variables that describe a 
      street address (street, city, state, zip), we can consider them as a single data clump. But later on, if we define a new random variable called 
      dollars, we should not consider it as part of the data clump. To do this, we can look for variables that are declared 
      together in a single statement or block, and group them together as a single data clump. But if the street address 
      variables are declared in separate statements, we can still consider them as a data clump if they are used 
      together in multiple functions.
*/
#ifndef DATA_CLUMP_DETECTOR_H
#define DATA_CLUMP_DETECTOR_H

#include <vector>
#include <unordered_map>
#include <string>
#include "ParsedSource.h"
#include "Detector.h"

class DataClumpDetector : public Detector {
    private:
        std::unordered_map<std::vector<std::string>, std::vector<std::string>> varaibleClumps; // key: sorted vector of variable names, value: list of function names that use that set of variables
        void visitNode(TSNode node, const ParsedSource& parsedSource, std::vector<Warning>& warnings) override;
    public:
        DataClumpDetector() = default;
        std::vector<Warning> analyzeSource(const ParsedSource& parsedSource) override;
};

#endif // DATA_CLUMP_DETECTOR_H