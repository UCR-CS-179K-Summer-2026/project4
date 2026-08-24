#include "PointsToAnalyzer.h"
#include <cstring>

namespace {
    std::string nodeText(TSNode node, const std::string& source) {
        uint32_t start = ts_node_start_byte(node);
        uint32_t end = ts_node_end_byte(node);
        return source.substr(start, end - start);
    }

    // Collects every bare identifier referenced anywhere in `subtree` into
    // `used`. Run over each statement in a first pass over a function body, so declaration in
    // the second pass knows whether a declared pointer is ever referenced
    // again elsewhere in the function.
    void collectReferencedIdentifiers(TSNode subtree, const std::string& source, std::set<std::string>& used) {
        std::string type = ts_node_type(subtree);
        if (type == "identifier") {
            used.insert(nodeText(subtree, source));
        }
        uint32_t count = ts_node_child_count(subtree);
        for (uint32_t i = 0; i < count; ++i) {
            collectReferencedIdentifiers(ts_node_child(subtree, i), source, used);
        }
    }
} // namespace

void PointsToAnalyzer::seedIfUsed(const VarId& var, const std::string& declaredType, bool isUsedDownstream) {
    if (!isUsedDownstream) return; // e.g. "Cat *f;" never referenced again -> not seeded
    pointsToSets_[var].insert(declaredType);
}

void PointsToAnalyzer::registerParams(const std::string& funcName, const std::vector<std::string>& paramNames) {
    std::vector<VarId> ids;
    for (auto& p : paramNames) ids.push_back(VarId{funcName, p});
    functionParams_[funcName] = ids;
}