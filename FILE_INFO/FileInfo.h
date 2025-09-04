#pragma once

#include <filesystem>
#include <string>
#include <map>
#include <ctime>

namespace fs = std::filesystem;

struct FileInfo {
    fs::file_type type;
    uintmax_t size;
    std::time_t last_write_time;
    std::string relative_path;
};

std::map<std::string, FileInfo> getFileList(const fs::path& basePath, const fs::path& currentPath = "");
void compareDirectories(const fs::path& path1, const fs::path& path2);
std::string timeToString(std::time_t time);