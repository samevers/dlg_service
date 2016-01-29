include $(top_srcdir)/common.mk

bin_PROGRAMS=
lib_LIBRARIES=
lib_LTLIBRARIES=
SUBDIRS=

lib_LTLIBRARIES+=libdoc_search.la
libdoc_search_la_SOURCES=Doc_Search.cpp
libdoc_search_la_LIBADD=-lm -lpthread -lssplatform -lencoding -ldicmap -lCoreSegmentor -L../index/inv_index -ldoc_index -L../segment -lSegment -lnormalize
AM_CPPFLAGS+=-DSS_DOCID_BITS=256 -I../include -I../index/inv_index -I../index/index_da -I../segment

#bin_PROGRAMS+=index_doc_make_index
#index_doc_make_index_SOURCES=index_doc_make_index.cpp
#index_doc_make_index_LDOCDD= -lpthread -lssplatform -lencoding -lindex_doc

