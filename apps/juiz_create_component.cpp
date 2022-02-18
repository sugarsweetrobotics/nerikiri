#include <filesystem>
#include <fstream>
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip> // put_time
#include <string>  // string

#include <juiz/utils/os.h>
#include <juiz/utils/argparse.h>
#include <juiz/utils/stringutil.h>

#include <juiz/utils/yaml.h>
#include <juiz/utils/json.h>
#include <juiz/value.h>


using namespace juiz;
namespace fs=std::filesystem;
namespace c=std::chrono;

namespace {

    std::string all_capital(const std::string& str) {
        std::string buf(str);
        std::transform(buf.begin(), buf.end(), buf.begin(), [](unsigned char c){ return std::toupper(c); });
        return buf;
    }

    std::string first_lower(const std::string& str) {
        std::string buf(str);
        buf[0] = std::tolower(buf[0]);
        return buf;
    }

    std::string all_lower(const std::string& str) {
        std::string buf(str);
        std::transform(buf.begin(), buf.end(), buf.begin(), [](unsigned char c){ return std::tolower(c); });
        return buf;
    }


    std::string timestamp_str() {
        auto now_t = c::system_clock::to_time_t(c::system_clock::now());
        std::stringstream ss;
        ss << std::put_time(std::localtime(&now_t), "%Y-%m-%d_%H_%M_%S");
        return ss.str();
    }

    std::string replace(std::string inputStr, const std::string& tempStr, const std::string& newStr) {
        std::string currentStr = inputStr;

        auto pos = currentStr.find(tempStr);
        while (pos != std::string::npos) {
            currentStr = currentStr.replace(pos, tempStr.length(), newStr);
            pos = currentStr.find(tempStr);
        }
        return currentStr;
    }

    std::map<std::string, std::string> setup_replace_dictionary(const std::string& ComponentName) {
        return std::map<std::string, std::string>{ 
            {"componentName", first_lower(ComponentName)},
            {"ComponentName", ComponentName},
            {"componentname", all_lower(ComponentName)},
            {"COMPONENTNAME", all_capital(ComponentName)}
        };
    }

    std::map<std::string, std::string> setup_replace_dictionary_for_container(const std::string& ContainerName) {
        return std::map<std::string, std::string>{ 
            {"containerName", first_lower(ContainerName)},
            {"ContainerName", ContainerName},
            {"containername", all_lower(ContainerName)}
        };
    }

    std::map<std::string, std::string> setup_replace_dictionary_for_operation(const std::string& operationName) {
        return std::map<std::string, std::string>{ 
            {"operationName", operationName},
            {"operationname", all_lower(operationName)}
        };
    }

    std::map<std::string, std::string> setup_replace_dictionary_for_container_operation(const std::string& ContainerName, const std::string& operationName) {
        return std::map<std::string, std::string>{ 
            {"containerName", first_lower(ContainerName)},
            {"ContainerName", ContainerName},
            {"containername", all_lower(ContainerName)},
            {"containerOperationName", operationName},
            {"containerOperationname", all_lower(operationName)}
        };
    }

    std::string replace_with_dictionary(const std::string& input_str, const std::map<std::string, std::string>& dictionary) {
        auto output_str(input_str);
        for(auto& [key, value] : dictionary) {
            output_str = replace(output_str, key, value);
        }
        return output_str;
    }

    void replace_and_copy_operation_with_dictionary(const fs::path& src, const fs::path& dst, const std::map<std::string, std::string>& dictionary) {
        std::ifstream src_f(src);
        std::ofstream dst_f(dst);
        std::string lineBuffer;
        while(std::getline(src_f, lineBuffer)) {
            dst_f << replace_with_dictionary(lineBuffer, dictionary) << '\n';
        }
    }

    std::string create_backup_file_if_exists(const fs::path& destPath, const std::string& fileName, const bool overwrite, const bool verbose) {
        const fs::path& newFilePath = destPath / fileName;
        if (fs::exists(newFilePath)) { /// すでに出力先が存在して，上書きオプションが設定されていない場合は何もしない．
            if (overwrite) {
                if (verbose) {
                    std::cout << "  FileConvert from " << newFilePath << ". But file found. OVERWRITTEN." << std::endl;
                }
                std::string backupFilename = fileName + timestamp_str();
                auto backupFilePath = destPath / fs::path(backupFilename);
                std::cout << "  Rename from " << newFilePath << " to " << backupFilePath << std::endl;
                fs::rename(newFilePath, backupFilePath);      
                return backupFilePath;  
            } else {
                if (verbose) {
                    std::cout << "  Converted to " << newFilePath << ". But file found. NOT GENERATED." << std::endl;
                }
                return "";
            }
        }
        return destPath / fileName;
    }

    using matcher_function = std::function<bool(const fs::path& filePath)>;

    /**
     * @brief 単一のファイルをリプレースしてコピーする関数
     * 
     * @param tempDir 
     * @param destPath 
     * @param filePath 
     * @param dictionary 
     * @param matcher 
     * @param overwrite 
     * @param verbose 
     */
    void generateTemplateCode(const fs::path& destPath, const fs::path& tempFilePath, const std::map<std::string, std::string>& dictionary, const bool overwrite, const bool verbose) {
        /// ファイル名をdictionaryで更新ß
        std::string fileName = replace_with_dictionary(tempFilePath.filename().string(), dictionary);
        if (create_backup_file_if_exists(destPath, fileName, overwrite, verbose).length() != 0) { // もし必要ならバックアップする
            if (verbose) {
                std::cout << "  Converted to " << destPath / fs::path(fileName) << ". GENERATED." << std::endl;
            }
            replace_and_copy_operation_with_dictionary(tempFilePath, destPath / fs::path(fileName), dictionary);
        }
    }

    void generateTemplateCodes(const fs::path& tempDir, const fs::path& destPath, const std::map<std::string, std::string>& dictionary, const matcher_function& matcher, const bool overwrite, const bool verbose) {
        if (verbose) {
            std::cout << "generatingTemplateCodes: called" << std::endl;
        }
        for(const fs::directory_entry& dir_entry: std::filesystem::recursive_directory_iterator{tempDir}) {
            if (matcher(dir_entry.path())) { /// もしmatcherでファイルが該当すると判断されたらテンプレートを更新してコピーする
                /// 表示
                if (verbose) { std::cout << "  parsing: " << dir_entry.path() << '\n'; }
                generateTemplateCode(destPath, dir_entry.path(), dictionary, overwrite, verbose);
            }
        }
        if (verbose) {
            std::cout << "  status: done" << std::endl;
        }
    }


    bool check_option_validity(juiz::Options& options, const bool verbose=false) {
        if (options.is_error()) {   
            std::cerr << "# Failed. Passed command-line argument is invalid." << std::endl;
            return false;
        }
        auto program_name = options.program_name;
        
        // もしテンプレートディレクトリが存在しないなら失敗
        auto juiz_root_list = juiz::getEnv("JUIZ_ROOT");
        std::string juiz_root = "";
        if (juiz_root_list.size() == 1) {
            juiz_root = juiz_root_list[0];
        } else if (juiz_root_list.size() > 1) {
            std::cout << "# juiz_root_list size is larger than 1" << std::endl;
            juiz_root = juiz_root_list[0];
        }
        auto templateDirName = juiz::getValue<std::string>(options.results["templateDir"], "$JUIZ_ROOT/share/component_template");
        if (templateDirName.find("$JUIZ_ROOT") != std::string::npos) {
            templateDirName = replace(templateDirName, "$JUIZ_ROOT", juiz_root);
        }
        fs::path tempDir(templateDirName);
        if (!fs::exists(tempDir)) {
            if (verbose) {
                std::cerr << "# templateDir is not found (" << templateDirName << ")" << std::endl;
            }
            return false;
        }

        /// もしコンテナオペレーションが設定されているのにコンテナが設定されていなければ
        auto ContainerName = juiz::getValue<std::string>(options.results["container"], "ContainerName");
        auto containerOperationName = juiz::getValue<std::string>(options.results["container_operation"], "");
        if (ContainerName.length() == 0 && containerOperationName.length() != 0) {
            std::cerr << "# containerOperationName is set but ContainerName is not set." << std::endl;
            return false;
        }

        /// プロジェクト名が指定されていないのに，--deestinationが設定されていない
        {
            auto proj_name = juiz::getValue<std::string>(options.results["name"], "");
            auto destination = juiz::getValue<std::string>(options.results["destination"], "");
            if (proj_name.length() == 0 && destination.length() == 0) {
                std::cerr << "# --name option or --destination option must be defined." << std::endl;
                return false;
            }
        }

        /// プロジェクト名が指定されていないのに，--deestinationが存在しない
        {
            auto proj_name = juiz::getValue<std::string>(options.results["name"], "");
            auto destination = juiz::getValue<std::string>(options.results["destination"], "");
            fs::path destinationPath(destination);
            if (proj_name.length() == 0 && !fs::exists(destinationPath)) {
                std::cerr << "# --name option must be defined if the destination path does not exists." << std::endl;
                return false;
            }
        }
        return true;
    }

    std::string detect_component_name(const fs::path& cmakelists_path) {
        std::ifstream src_f(cmakelists_path);
        std::string lineBuffer;
        while(std::getline(src_f, lineBuffer)) {
            auto pos = lineBuffer.find("project(");
            if (pos != std::string::npos) {
                auto project_name_and_dust = lineBuffer.substr(pos + std::string("project(").length());
                return project_name_and_dust.substr(0, project_name_and_dust.find(" "));
            }
        }
        return "";
    }

    std::vector<std::string> tokenize(const std::string& input) {
        return juiz::stringSplit(input);
    }

    std::vector<std::string> detect_module_names(const fs::path& cmakelists_path, const std::string& ModuleName) {
        std::ifstream src_f(cmakelists_path);
        std::vector<std::string> buf;
        std::string lineBuffer;
        std::string container_names = "";
        std::string container_names_and_dust = "";
        while(std::getline(src_f, lineBuffer)) {
            auto pos = lineBuffer.find("set(" + ModuleName); /// まずCONTAINERS変数の宣言を探す
            if (pos != std::string::npos) { /// 宣言が見つかったら
                container_names_and_dust = lineBuffer.substr(pos + std::string("set(" + ModuleName).length());
                
                pos = container_names_and_dust.find(")"); /// かっこが閉じているか確認
                while(pos == std::string::npos) {
                    if (!std::getline(src_f, lineBuffer)) { /// かっこが終わらないのにファイル終端にきたら
                        std::cout << "# FATAL ERROR: detecting file end without detecting " + ModuleName + "." << std::endl;
                        return buf;
                    }
                    container_names_and_dust += " "  + lineBuffer;
                    pos = container_names_and_dust.find(")"); /// かっこが閉じているか確認
                }
                container_names = container_names_and_dust.substr(0, pos);
                break;
            }
        }
        return tokenize(container_names);
    }


    void updateCMakeListsForModule(const fs::path& destinationPath, const std::string& ModuleName, const std::string& ContainerName, const bool overwrite, const bool verbose) {
        auto module_names = detect_module_names(destinationPath / "CMakeLists.txt", ModuleName);
        if (verbose) { 
            if (module_names.size() == 0) {
                std::cout << "No " + ModuleName + " are detected." << std::endl;
            } else {
                std::cout << "Detected " << ModuleName << " are (" << module_names.size() << "): " << std::endl;
                for(auto n : module_names) {
                    std::cout << " - " << n << std::endl;
                }
            }
        }
        if (std::find(module_names.begin(), module_names.end(), ContainerName) == module_names.end()) {
            module_names.push_back(ContainerName);
        }
        /// 設定するコンテナ名リストを作成
        const std::string newLine = "set(" + ModuleName + " " + juiz::stringJoin(module_names, ' ') + ")";
        const fs::path newPath(create_backup_file_if_exists(destinationPath, "CMakeLists.txt", true, verbose));

        std::ifstream src_f(newPath);
        std::ofstream dst_f(destinationPath / "CMakeLists.txt");
        std::string lineBuffer;
        while(std::getline(src_f, lineBuffer)) {
            auto pos = lineBuffer.find("set(" + ModuleName); /// まずCONTAINERS変数の宣言を探す
            if (pos != std::string::npos) { /// 宣言が見つかったら
                auto buf = lineBuffer;
                pos = buf.find(")"); /// かっこが閉じているか確認
                while(pos == std::string::npos) {
                    if (!std::getline(src_f, lineBuffer)) { /// かっこが終わらないのにファイル終端にきたら
                        std::cout << "FATAL ERROR: detecting file end without detecting " + ModuleName + "." << std::endl;
                        return;
                    }
                    buf += " "  + lineBuffer;
                    pos = buf.find(")"); /// かっこが閉じているか確認
                }
                dst_f << newLine << '\n';
            } else {
                dst_f << lineBuffer << '\n';
            }
        }
        dst_f.close();
        src_f.close();
    }

    void updateJShelfForContainer(const fs::path& destinationPath, const std::string& ComponentName, const std::string& ContainerName, const bool overwrite, const bool verbose) {
        std::ifstream src_f(destinationPath / (ComponentName + ".jshelf"));
        auto value = juiz::yaml::toValue(std::move(src_f));
        if(!value["containers"].hasKey("preload")) {
            value["containers"]["preload"] = juiz::Value::object();
        }
        if(!value["containers"]["preload"].hasKey(ContainerName)) {
            value["containers"]["preload"][ContainerName] = juiz::Value{
                {"operations", juiz::Value::list()}
            };
        }

        std::cout << juiz::yaml::toYAMLString(value, false);

        const fs::path newPath(create_backup_file_if_exists(destinationPath, (ComponentName + ".jshelf"), true, verbose));

        std::ofstream dst_f(destinationPath / (ComponentName + ".jshelf"));
        dst_f << juiz::yaml::toYAMLString(value, false);
        dst_f.close();
    }

    void updateJShelfForContainerOperation(const fs::path& destinationPath, const std::string& ComponentName, const std::string& ContainerName, const std::string& containerOperationName, const bool overwrite, const bool verbose) {
        std::ifstream src_f(destinationPath / (ComponentName + ".jshelf"));
        auto value = juiz::yaml::toValue(std::move(src_f));
        if(!value["containers"].hasKey("preload")) {
            value["containers"]["preload"] = juiz::Value::object();
        }
        if(!value["containers"]["preload"].hasKey(ContainerName)) {
            value["containers"]["preload"][ContainerName] = juiz::Value{
                {"operations", juiz::Value::list()}
            };
        }

        auto vlist = value["containers"]["preload"][ContainerName]["operations"].listValue();
        if(std::find(vlist.begin(), vlist.end(), containerOperationName) == vlist.end()) {
            value["containers"]["preload"][ContainerName]["operations"].push_back(containerOperationName);
        }

        std::cout << juiz::yaml::toYAMLString(value, false);

        const fs::path newPath(create_backup_file_if_exists(destinationPath, (ComponentName + ".jshelf"), true, verbose));

        std::ofstream dst_f(destinationPath / (ComponentName + ".jshelf"));
        dst_f << juiz::yaml::toYAMLString(value, false);
        dst_f.close();
    }

    void updateJShelfForOperation(const fs::path& destinationPath, const std::string& ComponentName, const std::string& operationName, const bool overwrite, const bool verbose) {
        std::ifstream src_f(destinationPath / (ComponentName + ".jshelf"));
        auto value = juiz::yaml::toValue(std::move(src_f));
        if(!value["operations"].hasKey("preload")) {
            value["operations"]["preload"] = juiz::Value::list();
        }
        auto vlist = value["operations"]["preload"].listValue();
        if(std::find(vlist.begin(), vlist.end(), operationName) == vlist.end()) {
            value["operations"]["preload"].push_back(operationName);
        }

        std::cout << juiz::yaml::toYAMLString(value, false);

        const fs::path newPath(create_backup_file_if_exists(destinationPath, (ComponentName + ".jshelf"), true, verbose));

        std::ofstream dst_f(destinationPath / (ComponentName + ".jshelf"));
        dst_f << juiz::yaml::toYAMLString(value, false);
        dst_f.close();
    }

}

int main(const int argc, const char* argv[]) {
    ArgParser parser;
    parser.option("-v", "--verbose", "Show a lot of messages", juiz::NOT_REQUIRED);
    parser.option<std::string>("-t", "--templateDir", "Template Directory of ContainerPackage (default=$JUIZ_ROOT/share/juiz/component_template)", juiz::NOT_REQUIRED, "$JUIZ_ROOT/share/juiz/component_template");
    parser.option<std::string>("-c", "--container", "Name of Container", juiz::NOT_REQUIRED, "");
    parser.option("-f", "--force", "Overwrite output file", juiz::NOT_REQUIRED);
    parser.option<std::string>("-co", "--container_operation", "Container Operation Name (ContainerName option must be set if this option is used.", juiz::NOT_REQUIRED, "");
    parser.option<std::string>("-op", "--operation", "Operation Name", juiz::NOT_REQUIRED, "");
    parser.option<std::string>("-n", "--name", "Compnent project name", juiz::NOT_REQUIRED, "");
    parser.option<std::string>("-d", "--destination", "Output Directory (default='.')", juiz::NOT_REQUIRED, "");

    auto options = parser.parse(argc, argv, true);
    if (!check_option_validity(options, true)) {
        return -1;
    }

    auto program_name = options.program_name;
    auto juiz_root_list = juiz::getEnv("JUIZ_ROOT");
    std::string juiz_root = "";
    if (juiz_root_list.size() == 1) {
        juiz_root = juiz_root_list[0];
    } else if (juiz_root_list.size() > 1) {
        std::cout << "# juiz_root_list size is larger than 1" << std::endl;
        juiz_root = juiz_root_list[0];
    }
    auto verbose = juiz::getBool(options.results["verbose"]);
    auto overwrite = juiz::getBool(options.results["force"]);
    auto templateDirName = juiz::getValue<std::string>(options.results["templateDir"], "$JUIZ_ROOT/share/juiz/component_template");
    if (templateDirName.find("$JUIZ_ROOT") != std::string::npos) {
        templateDirName = replace(templateDirName, "$JUIZ_ROOT", juiz_root);
    }
    auto ComponentName = juiz::getValue<std::string>(options.results["name"], "");
    auto ContainerName = juiz::getValue<std::string>(options.results["container"], "ContainerName");
    auto containerOperationName = juiz::getValue<std::string>(options.results["container_operation"], "");
    auto operationName = juiz::getValue<std::string>(options.results["operation"], "");
    auto destination = juiz::getValue<std::string>(options.results["destination"], "");

    /// 大文字，小文字のContainer名を取得する．　
    const std::string containerName = first_lower(ContainerName);
    const std::string containername = all_lower(ContainerName);

    if (destination == "") {
        destination = all_lower(ComponentName);
    }

    /// 入力テンプレートディレクトリの存在確認
    fs::path tempDir(templateDirName);

    const fs::path destinationPath(destination);
    if (fs::exists(destinationPath)) {
        if (verbose) {
            std::cout << "destinationPath: " << destinationPath << std::endl;
            std::cout << "  status: " << "found" << std::endl;
            std::cout << "juiz_root: " << juiz_root << std::endl;
        }
        if (!fs::exists(destinationPath / "CMakeLists.txt")) {
            if (verbose) {
                std::cout << "CMakeLists.txt: NotFound" << std::endl;
                std::cerr << "# destination path is found, but not found CMakeLists.txt in the folder." << std::endl;
                return -1;
            }
        }
        auto detected_component_name = detect_component_name(destinationPath / "CMakeLists.txt");
        
        if (ComponentName.length() == 0) {
            if (detected_component_name.length() == 0) {
                std::cerr << "# ComponentName is not defined, but destination is found. But detecting component name failed." << std::endl;
                return -1;
            }
            ComponentName = detected_component_name;
            if (verbose) {
                std::cout << "# ComponentName (" << ComponentName << ") detected." << std::endl;
            }
        } else if(ComponentName != detected_component_name) {
            std::cout << "# ComponentName is defined, but destination is found. But detecting component name (" << detected_component_name << ") is not same as ComponentName(" << ComponentName << ")." << std::endl;
            return -1;
        } else {
            
        }
    } else {
        if (verbose) {
            std::cout << "juiz_root: " << juiz_root << std::endl;
            std::cout << "destinationPath: " << destinationPath << std::endl;
            std::cout << "  status: " << "NotFound" << std::endl;
        }
        /// 存在しなければ
        fs::create_directory(destinationPath);
        auto matcher = [](const fs::path& p){
            return  p.string().find("CMakeLists.txt") != std::string::npos ||
                    p.string().find("juiz_component.cmake") != std::string::npos || 
                    p.string().find("ComponentName.") != std::string::npos;
        };
        auto dictionary = setup_replace_dictionary(ComponentName);
        generateTemplateCodes(tempDir, destinationPath, dictionary, matcher, overwrite, verbose);
    }

    if (verbose) {
        std::cout << "command: " << program_name << std::endl;
        std::cout << "  ComponentName          : " << (ComponentName) << std::endl;
        std::cout << "  componentname          : " << all_lower(ComponentName) << std::endl;
        std::cout << "  destinationPath        : " << destinationPath << std::endl;
        std::cout << "  ContainerName          : " << ContainerName << std::endl;
        std::cout << "  ContainerOperationName : " << containerOperationName << std::endl;
        std::cout << "  OperationName          : " << operationName << std::endl;
    }
    
    /// 出力テンプレートディレクトリの存在確認
    /// もしオペレーション名が指定されているのにコンテナがなければエラー
    /// 
    // fs::path outputDir(ContainerName);
    if (ContainerName.length() != 0) {
        auto matcher = [](const fs::path& p){
            return p.string().find("ContainerName.") != std::string::npos;
        };
        auto dictionary = setup_replace_dictionary_for_container(ContainerName);
        generateTemplateCodes(tempDir, destinationPath, dictionary, matcher, overwrite, verbose);
        updateCMakeListsForModule(destinationPath, "CONTAINERS", ContainerName, overwrite, verbose);
        updateJShelfForContainer(destinationPath, ComponentName, ContainerName, overwrite, verbose);
    }
    if (containerOperationName.length() != 0) {

        auto module_names = detect_module_names(destinationPath / "CMakeLists.txt", "CONTAINERS");
        if (std::find(module_names.begin(), module_names.end(), ContainerName) == module_names.end()) {
            std::cout << "# ContainerOperation (name=" << containerOperationName << ") is detected, but property ContainerName is not properly set. The container name can not be found in the component." << std::endl;
            return -1;
        }
        
        auto matcher = [](const fs::path& p){
            return p.string().find("containerOperationName") != std::string::npos;
        };
        auto dictionary = setup_replace_dictionary_for_container_operation(ContainerName, containerOperationName);
        generateTemplateCodes(tempDir, destinationPath, dictionary, matcher, overwrite, verbose);
        updateCMakeListsForModule(destinationPath, "CONTAINER_OPERATIONS", ContainerName + "_" + containerOperationName, overwrite, verbose);
        updateJShelfForContainerOperation(destinationPath, ComponentName, ContainerName, containerOperationName, overwrite, verbose);
    }
    if (operationName.length() != 0) {        
        auto matcher = [](const fs::path& p){
            return p.string().find("operationName") != std::string::npos;
        };
        auto dictionary = setup_replace_dictionary_for_operation(operationName);
        generateTemplateCodes(tempDir, destinationPath, dictionary, matcher, overwrite, verbose);
        updateCMakeListsForModule(destinationPath, "OPERATIONS", operationName, overwrite, verbose);
        updateJShelfForOperation(destinationPath, ComponentName, operationName, overwrite, verbose);
    }
    return 0;
}