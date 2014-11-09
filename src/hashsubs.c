/******************************************************************************

  hashsubs.c  SW Jan 02

  Routines for supporting fast filtering-type lookup, etc

******************************************************************************/

#include "bss.h"
#include "bss_errors.h"
/*#include "hash.h"*/ /* Now included by defines.h */

IFN Hash_fns[] = {
  { bitwisehash, "bitwisehash" },
  { bwhashpwr2, "bwhashpwr2" },
  { bwhash_kl, "bwhash_kl" },
  { bwhashpwr2_kl, "bwhashpwr2_kl" },
  { NULL, NULL },
} ;

/******************************************************************************

  make_hastab() self explanatory

  All types of record start with (void *)next followed by (void *)term, which
  is always the key. For all types except HASH_GEN the recsize and hence the
  amount allocated by hash_search is the size of the corresponding record
  type. For HASH_GEN the record size is given as the fifth arg.

******************************************************************************/


Hashtab *
make_hashtab(Hash_type type, int nelem, char *hashfuncname, u_int hashseed,
	     int recsize)
{
  Hashtab *table ;
  int i ;
  char *func = "make_hashtab" ;
  for ( i = 0 ;
	Hash_fns[i].name != NULL && strcasecmp(Hash_fns[i].name, hashfuncname);
	i++ )
    if ( Hash_fns[i].name == NULL ) {
      fprintf(stderr, "Can't find hash function %s\n", hashfuncname) ;
      return NULL ;
    }
  if ( strcasecmp(hashfuncname, "bwhashpwr2") == 0 &&
       ! ispower2(nelem) ) {
    fprintf(stderr,
	    "For hash function bwhashpwr2 table size must be power of 2\n") ;
    return NULL ;
  }
  /* If some kind of hash type */
  table = (Hashtab *)bss_calloc(1, sizeof(Hashtab)) ;
  if ( table == NULL ) return NULL ;
  table->tab = (Genrec **)bss_calloc(nelem, sizeof(Genrec *)) ;
  if ( table->tab == NULL ) return NULL ;
  switch (type) {
    /* NB recsize ignored unless HASH_GEN */
  case HASH_GEN:
    table->recsize = recsize ;
    break ;
  case HASH_MIN:
    table->recsize = sizeof(Genrec) ;
    break;
  case HASH_MED:
    table->recsize = sizeof(Med_termrec) ;
    break;
  case HASH_SPEC1:
    table->recsize = sizeof(Spec1_termrec) ;
    break;
  case HASH_FULL:
    table->recsize = sizeof(Full_termrec) ;
    break ;
  default:
    table->recsize = recsize ;
    break;
  }
  table->tabtype = type ;
  table->tabsize = nelem ;
  table->funcname = bss_strdup(hashfuncname) ;
  table->hashfn = Hash_fns[i].func ;
  table->seed = hashseed ;
  table->n_occ = table->n_keys = table->depth_sum = 0 ;
  table->max_depth = table->n_comps = 0 ;
  table->last_index = -1 ;
  table->last_depth = 0 ;
  return table ;
}  

/******************************************************************************

  Memory recovery only for "fully malloc'd" tables of termrecs
  (each record and each term is in separately allocated storage).

  Note free_tab() may only work for hash tables at present (Feb 02)

******************************************************************************/

void
free_tab(Hashtab *table) {
  Genrec *vtmp, *vnx ;
  int i ;
  for ( i = 0 ; i < table->tabsize ; i++ ) {
    vnx = (Genrec *)table->tab[i] ;
    while ( vnx != NULL ) {
      vtmp = vnx ;
      vnx = vnx->next ;
      bss_free(vtmp->term) ;
      bss_free(vtmp) ;
    }
  }
  bss_free(table->tab) ;
  bss_free(table->funcname) ;
  bss_free(table) ;
}

/* Author J. Zobel, April 2001.
   Permission to use this code is freely granted, provided that this
   statement is retained. */

/* Bitwise hash functions.  Note that table size does not have to be prime. 
   bwhashpwr2 only works if table size is integral power of 2 (SW).
   The length arg must be present but is ignored by bitwisehash() and
   bwhashpwr2() */
int
bitwisehash(char *word, int dummy, Hashtab *tab)
{
    char c;
    u_int h = tab->seed ;

    for ( ; ( c = *word ) != '\0' ; word++ ) h ^= ( (h << 5) + c + (h >> 2) );
    return(((h & 0x7fffffff) % tab->tabsize));
}

int
bwhashpwr2
(char *word, int dummy, Hashtab *tab)
{
  int c;
  u_int h = tab->seed ;
  
  for ( ; (c = *word) ; word++ ) h ^= ( (h << 5) + c + (h >> 2) );
  return(((h & 0x7fffffff) & (tab->tabsize - 1))) ;
}

int
bwhash_kl(char *word, int l, Hashtab *tab)
{
  char c;
  u_int h = tab->seed ;
  
  while ( l-- ) {
    c = *word++ ;
    h ^= ( (h << 5) + c + (h >> 2) );
  }
  return(((h & 0x7fffffff) % tab->tabsize));
}

int
bwhashpwr2_kl(char *word, int l, Hashtab *tab)
{
  char c;
  u_int h = tab->seed ;
  while ( l-- ) {
  c = *word++ ;
  h ^= ( (h << 5) + c + (h >> 2) );
  }
  return(((h & 0x7fffffff) & (tab->tabsize - 1))) ;
}


/******************************************************************************

  If action is H_ADD hash_search will add a record if term is not present
  and in any case return the record.

  If action is H_SEARCH it returns NULL if term is not present, otherwise
  the record.

  For either action the record for a term already present is moved to the top
  of its slot.

  NULL is returned if memory cannot be allocated.

  if new_rec is true tnp is incremented by 1 if HASH_ADD and term is
  already present (and the record type has an np field).

  Terms are stored as terminated strings but may be offered as known-length
  strings. If terminated, the tl arg should be negative.

  On test, storing 235000 types, 14500000 tokens, was about 10%-20% slower
  with input strings treated as known length.

  July 02: added H_DELETE

******************************************************************************/

void *
hash_search(Hashtab *table, char *term, int tl, Hash_action action,
	    BOOL new_rec) {

  Genrec *vprv, *vtmp ;
  Med_termrec *mt ;
  u_int hval ;
  int depth = 0 ;
  int (*sfunc)() ;
  char * (*dupfunc)() ;
  if ( tl < 0 ) {
    sfunc = strcmp ;
    dupfunc = bss_strdup ;
  }
  else {
    sfunc = cptsks ;
    dupfunc = bss_strdup_kl ;
  }
  hval = table->hashfn(term, tl, table) ;
  for ( vprv = NULL, vtmp = table->tab[hval] ;
	vtmp != NULL && sfunc(vtmp->term, term, tl) != 0 ;
	vprv = vtmp, vtmp = vtmp->next ) depth++ ;
  if ( vtmp == NULL ) {		/* Not found */
    if ( action == H_ADD ) {
      vtmp = (Genrec *)bss_calloc(1, table->recsize) ;
      if ( vtmp == NULL ) return NULL ;
      table->dmem += table->recsize ;
      vtmp->term = dupfunc(term, tl) ;
      if ( vtmp->term == NULL ) return NULL ;
      table->tmem += strlen(vtmp->term) + 1 ;
      switch (table->tabtype) {
      case HASH_GEN:
      case HASH_MIN:
	break ;
      case HASH_MED:
      case HASH_SPEC1:
      case HASH_FULL:
	mt = (Med_termrec *)vtmp ;
	mt->u.tnp = 1 ;
	mt->ttf = 1 ;
	break ;
      default:
	break ;
      }
      if ( vprv == NULL ) {	/* Inserted at top level */
	table->tab[hval] = vtmp ;
	table->n_occ++ ;
      }
      else {
	vprv->next = vtmp ;
	table->depth_sum += depth ;
	if ( depth > table->max_depth ) table->max_depth = depth ;
      }
      table->n_keys++ ;
      return vtmp ;
    } /* (H_ADD) */
    else /* H_SEARCH */ return NULL ;
  }
  else {			/* Already in table */
    if ( action == H_ADD ) {
      switch (table->tabtype) {
      case HASH_GEN:
      case HASH_MIN:
	break ;
      case HASH_MED:
      case HASH_SPEC1:
      case HASH_FULL:
	mt = (Med_termrec *)vtmp ;
	mt->ttf++ ;
	if ( new_rec ) mt->u.tnp++ ;
	break ;
      }
    } /* (H_ADD) */
    else if ( action == H_DELETE ) {
      if ( vprv != NULL ) vprv->next = vtmp->next ;
      else table->tab[hval] = vtmp->next ;
      table->tmem -= strlen(vtmp->term) ;
      bss_free(vtmp->term) ;
      table->dmem -= table->recsize ;
      bss_free(vtmp) ;
      table->n_keys-- ;
      if ( vprv == NULL ) table->n_occ-- ;
      table->depth_sum -= depth ;
    } /* H_DELETE */
    else {			/* H_SEARCH */
      if ( vprv != NULL ) {	/* Not top of list, then move to top */
	vprv->next = vtmp->next ; vtmp->next = table->tab[hval] ;
	table->tab[hval] = vtmp ;
      }
    }
    return vtmp ;
  }
}

/******************************************************************************

  traverse_hash(hashtab)  SW May 02

  Get next record from hash table. Returns record or NULL if no more.
  Initially, the lastindex field must be set to -1. It is maintained
  by this function.

******************************************************************************/

void *
traverse_hash(Hashtab *table)
{
  int i ;
  Genrec *gr ;
  if ( table->last_index >= table->tabsize ) return (void *)NULL ;
  if ( table->last_index >= 0 ) { /* It's initialised to -1 */
    gr = (Genrec *)table->tab[table->last_index] ;
    for ( i = 0 ; i <= table->last_depth ; i++ ) gr = gr->next ;
    if ( gr != NULL ) {
      table->last_depth++ ;
      return (void *)gr ;
    }
  }
  table->last_index++ ; table->last_depth = 0 ;
  while ( table->last_index < table->tabsize &&
	  table->tab[table->last_index] == NULL )
    table->last_index++ ;
  if ( table->last_index >= table->tabsize ) return (void *)NULL ; 
  else return (void *)table->tab[table->last_index] ;
}
  

void *
hash_add_itr(Hashtab *table, Transfer_itr *tr)
     /* There is no check for duplicates, simply inserts */
{
  Genrec *vprv, *vtmp ;
  Med_termrec *mt ;
  Full_termrec *ft ;
  u_int hval ;
  int depth = 0 ;

  hval = table->hashfn(tr->term, tr->termlen, table) ;
  for ( vprv = NULL, vtmp = table->tab[hval] ;
	vtmp != NULL ;
	vprv = vtmp, vtmp = vtmp->next )
    depth++ ;
  vtmp = bss_calloc(1, table->recsize) ;
  if ( vtmp == NULL ) return vtmp ;
  table->dmem += table->recsize ;
  /* Copy term as terminated string to malloc'd storage */
  vtmp->term = bss_strdup_kl(tr->term, tr->termlen) ;
  if ( vtmp->term == NULL ) return NULL ;
  table->tmem += tr->termlen + 1 ;
  switch (table->tabtype) {
  case HASH_MED:
  case HASH_SPEC1:
    mt = (Med_termrec *)vtmp ;
    mt->ttf = tr->ttf ;
    mt->u.tnp = tr->tnp ;
    break ;
  case HASH_FULL:
    ft = (Full_termrec *)vtmp ;
    ft->ttf = tr->ttf ;
    ft->tnp = tr->tnp ;
    ft->maxtf = tr->maxtf ;
    ft->limits = tr->limits ;
    ft->pstglen = tr->plength ;
    ft->pstg_vol = tr->pstg_vol ;
    ft->pstg_offset = tr->pstg_offset ;
    break ;
  default:			/* Only the term */
    break ;
  }
  if ( vprv == NULL ) {
    table->tab[hval] = vtmp ;
    table->n_occ++ ;
  }
  else {
    vprv->next = vtmp ;
    table->depth_sum += depth ;
    if ( depth > table->max_depth ) table->max_depth = depth ;
  }
  table->n_keys++ ;
  return vtmp ;
}

Hashtab *
hash_si(Hash_type hash_type, int nelem, char *hashfuncname,
	int itype, FILE *sifile, int *result)
{
  Hashtab *table ;
  Transfer_itr tr ;
  void *hashres ;
  char buf[512] ;

  static char *func = "hash_si" ;
  table = make_hashtab(hash_type, nelem, hashfuncname, HASHSEED, 0) ;
  if ( table == NULL ) {
    fprintf(stderr, "%s(): can't allocate memory for hash table\n", func) ;
    *result = -3 ;
    return NULL ;
  }
  /* Run off 1st two bytes of file */
  getc(sifile) ; getc(sifile) ;
  while ( read_itr_seq(buf, itype, sifile) == 1 ) {
    (void)parse_itr(buf, itype, &tr) ;
    hashres = hash_add_itr(table, &tr) ;
    if ( hashres == NULL ) {
      fprintf(stderr, "%s(): can't allocate memory for ITR\n", func) ;
      *result = -3 ;
      return NULL ;
    }
  }
  if ( Dbg & D_HASH ) 
    fprintf(bss_syslog,
	    "keys %d occ %d depth sum %d max depth %d tmem %d dmem %d\n",
	    table->n_keys, table->n_occ, table->depth_sum, table->max_depth,
	    table->tmem, table->dmem) ;
  *result = 0 ;
  return table ;
}

Hashtab *
hash_ix(int nelem, char *hashfuncname,
	int in_type, int out_type,
	char *si_name, int chunksize, int numvols, char **oi_names,
	int *result)
{
  FILE *oi, *si ;
  int lastpvol = -1 ;		/* Dummy starter */
  int irn = 0 ;
  int pvol ;
  OFFSET_TYPE ppos ;
  unsigned int pos ;
  BOOL eosi = FALSE, eochunk = TRUE ;
  void *sibuf ;
  int rres ;
  int errc = 0 ;
  int numchunks = 0, numterms = 0 ;
  LL_TYPE numpstgs = 0, numposes = 0 ;
  int tnp ;
  unsigned short tf ;
  int length ;
  char term[MAXEXITERMLEN] ;
  BOOL convert34 = FALSE, convert43 = FALSE ;
  int i, j ;

  Hashtab *table ;
  Transfer_itr tr ;
  Mitr *itr ;
  void *pstgs ;
  struct mpstg_20 *p20 ;
  struct mpstg_10 *p10 ;
  struct mpstg_8 *p8 ;
  char buf[512] ;

  static char *func = "hash_ix" ;

  if ( chunksize == 0 ) chunksize = SCHUNKSIZE ;

  si = fopen(si_name, "rb") ;
  if ( si == NULL ) {
    fprintf(stderr, "Can't open dictionary file %s\n", si_name) ;
    *result = -1 ;
    return NULL ;
  }

  table = make_hashtab(HASH_MED, nelem, hashfuncname, HASHSEED, 0) ;
  if ( table == NULL ) {
    fprintf(stderr, "%s(): can't allocate memory for hash table\n", func) ;
    *result = -3 ;
    return NULL ;
  }
  /* Run off 1st two bytes of file */
  getc(si) ; getc(si) ;
  while ( read_itr_seq(buf, in_type, si) == 1 ) {
    (void)parse_itr(buf, in_type, &tr) ;
    itr = hash_add_itr(table, &tr) ;
    if ( itr == NULL ) {
      fprintf(stderr, "%s(): can't allocate memory for ITR\n", func) ;
      *result = -3 ;
      return NULL ;
    }
    pvol = tr.pstg_vol ;
    if ( lastpvol >= 0 ) fclose(oi) ;
    oi = fopen(oi_names[pvol], "rb") ;
    if ( oi == NULL ) {
      fprintf(stderr, "Can't open postings file %s\n",
	      oi_names[pvol]) ;
      errc = -1 ;
      break ;
    }
    lastpvol = pvol ;
    switch (out_type) {
    case 10:
    case 11:
      pstgs = bss_malloc(tr.tnp * sizeof(struct mpstg_10)) ;
      for ( i = 0 ; i < tr.tnp ; i++, pstgs += sizeof(struct mpstg_10) ) {
	p10 = pstgs ;
	tf = fget_vshort(oi) ;
	p10->tf = tf ;
	numposes += tf ;
#if ! HILO
	fread((void *)&irn, 3, 1, oi) ;
	p10->rn = irn ;
#endif
      }
      break ;
    }
  }
  if ( Dbg & D_HASH ) 
    fprintf(bss_syslog,
	    "keys %d occ %d depth sum %d max depth %d tmem %d dmem %d\n",
	    table->n_keys, table->n_occ, table->depth_sum, table->max_depth,
	    table->tmem, table->dmem) ;
  *result = 0 ;
  return table ;
}

