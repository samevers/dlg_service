include $(top_srcdir)/common.mk

bin_PROGRAMS=
lib_LIBRARIES=
lib_LTLIBRARIES=
SUBDIRS=

lib_LTLIBRARIES+=libda_search.la
libda_search_la_SOURCES=Da_Search.cpp
libda_search_la_LIBADD=-lm -lpthread -lssplatform -lencoding -ldicmap -lCoreSegmentor -L../index/index_da -lindex_da -L../segment -lSegment -L../index/inv_index/ -lnormalize
AM_CPPFLAGS+=-DSS_DOCID_BITS=256 -I../include -I../index/index_da/ -I../index/inv_index -I../segment

#bin_PROGRAMS+=index_da_make_index
#index_da_make_index_SOURCES=index_da_make_index.cpp
#index_da_make_index_LDADD= -lpthread -lssplatform -lencoding -lindex_da

