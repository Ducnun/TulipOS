#pragma once
#include <string>
#include <vector>

class IFileSystem{
public:
    virtual ~IFileSystem() = default;
    virtual bool exists(const std::string& path) = 0;
    virtual std::string readFile(const std::string& path) = 0;
    virtual bool writeFile(const std::string& path, const std::string& content) = 0;
    virtual std::vector<std::string> listFiles() = 0;
    virtual bool deleteFile(const std::string& path) = 0;
};