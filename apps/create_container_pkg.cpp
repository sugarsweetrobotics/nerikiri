#include <filesystem>
#include <fstream>
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip> // put_time
#include <string>  // string

#include <juiz/utils/argparse.h>
using namespace juiz;
namespace fs=std::filesystem;
namespace c=std::chrono;

namespace {

    std::string timestamp_str() {
        auto now_t = c::system_clock::to_time_t(c::system_clock::now());
        std::stringstream ss;
        ss << std::put_time(std::localtime(&now_t), "%Y-%m-%d_%H_%M_%S");
        return ss.str();
    }

    std::string replace(std::string inputStr, const std::string& tempStr, const std::string& newStr) {
        const auto pos = inputStr.find(tempStr);
        if (pos == std::string::npos) {
            return inputStr;
        }
        return inputStr.replace(pos, tempStr.length(), newStr);
    }

    void replace_and_copy(const fs::path& src, fs::path& dst, const std::string& ContainerName) {
        std::string containerName{ContainerName};
        containerName[0] = std::tolower(containerName[0]);
        std::string containername(ContainerName);
        std::transform(containername.begin(), containername.end(), containername.begin(), [](unsigned char c){ return std::tolower(c); });
        
        std::ifstream src_f(src);
        std::ofstream dst_f(dst);
        std::string lineBuffer;
        while(std::getline(src_f, lineBuffer)) {
            lineBuffer = replace(lineBuffer, "ContainerName", ContainerName);
            lineBuffer = replace(lineBuffer, "containerName", containerName);
            lineBuffer = replace(lineBuffer, "containername", containername);
            dst_f << lineBuffer << '\n';
        }
    }

    void replace_and_copy_operation(const fs::path& src, fs::path& dst, const std::string& ContainerName, const std::string& operationName) {
        std::string containerName{ContainerName};
        containerName[0] = std::tolower(containerName[0]);
        std::string containername(ContainerName);
        std::transform(containername.begin(), containername.end(), containername.begin(), [](unsigned char c){ return std::tolower(c); });
        
        std::ifstream src_f(src);
        std::ofstream dst_f(dst);
        std::string lineBuffer;
        while(std::getline(src_f, lineBuffer)) {
            lineBuffer = replace(lineBuffer, "ContainerName", ContainerName);
            lineBuffer = replace(lineBuffer, "containerName", containerName);
            lineBuffer = replace(lineBuffer, "containername", containername);
            lineBuffer = replace(lineBuffer, "operationName", operationName);
            dst_f << lineBuffer << '\n';
        }
    }


    void generateContainerCodes(const fs::path& tempDir, const std::string& ContainerName, const bool overwrite, const bool verbose) {
        std::string containerName{ContainerName};
        containerName[0] = std::tolower(containerName[0]);
        std::string containername(ContainerName);
        std::transform(containername.begin(), containername.end(), containername.begin(), [](unsigned char c){ return std::tolower(c); });
        fs::path containerDir(ContainerName);

        auto current_path = fs::current_path();
        fs::current_path(tempDir);
        for(auto& dir_entry: std::filesystem::recursive_directory_iterator{"."}) {
            if (dir_entry.path().string().find("operationName") != std::string::npos) {
                continue;
            }
            if (verbose) {
                std::cout << " - parsing " << dir_entry.path() << '\n';
            }
            std::string fileName = replace(dir_entry.path().string(), "ContainerName", ContainerName);
            fileName = replace(fileName, "containerName", containerName);
            fileName = replace(fileName, "containername", containername);
            auto newFilePath = current_path / containerDir / fs::path(fileName);
            if (fs::exists(newFilePath) && !overwrite) { /// すでに出力先が存在して，上書きオプションが設定されていない場合は何もしない．
                if (verbose) {
                    std::cout << " --- Converted to " << newFilePath << ". But file found. NOT GENERATED." << std::endl;
                }
            } else {
                if (fs::exists(newFilePath)) { // すでにファイルが存在して上書きオプションが設定されている場合
                    if (verbose) {
                        std::cout << " --- Converted to " << newFilePath << ". But file found. OVERWRITTEN." << std::endl;
                    }
                    std::string backupFilename = fileName + timestamp_str();
                    auto backupFilePath = current_path / containerDir / fs::path(backupFilename);
                    std::cout << " --- Rename from " << newFilePath << " to " << backupFilePath << std::endl;
                    fs::rename(newFilePath, backupFilePath);
                } else {
                    if (verbose) {
                        std::cout << " --- Converted to " << newFilePath << ". GENERATED." << std::endl;
                    }
                }
                replace_and_copy(dir_entry.path(), newFilePath, ContainerName);
            }        
        }
    }

    void generateOperationCodes(const fs::path& tempDir, const std::string& ContainerName, const std::string& operationName, const bool overwrite, const bool verbose) {
        std::string containerName{ContainerName};
        containerName[0] = std::tolower(containerName[0]);
        std::string containername(ContainerName);
        std::transform(containername.begin(), containername.end(), containername.begin(), [](unsigned char c){ return std::tolower(c); });
        fs::path containerDir(ContainerName);

        auto current_path = fs::current_path();
        fs::current_path(tempDir);
        for(auto& dir_entry: std::filesystem::recursive_directory_iterator{"."}) {
            if (verbose) {
                std::cout << " - parsing " << dir_entry.path() << '\n';
            }
            if (dir_entry.path().string().find("operationName") == std::string::npos) {
                continue;
            }

            std::string fileName = replace(dir_entry.path().string(), "ContainerName", ContainerName);
            fileName = replace(fileName, "containerName", containerName);
            fileName = replace(fileName, "containername", containername);
            fileName = replace(fileName, "operationName", operationName);
            auto newFilePath = current_path / containerDir / fs::path(fileName);
            if (fs::exists(newFilePath) && !overwrite) { /// すでに出力先が存在して，上書きオプションが設定されていない場合は何もしない．
                if (verbose) {
                    std::cout << " --- Converted to " << newFilePath << ". But file found. NOT GENERATED." << std::endl;
                }
            } else {
                if (fs::exists(newFilePath)) { // すでにファイルが存在して上書きオプションが設定されている場合
                    if (verbose) {
                        std::cout << " --- Converted to " << newFilePath << ". But file found. OVERWRITTEN." << std::endl;
                    }
                    std::string backupFilename = fileName + timestamp_str();
                    auto backupFilePath = current_path / containerDir / fs::path(backupFilename);
                    std::cout << " --- Rename from " << newFilePath << " to " << backupFilePath << std::endl;
                    fs::rename(newFilePath, backupFilePath);
                } else {
                    if (verbose) {
                        std::cout << " --- Converted to " << newFilePath << ". GENERATED." << std::endl;
                    }
                }
                replace_and_copy_operation(dir_entry.path(), newFilePath, ContainerName, operationName);
            }        
        }
    }
}

int main(const int argc, const char* argv[]) {
    ArgParser parser;
    parser.option("-v", "--verbose", "Show a lot of messages", juiz::NOT_REQUIRED);
    parser.option<std::string>("-t", "--templateDir", "Template Directory of ContainerPackage", juiz::NOT_REQUIRED, "./container_template");
    parser.option<std::string>("-c", "--container", "Name of Container", juiz::REQUIRED, "ContainerName");
    parser.option("-f", "--force", "Overwrite output file", juiz::NOT_REQUIRED);
    parser.option<std::string>("-o", "--operation", "Operation Name", juiz::NOT_REQUIRED, "");

    auto options = parser.parse(argc, argv);
    auto program_name = options.program_name;
    auto verbose = juiz::getBool(options.results["verbose"]);
    auto overwrite = juiz::getBool(options.results["force"]);
    auto templateDirName = juiz::getValue<std::string>(options.results["templateDir"], "./container_template");
    auto ContainerName = juiz::getValue<std::string>(options.results["container"], "ContainerName");
    auto operationName = juiz::getValue<std::string>(options.results["operation"], "");
    std::string containerName{ContainerName};
    containerName[0] = std::tolower(containerName[0]);
    std::string containername(ContainerName);
    std::transform(containername.begin(), containername.end(), containername.begin(), [](unsigned char c){ return std::tolower(c); });

    /// 入力テンプレートディレクトリの存在確認
    fs::path tempDir(templateDirName);
    if (!fs::exists(tempDir)) {
        if (verbose) {
            std::cout << program_name << ": " << "templateDir is not found (" << templateDirName << ")" << std::endl;
        }
        return -1;
    }

    /// 出力テンプレートディレクトリの存在確認
    /// もしオペレーション名が指定されているのにコンテナがなければエラー
    /// 
    fs::path containerDir(ContainerName);
    if (fs::exists(containerDir)) {
        if (verbose) {
            std::cout << program_name << ": " << "containerDir is found (" << ContainerName << ")" << std::endl;
        }
    } else {
        /// 存在しなければ
        if (operationName.length() != 0) {
            if (verbose) {
                std::cout << "Error. OperationName is defined but Container is not found." << std::endl;
            }
            return -1;
        }
        fs::create_directory(containerDir);
    }
    if (operationName.length() == 0) {
        generateContainerCodes(tempDir, ContainerName, overwrite, verbose);
    } else {
        generateOperationCodes(tempDir, ContainerName, operationName, overwrite, verbose);
    }
    return 0;
}