#pragma once



namespace juiz {

    class PathParser {

    public:
        static std::string brokerTypeName(const std::string& fullPath) {
            return "CoreBroker";
        }

        static std::string brokerFullPath(const std::string& fullPath) {
            return "";
        }

        static std::string operationFullName(const std::string& fullPath) {
            return fullPath;
        }
    
    };


}