#define	VERSION			"2.11"
#define VERSION_DATE	"12 Mar 2005"

//#include <linux/hdreg.h>
#include "blkdev.h" /* replace by Hu Jiexun */
#include <linux/fs.h>
#include <linux/fd.h>
#include <endian.h>
#include <mntent.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <errno.h>
#include "sys_common.h"

#include <linux/version.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 0)
# define __KERNEL__
# include <asm/types.h>
# undef __KERNEL__
#endif

#if __BYTE_ORDER == __BIG_ENDIAN

#include <asm/byteorder.h>
#ifdef __le16_to_cpu
/* ++roman: 2.1 kernel headers define these function, they're probably more
 * efficient then coding the swaps machine-independently. */
#define CF_LE_W	__le16_to_cpu
#define CF_LE_L	__le32_to_cpu
#define CT_LE_W	__cpu_to_le16
#define CT_LE_L	__cpu_to_le32
#else
#define CF_LE_W(v) ((((v) & 0xff) << 8) | (((v) >> 8) & 0xff))
#define CF_LE_L(v) (((unsigned)(v)>>24) | (((unsigned)(v)>>8)&0xff00) | \
               (((unsigned)(v)<<8)&0xff0000) | ((unsigned)(v)<<24))
#define CT_LE_W(v) CF_LE_W(v)
#define CT_LE_L(v) CF_LE_L(v)
#endif /* defined(__le16_to_cpu) */
    
#else

#define CF_LE_W(v) (v)
#define CF_LE_L(v) (v)
#define CT_LE_W(v) (v)
#define CT_LE_L(v) (v)

#endif /* __BIG_ENDIAN */

#define O_NOINHERIT    0
#define O_TEMPORARY    0
#define O_SHORT_LIVED  0
#define O_SEQUENTIAL   0
#define O_RANDOM       0
#define O_BACKUP       0
#define O_SHARED       0
#ifndef O_NONE
# define O_NONE        0
#endif

typedef long long ll_t;

/* In earlier versions, an own llseek() was used, but glibc lseek() is
 * sufficient (or even better :) for 64 bit offsets in the meantime */
#define llseek lseek

#define TEST_BUFFER_BLOCKS 16
#define HARD_SECTOR_SIZE   512
#define SECTORS_PER_BLOCK ( BLOCK_SIZE / HARD_SECTOR_SIZE )


/* Macro definitions */

/* Report a failure message and return a failure error code */

#define die( str ) fatal_error( "%s: " str "\n" )


/* Mark a cluster in the FAT as bad */

#define mark_sector_bad( sector ) mark_FAT_sector( sector, FAT_BAD )

/* Compute ceil(a/b) */

int cdiv (int a, int b)
{
  return (a + b - 1) / b;
}

/* MS-DOS filesystem structures -- I included them here instead of
   including linux/msdos_fs.h since that doesn't include some fields we
   need */

#define ATTR_RO      1		/* read-only */
#define ATTR_HIDDEN  2		/* hidden */
#define ATTR_SYS     4		/* system */
#define ATTR_VOLUME  8		/* volume label */
#define ATTR_DIR     16		/* directory */
#define ATTR_ARCH    32		/* archived */

#define ATTR_NONE    0		/* no attribute bits */
#define ATTR_UNUSED  (ATTR_VOLUME | ATTR_ARCH | ATTR_SYS | ATTR_HIDDEN)
	/* attribute bits that are copied "as is" */

/* FAT values */
#define FAT_EOF      0x0ffffff8
#define FAT_BAD      0x0ffffff7

#define MSDOS_EXT_SIGN 0x29	/* extended boot sector signature */
#define MSDOS_FAT12_SIGN "FAT12   "	/* FAT12 filesystem signature */
#define MSDOS_FAT16_SIGN "FAT16   "	/* FAT16 filesystem signature */
#define MSDOS_FAT32_SIGN "FAT32   "	/* FAT32 filesystem signature */

#define BOOT_SIGN 0xAA55	/* Boot sector magic number */

#define MAX_CLUST_12	((1 << 12) - 16)
#define MAX_CLUST_16	((1 << 16) - 16)
#define MIN_CLUST_32    65529
/* M$ says the high 4 bits of a FAT32 FAT entry are reserved and don't belong
 * to the cluster number. So the max. cluster# is based on 2^28 */
#define MAX_CLUST_32	((1 << 28) - 16)

#define FAT12_THRESHOLD	4085

#define OLDGEMDOS_MAX_SECTORS	32765
#define GEMDOS_MAX_SECTORS	65531
#define GEMDOS_MAX_SECTOR_SIZE	(16*1024)

#define BOOTCODE_SIZE		448
#define BOOTCODE_FAT32_SIZE	420

/* __attribute__ ((packed)) is used on all structures to make gcc ignore any
 * alignments */

struct msdos_volume_info {
	__u8		drive_number;	/* BIOS drive number */
	__u8		RESERVED;	/* Unused */
	__u8		ext_boot_sign;	/* 0x29 if fields below exist (DOS 3.3+) */
	__u8		volume_id[4];	/* Volume ID number */
	__u8		volume_label[11];/* Volume label */
	__u8		fs_type[8];	/* Typically FAT12 or FAT16 */
} __attribute__ ((packed));

struct msdos_boot_sector
{
	__u8	        boot_jump[3];	/* Boot strap short or near jump */
	__u8          system_id[8];	/* Name - can be used to special case
					   partition manager volumes */
	__u8          sector_size[2];	/* bytes per logical sector */
	__u8          cluster_size;	/* sectors/cluster */
	__u16         reserved;	/* reserved sectors */
	__u8          fats;		/* number of FATs */
	__u8          dir_entries[2];	/* root directory entries */
	__u8          sectors[2];	/* number of sectors */
	__u8          media;		/* media code (unused) */
	__u16         fat_length;	/* sectors/FAT */
	__u16         secs_track;	/* sectors per track */
	__u16         heads;		/* number of heads */
	__u32         hidden;		/* hidden sectors (unused) */
	__u32         total_sect;	/* number of sectors (if sectors == 0) */
	union {
		struct {
			struct msdos_volume_info vi;
			__u8	boot_code[BOOTCODE_SIZE];
		} __attribute__ ((packed)) _oldfat;
		struct {
			__u32	fat32_length;	/* sectors/FAT */
			__u16	flags;		/* bit 8: fat mirroring, low 4: active fat */
			__u8	version[2];	/* major, minor filesystem version */
			__u32	root_cluster;	/* first cluster in root directory */
			__u16	info_sector;	/* filesystem info sector */
			__u16	backup_boot;	/* backup boot sector */
			__u16	reserved2[6];	/* Unused */
			struct msdos_volume_info vi;
			__u8	boot_code[BOOTCODE_FAT32_SIZE];
		} __attribute__ ((packed)) _fat32;
	} __attribute__ ((packed)) fstype;
	__u16		boot_sign;
} __attribute__ ((packed));
#define fat32	fstype._fat32
#define oldfat	fstype._oldfat

struct fat32_fsinfo {
	__u32		reserved1;	/* Nothing as far as I can tell */
	__u32		signature;	/* 0x61417272L */
	__u32		free_clusters;	/* Free cluster count.  -1 if unknown */
	__u32		next_cluster;	/* Most recently allocated cluster.
					 * Unused under Linux. */
	__u32		reserved2[4];
};

struct msdos_dir_entry
{
	char	name[8], ext[3];	/* name and extension */
	__u8        attr;			/* attribute bits */
	__u8	lcase;			/* Case for base and extension */
	__u8	ctime_ms;		/* Creation time, milliseconds */
	__u16	ctime;			/* Creation time */
	__u16	cdate;			/* Creation date */
	__u16	adate;			/* Last access date */
	__u16	starthi;		/* high 16 bits of first cl. (FAT32) */
	__u16	time, date, start;	/* time, date and first cluster */
	__u32	size;			/* file size (in bytes) */
} __attribute__ ((packed));


/* The "boot code" we put into the filesystem... it writes a message and
   tells the user to try again */

char dummy_boot_jump[3] = { 0xeb, 0x3c, 0x90 };

#define MSG_OFFSET_OFFSET 3
char dummy_boot_code[BOOTCODE_SIZE] =
  "\x0e"			/* push cs */
  "\x1f"			/* pop ds */
  "\xbe\x5b\x7c"		/* mov si, offset message_txt */
				/* write_msg: */
  "\xac"			/* lodsb */
  "\x22\xc0"			/* and al, al */
  "\x74\x0b"			/* jz key_press */
  "\x56"			/* push si */
  "\xb4\x0e"			/* mov ah, 0eh */
  "\xbb\x07\x00"		/* mov bx, 0007h */
  "\xcd\x10"			/* int 10h */
  "\x5e"			/* pop si */
  "\xeb\xf0"			/* jmp write_msg */
				/* key_press: */
  "\x32\xe4"			/* xor ah, ah */
  "\xcd\x16"			/* int 16h */
  "\xcd\x19"			/* int 19h */
  "\xeb\xfe"			/* foo: jmp foo */
				/* message_txt: */

  "This is not a bootable disk.  Please insert a bootable floppy and\r\n"
  "press any key to try again ... \r\n";

#define MESSAGE_OFFSET 29	/* Offset of message in above code */

//#define MKDOSFS_DBG
#ifdef MKDOSFS_DBG
#define MKDOSFS_INFO(fmt, args...) fprintf(stderr, fmt, ##args)
#else
#define MKDOSFS_INFO(fmt, args...)
#endif

/* Global variables - the root of all evil :-) - see these and weep! */

static char *program_name = "mkdosfs";	/* Name of the program */
static char *device_name = NULL;	/* Name of the device on which to create the filesystem */
static int check = FALSE;	/* Default to no readablity checking */
static int verbose = 0;		/* Default to verbose mode off */
static long volume_id;		/* Volume ID number */
static time_t create_time;	/* Creation time */
static char volume_name[] = "           "; /* Volume name */
static unsigned long long blocks;	/* Number of blocks in filesystem */
static int sector_size = 512;	/* Size of a logical sector */
static int backup_boot = 0;	/* Sector# of backup boot sector */
static int reserved_sectors = 0;/* Number of reserved sectors */
static int badblocks = 0;	/* Number of bad blocks in the filesystem */
static int nr_fats = 2;		/* Default number of FATs to produce */
static int size_fat = 0;	/* Size in bits of FAT entries */
static int size_fat_by_user = 0; /* 1 if FAT size user selected */
static int dev = -1;		/* FS block device file handle */
static off_t currently_testing = 0;	/* Block currently being tested (if autodetect bad blocks) */
static struct msdos_boot_sector bs;	/* Boot sector data */
static int start_data_sector;	/* Sector number for the start of the data area */
static int start_data_block;	/* Block number for the start of the data area */
static unsigned char *fat = NULL;	/* File allocation table */
static unsigned char *info_sector = NULL;	/* FAT32 info sector */
static struct msdos_dir_entry *root_dir = NULL;	/* Root directory */
static int size_root_dir;	/* Size of the root directory in bytes */
static int sectors_per_cluster = 0;	/* Number of sectors per disk cluster */
static int root_dir_entries = 0;	/* Number of root directory entries */
static char *blank_sector = NULL;		/* Blank sector - all zeros */
static int hidden_sectors = 0;		/* Number of hidden sectors */


/* Function prototype definitions */
static void fatal_error (const char *fmt_string);
static int mark_FAT_cluster(int cluster, unsigned int value);
static int mark_FAT_sector (int sector, unsigned int value);
static long do_check (char *buffer, int try, off_t current_block);
static void alarm_intr (int alnum);
static int check_blocks (void);
static int valid_offset (int fd, loff_t offset);
static unsigned long long count_blocks (char *filename);
static int check_mount(char *device_name);
static int establish_params(int  device_num, off_t size);
static int setup_tables(void);


/* The function implementations */

/* Function: fatal_error
 * Description: print the reporting fatal errors.
 * Input:	fmt_string - the string of the reporting errors
 * Output: none
 * Return: none
 */
static void fatal_error (const char *fmt_string)
{
	fprintf (stderr, fmt_string, program_name, device_name);
}

/* Function: mark_FAT_cluster
 * Description: Mark the specified cluster as having a particular value.
 * Input:	cluster - cluster no
              value - the value that want to mark
 * Output: none
 * Return: 0 if success, -1 if fail
 */
static int mark_FAT_cluster(int cluster, unsigned int value)
{
	switch( size_fat ) {
	case 12:
		value &= 0x0fff;
		if (((cluster * 3) & 0x1) == 0)
		{
			fat[3 * cluster / 2] = (unsigned char) (value & 0x00ff);
			fat[(3 * cluster / 2) + 1] = (unsigned char) ((fat[(3 * cluster / 2) + 1] & 0x00f0)
				| ((value & 0x0f00) >> 8));
		}
		else
		{
			fat[3 * cluster / 2] = (unsigned char) ((fat[3 * cluster / 2] & 0x000f) | ((value & 0x000f) << 4));
			fat[(3 * cluster / 2) + 1] = (unsigned char) ((value & 0x0ff0) >> 4);
		}
		break;

	case 16:
		value &= 0xffff;
		fat[2 * cluster] = (unsigned char) (value & 0x00ff);
		fat[(2 * cluster) + 1] = (unsigned char) (value >> 8);
		break;

	case 32:
		value &= 0xfffffff;
		fat[4 * cluster] =       (unsigned char)  (value & 0x000000ff);
		fat[(4 * cluster) + 1] = (unsigned char) ((value & 0x0000ff00) >> 8);
		fat[(4 * cluster) + 2] = (unsigned char) ((value & 0x00ff0000) >> 16);
		fat[(4 * cluster) + 3] = (unsigned char) ((value & 0xff000000) >> 24);
		break;

	default:
		die("Bad FAT size (not 12, 16, or 32)");
		return -1;
	}
	return 0;
}

/* Function: mark_FAT_sector
 * Description: Mark a specified sector as having a particular value in it's FAT entry.
 * Input:	cluster - sector no
              value - the value that want to mark
 * Output: none
 * Return: 0 if success, -1 if fail
 */
static int mark_FAT_sector (int sector, unsigned int value)
{
	int cluster;

	cluster = (sector - start_data_sector) / (int) (bs.cluster_size) /
		(sector_size/HARD_SECTOR_SIZE);
	if (cluster < 0)
	{
		die ("Invalid cluster number in mark_FAT_sector: probably bug!");
		return -1;
	}
	if (mark_FAT_cluster (cluster, value) < 0)
	{
		return -1;
	}
	return 0;
}

/* Function: do_check
 * Description: Perform a test on a block.
 * Input:	buffer - output buffer
              try - the number of blocks that want to read
              current_block - the block offset of the device
 * Output: none
 * Return: return the number of blocks that could be read successfully or -1 if fail
 */
static long do_check (char *buffer, int try, off_t current_block)
{
	long got;

	if (llseek (dev, current_block * BLOCK_SIZE, SEEK_SET) /* Seek to the correct location */
		!= current_block * BLOCK_SIZE)
	{
		die ("seek failed during testing for blocks");
		return -1;
	}
	got = read (dev, buffer, try * BLOCK_SIZE);	/* Try reading! */
	if (got < 0)
	{
		got = 0;
	}

	if (got & (BLOCK_SIZE - 1))
	{
		MKDOSFS_INFO ("Unexpected values in do_check: probably bugs\n");
	}
	got /= BLOCK_SIZE;

	return got;
}

/* Function: alarm_intr
 * Description:  Alarm clock handler - display the status of the quest for bad blocks!
 * Input:	alnum - signal number
 * Output: none
 * Return: none
 */
static void alarm_intr (int alnum)
{
	if (currently_testing >= blocks)
	{
		return;
	}
	signal (SIGALRM, alarm_intr);
	alarm (5);
	if (!currently_testing)
	{
		return;
	}
	MKDOSFS_INFO ("%lld... ", (unsigned long long)currently_testing);
	fflush (stdout);
}

/* Function: check_blocks
 * Description:  Check the device for bad blocks
 * Input:	none
 * Output: none
 * Return: 0 if success, -1 if fail
 */
static int check_blocks(void)
{
	int try, got;
	int i;
	static char blkbuf[BLOCK_SIZE * TEST_BUFFER_BLOCKS];

	if (verbose)
	{
		MKDOSFS_INFO ("Searching for bad blocks ");
		fflush (stdout);
	}
	currently_testing = 0;

	if (verbose)
	{
		signal (SIGALRM, alarm_intr);
		alarm (5);
	}

	try = TEST_BUFFER_BLOCKS;
	while (currently_testing < blocks)
	{
		if (currently_testing + try > blocks)
		{
			try = blocks - currently_testing;
		}
		got = do_check (blkbuf, try, currently_testing);
		if (got < 0)
		{
			return -1;
		}
		currently_testing += got;
		if (got == try)
		{
			try = TEST_BUFFER_BLOCKS;
			continue;
		}
		else
		{
			try = 1;
		}
		if (currently_testing < start_data_block)
		{
			die ("bad blocks before data-area: cannot make fs");
			return -1;
		}
		for (i = 0; i < SECTORS_PER_BLOCK; i++)	/* Mark all of the sectors in the block as bad */
		{
			if (mark_sector_bad (currently_testing * SECTORS_PER_BLOCK + i) < 0)
			{
				return -1;
			}
		}
		badblocks++;
		currently_testing++;
	}

	if (verbose)
	{
		MKDOSFS_INFO ("\n");
	}

	if (badblocks)
	{
		MKDOSFS_INFO ("%d bad block%s\n", badblocks,
			(badblocks > 1) ? "s" : "");
	}
	return 0;
}

/* Function: valid_offset
 * Description:  Given a file descriptor and an offset, check whether the offset is a valid offset for the file 
 * Input:	fd - the file descriptor of the device
              offset - the offset of the device
 * Output: none
 * Return: FALSE if it isn't valid or TRUE if it is
 */
static int valid_offset (int fd, loff_t offset)
{
	char ch;

	if (llseek (fd, offset, SEEK_SET) < 0)
	{
		return FALSE;
	}
	if (read (fd, &ch, 1) < 1)
	{
		return FALSE;
	}
	return TRUE;
}

/* Function: count_blocks
 * Description:  Given a filename, look to see how many blocks of BLOCK_SIZE are present
 * Input:	filename - the name of the device
 * Output: none
 * Return: return the number of blocks or -1 if fail
 */
static unsigned long long count_blocks (char *filename)
{
	off_t high, low;
	int fd;

	if ((fd = open (filename, O_RDONLY)) < 0)
	{
		perror (filename); 
		return -1; 
	}

	/* first try SEEK_END, which should work on most devices nowadays */
	if ((low = llseek(fd, 0, SEEK_END)) <= 0)
	{
		low = 0;
		for (high = 1; valid_offset (fd, high); high *= 2)
		{
			low = high;
		}
		while (low < high - 1)
		{
			const loff_t mid = (low + high) / 2;
			if (valid_offset (fd, mid))
			{
				low = mid;
			}
			else
			{
				high = mid;
			}
		}
		++low;
	}

	close (fd);
	return low / BLOCK_SIZE;
}

/* Function: check_mount
 * Description: Check to see if the specified device is currently mounted.
 * Input:	device_name - the name of the device
 * Output: none
 * Return: 0 if the device is unmounted, -1 if the device is mounted
 */
 static int check_mount(char *device_name)
 {
	FILE *f;
	struct mntent *mnt;

	if ((f = setmntent (MOUNTED, "r")) == NULL)
	{
		return 0;
	}
	while ((mnt = getmntent (f)) != NULL)
	{
		if (strcmp (device_name, mnt->mnt_fsname) == 0)
		{
			die ("%s contains a mounted filesystem .");
			endmntent(f);
			return -1;
		}
	}
	endmntent (f);
	return 0;
}

/* Function: establish_params
 * Description: Establish the geometry and media parameters for the device.
 * Input:	device_num - device ID
              size - total size of the device
 * Output: none
 * Return: 0 if success, -1 if fail
 */
static int  establish_params(int  device_num, off_t size)
{
	long loop_size;
	struct hd_geometry geometry;
	struct floppy_struct param;

	if ((0 == device_num) || ((device_num & 0xff00) == 0x0200))
	/* file image or floppy disk */
	{
		if (0 == device_num)
		{
			param.size = size/512;
			switch(param.size)
			{
			case 720:
				param.sect = 9 ;
				param.head = 2;
				break; 
			case 1440:
				param.sect = 9;
				param.head = 2;
				break;
			case 2400:
				param.sect = 15;
				param.head = 2;
				break;
			case 2880:
				param.sect = 18;
				param.head = 2;
				break;
			case 5760:
				param.sect = 36;
				param.head = 2;
				break;
			default:
				/* fake values */
				param.sect = 32;
				param.head = 64;
				break;
			}
	  
		}
		else 	/* is a floppy diskette */
		{
			if (ioctl (dev, FDGETPRM, &param))	/*  Can we get the diskette geometry? */
			{
				die ("unable to get diskette geometry for '%s'");
				return -1;
			}
		}
		bs.secs_track = CT_LE_W(param.sect);	/*  Set up the geometry information */
		bs.heads = CT_LE_W(param.head);
		switch (param.size)	/*  Set up the media descriptor byte */
		{
		case 720:		/* 5.25", 2, 9, 40 - 360K */
			bs.media = (char) 0xfd;
			bs.cluster_size = (char) 2;
			bs.dir_entries[0] = (char) 112;
			bs.dir_entries[1] = (char) 0;
			break;

		case 1440:		/* 3.5", 2, 9, 80 - 720K */
			bs.media = (char) 0xf9;
			bs.cluster_size = (char) 2;
			bs.dir_entries[0] = (char) 112;
			bs.dir_entries[1] = (char) 0;
			break;

		case 2400:		/* 5.25", 2, 15, 80 - 1200K */
			bs.media = (char) 0xf9;
			bs.cluster_size = (char)1;
			bs.dir_entries[0] = (char) 224;
			bs.dir_entries[1] = (char) 0;
			break;
	  
		case 5760:		/* 3.5", 2, 36, 80 - 2880K */
			bs.media = (char) 0xf0;
			bs.cluster_size = (char) 2;
			bs.dir_entries[0] = (char) 224;
			bs.dir_entries[1] = (char) 0;
			break;

		case 2880:		/* 3.5", 2, 18, 80 - 1440K */
floppy_default:
			bs.media = (char) 0xf0;
			bs.cluster_size = (char)1;
			bs.dir_entries[0] = (char) 224;
			bs.dir_entries[1] = (char) 0;
			break;

		default:		/* Anything else */
			if (0 == device_num)
				goto def_hd_params;
			else
				goto floppy_default;
		}
	}
	else if ((device_num & 0xff00) == 0x0700) /* This is a loop device */
	{
		if (ioctl (dev, BLKGETSIZE, &loop_size)) 
		{
			die ("unable to get loop device size");
			return -1;
		}
		switch (loop_size)  /* Assuming the loop device -> floppy later */
		{
		case 720:		/* 5.25", 2, 9, 40 - 360K */
			bs.secs_track = CF_LE_W(9);
			bs.heads = CF_LE_W(2);
			bs.media = (char) 0xfd;
			bs.cluster_size = (char) 2;
			bs.dir_entries[0] = (char) 112;
			bs.dir_entries[1] = (char) 0;
			break;

		case 1440:		/* 3.5", 2, 9, 80 - 720K */
			bs.secs_track = CF_LE_W(9);
			bs.heads = CF_LE_W(2);
			bs.media = (char) 0xf9;
			bs.cluster_size = (char) 2;
			bs.dir_entries[0] = (char) 112;
			bs.dir_entries[1] = (char) 0;
			break;

		case 2400:		/* 5.25", 2, 15, 80 - 1200K */
			bs.secs_track = CF_LE_W(15);
			bs.heads = CF_LE_W(2);
			bs.media = (char) 0xf9;
			bs.cluster_size = (char)1;
			bs.dir_entries[0] = (char) 224;
			bs.dir_entries[1] = (char) 0;
			break;
	  
		case 5760:		/* 3.5", 2, 36, 80 - 2880K */
			bs.secs_track = CF_LE_W(36);
			bs.heads = CF_LE_W(2);
			bs.media = (char) 0xf0;
			bs.cluster_size = (char) 2;
			bs.dir_entries[0] = (char) 224;
			bs.dir_entries[1] = (char) 0;
			break;

		case 2880:		/* 3.5", 2, 18, 80 - 1440K */
			bs.secs_track = CF_LE_W(18);
			bs.heads = CF_LE_W(2);
			bs.media = (char) 0xf0;
			bs.cluster_size = (char)1;
			bs.dir_entries[0] = (char) 224;
			bs.dir_entries[1] = (char) 0;
			break;

		default:		/* Anything else: default hd setup */
			MKDOSFS_INFO("Loop device does not match a floppy size, using "
				"default hd params\n");
			bs.secs_track = CT_LE_W(32); /* these are fake values... */
			bs.heads = CT_LE_W(64);
			goto def_hd_params;
		}
	}
	else
	/* Must be a hard disk then! */
	{
		/* Can we get the drive geometry? (Note I'm not too sure about */
		/* whether to use HDIO_GETGEO or HDIO_REQ) */
		if (ioctl (dev, HDIO_GETGEO, &geometry))
		{
			MKDOSFS_INFO ("unable to get drive geometry, using default 255/63");
			bs.secs_track = CT_LE_W(63);
			bs.heads = CT_LE_W(255);
		}
		else
		{
			bs.secs_track = CT_LE_W(geometry.sectors);	/* Set up the geometry information */
			bs.heads = CT_LE_W(geometry.heads);
		}
def_hd_params:
		bs.media = (char) 0xf8; /* Set up the media descriptor for a hard drive */
		bs.dir_entries[0] = (char) 0;	/* Default to 512 entries */
		bs.dir_entries[1] = (char) 2;
		if (!size_fat && blocks*SECTORS_PER_BLOCK > 1064960)
		{
			if (verbose)
			{
				MKDOSFS_INFO("Auto-selecting FAT32 for large filesystem\n");
			}
			size_fat = 32;
		}
		if (size_fat == 32)
		{
			/* For FAT32, try to do the same as M$'s format command:
			* fs size < 256M: 0.5k clusters
			* fs size <   8G: 4k clusters
			* fs size <  16G: 8k clusters
			* fs size >= 16G: 16k clusters
			*/
			unsigned long sz_mb =
				(blocks+(1<<(20-BLOCK_SIZE_BITS))-1) >> (20-BLOCK_SIZE_BITS);
			bs.cluster_size = sz_mb >= 16*1024 ? 32 :
				sz_mb >=  8*1024 ? 16 :
				sz_mb >=     256 ?  8 :
				1;
		}
		else
		{
			/* FAT12 and FAT16: start at 4 sectors per cluster */
			bs.cluster_size = (char) 4;
		}
	}
	MKDOSFS_INFO("bs.cluster_size=%d\n",bs.cluster_size);
	return 0;
}

/* Function: setup_tables
 * Description: Create the filesystem data tables.
 * Input:	none
 * Output: none
 * Return: 0 if success, -1 if fail
 */
static int setup_tables (void)
{
	unsigned num_sectors;
	unsigned cluster_count = 0, fat_length;
	unsigned fatdata;			/* Sectors for FATs + data area */
	struct tm *ctime;
	struct msdos_volume_info *vi = (size_fat == 32 ? &bs.fat32.vi : &bs.oldfat.vi);
	unsigned fatlength12, fatlength16, fatlength32;
	unsigned maxclust12, maxclust16, maxclust32;
	unsigned clust12, clust16, clust32;
	int maxclustsize;
#if 0
	strcpy (bs.system_id, "mkdosfs");
#else
	if (sector_size > 512)
	{
		strcpy ((char *)bs.system_id, "hikfat"); /* modified by Hu Jiexun */
	}
	else
	{
		strcpy ((char *)bs.system_id, "mkdosfs");
	}
#endif
	if (sectors_per_cluster)
	{
		bs.cluster_size = (char) sectors_per_cluster;
	}
	if (size_fat == 32)
	{
		/* Under FAT32, the root dir is in a cluster chain, and this is
		* signalled by bs.dir_entries being 0. */
		bs.dir_entries[0] = bs.dir_entries[1] = (char) 0;
		root_dir_entries = 0;
	}
	else
	{
		root_dir_entries = bs.dir_entries[0] + (bs.dir_entries[1] << 8);
	}

	vi->volume_id[0] = (unsigned char) (volume_id & 0x000000ff);
	vi->volume_id[1] = (unsigned char) ((volume_id & 0x0000ff00) >> 8);
	vi->volume_id[2] = (unsigned char) ((volume_id & 0x00ff0000) >> 16);
	vi->volume_id[3] = (unsigned char) (volume_id >> 24);

	memcpy(vi->volume_label, volume_name, 11);
  
	memcpy(bs.boot_jump, dummy_boot_jump, 3);
	/* Patch in the correct offset to the boot code */
	bs.boot_jump[1] = ((size_fat == 32 ?
			(char *)&bs.fat32.boot_code :
			(char *)&bs.oldfat.boot_code) -
			(char *)&bs) - 2;

	if (size_fat == 32)
	{
		int offset = (char *)&bs.fat32.boot_code -
			(char *)&bs + MESSAGE_OFFSET + 0x7c00;
		if (dummy_boot_code[BOOTCODE_FAT32_SIZE-1])
		{
			MKDOSFS_INFO ("Warning: message too long; truncated\n");
		}
		dummy_boot_code[BOOTCODE_FAT32_SIZE-1] = 0;
		memcpy(bs.fat32.boot_code, dummy_boot_code, BOOTCODE_FAT32_SIZE);
		bs.fat32.boot_code[MSG_OFFSET_OFFSET] = offset & 0xff;
		bs.fat32.boot_code[MSG_OFFSET_OFFSET+1] = offset >> 8;
	}
	else
	{
		memcpy(bs.oldfat.boot_code, dummy_boot_code, BOOTCODE_SIZE);
	}
	bs.boot_sign = CT_LE_W(BOOT_SIGN);

	if (verbose >= 2)
	{
		MKDOSFS_INFO( "Boot jump code is %02x %02x\n",
			bs.boot_jump[0], bs.boot_jump[1] );
	}

	reserved_sectors = (size_fat == 32) ? 32 : 1;
	bs.reserved = CT_LE_W(reserved_sectors);
	if (verbose >= 2)
	{
		MKDOSFS_INFO( "Using %d reserved sectors\n", reserved_sectors );
	}

	bs.fats = (char) nr_fats;

	bs.hidden = CT_LE_L(hidden_sectors);

	num_sectors = (ll_t)blocks*BLOCK_SIZE/sector_size;

	fatdata = num_sectors - cdiv (root_dir_entries * 32, sector_size) -
		reserved_sectors;

	if (sectors_per_cluster)
	{
		bs.cluster_size = maxclustsize = sectors_per_cluster;
	}
	else
	{
		/* An initial guess for bs.cluster_size should already be set */
		maxclustsize = 128;
	}

	if (verbose >= 2)
	{
		MKDOSFS_INFO( "%d sectors for FAT+data, starting with %d sectors/cluster\n",
			fatdata, bs.cluster_size );
	}
	do 
	{
		if (verbose >= 2)
		{
			MKDOSFS_INFO( "Trying with %d sectors/cluster:\n", bs.cluster_size );
		}
		/* The factor 2 below avoids cut-off errors for nr_fats == 1.
		* The "nr_fats*3" is for the reserved first two FAT entries */
		clust12 = 2*((ll_t) fatdata *sector_size + nr_fats*3) /
			(2*(int) bs.cluster_size * sector_size + nr_fats*3);
		fatlength12 = cdiv (((clust12+2) * 3 + 1) >> 1, sector_size);
		/* Need to recalculate number of clusters, since the unused parts of the
		* FATS and data area together could make up space for an additional,
		* not really present cluster. */
		clust12 = (fatdata - nr_fats*fatlength12)/bs.cluster_size;
		maxclust12 = (fatlength12 * 2 * sector_size) / 3;
		if (maxclust12 > MAX_CLUST_12)
		{
			maxclust12 = MAX_CLUST_12;
		}
		if (verbose >= 2)
		{
			MKDOSFS_INFO( "FAT12: #clu=%u, fatlen=%u, maxclu=%u, limit=%u\n",
			clust12, fatlength12, maxclust12, MAX_CLUST_12 );
		}
		if (clust12 > maxclust12-2)
		{
			clust12 = 0;
			if (verbose >= 2)
			{
				MKDOSFS_INFO( "FAT12: too much clusters\n" );
			}
		}

		clust16 = ((ll_t) fatdata *sector_size + nr_fats*4) /
			((int) bs.cluster_size * sector_size + nr_fats*2);
		fatlength16 = cdiv ((clust16+2) * 2, sector_size);
		/* Need to recalculate number of clusters, since the unused parts of the
		* FATS and data area together could make up space for an additional,
		* not really present cluster. */
		clust16 = (fatdata - nr_fats*fatlength16)/bs.cluster_size;
		maxclust16 = (fatlength16 * sector_size) / 2;
		if (maxclust16 > MAX_CLUST_16)
		{
			maxclust16 = MAX_CLUST_16;
		}
		if (verbose >= 2)
		{
			MKDOSFS_INFO( "FAT16: #clu=%u, fatlen=%u, maxclu=%u, limit=%u\n",
				clust16, fatlength16, maxclust16, MAX_CLUST_16 );
		}
		if (clust16 > maxclust16-2)
		{
			if (verbose >= 2)
			{
				MKDOSFS_INFO( "FAT16: too much clusters\n" );
			}
			clust16 = 0;
		}
		/* The < 4078 avoids that the filesystem will be misdetected as having a
		* 12 bit FAT. */
		if (clust16 < FAT12_THRESHOLD && !(size_fat_by_user && size_fat == 16))
		{
			if (verbose >= 2)
			{
				MKDOSFS_INFO("%s\n", clust16 < FAT12_THRESHOLD ?
				"FAT16: would be misdetected as FAT12\n" :
				"FAT16: too much clusters\n" );
			}
			clust16 = 0;
		}

		clust32 = ((ll_t) fatdata *sector_size + nr_fats*8) /
			((int) bs.cluster_size * sector_size + nr_fats*4);
		fatlength32 = cdiv ((clust32+2) * 4, sector_size);
		/* Need to recalculate number of clusters, since the unused parts of the
		* FATS and data area together could make up space for an additional,
		* not really present cluster. */
		clust32 = (fatdata - nr_fats*fatlength32)/bs.cluster_size;
		maxclust32 = (fatlength32 * sector_size) / 4;
		if (maxclust32 > MAX_CLUST_32)
		{
			maxclust32 = MAX_CLUST_32;
		}
		if (clust32 && clust32 < MIN_CLUST_32 && !(size_fat_by_user && size_fat == 32))
		{
			clust32 = 0;
			if (verbose >= 2)
			{
				MKDOSFS_INFO( "FAT32: not enough clusters (%d)\n", MIN_CLUST_32);
			}
		}
		if (verbose >= 2)
		{
			MKDOSFS_INFO( "FAT32: #clu=%u, fatlen=%u, maxclu=%u, limit=%u\n",
				clust32, fatlength32, maxclust32, MAX_CLUST_32 );
		}
		if (clust32 > maxclust32)
		{
			clust32 = 0;
			if (verbose >= 2)
			{
				MKDOSFS_INFO( "FAT32: too much clusters\n" );
			}
		}

		if ((clust12 && (size_fat == 0 || size_fat == 12)) ||
			(clust16 && (size_fat == 0 || size_fat == 16)) ||
			(clust32 && size_fat == 32))
		{
			break;
		}

		bs.cluster_size <<= 1;
	} while (bs.cluster_size && bs.cluster_size <= maxclustsize);

	/* Use the optimal FAT size if not specified;
	* FAT32 is (not yet) choosen automatically */
	if (!size_fat)
	{
		size_fat = (clust16 > clust12) ? 16 : 12;
		if (verbose >= 2)
		{
			MKDOSFS_INFO( "Choosing %d bits for FAT\n", size_fat );
		}
	}

	switch (size_fat)
	{
	case 12:
		cluster_count = clust12;
		fat_length = fatlength12;
		bs.fat_length = CT_LE_W(fatlength12);
		memcpy(vi->fs_type, MSDOS_FAT12_SIGN, 8);
		break;

	case 16:
		if (clust16 < FAT12_THRESHOLD)
		{
			if (size_fat_by_user)
			{
				fprintf( stderr, "WARNING: Not enough clusters for a "
					"16 bit FAT! The filesystem will be\n"
					"misinterpreted as having a 12 bit FAT without "
					"mount option \"fat=16\".\n" );
			}
			else
			{
				fprintf( stderr, "This filesystem has an unfortunate size. "
					"A 12 bit FAT cannot provide\n"
					"enough clusters, but a 16 bit FAT takes up a little "
					"bit more space so that\n"
					"the total number of clusters becomes less than the "
					"threshold value for\n"
					"distinction between 12 and 16 bit FATs.\n" );
				die( "Make the file system a bit smaller manually." );
				return -1;
			}
		}
		cluster_count = clust16;
		fat_length = fatlength16;
		bs.fat_length = CT_LE_W(fatlength16);
		memcpy(vi->fs_type, MSDOS_FAT16_SIGN, 8);
		break;

	case 32:
		cluster_count = clust32;
		fat_length = fatlength32;
		bs.fat_length = CT_LE_W(0);
		bs.fat32.fat32_length = CT_LE_L(fatlength32);
		memcpy(vi->fs_type, MSDOS_FAT32_SIGN, 8);
		break;
	
	default:
		die("FAT not 12, 16 or 32 bits");
		return -1;
	}

	bs.sector_size[0] = (char) (sector_size & 0x00ff);
	bs.sector_size[1] = (char) ((sector_size & 0xff00) >> 8);

	if (size_fat == 32)
	{
		/* set up additional FAT32 fields */
		bs.fat32.flags = CT_LE_W(0);
		bs.fat32.version[0] = 0;
		bs.fat32.version[1] = 0;
		bs.fat32.root_cluster = CT_LE_L(2);
		bs.fat32.info_sector = CT_LE_W(1);

		backup_boot = (reserved_sectors >= 7) ? 6 :
			(reserved_sectors >= 2) ? reserved_sectors-1 : 0;
		if (verbose >= 2)
		{
			MKDOSFS_INFO( "Using sector %d as backup boot sector (0 = none)\n",
				backup_boot );
		}
		bs.fat32.backup_boot = CT_LE_W(backup_boot);
		memset( &bs.fat32.reserved2, 0, sizeof(bs.fat32.reserved2) );
	}

	if (num_sectors >= 65536)
	{
		bs.sectors[0] = (char) 0;
		bs.sectors[1] = (char) 0;
		bs.total_sect = CT_LE_L(num_sectors);
	}
	else
	{
		bs.sectors[0] = (char) (num_sectors & 0x00ff);
		bs.sectors[1] = (char) ((num_sectors & 0xff00) >> 8);
		bs.total_sect = CT_LE_L(0);
	}

	vi->ext_boot_sign = MSDOS_EXT_SIGN;

	if (!cluster_count)
	{
		if (sectors_per_cluster)	/* If yes, die if we'd spec'd sectors per cluster */
		{
			die ("Too many clusters for file system - try more sectors per cluster");
			return -1;
		}
		else
		{
			die ("Attempting to create a too large file system");
			return -1;
		}
	}

	/* The two following vars are in hard sectors, i.e. 512 byte sectors! */
	start_data_sector = (reserved_sectors + nr_fats * fat_length) *
			(sector_size/HARD_SECTOR_SIZE);
	start_data_block = (start_data_sector + SECTORS_PER_BLOCK - 1) /
			SECTORS_PER_BLOCK;

	if (blocks < start_data_block + 32)	/* Arbitrary undersize file system! */
	{
		die ("Too few blocks for viable file system");
		return -1;
	}
	if (verbose)
	{
		MKDOSFS_INFO("%s has %d head%s and %d sector%s per track,\n",
			device_name, CF_LE_W(bs.heads), (CF_LE_W(bs.heads) != 1) ? "s" : "",
			CF_LE_W(bs.secs_track), (CF_LE_W(bs.secs_track) != 1) ? "s" : ""); 
		MKDOSFS_INFO("logical sector size is %d,\n",sector_size);
		MKDOSFS_INFO("using 0x%02x media descriptor, with %d sectors;\n",
			(int) (bs.media), num_sectors);
		MKDOSFS_INFO("file system has %d %d-bit FAT%s and %d sector%s per cluster.\n",
			(int) (bs.fats), size_fat, (bs.fats != 1) ? "s" : "",
			(int) (bs.cluster_size), (bs.cluster_size != 1) ? "s" : "");
		MKDOSFS_INFO ("FAT size is %d sector%s, and provides %d cluster%s.\n",
			fat_length, (fat_length != 1) ? "s" : "",
			cluster_count, (cluster_count != 1) ? "s" : "");
		if (size_fat != 32)
		{
			MKDOSFS_INFO ("Root directory contains %d slots.\n",
				(int) (bs.dir_entries[0]) + (int) (bs.dir_entries[1]) * 256);
		}
		MKDOSFS_INFO ("Volume ID is %08lx, ", volume_id & 0xffffffff);
		if ( strcmp(volume_name, "           ") )
		{
			MKDOSFS_INFO("volume label %s.\n", volume_name);
		}
		else
		{
			MKDOSFS_INFO("no volume label.\n");
		}
	}

	/* Make the file allocation tables! */
	if ((fat = (unsigned char *) malloc (sector_size)) == NULL)
	{
		die ("unable to allocate space for FAT image in memory");
		return -1;
	}
	memset( fat, 0, sector_size );

	if (mark_FAT_cluster (0, 0xffffffff) < 0)	/* Initial fat entries */
	{
		return -1;
	}
	if (mark_FAT_cluster (1, 0xffffffff) < 0)
	{
		return -1;
	}
	fat[0] = (unsigned char) bs.media;	/* Put media type in first byte! */
	if (size_fat == 32)
	{
		/* Mark cluster 2 as EOF (used for root dir) */
		if (mark_FAT_cluster (2, FAT_EOF) < 0)
		{
			return -1;
		}
	}

	/* Make the root directory entries */
	size_root_dir = (size_fat == 32) ?
		bs.cluster_size*sector_size :
		(((int)bs.dir_entries[1]*256+(int)bs.dir_entries[0]) *
		sizeof (struct msdos_dir_entry));
	if ((root_dir = (struct msdos_dir_entry *) malloc (size_root_dir)) == NULL)
	{
		free (fat);		/* Tidy up before we die! */
		fat = NULL;
		die ("unable to allocate space for root directory in memory");
		return -1;
	}

	memset(root_dir, 0, size_root_dir);
	if ( memcmp(volume_name, "           ", 11) )
	{
		struct msdos_dir_entry *de = &root_dir[0];
		memcpy(de->name, volume_name, 11);
		de->attr = ATTR_VOLUME;
		ctime = localtime(&create_time);
		de->time = CT_LE_W((unsigned short)((ctime->tm_sec >> 1) +
				(ctime->tm_min << 5) + (ctime->tm_hour << 11)));
		de->date = CT_LE_W((unsigned short)(ctime->tm_mday +
				((ctime->tm_mon+1) << 5) +
				((ctime->tm_year-80) << 9)));
		de->ctime_ms = 0;
		de->ctime = de->time;
		de->cdate = de->date;
		de->adate = de->date;
		de->starthi = CT_LE_W(0);
		de->start = CT_LE_W(0);
		de->size = CT_LE_L(0);
	}

	if (size_fat == 32)
	{
		/* For FAT32, create an info sector */
		struct fat32_fsinfo *info;
    
		if (!(info_sector = malloc( sector_size )))
		{
			die("Out of memory");
			return -1;
		}
		memset(info_sector, 0, sector_size);
		/* fsinfo structure is at offset 0x1e0 in info sector by observation */
		info = (struct fat32_fsinfo *)(info_sector + 0x1e0);

		/* Info sector magic */
		info_sector[0] = 'R';
		info_sector[1] = 'R';
		info_sector[2] = 'a';
		info_sector[3] = 'A';

		/* Magic for fsinfo structure */
		info->signature = CT_LE_L(0x61417272);
		/* We've allocated cluster 2 for the root dir. */
		info->free_clusters = CT_LE_L(cluster_count - 1);
		info->next_cluster = CT_LE_L(2);

		/* Info sector also must have boot sign */
		*(__u16 *)(info_sector + 0x1fe) = CT_LE_W(BOOT_SIGN);
	}
  
	if (!(blank_sector = malloc( sector_size )))
	{
		die( "Out of memory" );
		return -1;
	}
	memset(blank_sector, 0, sector_size);

	return 0;
}

#define error(str)				\
	do {						\
		free (fat);					\
		if (info_sector) free (info_sector);	\
		free (root_dir);				\
		fat = NULL;					\
		info_sector = NULL;			\
		root_dir = NULL;			\
		die (str);					\
	} while(0)

#define seekto(pos,errstr)						\
	do {									\
		loff_t __pos = (pos);						\
		if (llseek (dev, __pos, SEEK_SET) != __pos)	\
		{                                                  \
			error ("seek to " errstr " failed whilst writing tables");	\
			return -1;                                \
		}                                                 \
	} while(0)

#define writebuf(buf,size,errstr)			\
	do {							\
		int __size = (size);				\
		if (write (dev, buf, __size) != __size)		\
		{                                                  \
			error ("failed whilst writing " errstr);	\
			return -1;                                \
		}                                                 \
	} while(0)

/* Function: write_tables
 * Description: Write the new filesystem's data tables.
 * Input:	none
 * Output: none
 * Return:  0 if success, -1 if fail
 */
static int write_tables (int *pFormatPercent)
{	
	int iRet = -1;
	int x;
	int y;
	int times = sector_size / 512;
	int fat_length;

	fat_length = (size_fat == 32) ?
		CF_LE_L(bs.fat32.fat32_length) : CF_LE_W(bs.fat_length);

	int formatScale;
	int currentFormatProgress = 0;

	seekto( 0, "start of device" );
	/* tell the number of printed hash marks */
	char buf[20];
	snprintf(buf, 19, "%d\n", reserved_sectors + nr_fats + 3);
	buf[19] = '\0';
	iRet = write(STDERR_FILENO, buf, strlen(buf));
	if(iRet < 0)
	{
		return iRet;
	}
	/* clear all reserved sectors */
	iRet = write(STDERR_FILENO, "S", 1);
	for( x = 0; x < reserved_sectors; ++x )
	{
		writebuf( blank_sector, sector_size, "reserved sector" );
		iRet = write(STDERR_FILENO, "=", 1);
		if(iRet < 0)
		{
			return iRet;
		}
	}
	/* seek back to sector 0 and write the boot sector */
	seekto( 0, "boot sector" );

#if 0
	/* added by Hu Jiexun start */
	bs.sector_size[1] = 0x02;	/* 512 Bytes */
	MKDOSFS_INFO("sector_size = 0x%8.8x\n", bs.sector_size[1]);
	MKDOSFS_INFO("cluster_size = 0x%8.8x\n", bs.cluster_size);
	bs.cluster_size *= times;
	MKDOSFS_INFO("cluster_size = 0x%8.8x\n", bs.cluster_size);
	MKDOSFS_INFO("reserved = 0x%8.8x\n", bs.reserved);
	bs.reserved *= times;
	MKDOSFS_INFO("reserved = 0x%8.8x\n", bs.reserved);
	MKDOSFS_INFO("total-sect = 0x%8.8x\n", bs.total_sect);
	bs.total_sect *= times;
	MKDOSFS_INFO("total-sect = 0x%8.8x\n", bs.total_sect);
	MKDOSFS_INFO("fat32_length = 0x%8.8x\n", bs.fstype._fat32.fat32_length);
	bs.fstype._fat32.fat32_length *= times;
	MKDOSFS_INFO("fat32_length = 0x%8.8x\n", bs.fstype._fat32.fat32_length);
	MKDOSFS_INFO("info_sector = 0x%8.8x\n", bs.fstype._fat32.info_sector);
	bs.fstype._fat32.info_sector *= times;
	MKDOSFS_INFO("info_sector = 0x%8.8x\n", bs.fstype._fat32.info_sector);
	MKDOSFS_INFO("backup_boot = 0x%8.8x\n", bs.fstype._fat32.backup_boot);
	bs.fstype._fat32.backup_boot *= times;
	MKDOSFS_INFO("backup_boot = 0x%8.8x\n", bs.fstype._fat32.backup_boot);
	/* added by Hu Jiexun end */
#endif

#if 1
	/*chenxc add, support 4k sector size*/
	bs.sector_size[1] = 0x02;	/* 512 Bytes */
	MKDOSFS_INFO("sector_size =  %d\n", bs.sector_size[1]);

	MKDOSFS_INFO("sectors = %d\n", bs.sectors[0]);//fat32 0
	//b->sectors /= 8;
	MKDOSFS_INFO("sectors = %d\n", bs.sectors[1]);//fat32 0

	MKDOSFS_INFO("cluster_size =  %d\n", bs.cluster_size);
	bs.cluster_size *= times;
	MKDOSFS_INFO("cluster_size =  %d\n", bs.cluster_size);

	MKDOSFS_INFO("reserved =  %d\n", bs.reserved);
	bs.reserved *= times;//32*times
	MKDOSFS_INFO("reserved =  %d\n", bs.reserved);

	MKDOSFS_INFO("total-sect =  %d\n", bs.total_sect);
	bs.total_sect *= times;
	MKDOSFS_INFO("total-sect =  %d\n", bs.total_sect);

	MKDOSFS_INFO("fat32_length =  %d\n", bs.fstype._fat32.fat32_length);
	bs.fstype._fat32.fat32_length *= times;
	MKDOSFS_INFO("fat32_length =  %d\n", bs.fstype._fat32.fat32_length);

	MKDOSFS_INFO("backup_boot =  %d\n", bs.fstype._fat32.backup_boot);
	bs.fstype._fat32.backup_boot *= times;
	MKDOSFS_INFO("backup_boot =  %d\n", bs.fstype._fat32.backup_boot);
	/*chenxc modify end*/
#endif

	//writebuf( (char *) &bs, sizeof (struct msdos_boot_sector), "boot sector" );
	iRet = write(STDERR_FILENO, "=", 1);
    if(iRet < 0)
    {
        return iRet;
    }
	/* on FAT32, write the info sector and backup boot sector */
	if (size_fat == 32)
	{
		seekto( CF_LE_W(bs.fat32.info_sector)*sector_size, "info sector" );
		writebuf( info_sector, 512, "info sector" );
		if (backup_boot != 0)
		{
			seekto( backup_boot*sector_size, "backup boot sector" );
			writebuf( (char *) &bs, sizeof (struct msdos_boot_sector),
					"backup boot sector" );
		}
	}
	iRet = write(STDERR_FILENO, "=", 1);
    if(iRet < 0)
    {
        return iRet;
    }

#if 1
	/*chenxc add, support 4k sector size*/
  	MKDOSFS_INFO("info_sector =  %d\n", bs.fstype._fat32.info_sector);
  	bs.fstype._fat32.info_sector *= times;
  	MKDOSFS_INFO("info_sector =  %d\n", bs.fstype._fat32.info_sector);
  	
  	seekto( 0, "boot sector" );
  	writebuf( (char *) &bs, sizeof (struct msdos_boot_sector), "boot sector" );
	/*chenxc add end*/
#endif
	
	/* seek to start of FATS and write them all */
	seekto( reserved_sectors*sector_size, "first FAT" );

	formatScale = nr_fats * fat_length;
	MKDOSFS_INFO("mkdosfs: nr_fats = %d\n", nr_fats);
	for (x = 1; x <= nr_fats; x++)
	{
		writebuf(fat, sector_size, "FAT" );
		currentFormatProgress++;
        if (NULL != pFormatPercent)
        {
            *pFormatPercent = (float)(currentFormatProgress) / formatScale * 100;
        }

		y = fat_length-1;
		MKDOSFS_INFO("mkdosfs: x = %d, y = %d\n", x, y);
		while(y > 0)
		{
			writebuf(blank_sector, sector_size, "FAT" );
			currentFormatProgress++;
            if (NULL != pFormatPercent)
            {
                *pFormatPercent = (float)(currentFormatProgress) / formatScale * 100;
            }
			y--;
		}


		iRet = write(STDERR_FILENO, "=", 1);
        if(iRet < 0)
        {
            return iRet;
        }
	}
	/* Write the root directory directly after the last FAT. This is the root
	 * dir area on FAT12/16, and the first cluster on FAT32. */
	writebuf( (char *) root_dir, size_root_dir, "root directory" );
	iRet = write(STDERR_FILENO, "=", 1);
    if(iRet < 0)
    {
        return iRet;
    }
	iRet = write(STDERR_FILENO, "E", 1);
    if(iRet < 0)
    {
        return iRet;
    }
	/* added by Hu Jiexun start */
	iRet = write(STDERR_FILENO, "\r\n", 2);
    if(iRet < 0)
    {
        return iRet;
    }
	if (fsync(dev) != 0)
	{
		fprintf(stderr, "fsync fail, errno = %d(%s)\n", errno, strerror(errno));
		return -1;
	}
	/* added by Hu Jiexun end */

	if (blank_sector)
	{
		free( blank_sector );
		blank_sector = NULL;
	}
	if (info_sector)
	{
		free( info_sector );
		info_sector = NULL;
	}
	free (root_dir); /* Free up the root directory space from setup_tables */
	free (fat); /* Free up the fat table space reserved during setup_tables */
	root_dir = NULL;
	fat = NULL;
	return 0;
}

/* Function: usage
 * Description: Report the command usage.
 * Input:	none
 * Output: none
 * Return:  none
 */
void usage (void)
{
	fatal_error("\
	Usage: mkdosfs  [-c] [-v] [-I] [-n volume-name] [-i volume-id][-s sectors-per-cluster] \n\
		[-S logical-sector-size] [-f number-of-FATs][-h hidden-sectors] [-F fat-size] \n\
		/dev/name\n");
}

/* Function: mkdosfs_main
 * Description: The "main" entry point into the utility - we pick up the options and attempt to process them in some sort of sensible
   way.  In the event that some/all of the options are invalid we need to tell the user so that something can be done!
 * Input:	argc - arguments
 *		argv - the argument array
 * Output: none
 * Return: 0 if success, -1 if fail
 */
static int mkdosfs_main (int *pFormatPercent, int argc, char **argv)
{
	int rval = 0;
	int c;
	char *tmp;
	struct stat statbuf;

	unsigned long long cblocks = 0;

	/* added by Hu Jiexun start */
	optind = 1;
	sector_size = 512;	/* Size of a logical sector */
	sectors_per_cluster = 0;	/* Number of sectors per disk cluster */
	/* added by Hu Jiexun end */

	/*修改格式化硬盘后格式化USB 设备导致死机
	格式化USB 设备后格式化硬盘导致的格式化失败的问题*/
	optind = 1;
	
	if (argc && *argv) 		/* What's the program name? */
	{
		char *p;
		program_name = *argv;
		if ((p = strrchr( program_name, '/' )))
		{
			program_name = p+1;
		}
	}

	time(&create_time);
	volume_id = (long)create_time;	/* Default volume ID = creation time */

	MKDOSFS_INFO ("%s " VERSION " (" VERSION_DATE ")\n",  program_name);

	while ((c = getopt (argc, argv, "AbcCf:F:Ii:l:m:n:r:R:s:S:h:v")) != EOF)
	{
	/* Scan the command line for options */
		switch (c)
		{	
		case 'c':		/* c : Check FS as we build it */
			check = TRUE;
			break;

		case 'f':		/* f : Choose number of FATs */
			nr_fats = (int) strtol (optarg, &tmp, 0);
			if (*tmp || nr_fats < 1 || nr_fats > 4)
			{
				MKDOSFS_INFO ("Bad number of FATs : %s\n", optarg);
				usage ();
				return -1;
			}
			break;

		case 'F':		/* F : Choose FAT size */
			size_fat = (int) strtol (optarg, &tmp, 0);
			if (*tmp || (size_fat != 12 && size_fat != 16 && size_fat != 32))
			{
				MKDOSFS_INFO ("Bad FAT type : %s\n", optarg);
				usage ();
				return -1;	
			}
			size_fat_by_user = 1;
			break;

		case 'h':        /* h : number of hidden sectors */
			hidden_sectors = (int) strtol (optarg, &tmp, 0);
			if ( *tmp || hidden_sectors < 0 )
			{
				MKDOSFS_INFO("Bad number of hidden sectors : %s\n", optarg);
				usage ();
				return -1;	
			}
			break;

		case 'i':		/* i : specify volume ID */
			volume_id = strtoul(optarg, &tmp, 16);
			if ( *tmp )
			{
				MKDOSFS_INFO("Volume ID must be a hexadecimal number\n");
				usage();
				return -1;	
			}
			break;

		case 'n':		/* n : Volume name */
			sprintf(volume_name, "%-11.11s", optarg);
			break;
	
		case 's':		/* s : Sectors per cluster */
			sectors_per_cluster = (int) strtol (optarg, &tmp, 0);
			if (*tmp || (sectors_per_cluster != 1 && sectors_per_cluster != 2
			&& sectors_per_cluster != 4 && sectors_per_cluster != 8
			&& sectors_per_cluster != 16 && sectors_per_cluster != 32
			&& sectors_per_cluster != 64 && sectors_per_cluster != 128))
			{
				MKDOSFS_INFO ("Bad number of sectors per cluster : %s\n", optarg);
				usage ();
				return -1;
			}
			break;

		case 'S':		/* S : Sector size */
			sector_size = (int) strtol (optarg, &tmp, 0);
			if (*tmp || (sector_size != 512 && sector_size != 1024 &&
				sector_size != 2048 && sector_size != 4096 &&
				sector_size != 8192 && sector_size != 16384 &&
				sector_size != 32768))
			{
				MKDOSFS_INFO ("Bad logical sector size : %s\n", optarg);
				usage ();
				return -1;
			}
			break;

		case 'v':		/* v : Verbose execution */
			++verbose;
			break;
	
		default:
			MKDOSFS_INFO( "Unknown option: %c\n", c );
			usage ();
			return -1;
		}
	}
	if (optind < argc)
	{
		device_name = argv[optind];  /* Determine the number of blocks in the FS */
		cblocks = count_blocks (device_name); /*  Have a look and see! */
		if (cblocks == -1)
		{
			fprintf(stderr, "count blocks failed!\n");
			return -1;
		}
		blocks = cblocks;
	}
	else
	{
		fprintf (stderr, "No device specified!\n");
		usage ();
		return -1;	 
	}

	if (check_mount (device_name) < 0)	/* Is the device already mounted? */
	{
		return -1;
	}
	dev = open (device_name, O_RDWR|O_SHARED);	/* Is it a suitable device to build the FS on? */
	if (dev < 0)
	{
		die ("unable to open %s");
		return -1;
	}

	if (fstat (dev, &statbuf) < 0)
	{
		die ("unable to stat %s");
		rval = -1;
		goto exit;
	}

	if (!S_ISBLK (statbuf.st_mode))
	{
		statbuf.st_rdev = 0;
		check = 0;
	}
#if 0
	else if ((statbuf.st_rdev & 0xff3f) == 0x0300 || /* hda, hdb */
		(statbuf.st_rdev & 0xff0f) == 0x0800 || /* sd */
		(statbuf.st_rdev & 0xff3f) == 0x0d00 || /* xd */
		(statbuf.st_rdev & 0xff3f) == 0x1600)  /* hdc, hdd */
	{
		die ("Will not try to make filesystem on full-disk device '%s'");
		rval = -1;
		goto exit;
	}
#endif
	 /* Establish the media parameters */
	if (establish_params (statbuf.st_rdev,statbuf.st_size) < 0)
	{
		fprintf (stderr, "establish the media parameters failed.\n");
		rval = -1;
		goto exit;
	}

	/* NOTE: setup_tables() will allocate memory!!! */
	if (setup_tables () < 0)		/* Establish the file system tables */
	{
		fprintf (stderr, "setup the filesystem's tables failed.\n");
		rval = -1;
		goto exit;
	}
	if (check)			/* Determine any bad block locations and mark them */
	{
		if (check_blocks () < 0)
		{
			fprintf (stderr, "check bad blocks failed.\n");
			rval = -1;
			goto exit;
		}
	}

	if (write_tables (pFormatPercent) < 0)		/* Write the file system tables away! */
	{
		fprintf(stderr, "Write the filesystem's data tables failed.\n");
		rval = -1;
		goto exit;
	}

	/* added by Hu Jiexun start */
exit:
	if (blank_sector)
	{
		free( blank_sector );
		blank_sector = NULL;
	}
	if (info_sector)
	{
		free( info_sector );
		info_sector = NULL;
	}
	if (root_dir)
	{
		free (root_dir); /* Free up the root directory space from setup_tables */
		root_dir = NULL;
	}
	if (fat)
	{
		free (fat); /* Free up the fat table space reserved during setup_tables */
		fat = NULL;
	}
	/* added by Hu Jiexun end */

	if (dev != -1)			//add by lhz for 多次格式化
	{
		close(dev);
		dev = -1;
	}
	return rval;
}

/* Function: mkDosFsEx
 * Description: create a FAT file system
 * Input: pDevName - the name of the device
             sectors_per_cluster -number of sectors per cluster
             sect_size - size of a logical sector
             fat_size - fat size
 * Output: none
 * Return: 0 if success, -1 if fail
 */
int mkFatFsEx(const char *dev_name, int sectors_per_cluster, int sect_size, int fat_size, int volume_name, int *pFormatPercent)
{
	if (fat_size != 0 && fat_size != 12 && fat_size != 16 && fat_size != 32)
	{
		return -1;
	}

	int argc=0;
	char* argv[16];
	char partName[64];
	char sectSize[64];
	char clusterSize[64];
	char fatSize[16];
	char volumeName[16], name[16];
	int  devNameLen;
	int  partNo;

	sprintf(partName,"%s",dev_name);	
	MKDOSFS_INFO("mkdosfs: %s, %d, %d, %d\n", partName, sectors_per_cluster, sect_size, fat_size);

	argc = 0;
	argv[argc++] = "mkdosfs";

	if(sectors_per_cluster)
	{
		sprintf(clusterSize,"%d",sectors_per_cluster);
		argv[argc++] = "-s";
		argv[argc++] = clusterSize;
	}

	if(sect_size)
	{
		sprintf(sectSize,"%d",sect_size);
		argv[argc++] = "-S";
		argv[argc++] = sectSize;
	}

	if (fat_size)
	{
		sprintf(fatSize, "%d", fat_size);
		argv[argc++] = "-F";
		argv[argc++] = fatSize;
	}

	if(volume_name)		/* 卷标*/
	{
		devNameLen = strlen(dev_name);
		partNo     = dev_name[devNameLen - 1] - '1';
		sprintf(name, "netDVR%d", partNo);
		sprintf(volumeName, "%-11.11s", name);
		argv[argc++] = "-n";
		argv[argc++] = volumeName;
	}

	argv[argc++] = (char *) dev_name;
	argv[argc] = (char *) 0;

	return mkdosfs_main(pFormatPercent, argc, argv);
}

/* Function: mkDosFs
 * Description: create a FAT file system
 * Input: pDevName - the name of the device
             sectors_per_cluster -number of sectors per cluster
             sect_size - size of a logical sector
             fat_size - fat size
 * Output: none
 * Return: 0 if success, -1 if fail
 */
int make_fat_fs(const char *dev_name, int sectors_per_cluster, int sect_size, int fat_size, int volume_name)
{
	return mkFatFsEx(dev_name, sectors_per_cluster, sect_size, fat_size, volume_name, NULL);
}

