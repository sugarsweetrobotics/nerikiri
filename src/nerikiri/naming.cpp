#include <juiz/utils/naming.h>

using namespace juiz;

std::pair<std::string, std::string> juiz::separateNamespaceAndInstanceName(const std::string& fullName) {
    auto tokens = juiz::stringSplit(fullName, name_separator);
    if (tokens.size() == 1) { return {"", tokens[0]}; }
    return {stringJoin(tokens.begin(), tokens.end()-1, name_separator), tokens[tokens.size()-1]};
}


bool juiz::operationValidator(const Value& opinfo) {
    if (!nameValidator(opinfo.at("typeName").stringValue())) {
        return false;
    }
    return true;
}