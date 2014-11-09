#ifndef _bss_protos_h
#define _bss_protos_h

struct pos {
  int t ;
  int nt ;
} ;
struct tokmap {
  int s ;
  int f ;
} ;

int mymladen(); //mko
int extract_text_mladen(char *ip, int il, FIELD_TYPE field_type,
		 Extraction_regime reg, int (*stemfn)(char *, int, char *),
		 Gsl *gs, void *outptr, int outflag, int rec, u_int lims,
		 int fdnum, int diag) ;


void *bss_stype012(char *st, int stl, u_int limit, Ix *index,
		   int *result, int stype) ;
int bss_pisrch(char *term, int l, Ix *index) ;
void *bss_fftge(char *st, int l, void *itr, Ix *ix, int *res) ;
int bss_getnump(void *itr, u_int limit, Ix *index, int *ttf) ;
void *bss_gprt(void *itr, Ix *index, int *result) ;
void *bss_gprtc(void *itr, u_int limit, Ix *index, int *result) ;
void *skip_itr(void *itr, int itype) ;
void *bss_gnxtc(void *itr, u_int limit, Ix *index, int *result) ;
void *bss_gnxt(void *itr, Ix *index, int *result) ;
void * bss_getitr_by_addr(int chunk, int offset, Ix *index) ;
int bss_rdchunk(int chunknumber, Ix *index) ;
Transfer_itr *parse_itr(void *, int, Transfer_itr *) ;
char *get_term_itr(void *itr, int itype) ;
int get_tnp_itr(void *itr, int itype) ;
int get_ttf_itr(void *itr, Ix *index) ;
int get_maxtf_itr(void *itr, Ix *index) ;
OFFSET_TYPE get_ppos_itr(void *itr, int itype) ;
int get_pvol_itr(void *itr, int itype) ;
int get_plength_itr(void *itr, Ix *index) ;
u_int get_limits_itr(void *itr, Ix *index) ;
int sats_limit_itr(void *itr, Ix *index, u_int limit) ;
int is_db_open(Db *db) ;
int read_db_avail(void) ;
int open_bibdir(Db *db, int auxnum) ;
int open_bibfile(Db *db, int vol) ;
int open_db(char *name, Db *db, int flags) ;
int close_db(Db *db, int flags) ;
int calc_bign(Db *db, u_int limit) ;
void set_bign(Db *db, int N) ;
int get_bign(Db *db, u_int limit) ;
int read_field_types(char *db_name, char *suffix, FIELD_TYPE types[]) ;
int read_search_groups(Db *db, char *dbname, Sg *) ;
Sg *find_search_group_by_name(Db *db, char *attr) ;
int find_field_num(char *) ;
int prepare_paras(Db *db, int code) ;
int prepare_limits(Db *db) ;
int read_limits(Db *db) ;
int find_limit(char *limit_name) ;
char *bss_limits_avail(int *num_lims) ;
int read_dir(Db *db) ;
Ix *ixinit(Db *db, Sg *sg) ;
int ix_free(Ix *index) ;
int read_doclens(Db *, int) ;
int get_doclen_irn(Db *db, int ixnum, int irn) ;
int open_stemfile(Db *db, int ixnum, char *fn) ;
int get_avedoclen(Db *db, int ixnum, u_int limit) ;
#ifdef HAVE_LL
LL_TYPE get_cttf(Db *db, int ixnum, u_int limit) ;
#else
double get_cttf(Db *db, int ixnum, u_int limit) ;
#endif
void set_avedoclen(Db *db, int ixnum, int avedoclen) ;
void set_avedoclens(Db *, u_int) ;
int db_availq(char *dbname) ;
char *db_show(char *what) ;
void record_db(char *db_name, char *message) ;
int try_paths(char *main_stem, char *end_bit, int type, char *rw,
	      int mode, char *) ;
int read_db_desc (char *db_name, Db *dbs) ;
int read_parm_line(FILE *, char *, char *) ;
void free_buf(Db *db) ;
int write_db_desc(char *db_name, Db *dbs) ;

int bss_combine_0(int, int, u_int, int [], int, Setrec *, Setrec *[],
		  int [], int maxgap) ;
int bss_combine_pass(int, int, int, Setrec *, int [], u_int, u_int,
		     Setrec *[], double [], int [], int, int [], int,
		     double, double, double, double, double [],
		     int, int, int) ;
int bss_combine_lr(void) ;
int bss_combine_bm(int, int, int, Setrec *, u_int, u_int, Setrec *[],
		   double [], int [], int, int [], int,
		   double, double, double, double, double [] ) ;
int bss_quick_combine(int, int, Setrec *, u_int, Setrec **, double *, int *,
		      int, int, double, double, int) ;
int bss_quick_merge(int, int, u_int, Setrec **, float *, double *, int *, int,
		    double, double, double *, double *) ;
int bss_willett(int, Setrec *, float *, double, int *, int,
		double, double, double *, double *) ;
int bss_combine_limit(int, int, u_int, int [], Setrec *[], int [], int,
		      Setrec *, u_int) ;
int select_topn(int setnum, Setrec *outsrec, int n, u_int find_flags) ;
char *translate_opcode(int code) ;
void combine_stats(void) ;
int bss_copy_set(void) ;

void prepare_error(int, ...) ;
void prepare_warning(int, ...) ;
void mem_err(char *, char *, int) ;
char *bss_perror(void) ;
void sys_err(char *) ;
void internal_err(char *, char *) ;
void dbg_from_string(char *) ;

#ifdef LARGEFILES
int rdr_extern(OFFSET_TYPE, Db *) ;
#else
int rdr_extern(long, Db *) ;
#endif
int rdr_n(DIR_ADDR_TYPE, Db *, int, int) ;
int rd_recnum(int recnum, int fdnum, Db *db) ;
int real_reclen(char *rec) ;
DIR_ADDR_TYPE recnum_to_addr(int recnum, Db *db, int auxnum, int *reclen) ;
char *findfld(int fldno, u_char *rec, int *lp, Db *pdb, int reclen) ;
char *findfld_defaultdb(int fldno, u_char * rec, int *lp, int reclen) ;
#ifdef WRITEREC
int writerec(char *, int, int, int, FILE *, FILE *, FILE *, BOOL) ;
#endif /* WRITEREC */
Setrec *get_dummyset(void) ;
Setrec *get_setrec(int psize, int msize, int flags, int pstgtype) ;
int setup_pstgs(Setrec *s, int flags) ;
int open_pstgs(Setrec *s, int which, int flags) ;
int close_pstgs(Setrec *s, int which, int flags) ;
int map_pf(Setrec *s) ;
int read_all_pf(Setrec *s) ;
int s_unmap(Setrec *s, int which) ;
int s_free(Setrec *s, int which) ;
bss_Gp *get_next_pstg(Setrec *s) ;
int write_next_pstg(Setrec *s) ;
bss_Gp *get_pstg_n(Setrec *s, int n) ;
int get_pstg_offset(Setrec *s, int n) ;
int bss_limit(Setrec *old_srec, u_int mask, Setrec *srec, FILE *f_old, FILE *f_new) ;
bss_Gp *make_bss_Gp(int maxpos) ;
int read_pstg_unpacked(void *source, bss_Gp **buf, int type, int isfile) ;
int dump_unpacked_pstg(bss_Gp *p, int type, char *buf, int code) ;
int write_unpacked_pstg(void *target, bss_Gp *buf, int type, int isfile) ;
/* int get_packed_plength(void *p, int type, BOOL isfile); */
void copy_packed_pstg(void *t, void *s, BOOL isfile_t, BOOL isfile_s) ;
int get_numpos_pstg(bss_Gp *pstg) ;
int get_tf_pstg(bss_Gp *pstg) ;
int get_rec_pstg(bss_Gp *pstg) ;
int bss_get_doclen(Db *, char *, int, int) ;
int get_doclen(Db *, int, int) ;
int get_doclen_pstg(bss_Gp *pstg, Ix *index);
double get_wt_pstg(bss_Gp *pstg) ;
double get_auxwt_pstg(bss_Gp *pstg) ;
int translate_pstgtype(int t, char *p) ;
void translate_output_flags(int t, char *p) ;
void translate_set_type(int t, char *p) ;
FILE *set_open_pf(int setnum, int which, char *rw) ;
int set_close_pf(int setnum, int which) ;
int set_close_pfs(int setnum) ;
void bss_pfname(char *buf, int setnum, int ch) ;
int make_set_from_irn(int irn, int code) ;
int make_set_from_rec(int setnum, int recnum, int code) ;

u_char *bss_getrec_raw(int setnum, int recnum, Db *db, int *reclen,
		       double *weight, int fdnum) ;
char *bss_getfield_raw(int fieldnum, u_char *rec, int *fieldlen, Db *db, int) ;
int prepare_set_for_output(int setnum, int recnum) ;
int set_current_displayset(int setnum, int recnum) ;
void clear_current_displayset(void) ;
int bss_highlight_field(char *ip, int fdlen, int fdnum, FIELD_TYPE,
			char *, bss_Gp *, int, int, int) ;
#ifdef OLD_HIGHLIGHT
int bss_highlight_rec(u_char *rec, bss_Gp *pstg, int pstgtype) ;
#endif
int bss_highlight_para(char *ip, int il, char *op, FIELD_TYPE, U_pos *,
		       int fdnum, int *senum, int *pos_used,
		       int format, int start_oset) ;
int length_from_header(char *doc_header) ;

int bss_np(int setnum) ;
int bss_ttf(int setnum) ;
int check_setnum(int num) ;
int check_set_and_rec(int setnum, int recnum) ;
int check_db_set_rec_irn(Db *db, int setnum, int recnum) ;
Extraction_regime check_ext_reg(char *) ;
Gsl *check_gsl(char *gsl_name, char *stem_name, BOOL no_db) ;
int new_gsl(void) ;
PFI check_stemfn(char *sfname) ;

int bss_open(char * path, int flags) ;
FILE *bss_fopen(char *filename, char *type) ;
int bss_fclose(FILE *f) ;
int bss_close(int) ;
void report_files(char *buf) ;
void report_time(char *buf) ;
void *bss_malloc(size_t size) ;
void *bss_mmap(void *addr, int pages, int prot, int flags, int fd,
	       OFFSET_TYPE offset, BOOL hold) ;
void *get_zeroed_mem(int mpages, BOOL hold) ;
char *bss_strdup(char *) ;
char *bss_strdup_kl(char *, size_t) ;
void *bss_memdup(void *, size_t) ;
void *bss_realloc(void *ptr, size_t size)  ;
void *bss_calloc(size_t, size_t) ;
void bss_free(void *ptr) ;
#ifndef NO_MMAP
int bss_munmap(void *addr, int pages) ;
#endif
void report_allocation(char *buf) ;
int bss_startup(void) ;
void bss_exit(void) ;
int bss_open_db(char *db_name, int flags) ;
u_int bss_findlimit(char *names) ;
int bss_lkup(char *term, int l, Sg *sg, int limits, int stype, int *result,
	     char **found_term, int flags, int *np, int *ttf) ;
int bss_lkup_a(char *term, int l, char *attr, int limits, int stype,
	       int *result, char **found_term, int flags, int *np, int *ttf) ;
int bss_lkup_anum(char *term, int l, int sgnum, int limits, int stype,
		  int *result, char **found_term, int flags, int *np, int *ttf) ;
int make_set(int setnum, int recnum) ;
int bss_stem_term(char *term, char *fn, char *buf) ;
int bss_show(int setnum, int recnum, Db *db, int format, double *weight,
	     char *buf, int code, char *startstr, char *finstr) ;
int bss_do_combine(int num, int sets[], double weights[], int qtfs[],
		   int op, int scorefunc,
		   int maxgap, double aw, double gw, int target_number,
		   u_int find_flags, u_int limit,
		   double k1, double k2, double k3, double k7, double k8,
		   double b, double b3, int avedoclen,
		   int querylen, int avequerylen,
		   int passage_unit, int passage_step, int passage_maxlen,
		   double wt_frig_factor[]) ;
double bss_assign_weight_ln(int f, int limit, int n, int tf, int R, int r,
			    int Rloading, int rloading,
			    int bign, int S, int s, int qtf,
			    double k3, double k4, double k5, double k6,
			    int theta, double lamda,
#ifdef HAVE_LL
			    LL_TYPE tnp,
			    LL_TYPE cttf,
#else
			    double tnp,
			    double cttf,
#endif
			    double wtfactor
			    ) ;
int bss_parse(char *input, int length_input, char *attr, char *parsed_terms) ;
Hashtab *bss_parse_hash(char *, int, Hash_type, char *, char *, char *) ;
int bss_superparse(char *input, int length_input, char *attr, char *buf) ;
int bss_parse_rgf(char *input, int length, char *r, char *g, char *sf,
	      char *buffer) ;
int bss_superparse_rgf(char *input, int length, char *r, char *g, char *sf,
		       char *buffer) ;
int bss_superparse_nosource(char *input, int length_input, char *attr, char *buf) ;
int bss_superparse_doc(int setnum, int recnum, Db *db, int field, int offset,
		       int length, char *attr, char **buf, BOOL sources) ;
u_int bss_set_limit(u_int limit) ;
u_int bss_dolimit(int setnum, u_int mask, int flags) ;
int check_db_open(Db *) ;
int check_set(int num) ;
int check_set_rec(int setnum, int recnum) ;
int check_setnum(int num) ;
int check_set_and_rec(int setnum, int recnum) ;
int check_set_rec(int setnum, int recnum) ;
int check_irn(int irn) ;
int check_limit(u_int l) ;
Sg *check_attr(Db *db, char *attr_name, int code) ;
Ix *check_index(Db *db, struct search_group *sg) ;
int check_fdnum(Db *, int) ;
int bss_dump_setrec(int setnum, char *response, int code) ;
int bss_display_setstats(int setnum, char *buf) ;
int bss_describe_database(char *db_name, char *buf, int code) ;
void bss_describe_databases(char *buf, int code) ;
int bss_describe_attribute(char *name, char *buf) ;
int bss_describe_attributes(char *db_naame, char *buf, int code) ;
Sg *find_search_name(Db *, char *attr) ;
int bss_clear_setrec(int) ;
int bss_purge_setrec(int num) ;
int bss_clear_all_setrecs(void) ;
int get_rec_info_text(int irn, char *outstring);
int bss_get_big_n(u_int limit) ;
int bss_get_tnt(char *, u_int) ;
#ifdef HAVE_LL
int bss_get_avedoclen(char *attr, u_int limit, LL_TYPE *cttf) ;
#else
int bss_get_avedoclen(char *attr, u_int limit, double *cttf) ;
#endif
double bss_get_rscore(Db *, char *attr, u_int limit, int n, int R, int r) ;
int bss_get_db_name(char *buf) ;
int bss_get_stemfunctions(char *buf) ;
int bss_get_regimes(char *buf) ;
void bss_get_version(char *buf) ;
int bss_do_trec_stats(char *buf, int setnum, int mark, int R, int flags, int n, int code) ;
int bss_set_debug(int v) ;
int bss_calc_debug(char *) ;

int extract(char *input, int i_len, Extraction_regime reg, Gsl *gsl,
	    int (*stem_fn)(char *, int, char *), void *output, int type,
	    int rec, u_int limits, int fdnum, int senum, int tokens[],
	    int *stopped, int *outlen)  ;
int setup_ixparms(int ixnum) ;
int extract_text(char *ip, int il, FIELD_TYPE field_type,
		 Extraction_regime reg, int (*stemfn)(char *, int, char *),
		 Gsl *gs, void *outptr, int outflag, int rec, u_int lims,
		 int fdnum, int diag) ;
int extract_text_nosource(char *ip, int il, FIELD_TYPE field_type,
			  Extraction_regime reg,
			  int (*stemfn)(char *, int, char *), Gsl *gs,
			  void *outptr, int outflag, int rec, int diag) ;
#ifdef LARGEFILES
int read_plain_para_rec(OFFSET_TYPE oset, int parafile_type) ;
#else
int read_plain_para_rec(long oset, int parafile_type) ;
#endif
OFFSET_TYPE get_para_offset(int recnum) ;
int *get_para_rec(int) ;
int *para_get_field(int fdnum) ;
int find_para_len(int paranum) ;
int find_passage_len(int spara, int fpara) ;
int find_para_offset(int paranum) ;
int *setup_tot_tfs(int fdnum, int numstreams, int streams[], bss_Gp *pstgs[]) ;
int find_passage_tf(int fdnum, int spara, int fpara, U_pos *pos);
double find_passage_wt_component(int fdnum, int spara, int fpara, int pl, U_pos *pos, double, double, double, double) ;
double do_passage(int fdnum, int spara, int fpara, int pl, int numstreams, int streams[], bss_Gp *pstgs[], double streamweights[], double k1, double bm25_b, double avdl) ;
Para_rec *do_passages(int rec, int field, int unit, int max, int step, int nk, int numstreams, int streams[], bss_Gp *pstgs[], double wts[], double k1, double k2, double bm25_b, double avedl, int maxiters) ;

Gsl *setup_gsl(char *name, char *func_name, PFI stem_func, int *result) ;
int setup_gsl_sg(Sg *) ;
char *lookup_gsl(char *, int, Gsl *) ;
char *do_gsl(char *, int, Gsl *, char *gsclass, int *matchlength, char **token) ;
char *gsl_translate_token(Gsl *, char *token) ;
int free_gsl(Gsl **, int code) ;
int free_all_gsls(void) ;
int free_search_group(Sg *) ;
int free_search_groups(Db *) ;
int wstem(char *, int, char *) ;
int sstem(char *, int, char *) ;
int psstem(char *, int, char *) ;
int nostem(char *, int, char *) ;
int wstem_nospell(char *, int, char *) ;
int sstem_nospell(char *, int, char *) ;
int nostem_spell(char *, int, char *) ;
int portrealstem(char *, int, int) ;
#ifdef OLD_EXTRACT
int stem_phrase(char *ip, int length, char *op,
  int (*stemfn)(char *, int, char *), KLS *sources) ;
#endif /* OLD_EXTRACT */
int stem_phrase_token(char *ip, int length, char *op,
  int (*stemfn)(char *, int, char *), int intokens[], struct pos *) ;
int count_toks(char *, int) ;
#ifdef OLD_EXTRACT
char *get_next_sentence(u_char *ip, int *ilength, char **op, int *olength,
  FIELD_TYPE field_type, FIELD_TYPE *out_type,  int tokens[], int *numtoks) ;
#endif /* OLD_EXTRACT */
#ifdef OLD_HIGHLIGHT
char *hl_next_sentence(char *ip, int *ilength, char *op, int *olength,
  FIELD_TYPE field_type, struct highlight_table *htab, int *bytes_left) ;
#endif
int hl_text(char *ip, int il, char *op, U_pos *posp, int fdnum, int senum,
  int *pos_used, int format, int start_oset) ;
int phase2(char *, int, char *, BOOL) ;	/* Obsolescent (Dec 98) */
int phase2_token(char *s, int ls, char *t, BOOL convert, int intokens[],
		 int outtokens[], int *stopped) ;
int tokenize_text(char *, int, int **tokens) ;
int map_tokens(char *, int, struct tokmap *map) ;
int dohyphs_srch(u_char *, int, Gsl *) ;
int dohyphs_ind(char *, char *) ;
int extdewey(u_char *, int, u_char *) ;
BOOL is_a_year(char *, int) ;
int expand_date_range(char *s, int, char *t) ;

int isdewey(char *, int) ;

double nr_wt(double N, double n, double R, double r, double S, double s,
  double k4, double k5, double k6) ;
double plain_f4(int, int, int, int, int, int) ;
double f4_p_loaded_ln(double N, double n, double R, double r, double c,
  double Rloading, double rloading) ;
double f4_p_theta_ln(double N, double n, double R, double r, double theta) ;
double weight_to_s_v(double weight, int R, int r) ;
double s_v_to_weight(double sv, int R, int r) ;
double rscore(int N, int n, int R, int r, int V) ;
double idf(double, double) ;
double bm25mult(double, double, int, double) ;
double k3mult(double k3, int qtf) ;

int find_para(char *s, int l, int rule) ;
int count_paras(char *, int, int rule) ;
int make_para_rec(int fdnum, char *fdstart, int fdlen,
		  int toffset, FILE *parf, int maxtoks) ;
int find_sentence(char *, int, int rule, int maxtoks, int maxlength) ;
/* int find_sentence(char *, int, int rule, int maxtoks) ; */
int count_sentences(char *, int, int, int) ;

void nullfunction(void) ;
void movepcs(char *s, char *t) ;
int makepcs(char *s, int l, char *t) ;
int makets(char *s, char *t) ;
short reverse_16(short) ;
int reverse(int) ;
#ifdef HAVE_LL
LL_TYPE reverse_64(LL_TYPE) ;
#endif
u_int getw_reverse(FILE *f) ;
u_int reverse_bits(u_int u) ;
u_int reverse_int(u_int u) ;
u_int cv_pos_text_hllh(u_int u) ;
void cvint(char *s, u_int i) ;
char *itoascii(int) ;
int striplts(char *s, int l) ;
int real_length(char *s, int l, int *nchars) ;
int r_len(char *s) ;
char *slws(char *) ;
int stws(char *, int l) ;
void get_message_size(char *s, int *length, int *nlines) ;
void swapkls(KLS *k1, KLS *k2)  ;
int cpbytes(void *, int, void *, int) ;
int cpssi(char *, int, char **) ;
int cptsks(u_char *, u_char *, size_t) ;
int cptspcs(char *, char *) ;
int cpasc(u_char *, int, u_char *, int, int *) ;
int cppasc(u_char *, u_char *, int *) ;
int extwd(u_char *string, int length, u_char *output, int *olength,
	  u_char *delims, u_char *retain_list, int *start_offset) ;
int findwd(u_char *string, int ilength, KLS *target, u_char *delims) ;
int find_tok(char *string, int ilength, int *outlength, int start, int finish) ;
int find_name(char *name, struct names names[]) ;
int strdcpy(char *, const char *, int) ;
int cpwd(const void *, const void *) ;
int cptowb(u_char *, u_char *) ;
int stringscan(char *, char *) ;
int dedup(char *, int, int, char *) ;
void repover(char *, char *, int) ;
void gqsort(char *, int, int, int (*)(void *, void *)) ;
void gqsort_ins_thresh(char *, int, int, int (*)(void *, void *), int thresh) ;
void gqsort_int(u_int *, int n, int thresh) ;
int lmsort(char *base, int nel, int width, int (*comp)(char *, char *)) ;
int issub(char *s1, int l1, char *s2, int l2, int pos) ;
int srchrep(char *sub1, int l1, char *sub2, int l2, char *word, int *lw, int pos, int count) ;
int replace(char *s, int ls, int pos, int lp, char *t, int lt) ;
int isintab(int, u_char *) ;
int inw(int, int *, int) ;
int posnondig(u_char *, int) ;
int findchars(u_char *, u_char *, int) ;
int countchar(u_int ch, u_char *s, int length) ;
int lg2(int) ;
int ispower2(int) ;
int lige(int, int) ;
double n_choose_r(int, int) ;
void lcase(char *) ;
void cvlus(char *, int) ;
void cvuls(char *, int) ;
void cvulpcs(char *s) ;
int is_upper_text(char *, int) ;
int is_upper_tok(char *, int) ;
int is_lower_text(char *, int) ;
int is_lower_tok(char *, int) ;
int is_init_upper(char *, int) ;
long getsecs(void) ;
long elapsed(int since) ;
int ticks_elapsed(void) ;
int updatesecs(int) ;
int start_timers(struct itimerval *val) ;
double get_utime(void) ;
double get_stime(void) ;
int is_a_num(char *, int) ;
int pcit(u_int, u_char *) ;
int psit(char *, char *) ;
void rsft(char *, char *) ;
int search_for_command(char *source, int length, u_char *targets, u_char **first) ;
int read_file_buf(char *, char *, int max) ;
int file_to_buf(FILE *, char *, int max) ;
int is_a_bit(u_int) ;
int bits_to_right(u_int, u_int) ;
int num_bits(u_int) ;
int qaddslash(char *) ;
int readstr(FILE *, char *, int max) ;
int readcstr(FILE *, char *buf, int trm, int max, int *error) ;
int readstrstr(FILE *, char *buf, char *trm, int max, int *error) ;
int readline(FILE *, char *, int max) ;
int readln(FILE *, char *, int max) ;
int readlln(FILE *, char *, int max) ;
int skip_to_nextline(FILE *) ;
void *strrncpy(void* dst, void *src, size_t) ;
char *lastslash(char *) ;
char *dirname(char *) ;
u_int fget_vshort(FILE *) ;
int fput_vshort(int, FILE *) ;
int w1to4(int, FILE *) ;
int r1to4(int *, FILE *) ;
int w3or4(int, FILE *) ;
int st3or4(int, char *) ;
int r3or4(int *, FILE *) ;
int ld3or4(int *, char *) ;
int w1or2(int, FILE *) ;
int r1or2(int *, FILE *) ;
#ifdef HAVE_LL
int w5hl(u_char *, long long) ;
#endif
char * expand_pos_text(U_pos *) ;
char * expand_pos6(U_pos *) ;
char * expand_posnf(U_pos *) ;
OFFSET_TYPE fsize(char *) ;
OFFSET_TYPE ofsize(int) ;
int dfk(char *) ;
ssize_t get(int, OFFSET_TYPE, char *, size_t) ;
ssize_t put(int, OFFSET_TYPE, char *, size_t) ;
#ifdef LARGEFILES
#ifdef NO_FSEEKO
int fget(FILE *f, fpos_t pos, char *buf, int n) ;
#else
int fget(FILE *f, OFFSET_TYPE pos, char *buf, int n) ;
#endif
#else
int fget(FILE *f, long  pos, char *buf, int n) ;
#endif

void iinit(void) ;
void reinit(void) ;
int i0(char *, char *) ;
int i00(char *, char **) ;
void iexit(void) ;

OFFSET_TYPE imerge(int num, FILE **, FILE *, int itype) ;
OFFSET_TYPE merge(int, int num, int outsuffix, char *, char *, BOOL, int itype,
	  int del) ;
int read_itr_seq(void *, int, FILE *) ;
int ix_deconstruct(int itype, char *name, int numvols, char *names[],
		   char *outname, int chunksize, int verbosity, BOOL convert) ;
void icleanup(int) ;

#ifndef MMPUT
u_int mget_vshort(u_char *) ;
int mput_vshort(int, u_char *) ;
#endif

#endif /* _bss_protos_h */
