#include "repeatedCodeChecker.h"



void RepeatedCodeChecker::visitNode(TSNode node, const ParsedSource& parsedSource, std::vector<Warning>& warnings) override{
    if (ts_node_is_null(node)){
        return;
    }

    if(strcmp(ts_node_type(node),"class_specifier") == 0){
        checkClass(node, parsedSource, warnings);
    }

    uint32_t childCOunt = ts_node_child_count(node);

    for(uint32_t i = 0; i<childCOunt; ++i){
        visitNode(ts_node_child(node,i), parsedSource, warnings);
    }
}
std::string RepeatedCodeChecker::nodeText(TSNode node, const std::string& source) const{
    uint32_t startByte = ts_node_start_byte(node);
    uint32_t endByte = ts_node_end_byte(node);
    return source.substr(startByte, endByte - startByte);
}

TSNode RepeatedCodeChecker::findChildByType(TSNode node, const char* type) const{
    uint32_t childCount = ts_node_child_count(node);

    for (uint32_t i = 0; i<childCount; ++i){
        TSNode child = ts_node_child(node,i);
        
        if(strcmp(ts_node_type(child), type) ==0){
            return child;
        }
    }

    return{};
}

std::vector<std::string> RepeatedCodeChecker::extractBaseClasses(TSNode classDefNode, const std::string& source) const{

}

void RepeatedCodeChecker::scanForInheritance(TSNode classDefNode, const std::string& source, std::vector<bool> baseUsed, std::vector<Warning>& warnings) const{

}

void RepeatedCodeChecker::checkClass(TSNode classNode, const ParsedSource& parsedSource, std::vector<Warning>& warnIngs) const{

}

std::vector<Warning> RepeatedCodeChecker::analyzeSource(const ParsedSource& parsedSource) {
    std::vector<Warning> warnings;

    if (parsedSource.tree == nullptr) {
        return warnings;
    }

    TSNode rootNode = ts_tree_root_node(parsedSource.tree);
    visitNode(rootNode, parsedSource, warnings);

    return warnings;
}
