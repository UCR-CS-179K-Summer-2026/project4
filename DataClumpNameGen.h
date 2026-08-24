#ifndef DATA_CLUMP_NAME_GEN_H
#define DATA_CLUMP_NAME_GEN_H

#include <string>

class DataClumpNameGen {
    private:
        std::string getKey();
    public:
        std::string generateName(const std::string& variableList);
};

#endif // DATA_CLUMP_NAME_GEN_H