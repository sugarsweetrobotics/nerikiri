#pragma once

#include <memory>
#include <string>

namespace juiz {




    class ModelDataAPI {

    };



    std::shared_ptr<ModelDataAPI> modelDataFromPath(const std::string& path);

    std::shared_ptr<ModelDataAPI> modelDataFromString(const std::string& str);
}