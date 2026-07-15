#pragma once

#include "storage/ifile_system.h"
#include "iblock_device.h"

class FAT32Manager : public IFileSystem{
public:
    FAT32Manager(IBlockDevice& bd) : _bd(bd) {}
    ~FAT32Manager() = default;

    bool exists(const std::string& path) override;
    std::string readFile(const std::string& path) override;
    bool writeFile(const std::string& path, const std::string& content) override;
    std::vector<std::string> listFiles() override;
    bool deleteFile(const std::string& path) override;
private:
    IBlockDevice& _bd;
};