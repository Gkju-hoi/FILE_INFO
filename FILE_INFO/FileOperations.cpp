#include "FileInfo.h"
#include <iostream>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <sstream>

std::string timeToString(std::time_t time) {
    std::tm tm;
    localtime_s(&tm, &time);

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

std::map<std::string, FileInfo> getFileList(const fs::path& basePath, const fs::path& currentPath) {
    std::map<std::string, FileInfo> fileList;

    try {
        for (const auto& entry : fs::directory_iterator(basePath / currentPath)) {
            fs::path relativePath = currentPath / entry.path().filename();
            std::string relativePathStr = relativePath.string();

            FileInfo info;
            info.type = entry.status().type();
            info.relative_path = relativePathStr;

            if (entry.is_regular_file()) {
                info.size = entry.file_size();
                info.last_write_time = fs::last_write_time(entry).time_since_epoch().count();
            }
            else if (entry.is_directory()) {
                info.size = 0;
                info.last_write_time = fs::last_write_time(entry).time_since_epoch().count();

                auto subFiles = getFileList(basePath, relativePath);
                fileList.insert(subFiles.begin(), subFiles.end());
            }

            fileList[relativePathStr] = info;
        }
    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "Ошибка доступа к " << (basePath / currentPath).string() << ": " << e.what() << std::endl;
    }

    return fileList;
}

void compareDirectories(const fs::path& path1, const fs::path& path2) {
    std::cout << "Сравнение папок:\n";
    std::cout << "Папка 1: " << path1.string() << "\n";
    std::cout << "Папка 2: " << path2.string() << "\n\n";

    auto files1 = getFileList(path1);
    auto files2 = getFileList(path2);

    std::vector<std::string> onlyIn1;
    std::vector<std::string> onlyIn2;
    std::vector<std::string> differentFiles;
    std::vector<std::string> identicalFiles;

    for (const auto& [relativePath, info1] : files1) {
        if (files2.find(relativePath) == files2.end()) {
            onlyIn1.push_back(relativePath);
        }
        else {
            const auto& info2 = files2[relativePath];

            if (info1.type != info2.type) {
                differentFiles.push_back(relativePath + " (разный тип)");
            }
            else if (info1.type == fs::file_type::regular) {
                if (info1.size != info2.size || info1.last_write_time != info2.last_write_time) {
                    differentFiles.push_back(relativePath);
                }
                else {
                    identicalFiles.push_back(relativePath);
                }
            }
            else if (info1.type == fs::file_type::directory) {
                identicalFiles.push_back(relativePath + "/");
            }
        }
    }

    for (const auto& [relativePath, info2] : files2) {
        if (files1.find(relativePath) == files1.end()) {
            onlyIn2.push_back(relativePath);
        }
    }

    if (!onlyIn1.empty()) {
        std::cout << "Только в первой папке:\n";
        for (const auto& file : onlyIn1) {
            std::cout << "  " << file << "\n";
        }
        std::cout << "\n";
    }

    if (!onlyIn2.empty()) {
        std::cout << "Только во второй папке:\n";
        for (const auto& file : onlyIn2) {
            std::cout << "  " << file << "\n";
        }
        std::cout << "\n";
    }

    if (!differentFiles.empty()) {
        std::cout << "Различающиеся файлы:\n";
        for (const auto& file : differentFiles) {
            if (files1.find(file) != files1.end() && files2.find(file) != files2.end()) {
                auto info1 = files1[file];
                auto info2 = files2[file];

                std::cout << "  " << file << ":\n";
                std::cout << "    Папка 1: размер=" << info1.size
                    << ", время=" << timeToString(info1.last_write_time) << "\n";
                std::cout << "    Папка 2: размер=" << info2.size
                    << ", время=" << timeToString(info2.last_write_time) << "\n";
            }
            else {
                std::cout << "  " << file << "\n";
            }
        }
        std::cout << "\n";
    }

    if (!identicalFiles.empty()) {
        std::cout << "Идентичные файлы: " << identicalFiles.size() << "\n";
        std::cout << "\n";
    }

    std::cout << "Сравнение завершено.\n";
    std::cout << "Только в первой: " << onlyIn1.size() << " файлов\n";
    std::cout << "Только во второй: " << onlyIn2.size() << " файлов\n";
    std::cout << "Различаются: " << differentFiles.size() << " файлов\n";
    std::cout << "Идентичны: " << identicalFiles.size() << " файлов\n";
}