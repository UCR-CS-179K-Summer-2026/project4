#ifndef DETECTOR_H
#define DETECTOR_H

#include "ParsedSource.h"

class Detector {
    public:
        virtual ~Detector() = default;
        virtual int analyzeSource(const ParsedSource& parsedSource) = 0;

};

#endif