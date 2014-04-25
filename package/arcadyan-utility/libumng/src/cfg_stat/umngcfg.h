/*
 * manipulate user space management configuration
 *
 * Copyright 2009, Arcadyan Corporation
 * All Rights Reserved.
 *
 */

#ifndef _umngcfg_h_
#define _umngcfg_h_

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/stat.h>
//#include <linux/limits.h>
#include <arpa/inet.h>

#define SECT_HASH
#ifdef SECT_HASH
#define SECT_NUM_PER_HASH_ENTRY     16
#define SECT_HASH_TBL_MASK          0x3f
#define SECT_HASH_TBL_SZ            64
#endif

#define NO_SECTION  "DFT_SECTION"

#define UMNGCFG_MAX_SECTION_NUM		256
#define UMNGCFG_MAX_OPEN_INTERVAL	100	/* 100 msec */
#define UMNGCFG_MAX_OPEN_TRIAL		100	/* try up to 100 times */

#define UMNGCFG_NO_LCK  0
#define UMNGCFG_RD_LCK  1
#define UMNGCFG_WR_LCK  2

#define UMNGCFG_MATCH_EXACT				0x0
#define UMNGCFG_MATCH_PREFIX			0x1
#define UMNGCFG_MATCH_INFIX				0x2
#define UMNGCFG_MATCH_POSTFIX			0x3
#define UMNGCFG_MATCH_SPC_FIRST			0x4	/*section is prior to name, name is prior to value*/
#define UMNGCFG_MATCH_SPC_LAST			0x8	/*section is prior to name, name is prior to value*/

#define UMNGCFG_GET_FIRST				0
#define UMNGCFG_GET_NEXT				1
#define UMNGCFG_GET_LAST				2
#define UMNGCFG_SORT_RAW				0
#define UMNGCFG_SORT_NAME				1
#define UMNGCFG_IMPORT_REPLACE			0
#define UMNGCFG_IMPORT_MERGE			1
#define UMNGCFG_EXPORT_RAW				0
#define UMNGCFG_EXPORT_ENCODEBYLIST		1

#define UMNGCFG_MATCH_SECT_MASK			0x0f00
#define UMNGCFG_MATCH_SECT_TYPE_MASK	0x0300
#define UMNGCFG_MATCH_SECT_SPC_MASK		0x0c00
#define UMNGCFG_MATCH_SECT_SHIFT		8
#define UMNGCFG_MATCH_SECT_TYPE(t)		( ( (t) & UMNGCFG_MATCH_SECT_TYPE_MASK ) >> UMNGCFG_MATCH_SECT_SHIFT )
#define UMNGCFG_MATCH_SECT_SPC(t)		( ( (t) & UMNGCFG_MATCH_SECT_SPC_MASK ) >> UMNGCFG_MATCH_SECT_SHIFT )
#define UMNGCFG_MATCH_NAME_MASK			0x00f0
#define UMNGCFG_MATCH_NAME_TYPE_MASK	0x0030
#define UMNGCFG_MATCH_NAME_SPC_MASK		0x00c0
#define UMNGCFG_MATCH_NAME_SHIFT		4
#define UMNGCFG_MATCH_NAME_TYPE(t)		( ( (t) & UMNGCFG_MATCH_NAME_TYPE_MASK ) >> UMNGCFG_MATCH_NAME_SHIFT )
#define UMNGCFG_MATCH_NAME_SPC(t)		( ( (t) & UMNGCFG_MATCH_NAME_SPC_MASK ) >> UMNGCFG_MATCH_NAME_SHIFT )
#define UMNGCFG_MATCH_VAL_MASK			0x000f
#define UMNGCFG_MATCH_VAL_TYPE_MASK		0x0003
#define UMNGCFG_MATCH_VAL_SPC_MASK		0x000c
#define UMNGCFG_MATCH_VAL_SHIFT			0
#define UMNGCFG_MATCH_VAL_TYPE(t)		( ( (t) & UMNGCFG_MATCH_VAL_TYPE_MASK ) >> UMNGCFG_MATCH_VAL_SHIFT )
#define UMNGCFG_MATCH_VAL_SPC(t)		( ( (t) & UMNGCFG_MATCH_VAL_SPC_MASK ) >> UMNGCFG_MATCH_VAL_SHIFT )
#define UMNGCFG_MATCH_TYPE( sectType, nameType, valType ) \
										(  ( ( (sectType) << UMNGCFG_MATCH_SECT_SHIFT ) & UMNGCFG_MATCH_SECT_MASK ) \
										 | ( ( (nameType) << UMNGCFG_MATCH_NAME_SHIFT ) & UMNGCFG_MATCH_NAME_MASK ) \
										 | ( ( (valType ) << UMNGCFG_MATCH_VAL_SHIFT  ) & UMNGCFG_MATCH_VAL_MASK  ) )
#define UMNGCFG_MATCH_SPC(t)			( (t) & (UMNGCFG_MATCH_SECT_SPC_MASK | UMNGCFG_MATCH_NAME_SPC_MASK | UMNGCFG_MATCH_VAL_SPC_MASK) )

typedef struct umngcfg_tuple {
	char *name;
	char *value;
	struct umngcfg_tuple *next;
	char *comment;
} umngcfg_tuple_t;

typedef struct umngcfg_handle {
	int fd;
	int lock_type;
	char *mmap;
	unsigned long length;
	int section_num;
	char *section[UMNGCFG_MAX_SECTION_NUM];
	char *comment[UMNGCFG_MAX_SECTION_NUM];
	struct umngcfg_tuple *umngcfg_section[UMNGCFG_MAX_SECTION_NUM];
#ifdef SECT_HASH
    int sect_hash[SECT_HASH_TBL_SZ][SECT_NUM_PER_HASH_ENTRY];
#endif
} umngcfg_handle_t;

// open umngcfg config file and obtain a handle
umngcfg_handle_t * umngcfg_open(const char *file, int lock_type);

char*	umngcfg_get_secname(umngcfg_handle_t *h, int sec_idx);
int		umngcfg_get_secidx(umngcfg_handle_t *h, const char *section);
int		umngcfg_get_varnum(umngcfg_handle_t *h, int sec_idx);
char*	umngcfg_get_varname(umngcfg_handle_t *h, int sec_idx, int var_idx);
int		umngcfg_get_varidx(umngcfg_handle_t *h, int sec_idx, char* var);

// set the value of a umngcfg variable
int		umngcfg_set(umngcfg_handle_t *h, const char *section, const char *name, const char *value);
int		umngcfg_set_by_idx(umngcfg_handle_t *h, const char *section, const char *name, const char *value, const int idx);

// get the value of a umngcfg variable, return NULL if it it is not found
char*	umngcfg_get(umngcfg_handle_t *h, const char *section, const char *name);
char*	umngcfg_get_by_idx(umngcfg_handle_t *h, const char *section, const char *name, const int idx);
int		umngcfg_get_cnt(umngcfg_handle_t *h, const char *section);
char*	umngcfg_get_entry(umngcfg_handle_t *h, const char *section, char *name, int nameSz, int which, int sortType);
char*	umngcfg_get_raw_entry(umngcfg_handle_t *h, const char *section, char *name, int nameSz, int which);
char*	umngcfg_get_first(umngcfg_handle_t *h, const char *section, char *name, int nameSz);
char*	umngcfg_get_next(umngcfg_handle_t *h, const char *section, char *name, int nameSz);
char*	umngcfg_get_last(umngcfg_handle_t *h, const char *section, char *name, int nameSz);
char*	umngcfg_get_sort_entry(umngcfg_handle_t *h, const char *section, char *name, int nameSz, int which);
char*	umngcfg_get_sort_first(umngcfg_handle_t *h, const char *section, char *name, int nameSz);
char*	umngcfg_get_sort_next(umngcfg_handle_t *h, const char *section, char *name, int nameSz);
char*	umngcfg_get_sort_last(umngcfg_handle_t *h, const char *section, char *name, int nameSz);

// get the value of a umngcfg variable, return caller-assigned default value "dftVal" if it it is not found
char*	umngcfg_get_with_dft(umngcfg_handle_t *h, const char *section, const char *name, const char *dftVal);
char*	umngcfg_get_by_idx_with_dft(umngcfg_handle_t *h, const char *section, const char *name, const int idx, const char *dftVal);

// remove a umngcfg variable
int		umngcfg_unset(umngcfg_handle_t *h, const char *section, const char *name);

// remove multiple umngcfg variables by partial name and type (prefix - name*, infix - *name, post - *name).
//      if "name" is null or empty string, all variables of a section will be removed
int		umngcfg_unset_multi(umngcfg_handle_t *h, const char *section, const char *partName, const int matchType);

// duplicate a umngcfg section
int		umngcfg_dup(umngcfg_handle_t *h, const char *dstSect, const char *srcSect);

// search a umngcfg variable
int		umngcfg_search(umngcfg_handle_t *h, char *section, char *name, char *value, int matchType);

// find an unused umngcfg variable
int		umngcfg_find_unused(umngcfg_handle_t *h, char *section, char *name, int iFrom);

// get all umngcfg variables
umngcfg_tuple_t * umngcfg_getall(umngcfg_handle_t *h);

// commit changes
int		umngcfg_commit(umngcfg_handle_t *h);

// close umngcfg config file and free memory
int		umngcfg_close(umngcfg_handle_t *h);

// get the value of a umngcfg variable in a safe way, use "" instead of NULL
#define umngcfg_safe_get(h, section, name) (umngcfg_get(h, section, name) ? : "")

#define UMNGCFG_INT8_TYPE      0
#define UMNGCFG_UINT8_TYPE     1
#define UMNGCFG_INT16_TYPE      2
#define UMNGCFG_UINT16_TYPE     3
#define UMNGCFG_INT32_TYPE      4
#define UMNGCFG_UINT32_TYPE     5
#define UMNGCFG_INET4_TYPE      6
#define UMNGCFG_INET6_TYPE      7

// get type-converted value of a variable
int umngcfg_get_by_type(umngcfg_handle_t *h, const char *section, const char *name, void *value, int type);

// set value of a variable by type
int umngcfg_set_by_type(umngcfg_handle_t *h, const char *section, const char *name, void *value, int type);


int   umngcfg_get_secnum(umngcfg_handle_t *h);
char* umngcfg_get_secname(umngcfg_handle_t *h, int sec_idx);
int   umngcfg_get_secidx(umngcfg_handle_t *h, const char *section);
int   umngcfg_get_varnum(umngcfg_handle_t *h, int sec_idx);
char* umngcfg_get_varname(umngcfg_handle_t *h, int sec_idx, int var_idx);
int   umngcfg_get_varidx(umngcfg_handle_t *h, int sec_idx, char* var);
int   umngcfg_import( umngcfg_handle_t *h, char* srcCfg, int iType, char* imList);

int umngcfg_delete(umngcfg_handle_t *h, const char *DelList);
int umngcfg_commit_whitelist(umngcfg_handle_t *h);
int umngcfg_merge_whitelist(umngcfg_handle_t *h, const char *SrcWL);
int	umngcfg_config_filter(umngcfg_handle_t *h, const char* ruleList, const char *predef_rule, const unsigned int val_len_lmt);

int	umngcfg_get_whole_section(umngcfg_handle_t *h, const char *section, void *data_ptr, void *mem_bound);

int   indexOfC(char* str, const char delimiter, const int num);
int   getListSize(char *srcList, char delimiter);
int   getStrInList(char *srcList, char delimiter, int idxItem, char* str, int sizeStr);
void  setStrInList(char *srcList, int size, const char delimiter, const int idxItem, char* str);
void  setItemInList(char *srcList, int size, char delimiter, int idxItem, int item);
int   insertItemInList(char *srcList, int size, char delimiter, int idxItem, int item);
int   delItemInList(char *srcList, char delimiter, int idxItem);
int   getItemInList(char *srcList, char delimiter, int idxItem, int retdef);
int   isListMatch(char *listA, char delimiter, void *listB, int listBtype, int num);
int   copyListToArray(void *dstlist, const int dstlisttype, char *srclist, char delimiter, int num);
int   copyArrayToList(char *dstlist, const char delimiter, void * srcA, const int srcAtype, const int sizeA, const int startPos, const int num, int base);


#endif // _umngcfg_h_
