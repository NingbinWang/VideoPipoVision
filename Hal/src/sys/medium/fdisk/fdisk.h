#ifndef __FDISK_H__
#define __FDISK_H__

#define DEFAULT_SECTOR_SIZE	512
#define MAX_SECTOR_SIZE	2048
#define SECTOR_SIZE	512	/* still used in BSD code */
#define MAXIMUM_PARTS	60

#define EXTENDED        0x05
#define WIN95_FAT32_LBA 0x0c
#define WIN98_EXTENDED  0x0f
#define LINUX_PARTITION 0x81
#define LINUX_SWAP      0x82
#define LINUX_NATIVE    0x83
#define LINUX_EXTENDED  0x85
#define LINUX_LVM       0x8e
#define LINUX_RAID      0xfd

#define IS_EXTENDED(i) \
	((i) == EXTENDED || (i) == WIN98_EXTENDED || (i) == LINUX_EXTENDED)

#define SIZE(a)	(sizeof(a)/sizeof((a)[0]))

#define cround(n)	(display_in_cyl_units ? ((n)/units_per_sector)+1 : (n))
#define scround(x)	(((x)+units_per_sector-1)/units_per_sector)

#if defined(__GNUC__) && (defined(__arm__) || defined(__alpha__))
# define PACKED __attribute__ ((packed))
#else
# define PACKED
#endif

#define _(Text) (Text)
#define N_(Text) (Text)

struct systypes {
	unsigned char type;
	char *name;
};

extern struct systypes i386_sys_types[];
extern const char * scsi_device_types[];
extern const char * scsi_short_device_types[];

enum dev_type {BLK_DEVICE, CHR_DEVICE};

#define USB_DEVICE_PATTERN "ms"
#define USB_MOUSE_DEVICE "msmouse"
#define SATA_DEVICE_PATTERN "ms"
#define DVD_DEVICE_PATTERN "dvd"
#define SATA_DVD_PATTERN "sr"
#define MAX_DEVICES_NUM 32
#define ONE_PARTITION_MB (20*1000)

#ifndef MS_SILENT
#define MS_SILENT 32768
#endif

typedef struct
{
	//NODE node;
	int partNumber; /* the partition number in the disk */
	unsigned long megabytes; /* the partition size */
	char mounted[32]; /* the mounted point, if not mounted, mounted[0] = 0 */
	unsigned char sysId __attribute__ ((aligned(4))); /* the file system type */
}PARTITION_INFO;

typedef struct
{
	char name[32]; /* the device path name: "/dev/msb", ... */
	char showName[32]; /* the show name: "USB", ... */
	int index; /* index: 1, 2, ... */
	unsigned long megabytes; /* the disk device capacity */
	//LIST partitionList; /* partition list in the device */
	int type; /* the device type: "disk", "cd/dvd", ... */
	int maj, min; /* the major and minor number of the device node */
	time_t mtime; /* time of last modification */
	int devType; /* the device type: block or char device */
	char model[128]; /* the model info: "WDC WD800JD-75MS", ... */
	char vendor[128]; /* the vendor info: "ATA", ... */
}DEVICE_INFO;

struct partition {
	unsigned char boot_ind;         /* 0x80 - active */
	unsigned char head;             /* starting head */
	unsigned char sector;           /* starting sector */
	unsigned char cyl;              /* starting cylinder */
	unsigned char sys_ind;          /* What partition type */
	unsigned char end_head;         /* end head */
	unsigned char end_sector;       /* end sector */
	unsigned char end_cyl;          /* end cylinder */
	unsigned char start4[4];        /* starting sector counting from 0 */
	unsigned char size4[4];         /* nr of sectors in partition */
} PACKED;

enum failure {ioctl_error,
	unable_to_open, unable_to_read, unable_to_seek,
	unable_to_write, out_of_memory};

struct geom {
	unsigned int heads;
	unsigned int sectors;
	unsigned int cylinders;
};

int is_probably_full_disk(const char *name);
int try2parse(const char *device, DEVICE_INFO *devInfo); 
char *partition_type(unsigned char type);


#endif /* not __FDISK_H__ */
