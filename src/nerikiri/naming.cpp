#include <nerikiri/naming.h>

using namespace nerikiri;

std::pair<std::string, std::string> nerikiri::separateNamespaceAndInstanceName(const std::string& fullName) {
    auto tokens = nerikiri::stringSplit(fullName, name_separator);
    if (tokens.size() == 1) { return {"", ""}; }
    return {stringJoin(tokens.begin(), tokens.end()-1, name_separator), tokens[tokens.size()-1]};
}


bool nerikiri::operationValidator(const Value& opinfo) {
    if (!nameValidator(opinfo.at("typeName").stringValue())) {
        return false;
    }
    return true;
}