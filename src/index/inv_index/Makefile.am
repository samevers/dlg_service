include $(top_srcdir)/common.mk

bin_PROGRAMS=
lib_LIBRARIES=
lib_LTLIBRARIES=
SUBDIRS=

lib_LTLIBRARIES+=libnormalize.la
libnormalize_la_SOURCES=normalize.cpp
libnormalize_la_LIBADD= -lpthread -L../index_da -lindex_da -L../../segment -lSegment
AM_CPPFLAGS+= -I../index_da -I../../include -I../../segment


bin_PROGRAMS+=normalize
normalize_SOURCES=normalize_main.cpp
normalize_LDADD=-ldoc_index -lpthread -L../index_da -lindex_da -lnormalize -L../../segment -lSegment
#AM_CPPFLAGS= -I../index_da -I../../include


lib_LTLIBRARIES+=libdoc_index.la
libdoc_index_la_SOURCES=doc_index.cpp
libdoc_index_la_LIBADD=-lm -lpthread -lssplatform -lencoding -ldicmap -lCoreSegmentor -L../index_da -lindex_da -lnormalize  -L../../segment -lSegment
#AM_CPPFLAGS= -I../index_da  

#bin_PROGRAMS+=doc_index_test
#doc_index_test_SOURCES=doc_index_test.cpp
#doc_index_test_LDADD=-ldoc_index -lgtest -lpthread

bin_PROGRAMS+=doc_index_make_index
doc_index_make_index_SOURCES=doc_index_make_index.cpp
doc_index_make_index_LDADD=-ldoc_index -lpthread -L../index_da -lindex_da -L../../segment -lSegment
#AM_CPPFLAGS= -I../index_da  

#bin_PROGRAMS+=segment
#segment_SOURCES=segment.cpp
#segment_LDADD=-ldoc_index -lpthread -L../index_da -lindex_da
#AM_CPPFLAGS= -I../index_da -I../../include


