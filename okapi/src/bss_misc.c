/******************************************************************************

  bss_misc.c  SW May 92

  Misc BSS functions inc init and exit

******************************************************************************/

#include "bss.h"
#include "parms.h"
#include "bss_errors.h"

#ifndef NO_MMAP
#include <sys/mman.h>
#endif

/******************************************************************************
  
  bss_np

  Don't think this is used

  Oh yes it is...

  bss_ttf added Mar 2000 SW

******************************************************************************/

int
bss_np(int setnum)
{
  if ( Setrecs[setnum] == NULL ) return -1 ;
  else return Setrecs[setnum]->naw ;
}

int
bss_ttf(int setnum)
{
  if ( Setrecs[setnum] == NULL ) return -1 ;
  else return Setrecs[setnum]->ttf ;
}


/******************************************************************************

  check_setnum(num)

  Returns 1 if num is valid and in use, otherwise 0

  check_set_and_rec(setnum, recnum)

  Returns 1 if both OK, otherwise 0

******************************************************************************/

int
check_setnum(int num)
{
  if ( num >= 0 && num <= MAXSETRECS &&
      Setrecs[num] != NULL )
    return 1 ;
  else return 0 ;
}

static int
check_recnum(int setnum, int recnum)
{
  if ( recnum >= 0 && recnum < Setrecs[setnum]->naw )
    return 1 ;
  else return 0 ;
}

int
check_set_and_rec(int setnum, int recnum)
{
  if ( check_setnum(setnum) && check_recnum(setnum, recnum) )
    return 1 ;
  else return 0 ;
}

/******************************************************************************

  BSS file opening/closing, maintaining count of open files

******************************************************************************/

int
bss_open(char *path, int flags)
{
  int result ;

  if ( Num_open_files >= Maxfiles ) return -2 ;
  result = open(path, flags) ;
  if ( result >= 0 ) {
    Num_open_files++ ;
    if ( Dbg & D_FILES )
      fprintf(bss_syslog, "Opened %s, %d open files\n",
	    path, Num_open_files) ;
  }
  return result ;
}

FILE *
bss_fopen(char *filename, char *type)
{
  FILE *result ;

  if ( Num_open_files >= Maxfiles ) return NULL ;
  result = fopen(filename, type) ;
  if ( result != NULL ) {
    Num_open_files++ ;
    if ( Dbg & D_FILES )
      fprintf(bss_syslog, "Opened %s, %d open files\n",
	  filename, Num_open_files) ;
  }
  return result ;
}

int
bss_close(int fd)
{
  int result ;

  result = close(fd) ;
  if ( result == 0 ) {
    Num_open_files-- ;
    if ( Dbg & D_FILES )
      fprintf(bss_syslog, "Closed a file, %d open files\n",
	    Num_open_files) ;
  }
  return result ;
}

int
bss_fclose(FILE *f)
{
  int result ;

  result = fclose(f) ;
  if ( result == 0 ) {
    Num_open_files-- ;
    if ( Dbg & D_FILES )
      fprintf(bss_syslog, "Closed a file, %d open files\n",
	    Num_open_files) ;
  }
  return result ;
}


void
report_files(char *buf)
{
  sprintf(buf, "Files: %d open %d max\n", Num_open_files, Maxfiles) ;
}

#ifndef NO_SETITIMER
void
report_time(char *buf)
{
  sprintf(buf, "u: %.2f s: %.2f\n", get_utime(), get_stime()) ;
}
#endif

/******************************************************************************

  BSS memory allocation, recording at least amount allocated and freed

  The recording of memory only works properly if unmapping is never
  done implicitly, and unmapping is always done on a contiguous block
  of pages ending at the top of the mapped area.

  Might want to make detailed recording optional

******************************************************************************/

#define ALLOC_TABLE_SIZE 8192

int Allocated_mem = 0 ;
int Mapped_mem = 0 ;
struct alloc_table {
  int used ;
  struct {
    int size ;
    char *where ;
  } tab[ALLOC_TABLE_SIZE] ;
} Alloc_table ;		/* Temp, trials */
int Alloc_table_size = ALLOC_TABLE_SIZE ;

struct alloc_table Map_table ;

/******************************************************************************

  alloc_find_slot(ptr)

  If ptr = NULL finds free slot
  else looks for slot with "where"-value = ptr
  Returns slot number or -1

  March 96: mod so that finds if ptr in [where + size]

  Apr 99: new arg exact: if true only finds if ptr=where

******************************************************************************/

static int
alloc_find_slot(struct alloc_table *table, char *ptr, int exact)
{
  int j ;
  char *where ;
  BOOL found ;

  if ( ptr == NULL && table->used >= Alloc_table_size ) {
    if ( Dbg & D_MEM ) fprintf(bss_syslog, "Alloc_table full\n") ;
    return -1 ;
  }
  for ( j = 0, found = FALSE ; j < ALLOC_TABLE_SIZE ; j++ ) {
    if ( exact ) {
      if ( ptr == (where = table->tab[j].where) ) found = TRUE ;
    }
    else {
      if ( ptr >= (where = table->tab[j].where) &&
	   ptr <= where + table->tab[j].size ) found = TRUE ;
    }
    if ( found ) {
      if ( (Dbg & DD_MEM) && (unsigned)ptr > (unsigned)where )
	fprintf(bss_syslog,
		"alloc_find_slot(): ptr=%x, where=%x, where+size=%x\n",
		(unsigned)ptr, (unsigned)where,
		(unsigned)where + table->tab[j].size) ;
      break ;
    }
  }
  return (found)? j : -1 ;
}

void *
bss_malloc(size_t size)
{
  struct alloc_table *table ;
  int slot ;
  void *result = malloc(size) ;
  //if ( Dbg & D_MEM ) 
    //fprintf(bss_syslog, "malloc(): %d bytes at %x\n", size, (unsigned)result) ;
    //fprintf(stdout, "malloc(): %d bytes at %x\n", size, (unsigned)result) ; // mko
  if ( result != NULL ) {
    Allocated_mem += size ;
    if ( Dbg & D_RECORD_MEM ) {
      table = &Alloc_table ;
      slot = alloc_find_slot(table, NULL, 1) ;
      if ( slot >= 0 ) {
	table->tab[slot].size = size ;
	table->tab[slot].where = (char *)result ;
	table->used++ ;
	if ( Dbg & DD_MEM )
	  fprintf(bss_syslog, "malloc(): Alloc_table_free_slots=%d\n",
		  Alloc_table_size - table->used) ;
      }
    }
  }
  return result ;
}

#ifndef NO_MMAP

void *
bss_mmap(void *addr, int pages, int prot, int flags, int fd, OFFSET_TYPE offset, BOOL hold)
{
  void *result ;
  int ps = Pagesize ;
  struct alloc_table *table ;
  int slot ;
  if ( offset % ps ) {
    if ( Dbg & D_MEM )
#ifdef LARGEFILES
#ifdef _WIN32
      fprintf(bss_syslog,
	      "bss_mmap(): offset %I64d not a multiple of pagesize\n",
	      (LL_TYPE)offset) ;
#else
      fprintf(bss_syslog,
	      "bss_mmap(): offset %lld not a multiple of pagesize\n",
	      (LL_TYPE)offset) ;
#endif
#else
      fprintf(bss_syslog,
	      "bss_mmap(): offset %u not a multiple of pagesize\n",
	      (unsigned int)offset) ;
#endif
    return (void *)-1 ;
  }
  if ( (result =
	mmap(addr, (size_t)(pages * ps), prot, flags, fd, offset))
      != (void *)-1 ) {
#ifdef HAVE_MADVISE
    if ( ! hold ) madvise(result, pages * ps, MADV_SEQUENTIAL) ;
#endif
    Mapped_mem += pages * ps ;
    if ( Dbg & D_RECORD_MEM ) {
      table = &Map_table ;
      slot = alloc_find_slot(table, NULL, 0) ;
      table->tab[slot].size = pages * ps ;
      table->tab[slot].where = result ;
      table->used++ ;
    }
    if ( Dbg & D_MEM )
      fprintf(bss_syslog, "mapped %d pages at %x\n",
	      pages, (unsigned)result) ;
  }
  else if ( Dbg & D_MEM ) {
    fprintf(bss_syslog, "mapping of %d pages failed\n", pages) ;
    perror(NULL) ;
  }
  return result ;
}

#endif /* NO_MMAP */

void *
get_zeroed_mem(int mpages, BOOL hold)
{
  char *result ;
#if defined(NO_MMAP) || defined(NO_ZERO)
  result = (char *)bss_calloc(mpages, Pagesize) ;
  if ( result == NULL ) result = (char *)-1 ;
#else
  result =  bss_mmap((void *)0, mpages, PROT_READ|PROT_WRITE, MAP_SHARED,
		     stdzero, (OFFSET_TYPE) 0, hold) ;
#endif
  return result ;
}

char *
bss_strdup(char *ptr)
{
  char *result = bss_malloc(strlen(ptr) + 1) ;
  if ( result != NULL ) strcpy(result, ptr) ;
  return result ;
}
  
/******************************************************************************

  bss_strdup_kl(ptr, len) allocates storage for len + 1, copies len bytes into
  it and null-terminates (like strdup but doesn't assume termination)

  bss_memdup() is similar but doesn't terminate (so e.g. could be used for
  terminated strings if length is given as strlen() + 1)
  
******************************************************************************/


char *
bss_strdup_kl(char *ptr, size_t length)
{
  char *result = bss_malloc(length + 1) ;
  if ( result != NULL ) {
    memcpy(result, ptr, length) ;
    *(result + length) = '\0' ;
  }
  return result ;
}

void *
bss_memdup(void *ptr, size_t length)
{
  void *result = bss_malloc(length) ;
  if ( result != NULL ) memcpy(result, ptr, length) ;
  return result ;
}

void *
bss_calloc(size_t nelem, size_t elsize)
{
  void *ptr = bss_malloc(nelem * elsize) ;
  if ( ptr != NULL ) (void)memset(ptr, 0, nelem * elsize) ;
  return ptr ;
}

void *
bss_realloc(void *ptr, size_t size) 
{
  void *result ;
  int slot ;
  struct alloc_table *table = &Alloc_table ;
  result = realloc((void *)ptr, (size_t)size) ;
  if ( (Dbg & D_RECORD_MEM) && result != NULL ) {
    slot = alloc_find_slot(table, ptr, 1) ;
    if ( slot >= 0 ) {
      if ( Dbg & D_MEM ) 
	fprintf(stderr, "realloc(): was %d bytes at %x, now %d at %x\n",
		table->tab[slot].size, (unsigned int)table->tab[slot].where,
		size, (unsigned int)result) ;
      Allocated_mem += size - table->tab[slot].size ;
      table->tab[slot].size = size ;
      table->tab[slot].where = result ;
      if ( Dbg & DD_MEM )
	fprintf(bss_syslog, "realloc(): Alloc_table_free_slots=%d\n",
		Alloc_table_size - table->used) ;
    }
    else if ( Dbg & D_MEM )
      fprintf(bss_syslog,
	      "Trying to reallocate unallocated pointer %x\n",
	      (unsigned int)ptr) ;
  }
  return result ;
}

void
bss_free(void *ptr)
{
  int /* result, */ slot ;
  struct alloc_table *table ;
  if ( ptr == NULL ) return ;
  free((void *)ptr) ; /* Sys V free() doesn't return a value */
  {
    if ( (Dbg & D_RECORD_MEM) ) {
      table = &Alloc_table ;
      slot = alloc_find_slot(table, ptr, 1) ;
      if ( slot >= 0 ) {
	Allocated_mem -= table->tab[slot].size ;
	table->used-- ;
	if ( Dbg & D_MEM )
	  fprintf(bss_syslog, "freeing %d from %x\n",
		  table->tab[slot].size, (u_int)table->tab[slot].where) ;
	if ( Dbg & DD_MEM )
	  fprintf(bss_syslog,
		  "free(): freeing %d from slot %d, free_slots=%d, mem %d\n",
		  table->tab[slot].size, slot, Alloc_table_size - table->used,
		  Allocated_mem) ;
	table->tab[slot].size = 0 ;
	table->tab[slot].where = NULL ;
      }
      else if ( Dbg & D_MEM ) {
	fprintf(bss_syslog,
		"Trying to free unallocated memory at %x\n",
		(unsigned int)ptr) ;
      }
    }
  }
}

#ifndef NO_MMAP

int
bss_munmap(void *addr, int pages)
{
  int result ;
  int ps = Pagesize ;
  struct alloc_table *table = &Map_table ;
  int slot ;
  if ( (int)addr % ps ) {
    fprintf(bss_syslog,
	    "bss_munmap(): addr %x not a multiple of pagesize\n",
	    (unsigned)addr) ;
    return -1 ;
  }
  result = munmap((void *)addr, (size_t)(pages * ps)) ;
  if ( result == 0 ) {
    Mapped_mem -= pages * ps ;
    if ( Dbg & D_RECORD_MEM ) {
      slot = alloc_find_slot(table, addr, 0) ;
      if ( slot >= 0 ) {
	table->tab[slot].size -= pages * ps ;
	if ( table->tab[slot].size == 0 ) {
	  table->tab[slot].where = NULL ;
	  table->used-- ;
	}
      }
    }
    if ( Dbg & D_MEM )
      fprintf(bss_syslog, "Unmapped %d pages at %x: result %d\n",
	      pages, (unsigned)addr, result) ;
  }
  return result ;
}

#endif

/******************************************************************************

  report_allocation 

******************************************************************************/
    
void
report_allocation(char *buf)
{

  if ( Dbg & D_RECORD_MEM ) {
    sprintf(buf,
	    "Allocated memory: total %d in %d parcels\n",
	    Allocated_mem, Alloc_table.used) ;
    sprintf(buf + strlen(buf),
	    "Mapped memory: total %d (%d pages) in %d parcels\n",
	    Mapped_mem, Mapped_mem/Pagesize, Map_table.used) ;
  }
  else {
    sprintf(buf,
	    "Allocated memory: total %d\n",
	    Allocated_mem) ;
    sprintf(buf + strlen(buf),
	    "Mapped memory: total %d (%d pages)\n",
	    Mapped_mem, Mapped_mem/Pagesize) ;
  }
}

