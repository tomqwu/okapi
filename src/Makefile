#CC should be in environment
#CC = cc -fast -xO5 -xchip=ultra -xarch=v8plusa
#CC = gcc -D_FILE_OFFSET_BITS=64 -D_LARGEFILE64_SOURCE -g -march=i586 -ffast-math

CC = gcc -m32 -g -march=x86-64 -ffast-math -fPIC

DFLAG = -DMMPUT -DHOLD_ATOMIC_MAPS=0 -DHOLD_DOCLENS=1
IFLAG = -I.
SHELL = /bin/bash
CFLAGS = $(OFLAG) $(DFLAG) $(IFLAG) -L.
YACC = bison -y
YFLAGS = -v -d
LEX = flex
LFLAGS = -t
PARSEFLAGS =

HEADERS=defines.h parms.h bss.h bss_defaults.h \
	charclass.h bss_errors.h para.h bss_protos.h \
	win32extras.h bss_indexing.h hash.h

IHEADERS=i0_defs.h

HEADERS1=i1_defs.h

SOURCES = ctype.c bss_lookup.c bss_globs.c bss_db.c bss_db_desc.c bss_combine.c \
	bss_sets.c bss_show.c bss_misc.c bss_funcs.c \
	bss_extract.c bss_errors.c bss_passages.c \
	bss_readrecs.c \
	utils.c isdewey.c \
	porter.c realport.c gslsubs.c \
	weightsubs.c extsubs.c Toktab.c \
	setup_toktab.c para.c sentence.c \
	i0_parse+.y i0_reader+.l i0_main.c win32extras.c \
	hashsubs.c

SOURCES1 = i1_main.c 

ISOURCES = ix1.c ixf.c ixsubs.c mergeonly.c mergeonly_gen.c \
	ix_deconstruct.c

CONVSOURCES = convert_runtime.c make_parafile.c make_para_rec.c

L_OBJS= bss_lookup.o bss_globs.o bss_db.o bss_db_desc.o bss_combine.o \
	bss_errors.o bss_readrecs.o \
	bss_sets.o bss_show.o bss_misc.o bss_funcs.o \
	bss_extract.o bss_passages.o \
	utils.o  isdewey.o \
	porter.o realport.o gslsubs.o Toktab.o \
	weightsubs.o extsubs.o \
	para.o sentence.o hashsubs.o ctype.o

Y_OBJS=	i0_parse+.o i0_reader+.o i0_main.o

all:	libs i1+ indexing convert_runtime make_parafile

#libs:	libbss.a libi0+.a libOkapiYorkInterface.so
libs:	libbss.a libi0+.a 

libbss.a:	$(L_OBJS) $(HEADERS) $(IHEADERS) bss_version.o
	rm -f libbss.a
	ar -r libbss.a $(L_OBJS) bss_version.o

libbss.so:	$(L_OBJS) $(HEADERS) $(IHEADERS) bss_version.o
		$(CC) $(CFLAGS) -G -o $@ $(L_OBJS) bss_version.o

librelex.so:	$(L_OBJS) libi0+.a
	rm -f librelex.so
	javah -jni	-classpath "." relex
	$(CC) $(CFLAGS) -I"/usr/lib/jvm/java-6-oracle/include" -I"." -I"/usr/lib/jvm/java-6-oracle/include/linux" -c relex.c	-o	relex.o -li0+ -lm
	$(CC) -shared -o librelex.so relex.o libbss.a libi0+.a

$(L_OBJS):	$(HEADERS) $(SOURCES)

bss_version.c:
		version=`cat bss_VERSION`;\
		echo "char *version=\"$$version\" ;">tmpvers.$$$$;\
		echo "char *vdate=\"`date +%Y%m%d%H%M`\" ;" >> tmpvers.$$$$;\
		mv -f tmpvers.$$$$ bss_version.c

bss_version.o:	bss_VERSION bss_version.c
		$(CC) -c bss_version.c
		rm bss_version.c

libi0+.a: 	$(L_OBJS) $(HEADERS) $(Y_OBJS) $(IHEADERS) bss_version.o
		ar -r libi0+.a $(L_OBJS) $(Y_OBJS) bss_version.o

libi0+.so:	$(L_OBJS) $(HEADERS) $(Y_OBJS) $(IHEADERS) bss_version.o
		$(CC) $(CFLAGS) -G -o $@ $(L_OBJS) $(Y_OBJS) bss_version.o

i1+:	i1_main.c $(HEADERS1) libi0+.a
	$(CC) $(CFLAGS) -o $@ i1_main.c -li0+ -lm

indexing:	ix1 ixf mergeonly mergeonly_gen

ix1:	libbss.a ix1.c ixsubs.c
	$(CC) $(CFLAGS) $@.c ixsubs.c -lbss -lm -o $@

ixf:	ixf.c libbss.a
	$(CC) $(CFLAGS) $@.c -lbss  -lm -o $@

convert_runtime:	convert_runtime.c make_para_rec.c libbss.a
	$(CC) $(CFLAGS) $@.c make_para_rec.c -o $@ -lbss -lm 

ix_deconstruct:	ix_deconstruct.c libbss.a
	$(CC) $(CFLAGS) ix_deconstruct.c -o $@ -lbss -lm

mergeonly:	mergeonly.c ixsubs.c utils.c
	$(CC) $(CFLAGS) ixsubs.c utils.c Toktab.c mergeonly.c -o $@

mergeonly_gen:	mergeonly_gen.c ixsubs.c utils.c
	$(CC) $(CFLAGS) ixsubs.c utils.c Toktab.c mergeonly_gen.c -o $@

tar:
	tar cvf bss.tar $(SOURCES) $(SOURCES1) $(ISOURCES) $(HEADERS) \
	$(IHEADERS) $(HEADERS1) bss_VERSION $(CONVSOURCES) site.*.h m.*.h \
	Makefile.*

i0_parse+.c: 	i0_parse+.y $(HEADERS) $(IHEADERS)
	$(YACC) $(YFLAGS) i0_parse+.y
	cp y.tab.c i0_parse+.c

i0_parse+.o:	i0_parse+.c 
	$(CC) $(CFLAGS) $(PARSEFLAGS) -c i0_parse+.c

i0_reader+.c:	i0_reader+.l y.tab.h $(HEADERS) $(IHEADERS)
	echo "#include \"defines.h\"" > i0_reader+.c
	$(LEX) $(LFLAGS) i0_reader+.l >> i0_reader+.c

i0_reader+.o: i0_reader+.c
	$(CC) $(CFLAGS) $(PARSEFLAGS) -c i0_reader+.c

y.tab.h:	i0_parse+.c

clean:
	rm -f core *~ *.o *.a i0_parse+.c i0_reader+.c bss_version.c y.tab.h *.a *.so y.output convert_runtime i1+ ix1 ixf make_parafile mergeonly mergeonly_gen

make_parafile:	make_parafile.c make_para_rec.c libi0+.a
	$(CC) $(CFLAGS) make_parafile.c make_para_rec.c -li0+ -lm -o $@

etags:	 TAGS

TAGS:	$(SOURCES) $(HEADERS) $(SOURCES1) $(ISOURCES) $(CONVSOURCES)
	etags -t $(SOURCES) $(HEADERS) $(IHEADERS) $(HEADERS1) \
	$(SOURCES1) $(ISOURCES) $(CONVSOURCES)

install:	install_libs install_i1 install_ix \
		install_conv

install_libs:	libbss.a libi0+.a
	version=`cat bss_VERSION`;\
        for f in libbss libi0+;\
	do cp -p $$f.a $$OKLIB/$$f.$$version.a;done

install_i1:	i1+
	version=`cat bss_VERSION`;\
	name="i1+.$$version";\
	mv i1+ $$OKBIN/$$name

install_ix: ix1 ixf mergeonly mergeonly_gen
	version=`cat bss_VERSION`;\
	for f in ix1 ixf mergeonly mergeonly_gen;\
	do mv $$f $$OKLIB/$$f.$$version;\
	done

install_conv:	convert_runtime
	version=`cat bss_VERSION`;\
	mv convert_runtime $$OKLIB/convert_runtime.$$version

