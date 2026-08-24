#pragma once
#include <tree_sitter/api.h>
#include <string>
#include <set>
#include <map>
#include <vector>
#include <unordered_map>
#include "ParsedSource.h"

using TypeSet = std::set<std::string>;

struct VarId {
    std::string function;
    std::string name;
    bool operator<(const VarId& o) const {
        return function != o.function ? function < o.function : name < o.name;
    }
};

struct FlowEdge {
    VarId from;
    VarId to;
};

// Class hierarchy + per-class method definitions, populated by
// DeadCodeChecker::collectClassMethods while it walks class_specifier nodes
struct ClassHierarchy {
    std::map<std::string, std::vector<std::string>> bases;             // class -> direct bases
    std::map<std::string, std::set<std::string>> methodsDefinedIn;     // class -> method names it defines

    // Nearest definition of `methodName` walking from `concreteType`
    // upward through its bases. Returns the defining class name, or ""
    // if nobody in the hierarchy defines it
    std::string resolveOverride(const std::string& concreteType,const std::string& methodName) const {
        std::string current = concreteType;
        std::set<std::string> visited; // guard against malformed/cyclic input
        while (!current.empty() && !visited.count(current)) {
            visited.insert(current);
            auto it = methodsDefinedIn.find(current);
            if (it != methodsDefinedIn.end() && it->second.count(methodName)) {
                return current;
            }
            auto baseIt = bases.find(current);
            if (baseIt == bases.end() || baseIt->second.empty()) break;
            current = baseIt->second.front(); // single inheritance assumed
        }
        return "";
    }
 
    // All known types defining methodName, used for the "empty points-to
    // set -> assume any override could be live" conservative fallback.
    std::set<std::string> allTypesDefining(const std::string& methodName) const {
        std::set<std::string> result;
        for (auto& [cls, methods] : methodsDefinedIn) {
            if (methods.count(methodName)) result.insert(cls);
        }
        return result;
    }
};

class PointsToAnalyzer {
public:
    explicit PointsToAnalyzer(const ClassHierarchy& hierarchy) : hierarchy_(hierarchy) {}
    void collectFromFunction(const std::string& funcName, TSNode funcBody, const ParsedSource& parsedSource);

    void solve();

    const TypeSet& pointsTo(const VarId& v) const;

    // Returns the set of qualified reachable methods, e.g. {"Dog::makeSound"}.
    // If the pointer's points-to set is empty, falls
    // back to marking EVERY known override of methodName reachable.
    std::set<std::string> resolveVirtualCall(const VarId& ptrVar,const std::string& methodName) const;

    // Registers the ordered parameter VarIds for a function, so call-site
    // argument propagation (helper(d) -> param x) can look them up.
    void registerParams(const std::string& funcName, const std::vector<std::string>& paramNames);

    private:
        const ClassHierarchy& hierarchy_;
        std::map<VarId, TypeSet> pointsToSets_;
        std::vector<FlowEdge> edges_;
        std::unordered_map<std::string, std::vector<VarId>> functionParams_;

        void seedIfUsed(const VarId& var, const std::string& declaredType, bool isUsedDownstream);
};