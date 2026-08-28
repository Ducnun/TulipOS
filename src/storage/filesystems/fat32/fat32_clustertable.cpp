#include "storage/filesystems/fat32/fat32_clustertable.h"

FAT32ClusterTable::FAT32ClusterTable(IBlockDevice& device, const FAT32BPB& bpb) : _device(device), _bpb(bpb) {
    _fatStartSector = _bpb.reservedSectors();
    _dataStartSector = _bpb.reservedSectors() + (_bpb.numberOfFats() * _bpb.fatSize());
}

uint32_t FAT32ClusterTable::nextCluster(uint32_t cluster) {
    uint32_t fatOffset = cluster * 4;   // 32 bit lookups

    // Get sector and offset inside of sector (remainder of first calc)
    uint32_t fatSector = _fatStartSector + fatOffset / _bpb.bytesPerSector();
    uint32_t offsetInSector = fatOffset % _bpb.bytesPerSector();

    // Retrieve sector that holds index of cluster
    uint8_t sector[512];
    if (!_device.readBlock(fatSector, sector)) return FAT32_CLUSTER_INVALID;

    // Read out exact cluster index using offset inside of sector. Mask out upper nibble (FAT32 reasons)
    return util::readLE32(&sector[offsetInSector]) & 0x0FFFFFFF;
}