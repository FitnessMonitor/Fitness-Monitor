//#include "ff.h"			/* FatFs configurations and declarations */
#include "diskio.h"		/* Declarations of low level disk I/O functions */


/*--------------------------------------------------------------------------

   Module Private Definitions

---------------------------------------------------------------------------*/

#if _FATFS != 8255
#error Wrong include file (ff.h).
#endif


/* Definitions on sector size */
#if _MAX_SS != 512 && _MAX_SS != 1024 && _MAX_SS != 2048 && _MAX_SS != 4096
#error Wrong sector size.
#endif
#if _MAX_SS != 512
#define	SS(fs)	((fs)->ssize)	/* Multiple sector size */
#else
#define	SS(fs)	512U			/* Fixed sector size */
#endif


/* Reentrancy related */
#if _FS_REENTRANT
#if _USE_LFN == 1
#error Static LFN work area must not be used in re-entrant configuration.
#endif
#define	ENTER_FF(fs)		{ if (!lock_fs(fs)) return FR_TIMEOUT; }
#define	LEAVE_FF(fs, res)	{ unlock_fs(fs, res); return res; }
#else
#define	ENTER_FF(fs)
#define LEAVE_FF(fs, res)	return res
#endif

#define	ABORT(fs, res)		{ fp->flag |= FA__ERROR; LEAVE_FF(fs, res); }


/* File shareing feature */
#if _FS_SHARE
#if _FS_READONLY
#error _FS_SHARE must be 0 on read-only cfg.
#endif
typedef struct {
	FATFS *fs;				/* File ID 1, volume (NULL:blank entry) */
	DWORD clu;				/* File ID 2, directory */
	WORD idx;				/* File ID 3, directory index */
	WORD ctr;				/* File open counter, 0:none, 0x01..0xFF:read open count, 0x100:write mode */
} FILESEM;
#endif


/* Misc definitions */
#define LD_CLUST(dir)	(((DWORD)LD_WORD(dir+DIR_FstClusHI)<<16) | LD_WORD(dir+DIR_FstClusLO))
#define ST_CLUST(dir,cl) {ST_WORD(dir+DIR_FstClusLO, cl); ST_WORD(dir+DIR_FstClusHI, (DWORD)cl>>16);}


/* Character code support macros */
#define IsUpper(c)	(((c)>='A')&&((c)<='Z'))
#define IsLower(c)	(((c)>='a')&&((c)<='z'))
#define IsDigit(c)	(((c)>='0')&&((c)<='9'))

#if _DF1S		/* Code page is DBCS */

#ifdef _DF2S	/* Two 1st byte areas */
#define IsDBCS1(c)	(((BYTE)(c) >= _DF1S && (BYTE)(c) <= _DF1E) || ((BYTE)(c) >= _DF2S && (BYTE)(c) <= _DF2E))
#else			/* One 1st byte area */
#define IsDBCS1(c)	((BYTE)(c) >= _DF1S && (BYTE)(c) <= _DF1E)
#endif

#ifdef _DS3S	/* Three 2nd byte areas */
#define IsDBCS2(c)	(((BYTE)(c) >= _DS1S && (BYTE)(c) <= _DS1E) || ((BYTE)(c) >= _DS2S && (BYTE)(c) <= _DS2E) || ((BYTE)(c) >= _DS3S && (BYTE)(c) <= _DS3E))
#else			/* Two 2nd byte areas */
#define IsDBCS2(c)	(((BYTE)(c) >= _DS1S && (BYTE)(c) <= _DS1E) || ((BYTE)(c) >= _DS2S && (BYTE)(c) <= _DS2E))
#endif

#else			/* Code page is SBCS */

#define IsDBCS1(c)	0
#define IsDBCS2(c)	0

#endif /* _DF1S */


/* Name status flags */
#define NS			11		/* Offset of name status byte */
#define NS_LOSS		0x01	/* Out of 8.3 format */
#define NS_LFN		0x02	/* Force to create LFN entry */
#define NS_LAST		0x04	/* Last segment */
#define NS_BODY		0x08	/* Lower case flag (body) */
#define NS_EXT		0x10	/* Lower case flag (ext) */
#define NS_DOT		0x20	/* Dot entry */


/* FAT sub-type boundaries */
/* Note that the FAT spec by Microsoft says 4085 but Windows works with 4087! */
#define MIN_FAT16	4086	/* Minimum number of clusters for FAT16 */
#define	MIN_FAT32	65526	/* Minimum number of clusters for FAT32 */


/* FatFs refers the members in the FAT structures as byte array instead of
/ structure member because there are incompatibility of the packing option
/ between compilers. */

#define BS_jmpBoot			0
#define BS_OEMName			3
#define BPB_BytsPerSec		11
#define BPB_SecPerClus		13
#define BPB_RsvdSecCnt		14
#define BPB_NumFATs			16
#define BPB_RootEntCnt		17
#define BPB_TotSec16		19
#define BPB_Media			21
#define BPB_FATSz16			22
#define BPB_SecPerTrk		24
#define BPB_NumHeads		26
#define BPB_HiddSec			28
#define BPB_TotSec32		32
#define BS_DrvNum			36
#define BS_BootSig			38
#define BS_VolID			39
#define BS_VolLab			43
#define BS_FilSysType		54
#define BPB_FATSz32			36
#define BPB_ExtFlags		40
#define BPB_FSVer			42
#define BPB_RootClus		44
#define BPB_FSInfo			48
#define BPB_BkBootSec		50
#define BS_DrvNum32			64
#define BS_BootSig32		66
#define BS_VolID32			67
#define BS_VolLab32			71
#define BS_FilSysType32		82
#define	FSI_LeadSig			0
#define	FSI_StrucSig		484
#define	FSI_Free_Count		488
#define	FSI_Nxt_Free		492
#define MBR_Table			446
#define BS_55AA				510

#define	DIR_Name			0
#define	DIR_Attr			11
#define	DIR_NTres			12
#define	DIR_CrtTime			14
#define	DIR_CrtDate			16
#define	DIR_FstClusHI		20
#define	DIR_WrtTime			22
#define	DIR_WrtDate			24
#define	DIR_FstClusLO		26
#define	DIR_FileSize		28
#define	LDIR_Ord			0
#define	LDIR_Attr			11
#define	LDIR_Type			12
#define	LDIR_Chksum			13
#define	LDIR_FstClusLO		26



/*------------------------------------------------------------*/
/* Work area                                                  */

#if _VOLUMES
static
FATFS *FatFs[_VOLUMES];	/* Pointer to the file system objects (logical drives) */
#else
#error Number of drives must not be 0.
#endif

static
WORD Fsid;				/* File system mount ID */

#if _FS_RPATH
static
BYTE CurrVol;			/* Current drive */
#endif

#if _FS_SHARE
static
FILESEM	Files[_FS_SHARE];	/* File lock semaphores */
#endif

#if _USE_LFN == 0			/* No LFN */
#define	DEF_NAMEBUF			BYTE sfn[12]
#define INIT_BUF(dobj)		(dobj).fn = sfn
#define	FREE_BUF()

#elif _USE_LFN == 1			/* LFN with static LFN working buffer */
static WCHAR LfnBuf[_MAX_LFN+1];
#define	DEF_NAMEBUF			BYTE sfn[12]
#define INIT_BUF(dobj)		{ (dobj).fn = sfn; (dobj).lfn = LfnBuf; }
#define	FREE_BUF()

#elif _USE_LFN == 2 		/* LFN with dynamic LFN working buffer on the stack */
#define	DEF_NAMEBUF			BYTE sfn[12]; WCHAR lbuf[_MAX_LFN+1]
#define INIT_BUF(dobj)		{ (dobj).fn = sfn; (dobj).lfn = lbuf; }
#define	FREE_BUF()

#elif _USE_LFN == 3 		/* LFN with dynamic LFN working buffer on the heap */
#define	DEF_NAMEBUF			BYTE sfn[12]; WCHAR *lfn
#define INIT_BUF(dobj)		{ lfn = ff_memalloc((_MAX_LFN + 1) * 2); \
							  if (!lfn) LEAVE_FF((dobj).fs, FR_NOT_ENOUGH_CORE); \
							  (dobj).lfn = lfn;	(dobj).fn = sfn; }
#define	FREE_BUF()			ff_memfree(lfn)

#else
#error Wrong LFN configuration.
#endif



/*--------------------------------------------------------------------------

   Module Private Functions

---------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------*/
/* String functions                                                      */
/*-----------------------------------------------------------------------*/

/* Copy memory to memory */
static
void mem_cpy (void* dst, const void* src, UINT cnt) {
	BYTE *d = (BYTE*)dst;
	const BYTE *s = (const BYTE*)src;

#if _WORD_ACCESS == 1
	while (cnt >= sizeof(int)) {
		*(int*)d = *(int*)s;
		d += sizeof(int); s += sizeof(int);
		cnt -= sizeof(int);
	}
#endif
	while (cnt--)
		*d++ = *s++;
}

/* Fill memory */
static
void mem_set (void* dst, int val, UINT cnt) {
	BYTE *d = (BYTE*)dst;

	while (cnt--)
		*d++ = (BYTE)val;
}

/* Compare memory to memory */
static
int mem_cmp (const void* dst, const void* src, UINT cnt) {
	const BYTE *d = (const BYTE *)dst, *s = (const BYTE *)src;
	int r = 0;

	while (cnt-- && (r = *d++ - *s++) == 0) ;
	return r;
}

/* Check if chr is contained in the string */
static
int chk_chr (const char* str, int chr) {
	while (*str && *str != chr) str++;
	return *str;
}



/*-----------------------------------------------------------------------*/
/* Request/Release grant to access the volume                            */
/*-----------------------------------------------------------------------*/
#if _FS_REENTRANT

static
int lock_fs (
	FATFS *fs		/* File system object */
)
{
	return ff_req_grant(fs->sobj);
}


static
void unlock_fs (
	FATFS *fs,		/* File system object */
	FRESULT res		/* Result code to be returned */
)
{
	if (res != FR_NOT_ENABLED &&
		res != FR_INVALID_DRIVE &&
		res != FR_INVALID_OBJECT &&
		res != FR_TIMEOUT) {
		ff_rel_grant(fs->sobj);
	}
}
#endif



/*-----------------------------------------------------------------------*/
/* File shareing control functions                                       */
/*-----------------------------------------------------------------------*/
#if _FS_SHARE

static
FRESULT chk_lock (	/* Check if the file can be accessed */
	DIR* dj,		/* Directory object pointing the file to be checked */
	int acc			/* Desired access (0:Read, 1:Write, 2:Delete/Rename) */
)
{
	UINT i, be;

	/* Search file semaphore table */
	for (i = be = 0; i < _FS_SHARE; i++) {
		if (Files[i].fs) {	/* Existing entry */
			if (Files[i].fs == dj->fs &&	 	/* Check if the file matched with an open file */
				Files[i].clu == dj->sclust &&
				Files[i].idx == dj->index) break;
		} else {			/* Blank entry */
			be++;
		}
	}
	if (i == _FS_SHARE)	/* The file is not opened */
		return (be || acc == 2) ? FR_OK : FR_TOO_MANY_OPEN_FILES;	/* Is there a blank entry for new file? */

	/* The file has been opened. Reject any open against writing file and all write mode open */
	return (acc || Files[i].ctr == 0x100) ? FR_LOCKED : FR_OK;
}


static
int enq_lock (	/* Check if an entry is available for a new file */
	FATFS* fs	/* File system object */
)
{
	UINT i;

	for (i = 0; i < _FS_SHARE && Files[i].fs; i++) ;
	return (i == _FS_SHARE) ? 0 : 1;
}


static
UINT inc_lock (	/* Increment file open counter and returns its index (0:int error) */
	DIR* dj,	/* Directory object pointing the file to register or increment */
	int acc		/* Desired access mode (0:Read, !0:Write) */
)
{
	UINT i;


	for (i = 0; i < _FS_SHARE; i++) {	/* Find the file */
		if (Files[i].fs == dj->fs &&
			Files[i].clu == dj->sclust &&
			Files[i].idx == dj->index) break;
	}

	if (i == _FS_SHARE) {				/* Not opened. Register it as new. */
		for (i = 0; i < _FS_SHARE && Files[i].fs; i++) ;
		if (i == _FS_SHARE) return 0;	/* No space to register (int err) */
		Files[i].fs = dj->fs;
		Files[i].clu = dj->sclust;
		Files[i].idx = dj->index;
		Files[i].ctr = 0;
	}

	if (acc && Files[i].ctr) return 0;	/* Access violation (int err) */

	Files[i].ctr = acc ? 0x100 : Files[i].ctr + 1;	/* Set semaphore value */

	return i + 1;
}


static
FRESULT dec_lock (	/* Decrement file open counter */
	UINT i			/* Semaphore index */
)
{
	WORD n;
	FRESULT res;


	if (--i < _FS_SHARE) {
		n = Files[i].ctr;
		if (n == 0x100) n = 0;
		if (n) n--;
		Files[i].ctr = n;
		if (!n) Files[i].fs = 0;
		res = FR_OK;
	} else {
		res = FR_INT_ERR;
	}
	return res;
}


static
void clear_lock (	/* Clear lock entries of the volume */
	FATFS *fs
)
{
	UINT i;

	for (i = 0; i < _FS_SHARE; i++) {
		if (Files[i].fs == fs) Files[i].fs = 0;
	}
}
#endif



/*-----------------------------------------------------------------------*/
/* Change window offset                                                  */
/*-----------------------------------------------------------------------*/

static
FRESULT move_window (
	FATFS *fs,		/* File system object */
	DWORD sector	/* Sector number to make appearance in the fs->win[] */
)					/* Move to zero only writes back dirty window */
{
	DWORD wsect;


	wsect = fs->winsect;
	if (wsect != sector) {	/* Changed current window */
#if !_FS_READONLY
		if (fs->wflag) {	/* Write back dirty window if needed */
			if (disk_write(fs->drv, fs->win, wsect, 1) != RES_OK)
				return FR_DISK_ERR;
			fs->wflag = 0;
			if (wsect < (fs->fatbase + fs->fsize)) {	/* In FAT area */
				BYTE nf;
				for (nf = fs->n_fats; nf > 1; nf--) {	/* Reflect the change to all FAT copies */
					wsect += fs->fsize;
					disk_write(fs->drv, fs->win, wsect, 1);
				}
			}
		}
#endif
		if (sector) {
			if (disk_read(fs->drv, fs->win, sector, 1) != RES_OK)
				return FR_DISK_ERR;
			fs->winsect = sector;
		}
	}

	return FR_OK;
}




/*-----------------------------------------------------------------------*/
/* Clean-up cached data                                                  */
/*-----------------------------------------------------------------------*/
#if !_FS_READONLY
static
FRESULT sync (	/* FR_OK: successful, FR_DISK_ERR: failed */
	FATFS *fs	/* File system object */
)
{
	FRESULT res;


	res = move_window(fs, 0);
	if (res == FR_OK) {
		/* Update FSInfo sector if needed */
		if (fs->fs_type == FS_FAT32 && fs->fsi_flag) {
			fs->winsect = 0;
			mem_set(fs->win, 0, 512);
			ST_WORD(fs->win+BS_55AA, 0xAA55);
			ST_DWORD(fs->win+FSI_LeadSig, 0x41615252);
			ST_DWORD(fs->win+FSI_StrucSig, 0x61417272);
			ST_DWORD(fs->win+FSI_Free_Count, fs->free_clust);
			ST_DWORD(fs->win+FSI_Nxt_Free, fs->last_clust);
			disk_write(fs->drv, fs->win, fs->fsi_sector, 1);
			fs->fsi_flag = 0;
		}
		/* Make sure that no pending write process in the physical drive */
		if (disk_ioctl(fs->drv, CTRL_SYNC, (void*)0) != RES_OK)
			res = FR_DISK_ERR;
	}

	return res;
}
#endif




/*-----------------------------------------------------------------------*/
/* Get sector# from cluster#                                             */
/*-----------------------------------------------------------------------*/


DWORD clust2sect (	/* !=0: Sector number, 0: Failed - invalid cluster# */
	FATFS *fs,		/* File system object */
	DWORD clst		/* Cluster# to be converted */
)
{
	clst -= 2;
	if (clst >= (fs->n_fatent - 2)) return 0;		/* Invalid cluster# */
	return clst * fs->csize + fs->database;
}




/*-----------------------------------------------------------------------*/
/* FAT access - Read value of a FAT entry                                */
/*-----------------------------------------------------------------------*/


DWORD get_fat (	/* 0xFFFFFFFF:Disk error, 1:Internal error, Else:Cluster status */
	FATFS *fs,	/* File system object */
	DWORD clst	/* Cluster# to get the link information */
)
{
	UINT wc, bc;
	BYTE *p;


	if (clst < 2 || clst >= fs->n_fatent)	/* Chack range */
		return 1;

	switch (fs->fs_type) {
	case FS_FAT12 :
		bc = (UINT)clst; bc += bc / 2;
		if (move_window(fs, fs->fatbase + (bc / SS(fs)))) break;
		wc = fs->win[bc % SS(fs)]; bc++;
		if (move_window(fs, fs->fatbase + (bc / SS(fs)))) break;
		wc |= fs->win[bc % SS(fs)] << 8;
		return (clst & 1) ? (wc >> 4) : (wc & 0xFFF);

	case FS_FAT16 :
		if (move_window(fs, fs->fatbase + (clst / (SS(fs) / 2)))) break;
		p = &fs->win[clst * 2 % SS(fs)];
		return LD_WORD(p);

	case FS_FAT32 :
		if (move_window(fs, fs->fatbase + (clst / (SS(fs) / 4)))) break;
		p = &fs->win[clst * 4 % SS(fs)];
		return LD_DWORD(p) & 0x0FFFFFFF;
	}

	return 0xFFFFFFFF;	/* An error occurred at the disk I/O layer */
}




/*-----------------------------------------------------------------------*/
/* FAT access - Change value of a FAT entry                              */
/*-----------------------------------------------------------------------*/
#if !_FS_READONLY

FRESULT put_fat (
	FATFS *fs,	/* File system object */
	DWORD clst,	/* Cluster# to be changed in range of 2 to fs->n_fatent - 1 */
	DWORD val	/* New value to mark the cluster */
)
{
	UINT bc;
	BYTE *p;
	FRESULT res;


	if (clst < 2 || clst >= fs->n_fatent) {	/* Check range */
		res = FR_INT_ERR;

	} else {
		switch (fs->fs_type) {
		case FS_FAT12 :
			bc = clst; bc += bc / 2;
			res = move_window(fs, fs->fatbase + (bc / SS(fs)));
			if (res != FR_OK) break;
			p = &fs->win[bc % SS(fs)];
			*p = (clst & 1) ? ((*p & 0x0F) | ((BYTE)val << 4)) : (BYTE)val;
			bc++;
			fs->wflag = 1;
			res = move_window(fs, fs->fatbase + (bc / SS(fs)));
			if (res != FR_OK) break;
			p = &fs->win[bc % SS(fs)];
			*p = (clst & 1) ? (BYTE)(val >> 4) : ((*p & 0xF0) | ((BYTE)(val >> 8) & 0x0F));
			break;

		case FS_FAT16 :
			res = move_window(fs, fs->fatbase + (clst / (SS(fs) / 2)));
			if (res != FR_OK) break;
			p = &fs->win[clst * 2 % SS(fs)];
			ST_WORD(p, (WORD)val);
			break;

		case FS_FAT32 :
			res = move_window(fs, fs->fatbase + (clst / (SS(fs) / 4)));
			if (res != FR_OK) break;
			p = &fs->win[clst * 4 % SS(fs)];
			val |= LD_DWORD(p) & 0xF0000000;
			ST_DWORD(p, val);
			break;

		default :
			res = FR_INT_ERR;
		}
		fs->wflag = 1;
	}

	return res;
}
#endif /* !_FS_READONLY */




/*-----------------------------------------------------------------------*/
/* FAT handling - Remove a cluster chain                                 */
/*-----------------------------------------------------------------------*/
#if !_FS_READONLY
static
FRESULT remove_chain (
	FATFS *fs,			/* File system object */
	DWORD clst			/* Cluster# to remove a chain from */
)
{
	FRESULT res;
	DWORD nxt;
#if _USE_ERASE
	DWORD scl = clst, ecl = clst, resion[2];
#endif

	if (clst < 2 || clst >= fs->n_fatent) {	/* Check range */
		res = FR_INT_ERR;

	} else {
		res = FR_OK;
		while (clst < fs->n_fatent) {			/* Not a last link? */
			nxt = get_fat(fs, clst);			/* Get cluster status */
			if (nxt == 0) break;				/* Empty cluster? */
			if (nxt == 1) { res = FR_INT_ERR; break; }	/* Internal error? */
			if (nxt == 0xFFFFFFFF) { res = FR_DISK_ERR; break; }	/* Disk error? */
			res = put_fat(fs, clst, 0);			/* Mark the cluster "empty" */
			if (res != FR_OK) break;
			if (fs->free_clust != 0xFFFFFFFF) {	/* Update FSInfo */
				fs->free_clust++;
				fs->fsi_flag = 1;
			}
#if _USE_ERASE
			if (ecl + 1 == nxt) {	/* Next cluster is contiguous */
				ecl = nxt;
			} else {				/* End of contiguous clusters */ 
				resion[0] = clust2sect(fs, scl);					/* Start sector */
				resion[1] = clust2sect(fs, ecl) + fs->csize - 1;	/* End sector */
				disk_ioctl(fs->drv, CTRL_ERASE_SECTOR, resion);		/* Erase the block */
				scl = ecl = nxt;
			}
#endif
			clst = nxt;	/* Next cluster */
		}
	}

	return res;
}
#endif




/*-----------------------------------------------------------------------*/
/* FAT handling - Stretch or Create a cluster chain                      */
/*-----------------------------------------------------------------------*/
#if !_FS_READONLY
static
DWORD create_chain (	/* 0:No free cluster, 1:Internal error, 0xFFFFFFFF:Disk error, >=2:New cluster# */
	FATFS *fs,			/* File system object */
	DWORD clst			/* Cluster# to stretch. 0 means create a new chain. */
)
{
	DWORD cs, ncl, scl;
	FRESULT res;


	if (clst == 0) {		/* Create a new chain */
		scl = fs->last_clust;			/* Get suggested start point */
		if (!scl || scl >= fs->n_fatent) scl = 1;
	}
	else {					/* Stretch the current chain */
		cs = get_fat(fs, clst);			/* Check the cluster status */
		if (cs < 2) return 1;			/* It is an invalid cluster */
		if (cs < fs->n_fatent) return cs;	/* It is already followed by next cluster */
		scl = clst;
	}

	ncl = scl;				/* Start cluster */
	for (;;) {
		ncl++;							/* Next cluster */
		if (ncl >= fs->n_fatent) {		/* Wrap around */
			ncl = 2;
			if (ncl > scl) return 0;	/* No free cluster */
		}
		cs = get_fat(fs, ncl);			/* Get the cluster status */
		if (cs == 0) break;				/* Found a free cluster */
		if (cs == 0xFFFFFFFF || cs == 1)/* An error occurred */
			return cs;
		if (ncl == scl) return 0;		/* No free cluster */
	}

	res = put_fat(fs, ncl, 0x0FFFFFFF);	/* Mark the new cluster "last link" */
	if (res == FR_OK && clst != 0) {
		res = put_fat(fs, clst, ncl);	/* Link it to the previous one if needed */
	}
	if (res == FR_OK) {
		fs->last_clust = ncl;			/* Update FSINFO */
		if (fs->free_clust != 0xFFFFFFFF) {
			fs->free_clust--;
			fs->fsi_flag = 1;
		}
	} else {
		ncl = (res == FR_DISK_ERR) ? 0xFFFFFFFF : 1;
	}

	return ncl;		/* Return new cluster number or error code */
}
#endif /* !_FS_READONLY */




/*-----------------------------------------------------------------------*/
/* Directory handling - Set directory index                              */
/*-----------------------------------------------------------------------*/

static
FRESULT dir_sdi (
	DIR *dj,		/* Pointer to directory object */
	WORD idx		/* Directory index number */
)
{
	DWORD clst;
	WORD ic;


	dj->index = idx;
	clst = dj->sclust;
	if (clst == 1 || clst >= dj->fs->n_fatent)	/* Check start cluster range */
		return FR_INT_ERR;
	if (!clst && dj->fs->fs_type == FS_FAT32)	/* Replace cluster# 0 with root cluster# if in FAT32 */
		clst = dj->fs->dirbase;

	if (clst == 0) {	/* Static table (root-dir in FAT12/16) */
		dj->clust = clst;
		if (idx >= dj->fs->n_rootdir)		/* Index is out of range */
			return FR_INT_ERR;
		dj->sect = dj->fs->dirbase + idx / (SS(dj->fs) / 32);	/* Sector# */
	}
	else {				/* Dynamic table (sub-dirs or root-dir in FAT32) */
		ic = SS(dj->fs) / 32 * dj->fs->csize;	/* Entries per cluster */
		while (idx >= ic) {	/* Follow cluster chain */
			clst = get_fat(dj->fs, clst);				/* Get next cluster */
			if (clst == 0xFFFFFFFF) return FR_DISK_ERR;	/* Disk error */
			if (clst < 2 || clst >= dj->fs->n_fatent)	/* Reached to end of table or int error */
				return FR_INT_ERR;
			idx -= ic;
		}
		dj->clust = clst;
		dj->sect = clust2sect(dj->fs, clst) + idx / (SS(dj->fs) / 32);	/* Sector# */
	}

	dj->dir = dj->fs->win + (idx % (SS(dj->fs) / 32)) * 32;	/* Ptr to the entry in the sector */

	return FR_OK;	/* Seek succeeded */
}




/*-----------------------------------------------------------------------*/
/* Directory handling - Move directory index next                        */
/*-----------------------------------------------------------------------*/

static
FRESULT dir_next (	/* FR_OK:Succeeded, FR_NO_FILE:End of table, FR_DENIED:EOT and could not stretch */
	DIR *dj,		/* Pointer to directory object */
	int stretch		/* 0: Do not stretch table, 1: Stretch table if needed */
)
{
	DWORD clst;
	WORD i;


	i = dj->index + 1;
	if (!i || !dj->sect)	/* Report EOT when index has reached 65535 */
		return FR_NO_FILE;

	if (!(i % (SS(dj->fs) / 32))) {	/* Sector changed? */
		dj->sect++;					/* Next sector */

		if (dj->clust == 0) {	/* Static table */
			if (i >= dj->fs->n_rootdir)	/* Report EOT when end of table */
				return FR_NO_FILE;
		}
		else {					/* Dynamic table */
			if (((i / (SS(dj->fs) / 32)) & (dj->fs->csize - 1)) == 0) {	/* Cluster changed? */
				clst = get_fat(dj->fs, dj->clust);				/* Get next cluster */
				if (clst <= 1) return FR_INT_ERR;
				if (clst == 0xFFFFFFFF) return FR_DISK_ERR;
				if (clst >= dj->fs->n_fatent) {					/* When it reached end of dynamic table */
#if !_FS_READONLY
					BYTE c;
					if (!stretch) return FR_NO_FILE;			/* When do not stretch, report EOT */
					clst = create_chain(dj->fs, dj->clust);		/* Stretch cluster chain */
					if (clst == 0) return FR_DENIED;			/* No free cluster */
					if (clst == 1) return FR_INT_ERR;
					if (clst == 0xFFFFFFFF) return FR_DISK_ERR;
					/* Clean-up stretched table */
					if (move_window(dj->fs, 0)) return FR_DISK_ERR;	/* Flush active window */
					mem_set(dj->fs->win, 0, SS(dj->fs));			/* Clear window buffer */
					dj->fs->winsect = clust2sect(dj->fs, clst);	/* Cluster start sector */
					for (c = 0; c < dj->fs->csize; c++) {		/* Fill the new cluster with 0 */
						dj->fs->wflag = 1;
						if (move_window(dj->fs, 0)) return FR_DISK_ERR;
						dj->fs->winsect++;
					}
					dj->fs->winsect -= c;						/* Rewind window address */
#else
					return FR_NO_FILE;			/* Report EOT */
#endif
				}
				dj->clust = clst;				/* Initialize data for new cluster */
				dj->sect = clust2sect(dj->fs, clst);
			}
		}
	}

	dj->index = i;
	dj->dir = dj->fs->win + (i % (SS(dj->fs) / 32)) * 32;

	return FR_OK;
}




/*-----------------------------------------------------------------------*/
/* LFN handling - Test/Pick/Fit an LFN segment from/to directory entry   */
/*-----------------------------------------------------------------------*/
#if _USE_LFN
static
const BYTE LfnOfs[] = {1,3,5,7,9,14,16,18,20,22,24,28,30};	/* Offset of LFN chars in the directory entry */


static
int cmp_lfn (			/* 1:Matched, 0:Not matched */
	WCHAR *lfnbuf,		/* Pointer to the LFN to be compared */
	BYTE *dir			/* Pointer to the directory entry containing a part of LFN */
)
{
	UINT i, s;
	WCHAR wc, uc;


	i = ((dir[LDIR_Ord] & 0xBF) - 1) * 13;	/* Get offset in the LFN buffer */
	s = 0; wc = 1;
	do {
		uc = LD_WORD(dir+LfnOfs[s]);	/* Pick an LFN character from the entry */
		if (wc) {	/* Last char has not been processed */
			wc = ff_wtoupper(uc);		/* Convert it to upper case */
			if (i >= _MAX_LFN || wc != ff_wtoupper(lfnbuf[i++]))	/* Compare it */
				return 0;				/* Not matched */
		} else {
			if (uc != 0xFFFF) return 0;	/* Check filler */
		}
	} while (++s < 13);				/* Repeat until all chars in the entry are checked */

	if ((dir[LDIR_Ord] & 0x40) && wc && lfnbuf[i])	/* Last segment matched but different length */
		return 0;

	return 1;						/* The part of LFN matched */
}



static
int pick_lfn (			/* 1:Succeeded, 0:Buffer overflow */
	WCHAR *lfnbuf,		/* Pointer to the Unicode-LFN buffer */
	BYTE *dir			/* Pointer to the directory entry */
)
{
	UINT i, s;
	WCHAR wc, uc;


	i = ((dir[LDIR_Ord] & 0x3F) - 1) * 13;	/* Offset in the LFN buffer */

	s = 0; wc = 1;
	do {
		uc = LD_WORD(dir+LfnOfs[s]);		/* Pick an LFN character from the entry */
		if (wc) {	/* Last char has not been processed */
			if (i >= _MAX_LFN) return 0;	/* Buffer overflow? */
			lfnbuf[i++] = wc = uc;			/* Store it */
		} else {
			if (uc != 0xFFFF) return 0;		/* Check filler */
		}
	} while (++s < 13);						/* Read all character in the entry */

	if (dir[LDIR_Ord] & 0x40) {				/* Put terminator if it is the last LFN part */
		if (i >= _MAX_LFN) return 0;		/* Buffer overflow? */
		lfnbuf[i] = 0;
	}

	return 1;
}


#if !_FS_READONLY
static
void fit_lfn (
	const WCHAR *lfnbuf,	/* Pointer to the LFN buffer */
	BYTE *dir,				/* Pointer to the directory entry */
	BYTE ord,				/* LFN order (1-20) */
	BYTE sum				/* SFN sum */
)
{
	UINT i, s;
	WCHAR wc;


	dir[LDIR_Chksum] = sum;			/* Set check sum */
	dir[LDIR_Attr] = AM_LFN;		/* Set attribute. LFN entry */
	dir[LDIR_Type] = 0;
	ST_WORD(dir+LDIR_FstClusLO, 0);

	i = (ord - 1) * 13;				/* Get offset in the LFN buffer */
	s = wc = 0;
	do {
		if (wc != 0xFFFF) wc = lfnbuf[i++];	/* Get an effective char */
		ST_WORD(dir+LfnOfs[s], wc);	/* Put it */
		if (!wc) wc = 0xFFFF;		/* Padding chars following last char */
	} while (++s < 13);
	if (wc == 0xFFFF || !lfnbuf[i]) ord |= 0x40;	/* Bottom LFN part is the start of LFN sequence */
	dir[LDIR_Ord] = ord;			/* Set the LFN order */
}

#endif
#endif



/*-----------------------------------------------------------------------*/
/* Create numbered name                                                  */
/*-----------------------------------------------------------------------*/
#if _USE_LFN
void gen_numname (
	BYTE *dst,			/* Pointer to generated SFN */
	const BYTE *src,	/* Pointer to source SFN to be modified */
	const WCHAR *lfn,	/* Pointer to LFN */
	WORD seq			/* Sequence number */
)
{
	BYTE ns[8], c;
	UINT i, j;


	mem_cpy(dst, src, 11);

	if (seq > 5) {	/* On many collisions, generate a hash number instead of sequential number */
		do seq = (seq >> 1) + (seq << 15) + (WORD)*lfn++; while (*lfn);
	}

	/* itoa */
	i = 7;
	do {
		c = (seq % 16) + '0';
		if (c > '9') c += 7;
		ns[i--] = c;
		seq /= 16;
	} while (seq);
	ns[i] = '~';

	/* Append the number */
	for (j = 0; j < i && dst[j] != ' '; j++) {
		if (IsDBCS1(dst[j])) {
			if (j == i - 1) break;
			j++;
		}
	}
	do {
		dst[j++] = (i < 8) ? ns[i++] : ' ';
	} while (j < 8);
}
#endif




/*-----------------------------------------------------------------------*/
/* Calculate sum of an SFN                                               */
/*-----------------------------------------------------------------------*/
#if _USE_LFN
static
BYTE sum_sfn (
	const BYTE *dir		/* Ptr to directory entry */
)
{
	BYTE sum = 0;
	UINT n = 11;

	do sum = (sum >> 1) + (sum << 7) + *dir++; while (--n);
	return sum;
}
#endif




/*-----------------------------------------------------------------------*/
/* Directory handling - Find an object in the directory                  */
/*-----------------------------------------------------------------------*/

static
FRESULT dir_find (
	DIR *dj			/* Pointer to the directory object linked to the file name */
)
{
	FRESULT res;
	BYTE c, *dir;
#if _USE_LFN
	BYTE a, ord, sum;
#endif

	res = dir_sdi(dj, 0);			/* Rewind directory object */
	if (res != FR_OK) return res;

#if _USE_LFN
	ord = sum = 0xFF;
#endif
	do {
		res = move_window(dj->fs, dj->sect);
		if (res != FR_OK) break;
		dir = dj->dir;					/* Ptr to the directory entry of current index */
		c = dir[DIR_Name];
		if (c == 0) { res = FR_NO_FILE; break; }	/* Reached to end of table */
#if _USE_LFN	/* LFN configuration */
		a = dir[DIR_Attr] & AM_MASK;
		if (c == 0xE5 || ((a & AM_VOL) && a != AM_LFN)) {	/* An entry without valid data */
			ord = 0xFF;
		} else {
			if (a == AM_LFN) {			/* An LFN entry is found */
				if (dj->lfn) {
					if (c & 0x40) {		/* Is it start of LFN sequence? */
						sum = dir[LDIR_Chksum];
						c &= 0xBF; ord = c;	/* LFN start order */
						dj->lfn_idx = dj->index;
					}
					/* Check validity of the LFN entry and compare it with given name */
					ord = (c == ord && sum == dir[LDIR_Chksum] && cmp_lfn(dj->lfn, dir)) ? ord - 1 : 0xFF;
				}
			} else {					/* An SFN entry is found */
				if (!ord && sum == sum_sfn(dir)) break;	/* LFN matched? */
				ord = 0xFF; dj->lfn_idx = 0xFFFF;	/* Reset LFN sequence */
				if (!(dj->fn[NS] & NS_LOSS) && !mem_cmp(dir, dj->fn, 11)) break;	/* SFN matched? */
			}
		}
#else		/* Non LFN configuration */
		if (!(dir[DIR_Attr] & AM_VOL) && !mem_cmp(dir, dj->fn, 11)) /* Is it a valid entry? */
			break;
#endif
		res = dir_next(dj, 0);		/* Next entry */
	} while (res == FR_OK);

	return res;
}




/*-----------------------------------------------------------------------*/
/* Read an object from the directory                                     */
/*-----------------------------------------------------------------------*/
#if _FS_MINIMIZE <= 1
static
FRESULT dir_read (
	DIR *dj			/* Pointer to the directory object that pointing the entry to be read */
)
{
	FRESULT res;
	BYTE c, *dir;
#if _USE_LFN
	BYTE a, ord = 0xFF, sum = 0xFF;
#endif

	res = FR_NO_FILE;
	while (dj->sect) {
		res = move_window(dj->fs, dj->sect);
		if (res != FR_OK) break;
		dir = dj->dir;					/* Ptr to the directory entry of current index */
		c = dir[DIR_Name];
		if (c == 0) { res = FR_NO_FILE; break; }	/* Reached to end of table */
#if _USE_LFN	/* LFN configuration */
		a = dir[DIR_Attr] & AM_MASK;
		if (c == 0xE5 || (!_FS_RPATH && c == '.') || ((a & AM_VOL) && a != AM_LFN)) {	/* An entry without valid data */
			ord = 0xFF;
		} else {
			if (a == AM_LFN) {			/* An LFN entry is found */
				if (c & 0x40) {			/* Is it start of LFN sequence? */
					sum = dir[LDIR_Chksum];
					c &= 0xBF; ord = c;
					dj->lfn_idx = dj->index;
				}
				/* Check LFN validity and capture it */
				ord = (c == ord && sum == dir[LDIR_Chksum] && pick_lfn(dj->lfn, dir)) ? ord - 1 : 0xFF;
			} else {					/* An SFN entry is found */
				if (ord || sum != sum_sfn(dir))	/* Is there a valid LFN? */
					dj->lfn_idx = 0xFFFF;		/* It has no LFN. */
				break;
			}
		}
#else		/* Non LFN configuration */
		if (c != 0xE5 && (_FS_RPATH || c != '.') && !(dir[DIR_Attr] & AM_VOL))	/* Is it a valid entry? */
			break;
#endif
		res = dir_next(dj, 0);				/* Next entry */
		if (res != FR_OK) break;
	}

	if (res != FR_OK) dj->sect = 0;

	return res;
}
#endif



/*-----------------------------------------------------------------------*/
/* Register an object to the directory                                   */
/*-----------------------------------------------------------------------*/
#if !_FS_READONLY
static
FRESULT dir_register (	/* FR_OK:Successful, FR_DENIED:No free entry or too many SFN collision, FR_DISK_ERR:Disk error */
	DIR *dj				/* Target directory with object name to be created */
)
{
	FRESULT res;
	BYTE c, *dir;
#if _USE_LFN	/* LFN configuration */
	WORD n, ne, is;
	BYTE sn[12], *fn, sum;
	WCHAR *lfn;


	fn = dj->fn; lfn = dj->lfn;
	mem_cpy(sn, fn, 12);

	if (_FS_RPATH && (sn[NS] & NS_DOT))		/* Cannot create dot entry */
		return FR_INVALID_NAME;

	if (sn[NS] & NS_LOSS) {			/* When LFN is out of 8.3 format, generate a numbered name */
		fn[NS] = 0; dj->lfn = 0;			/* Find only SFN */
		for (n = 1; n < 100; n++) {
			gen_numname(fn, sn, lfn, n);	/* Generate a numbered name */
			res = dir_find(dj);				/* Check if the name collides with existing SFN */
			if (res != FR_OK) break;
		}
		if (n == 100) return FR_DENIED;		/* Abort if too many collisions */
		if (res != FR_NO_FILE) return res;	/* Abort if the result is other than 'not collided' */
		fn[NS] = sn[NS]; dj->lfn = lfn;
	}

	if (sn[NS] & NS_LFN) {			/* When LFN is to be created, reserve an SFN + LFN entries. */
		for (ne = 0; lfn[ne]; ne++) ;
		ne = (ne + 25) / 13;
	} else {						/* Otherwise reserve only an SFN entry. */
		ne = 1;
	}

	/* Reserve contiguous entries */
	res = dir_sdi(dj, 0);
	if (res != FR_OK) return res;
	n = is = 0;
	do {
		res = move_window(dj->fs, dj->sect);
		if (res != FR_OK) break;
		c = *dj->dir;				/* Check the entry status */
		if (c == 0xE5 || c == 0) {	/* Is it a blank entry? */
			if (n == 0) is = dj->index;	/* First index of the contiguous entry */
			if (++n == ne) break;	/* A contiguous entry that required count is found */
		} else {
			n = 0;					/* Not a blank entry. Restart to search */
		}
		res = dir_next(dj, 1);		/* Next entry with table stretch */
	} while (res == FR_OK);

	if (res == FR_OK && ne > 1) {	/* Initialize LFN entry if needed */
		res = dir_sdi(dj, is);
		if (res == FR_OK) {
			sum = sum_sfn(dj->fn);	/* Sum of the SFN tied to the LFN */
			ne--;
			do {					/* Store LFN entries in bottom first */
				res = move_window(dj->fs, dj->sect);
				if (res != FR_OK) break;
				fit_lfn(dj->lfn, dj->dir, (BYTE)ne, sum);
				dj->fs->wflag = 1;
				res = dir_next(dj, 0);	/* Next entry */
			} while (res == FR_OK && --ne);
		}
	}

#else	/* Non LFN configuration */
	res = dir_sdi(dj, 0);
	if (res == FR_OK) {
		do {	/* Find a blank entry for the SFN */
			res = move_window(dj->fs, dj->sect);
			if (res != FR_OK) break;
			c = *dj->dir;
			if (c == 0xE5 || c == 0) break;	/* Is it a blank entry? */
			res = dir_next(dj, 1);			/* Next entry with table stretch */
		} while (res == FR_OK);
	}
#endif

	if (res == FR_OK) {		/* Initialize the SFN entry */
		res = move_window(dj->fs, dj->sect);
		if (res == FR_OK) {
			dir = dj->dir;
			mem_set(dir, 0, 32);		/* Clean the entry */
			mem_cpy(dir, dj->fn, 11);	/* Put SFN */
#if _USE_LFN
			dir[DIR_NTres] = *(dj->fn+NS) & (NS_BODY | NS_EXT);	/* Put NT flag */
#endif
			dj->fs->wflag = 1;
		}
	}

	return res;
}
#endif /* !_FS_READONLY */




/*-----------------------------------------------------------------------*/
/* Remove an object from the directory                                   */
/*-----------------------------------------------------------------------*/
#if !_FS_READONLY && !_FS_MINIMIZE
static
FRESULT dir_remove (	/* FR_OK: Successful, FR_DISK_ERR: A disk error */
	DIR *dj				/* Directory object pointing the entry to be removed */
)
{
	FRESULT res;
#if _USE_LFN	/* LFN configuration */
	WORD i;

	i = dj->index;	/* SFN index */
	res = dir_sdi(dj, (WORD)((dj->lfn_idx == 0xFFFF) ? i : dj->lfn_idx));	/* Goto the SFN or top of the LFN entries */
	if (res == FR_OK) {
		do {
			res = move_window(dj->fs, dj->sect);
			if (res != FR_OK) break;
			*dj->dir = 0xE5;			/* Mark the entry "deleted" */
			dj->fs->wflag = 1;
			if (dj->index >= i) break;	/* When reached SFN, all entries of the object has been deleted. */
			res = dir_next(dj, 0);		/* Next entry */
		} while (res == FR_OK);
		if (res == FR_NO_FILE) res = FR_INT_ERR;
	}

#else			/* Non LFN configuration */
	res = dir_sdi(dj, dj->index);
	if (res == FR_OK) {
		res = move_window(dj->fs, dj->sect);
		if (res == FR_OK) {
			*dj->dir = 0xE5;			/* Mark the entry "deleted" */
			dj->fs->wflag = 1;
		}
	}
#endif

	return res;
}
#endif /* !_FS_READONLY */




/*-----------------------------------------------------------------------*/
/* Pick a segment and create the object name in directory form           */
/*-----------------------------------------------------------------------*/

static
FRESULT create_name (
	DIR *dj,			/* Pointer to the directory object */
	const TCHAR **path	/* Pointer to pointer to the segment in the path string */
)
{
#ifdef _EXCVT
	static const BYTE excvt[] = _EXCVT;	/* Upper conversion table for extended chars */
#endif

#if _USE_LFN	/* LFN configuration */
	BYTE b, cf;
	WCHAR w, *lfn;
	UINT i, ni, si, di;
	const TCHAR *p;

	/* Create LFN in Unicode */
	si = di = 0;
	p = *path;
	lfn = dj->lfn;
	for (;;) {
		w = p[si++];					/* Get a character */
		if (w < ' ' || w == '/' || w == '\\') break;	/* Break on end of segment */
		if (di >= _MAX_LFN)				/* Reject too long name */
			return FR_INVALID_NAME;
#if !_LFN_UNICODE
		w &= 0xFF;
		if (IsDBCS1(w)) {				/* Check if it is a DBC 1st byte (always false on SBCS cfg) */
			b = (BYTE)p[si++];			/* Get 2nd byte */
			if (!IsDBCS2(b))
				return FR_INVALID_NAME;	/* Reject invalid sequence */
			w = (w << 8) + b;			/* Create a DBC */
		}
		w = ff_convert(w, 1);			/* Convert ANSI/OEM to Unicode */
		if (!w) return FR_INVALID_NAME;	/* Reject invalid code */
#endif
		if (w < 0x80 && chk_chr("\"*:<>\?|\x7F", w)) /* Reject illegal chars for LFN */
			return FR_INVALID_NAME;
		lfn[di++] = w;					/* Store the Unicode char */
	}
	*path = &p[si];						/* Return pointer to the next segment */
	cf = (w < ' ') ? NS_LAST : 0;		/* Set last segment flag if end of path */
#if _FS_RPATH
	if ((di == 1 && lfn[di-1] == '.') || /* Is this a dot entry? */
		(di == 2 && lfn[di-1] == '.' && lfn[di-2] == '.')) {
		lfn[di] = 0;
		for (i = 0; i < 11; i++)
			dj->fn[i] = (i < di) ? '.' : ' ';
		dj->fn[i] = cf | NS_DOT;		/* This is a dot entry */
		return FR_OK;
	}
#endif
	while (di) {						/* Strip trailing spaces and dots */
		w = lfn[di-1];
		if (w != ' ' && w != '.') break;
		di--;
	}
	if (!di) return FR_INVALID_NAME;	/* Reject nul string */

	lfn[di] = 0;						/* LFN is created */

	/* Create SFN in directory form */
	mem_set(dj->fn, ' ', 11);
	for (si = 0; lfn[si] == ' ' || lfn[si] == '.'; si++) ;	/* Strip leading spaces and dots */
	if (si) cf |= NS_LOSS | NS_LFN;
	while (di && lfn[di - 1] != '.') di--;	/* Find extension (di<=si: no extension) */

	b = i = 0; ni = 8;
	for (;;) {
		w = lfn[si++];					/* Get an LFN char */
		if (!w) break;					/* Break on end of the LFN */
		if (w == ' ' || (w == '.' && si != di)) {	/* Remove spaces and dots */
			cf |= NS_LOSS | NS_LFN; continue;
		}

		if (i >= ni || si == di) {		/* Extension or end of SFN */
			if (ni == 11) {				/* Long extension */
				cf |= NS_LOSS | NS_LFN; break;
			}
			if (si != di) cf |= NS_LOSS | NS_LFN;	/* Out of 8.3 format */
			if (si > di) break;			/* No extension */
			si = di; i = 8; ni = 11;	/* Enter extension section */
			b <<= 2; continue;
		}

		if (w >= 0x80) {				/* Non ASCII char */
#ifdef _EXCVT
			w = ff_convert(w, 0);		/* Unicode -> OEM code */
			if (w) w = excvt[w - 0x80];	/* Convert extended char to upper (SBCS) */
#else
			w = ff_convert(ff_wtoupper(w), 0);	/* Upper converted Unicode -> OEM code */
#endif
			cf |= NS_LFN;				/* Force create LFN entry */
		}

		if (_DF1S && w >= 0x100) {		/* Double byte char (always false on SBCS cfg) */
			if (i >= ni - 1) {
				cf |= NS_LOSS | NS_LFN; i = ni; continue;
			}
			dj->fn[i++] = (BYTE)(w >> 8);
		} else {						/* Single byte char */
			if (!w || chk_chr("+,;=[]", w)) {	/* Replace illegal chars for SFN */
				w = '_'; cf |= NS_LOSS | NS_LFN;/* Lossy conversion */
			} else {
				if (IsUpper(w)) {		/* ASCII large capital */
					b |= 2;
				} else {
					if (IsLower(w)) {	/* ASCII small capital */
						b |= 1; w -= 0x20;
					}
				}
			}
		}
		dj->fn[i++] = (BYTE)w;
	}

	if (dj->fn[0] == 0xE5) dj->fn[0] = 0x05;	/* If the first char collides with deleted mark, replace it with 0x05 */

	if (ni == 8) b <<= 2;
	if ((b & 0x0C) == 0x0C || (b & 0x03) == 0x03)	/* Create LFN entry when there are composite capitals */
		cf |= NS_LFN;
	if (!(cf & NS_LFN)) {						/* When LFN is in 8.3 format without extended char, NT flags are created */
		if ((b & 0x03) == 0x01) cf |= NS_EXT;	/* NT flag (Extension has only small capital) */
		if ((b & 0x0C) == 0x04) cf |= NS_BODY;	/* NT flag (Filename has only small capital) */
	}

	dj->fn[NS] = cf;	/* SFN is created */

	return FR_OK;


#else	/* Non-LFN configuration */
	BYTE b, c, d, *sfn;
	UINT ni, si, i;
	const char *p;

	/* Create file name in directory form */
	sfn = dj->fn;
	mem_set(sfn, ' ', 11);
	si = i = b = 0; ni = 8;
	p = *path;
#if _FS_RPATH
	if (p[si] == '.') { /* Is this a dot entry? */
		for (;;) {
			c = (BYTE)p[si++];
			if (c != '.' || si >= 3) break;
			sfn[i++] = c;
		}
		if (c != '/' && c != '\\' && c > ' ') return FR_INVALID_NAME;
		*path = &p[si];									/* Return pointer to the next segment */
		sfn[NS] = (c <= ' ') ? NS_LAST | NS_DOT : NS_DOT;	/* Set last segment flag if end of path */
		return FR_OK;
	}
#endif
	for (;;) {
		c = (BYTE)p[si++];
		if (c <= ' ' || c == '/' || c == '\\') break;	/* Break on end of segment */
		if (c == '.' || i >= ni) {
			if (ni != 8 || c != '.') return FR_INVALID_NAME;
			i = 8; ni = 11;
			b <<= 2; continue;
		}
		if (c >= 0x80) {				/* Extended char? */
			b |= 3;						/* Eliminate NT flag */
#ifdef _EXCVT
			c = excvt[c-0x80];			/* Upper conversion (SBCS) */
#else
#if !_DF1S	/* ASCII only cfg */
			return FR_INVALID_NAME;
#endif
#endif
		}
		if (IsDBCS1(c)) {				/* Check if it is a DBC 1st byte (always false on SBCS cfg) */
			d = (BYTE)p[si++];			/* Get 2nd byte */
			if (!IsDBCS2(d) || i >= ni - 1)	/* Reject invalid DBC */
				return FR_INVALID_NAME;
			sfn[i++] = c;
			sfn[i++] = d;
		} else {						/* Single byte code */
			if (chk_chr("\"*+,:;<=>\?[]|\x7F", c))	/* Reject illegal chrs for SFN */
				return FR_INVALID_NAME;
			if (IsUpper(c)) {			/* ASCII large capital? */
				b |= 2;
			} else {
				if (IsLower(c)) {		/* ASCII small capital? */
					b |= 1; c -= 0x20;
				}
			}
			sfn[i++] = c;
		}
	}
	*path = &p[si];						/* Return pointer to the next segment */
	c = (c <= ' ') ? NS_LAST : 0;		/* Set last segment flag if end of path */

	if (!i) return FR_INVALID_NAME;		/* Reject nul string */
	if (sfn[0] == 0xE5) sfn[0] = 0x05;	/* When first char collides with 0xE5, replace it with 0x05 */

	if (ni == 8) b <<= 2;
	if ((b & 0x03) == 0x01) c |= NS_EXT;	/* NT flag (Name extension has only small capital) */
	if ((b & 0x0C) == 0x04) c |= NS_BODY;	/* NT flag (Name body has only small capital) */

	sfn[NS] = c;		/* Store NT flag, File name is created */

	return FR_OK;
#endif
}




/*-----------------------------------------------------------------------*/
/* Get file information from directory entry                             */
/*-----------------------------------------------------------------------*/
#if _FS_MINIMIZE <= 1
static
void get_fileinfo (		/* No return code */
	DIR *dj,			/* Pointer to the directory object */
	FILINFO *fno	 	/* Pointer to the file information to be filled */
)
{
	UINT i;
	BYTE nt, *dir;
	TCHAR *p, c;


	p = fno->fname;
	if (dj->sect) {
		dir = dj->dir;
		nt = dir[DIR_NTres];		/* NT flag */
		for (i = 0; i < 8; i++) {	/* Copy name body */
			c = dir[i];
			if (c == ' ') break;
			if (c == 0x05) c = (TCHAR)0xE5;
			if (_USE_LFN && (nt & NS_BODY) && IsUpper(c)) c += 0x20;
#if _LFN_UNICODE
			if (IsDBCS1(c) && i < 7 && IsDBCS2(dir[i+1]))
				c = (c << 8) | dir[++i];
			c = ff_convert(c, 1);
			if (!c) c = '?';
#endif
			*p++ = c;
		}
		if (dir[8] != ' ') {		/* Copy name extension */
			*p++ = '.';
			for (i = 8; i < 11; i++) {
				c = dir[i];
				if (c == ' ') break;
				if (_USE_LFN && (nt & NS_EXT) && IsUpper(c)) c += 0x20;
#if _LFN_UNICODE
				if (IsDBCS1(c) && i < 10 && IsDBCS2(dir[i+1]))
					c = (c << 8) | dir[++i];
				c = ff_convert(c, 1);
				if (!c) c = '?';
#endif
				*p++ = c;
			}
		}
		fno->fattrib = dir[DIR_Attr];				/* Attribute */
		fno->fsize = LD_DWORD(dir+DIR_FileSize);	/* Size */
		fno->fdate = LD_WORD(dir+DIR_WrtDate);		/* Date */
		fno->ftime = LD_WORD(dir+DIR_WrtTime);		/* Time */
	}
	*p = 0;		/* Terminate SFN str by a \0 */

#if _USE_LFN
	if (fno->lfname && fno->lfsize) {
		TCHAR *tp = fno->lfname;
		WCHAR w, *lfn;

		i = 0;
		if (dj->sect && dj->lfn_idx != 0xFFFF) {/* Get LFN if available */
			lfn = dj->lfn;
			while ((w = *lfn++) != 0) {			/* Get an LFN char */
#if !_LFN_UNICODE
				w = ff_convert(w, 0);			/* Unicode -> OEM conversion */
				if (!w) { i = 0; break; }		/* Could not convert, no LFN */
				if (_DF1S && w >= 0x100)		/* Put 1st byte if it is a DBC (always false on SBCS cfg) */
					tp[i++] = (TCHAR)(w >> 8);
#endif
				if (i >= fno->lfsize - 1) { i = 0; break; }	/* Buffer overflow, no LFN */
				tp[i++] = (TCHAR)w;
			}
		}
		tp[i] = 0;	/* Terminate the LFN str by a \0 */
	}
#endif
}
#endif /* _FS_MINIMIZE <= 1 */




/*-----------------------------------------------------------------------*/
/* Follow a file path                                                    */
/*-----------------------------------------------------------------------*/

static
FRESULT follow_path (	/* FR_OK(0): successful, !=0: error code */
	DIR *dj,			/* Directory object to return last directory and found object */
	const TCHAR *path	/* Full-path string to find a file or directory */
)
{
	FRESULT res;
	BYTE *dir, ns;


#if _FS_RPATH
	if (*path == '/' || *path == '\\') { /* There is a heading separator */
		path++;	dj->sclust = 0;		/* Strip it and start from the root dir */
	} else {							/* No heading separator */
		dj->sclust = dj->fs->cdir;	/* Start from the current dir */
	}
#else
	if (*path == '/' || *path == '\\')	/* Strip heading separator if exist */
		path++;
	dj->sclust = 0;						/* Start from the root dir */
#endif

	if ((UINT)*path < ' ') {			/* Nul path means the start directory itself */
		res = dir_sdi(dj, 0);
		dj->dir = 0;

	} else {							/* Follow path */
		for (;;) {
			res = create_name(dj, &path);	/* Get a segment */
			if (res != FR_OK) break;
			res = dir_find(dj);				/* Find it */
			ns = *(dj->fn+NS);
			if (res != FR_OK) {				/* Failed to find the object */
				if (res != FR_NO_FILE) break;	/* Abort if any hard error occured */
				/* Object not found */
				if (_FS_RPATH && (ns & NS_DOT)) {	/* If dot entry is not exit */
					dj->sclust = 0; dj->dir = 0;	/* It is the root dir */
					res = FR_OK;
					if (!(ns & NS_LAST)) continue;
				} else {							/* Could not find the object */
					if (!(ns & NS_LAST)) res = FR_NO_PATH;
				}
				break;
			}
			if (ns & NS_LAST) break;			/* Last segment match. Function completed. */
			dir = dj->dir;						/* There is next segment. Follow the sub directory */
			if (!(dir[DIR_Attr] & AM_DIR)) {	/* Cannot follow because it is a file */
				res = FR_NO_PATH; break;
			}
			dj->sclust = LD_CLUST(dir);
		}
	}

	return res;
}




/*-----------------------------------------------------------------------*/
/* Load boot record and check if it is an FAT boot record                */
/*-----------------------------------------------------------------------*/

static
BYTE check_fs (	/* 0:The FAT BR, 1:Valid BR but not an FAT, 2:Not a BR, 3:Disk error */
	FATFS *fs,	/* File system object */
	DWORD sect	/* Sector# (lba) to check if it is an FAT boot record or not */
)
{
	if (disk_read(fs->drv, fs->win, sect, 1) != RES_OK)	/* Load boot record */
		return 3;
	if (LD_WORD(&fs->win[BS_55AA]) != 0xAA55)		/* Check record signature (always placed at offset 510 even if the sector size is >512) */
		return 2;

	if ((LD_DWORD(&fs->win[BS_FilSysType]) & 0xFFFFFF) == 0x544146)	/* Check "FAT" string */
		return 0;
	if ((LD_DWORD(&fs->win[BS_FilSysType32]) & 0xFFFFFF) == 0x544146)
		return 0;

	return 1;
}




/*-----------------------------------------------------------------------*/
/* Check if the file system object is valid or not                       */
/*-----------------------------------------------------------------------*/

static
FRESULT chk_mounted (	/* FR_OK(0): successful, !=0: any error occurred */
	const TCHAR **path,	/* Pointer to pointer to the path name (drive number) */
	FATFS **rfs,		/* Pointer to pointer to the found file system object */
	BYTE chk_wp			/* !=0: Check media write protection for write access */
)
{
	BYTE fmt, b, *tbl;
	UINT vol;
	DSTATUS stat;
	DWORD bsect, fasize, tsect, sysect, nclst, szbfat;
	WORD nrsv;
	const TCHAR *p = *path;
	FATFS *fs;

	/* Get logical drive number from the path name */
	vol = p[0] - '0';					/* Is there a drive number? */
	if (vol <= 9 && p[1] == ':') {		/* Found a drive number, get and strip it */
		p += 2; *path = p;				/* Return pointer to the path name */
	} else {							/* No drive number is given */
#if _FS_RPATH
		vol = CurrVol;					/* Use current drive */
#else
		vol = 0;						/* Use drive 0 */
#endif
	}

	/* Check if the logical drive is valid or not */
	if (vol >= _VOLUMES) 				/* Is the drive number valid? */
		return FR_INVALID_DRIVE;
	*rfs = fs = FatFs[vol];				/* Return pointer to the corresponding file system object */
	if (!fs) return FR_NOT_ENABLED;		/* Is the file system object available? */

	ENTER_FF(fs);						/* Lock file system */

	if (fs->fs_type) {					/* If the logical drive has been mounted */
		stat = disk_status(fs->drv);
		if (!(stat & STA_NOINIT)) {		/* and the physical drive is kept initialized (has not been changed), */
#if !_FS_READONLY
			if (chk_wp && (stat & STA_PROTECT))	/* Check write protection if needed */
				return FR_WRITE_PROTECTED;
#endif
			return FR_OK;				/* The file system object is valid */
		}
	}

	/* The logical drive must be mounted. */
	/* Following code attempts to mount a volume. (analyze BPB and initialize the fs object) */

	fs->fs_type = 0;					/* Clear the file system object */
	fs->drv = (BYTE)LD2PD(vol);			/* Bind the logical drive and a physical drive */
	stat = disk_initialize(fs->drv);	/* Initialize low level disk I/O layer */
	if (stat & STA_NOINIT)				/* Check if the initialization succeeded */
		return FR_NOT_READY;			/* Failed to initialize due to no media or hard error */
#if _MAX_SS != 512						/* Get disk sector size (variable sector size cfg only) */
	if (disk_ioctl(fs->drv, GET_SECTOR_SIZE, &fs->ssize) != RES_OK)
		return FR_DISK_ERR;
#endif
#if !_FS_READONLY
	if (chk_wp && (stat & STA_PROTECT))	/* Check disk write protection if needed */
		return FR_WRITE_PROTECTED;
#endif
	/* Search FAT partition on the drive. Supports only generic partitionings, FDISK and SFD. */
	fmt = check_fs(fs, bsect = 0);		/* Check sector 0 if it is a VBR */
	if (fmt == 1) {						/* Not an FAT-VBR, the disk may be partitioned */
		/* Check the partition listed in top of the partition table */
		tbl = &fs->win[MBR_Table + LD2PT(vol) * 16];	/* Partition table */
		if (tbl[4]) {									/* Is the partition existing? */
			bsect = LD_DWORD(&tbl[8]);					/* Partition offset in LBA */
			fmt = check_fs(fs, bsect);					/* Check the partition */
		}
	}
	if (fmt == 3) return FR_DISK_ERR;
	if (fmt) return FR_NO_FILESYSTEM;					/* No FAT volume is found */

	/* Following code initializes the file system object */

	if (LD_WORD(fs->win+BPB_BytsPerSec) != SS(fs))		/* (BPB_BytsPerSec must be equal to the physical sector size) */
		return FR_NO_FILESYSTEM;

	fasize = LD_WORD(fs->win+BPB_FATSz16);				/* Number of sectors per FAT */
	if (!fasize) fasize = LD_DWORD(fs->win+BPB_FATSz32);
	fs->fsize = fasize;

	fs->n_fats = b = fs->win[BPB_NumFATs];				/* Number of FAT copies */
	if (b != 1 && b != 2) return FR_NO_FILESYSTEM;		/* (Must be 1 or 2) */
	fasize *= b;										/* Number of sectors for FAT area */

	fs->csize = b = fs->win[BPB_SecPerClus];			/* Number of sectors per cluster */
	if (!b || (b & (b - 1))) return FR_NO_FILESYSTEM;	/* (Must be power of 2) */

	fs->n_rootdir = LD_WORD(fs->win+BPB_RootEntCnt);	/* Number of root directory entries */
	if (fs->n_rootdir % (SS(fs) / 32)) return FR_NO_FILESYSTEM;	/* (BPB_RootEntCnt must be sector aligned) */

	tsect = LD_WORD(fs->win+BPB_TotSec16);				/* Number of sectors on the volume */
	if (!tsect) tsect = LD_DWORD(fs->win+BPB_TotSec32);

	nrsv = LD_WORD(fs->win+BPB_RsvdSecCnt);				/* Number of reserved sectors */
	if (!nrsv) return FR_NO_FILESYSTEM;					/* (BPB_RsvdSecCnt must not be 0) */

	/* Determine the FAT sub type */
	sysect = nrsv + fasize + fs->n_rootdir / (SS(fs) / 32);	/* RSV+FAT+DIR */
	if (tsect < sysect) return FR_NO_FILESYSTEM;		/* (Invalid volume size) */
	nclst = (tsect - sysect) / fs->csize;				/* Number of clusters */
	if (!nclst) return FR_NO_FILESYSTEM;				/* (Invalid volume size) */
	fmt = FS_FAT12;
	if (nclst >= MIN_FAT16) fmt = FS_FAT16;
	if (nclst >= MIN_FAT32) fmt = FS_FAT32;

	/* Boundaries and Limits */
	fs->n_fatent = nclst + 2;							/* Number of FAT entries */
	fs->database = bsect + sysect;						/* Data start sector */
	fs->fatbase = bsect + nrsv; 						/* FAT start sector */
	if (fmt == FS_FAT32) {
		if (fs->n_rootdir) return FR_NO_FILESYSTEM;		/* (BPB_RootEntCnt must be 0) */
		fs->dirbase = LD_DWORD(fs->win+BPB_RootClus);	/* Root directory start cluster */
		szbfat = fs->n_fatent * 4;						/* (Required FAT size) */
	} else {
		if (!fs->n_rootdir)	return FR_NO_FILESYSTEM;	/* (BPB_RootEntCnt must not be 0) */
		fs->dirbase = fs->fatbase + fasize;				/* Root directory start sector */
		szbfat = (fmt == FS_FAT16) ?					/* (Required FAT size) */
			fs->n_fatent * 2 : fs->n_fatent * 3 / 2 + (fs->n_fatent & 1);
	}
	if (fs->fsize < (szbfat + (SS(fs) - 1)) / SS(fs))	/* (FAT size must not be less than FAT sectors */
		return FR_NO_FILESYSTEM;

#if !_FS_READONLY
	/* Initialize cluster allocation information */
	fs->free_clust = 0xFFFFFFFF;
	fs->last_clust = 0;

	/* Get fsinfo if available */
	if (fmt == FS_FAT32) {
	 	fs->fsi_flag = 0;
		fs->fsi_sector = bsect + LD_WORD(fs->win+BPB_FSInfo);
		if (disk_read(fs->drv, fs->win, fs->fsi_sector, 1) == RES_OK &&
			LD_WORD(fs->win+BS_55AA) == 0xAA55 &&
			LD_DWORD(fs->win+FSI_LeadSig) == 0x41615252 &&
			LD_DWORD(fs->win+FSI_StrucSig) == 0x61417272) {
				fs->last_clust = LD_DWORD(fs->win+FSI_Nxt_Free);
				fs->free_clust = LD_DWORD(fs->win+FSI_Free_Count);
		}
	}
#endif
	fs->fs_type = fmt;		/* FAT sub-type */
	fs->id = ++Fsid;		/* File system mount ID */
	fs->winsect = 0;		/* Invalidate sector cache */
	fs->wflag = 0;
#if _FS_RPATH
	fs->cdir = 0;			/* Current directory (root dir) */
#endif
#if _FS_SHARE				/* Clear file lock semaphores */
	clear_lock(fs);
#endif

	return FR_OK;
}




/*-----------------------------------------------------------------------*/
/* Check if the file/dir object is valid or not                          */
/*-----------------------------------------------------------------------*/

static
FRESULT validate (	/* FR_OK(0): The object is valid, !=0: Invalid */
	FATFS *fs,		/* Pointer to the file system object */
	WORD id			/* Member id of the target object to be checked */
)
{
	if (!fs || !fs->fs_type || fs->id != id)
		return FR_INVALID_OBJECT;

	ENTER_FF(fs);		/* Lock file system */

	if (disk_status(fs->drv) & STA_NOINIT)
		return FR_NOT_READY;

	return FR_OK;
}




/*--------------------------------------------------------------------------

   Public Functions

--------------------------------------------------------------------------*/



/*-----------------------------------------------------------------------*/
/* Mount/Unmount a Logical Drive                                         */
/*-----------------------------------------------------------------------*/

FRESULT f_mount (
	BYTE vol,		/* Logical drive number to be mounted/unmounted */
	FATFS *fs		/* Pointer to new file system object (NULL for unmount)*/
)
{
	FATFS *rfs;


	if (vol >= _VOLUMES)			/* Check if the drive number is valid */
		return FR_INVALID_DRIVE;
	rfs = FatFs[vol];				/* Get current fs object */

	if (rfs) {
#if _FS_SHARE
		clear_lock(rfs);
#endif
#if _FS_REENTRANT					/* Discard sync object of the current volume */
		if (!ff_del_syncobj(rfs->sobj)) return FR_INT_ERR;
#endif
		rfs->fs_type = 0;			/* Clear old fs object */
	}

	if (fs) {
		fs->fs_type = 0;			/* Clear new fs object */
#if _FS_REENTRANT					/* Create sync object for the new volume */
		if (!ff_cre_syncobj(vol, &fs->sobj)) return FR_INT_ERR;
#endif
	}
	FatFs[vol] = fs;				/* Register new fs object */

	return FR_OK;
}



