#include "Detector.h"
#include <algorithm>

std::string applyFixes(const std::string& source, std::vector<Warning>& warnings) {
    std::vector<Edit> edits;
    for (auto& w : warnings) {
        if (w.fix) edits.push_back(*w.fix);
    }

    std::sort(edits.begin(), edits.end(),
        [](const Edit& a, const Edit& b) { return a.startByte > b.startByte; });

    std::string result = source;
    uint32_t lastAppliedStart = static_cast<uint32_t>(result.size()) + 1;

    for (auto& e : edits) {
        if (e.endByte > lastAppliedStart) {
            continue; // overlaps a higher-offset edit already applied — skip
        }
        result.replace(e.startByte, e.endByte - e.startByte, e.replacement);
        lastAppliedStart = e.startByte;
    }

    return result;
}