#ifndef _FAT_FS_H_
#define _FAT_FS_H_

int make_fat_fs(const char *dev_name, int sectors_per_cluster, int sect_size, int fat_size, int volume_name);

int fdisk_make_part(const char *dev_name, unsigned int parts_num);

/**
 * @brief 创建指定容量占比的分区
 * @param dev_name 设备名称 (e.g., "/dev/sda")
 * @param part_count 分区数量 (1-4)
 * @param ratios 容量占比数组 (e.g., [20, 20, 60] 表示20%、20%、60%)
 * @return 成功返回0，失败返回-1
 */
int fdisk_make_part_ratio(const char *dev_name, unsigned int part_count, const unsigned int ratios[]);

/**
 * @brief 获取设备节点容量
 * @param dev_name 设备名称 (e.g., "/dev/mmcblk0" "/dev/mmcblk0p1")
 * @param size     分区容量 单位MB
 * @return 成功返回0，失败返回-1
 */
int fdisk_get_dev_node_size(const char *dev_name,  unsigned int *size);

/**
 * @brief 获取设备根节点分区数量
 * @param dev_name 设备名称 (e.g., "/dev/mmcblk0")
 * @param num      分区数量
 * @return 成功返回0，失败返回-1
 */
int fdisk_get_dev_node_partition_num(const char *device_path,  unsigned int *num);

#endif
