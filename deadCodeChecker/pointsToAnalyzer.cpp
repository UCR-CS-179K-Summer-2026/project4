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

// ---------------------------------------------------------------------------
// Orchestration: two passes over one function body
// ---------------------------------------------------------------------------

void PointsToAnalyzer::handleDeclaration(TSNode declNode, const std::string& funcName, const ParsedSource& parsedSource, const std::set<std::string>& referencedElsewhere) {
    const std::string& source = parsedSource.source;
    TSNode typeNode = ts_node_child_by_field_name(declNode, "type", strlen("type"));
    TSNode declarator = ts_node_child_by_field_name(declNode, "declarator", strlen("declarator"));
    if (ts_node_is_null(typeNode) || ts_node_is_null(declarator)) return;
    std::string declaredType = nodeText(typeNode, source);

    std::string declKind = ts_node_type(declarator);
    if (declKind == "init_declarator") {
        TSNode nameNode = ts_node_child_by_field_name(declarator, "declarator", strlen("declarator"));
        TSNode valueNode = ts_node_child_by_field_name(declarator, "value", strlen("value"));
        if (ts_node_is_null(nameNode) || ts_node_is_null(valueNode)) return;

        // nameNode may still be wrapped (pointer_declarator); use
        // DeadCodeChecker::extractIdentifierFromDeclarator to unwrap it in
        // your actual call site rather than assuming a bare identifier here.
        std::string varName = nodeText(nameNode, source);
        VarId target{funcName, varName};

        std::string valueKind = ts_node_type(valueNode);
        if (valueKind == "conditional_expression") {
            handleConditionalExpression(valueNode, target, funcName, parsedSource);
        } else if (valueKind == "identifier") {
            edges_.push_back({VarId{funcName, nodeText(valueNode, source)}, target});
        }
    } else {
        // Plain "Dog *d;" -- no initializer. Unwrap to the bare name the
        // same way (pointer_declarator -> identifier) and seed only if
        // sub-pass A saw it referenced again anywhere later in the function.
        std::string varName = nodeText(declarator, source); // unwrap as above in real integration
        bool usedLater = referencedElsewhere.count(varName) > 0;
        seedIfUsed(VarId{funcName, varName}, declaredType, usedLater);
    }
}

void PointsToAnalyzer::handleConditionalExpression(TSNode condExprNode, const VarId& targetVar, const std::string& funcName, const ParsedSource& parsedSource) {
    const std::string& source = parsedSource.source;
    TSNode consequence = ts_node_child_by_field_name(condExprNode, "consequence", strlen("consequence"));
    TSNode alternative = ts_node_child_by_field_name(condExprNode, "alternative", strlen("alternative"));

    for (TSNode arm : {consequence, alternative}) {
        if (ts_node_is_null(arm)) continue;
        if (std::string(ts_node_type(arm)) == "identifier") {
            edges_.push_back({VarId{funcName, nodeText(arm, source)}, targetVar});
        }
    }
}