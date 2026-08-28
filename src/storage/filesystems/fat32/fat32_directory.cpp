#include "storage/filesystems/fat32/fat32_directory.h"

FAT32Directory::FAT32Directory(IBlockDevice& device, FAT32ClusterTable& clusters, uint32_t firstCluster) : 
        _device(device),
        _clusters(clusters),
        _firstCluster(firstCluster),
        _currentCluster(firstCluster),
        _sectorIndex(0),
        _entryIndex(0)
    {}

bool FAT32Directory::next(FAT32DirectoryEntry& entry){

}