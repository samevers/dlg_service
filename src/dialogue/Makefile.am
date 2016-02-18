include $(top_srcdir)/common.mk

bin_PROGRAMS=
lib_LIBRARIES=
lib_LTLIBRARIES=
SUBDIRS=

lib_LTLIBRARIES+=libdialogue.la
libdialogue_la_SOURCES=dialogue.cpp
libdialogue_la_LIBADD=-lpthread -lssplatform -lencoding -lgflags -lglog -lgtest -L../index/inv_index/ -ldoc_index -L../index/index_da -lindex_da -L../doc_search -ldoc_search -L../include/common -leditdist -L../index/inv_index -lnormalize

AM_CPPFLAGS=-DSS_DOCID_BITS=256 -I../include -I../index/index_da/ -I../segment  -I../doc_search -I../index/inv_index 
libdialogue_la_CPPFLAGS=$(AM_CPPFLAGS) -DSS_DOCID_BITS=256


##
bin_PROGRAMS=test_run
test_run_SOURCES=test.cpp
test_run_LDADD= -lssplatform -lencoding -ldialogue -L../index/index_da -lindex_da -L../doc_search -ldoc_search -L../include/common -leditdist -L../index/inv_index -lnormalize

test_run_LDFLAGS=-DHAVE_CXX_STDHEADERS -DLINUX -DACE_HAS_EVENT_POLL -D_FILE_OFFSET_BITS=64 -fPIC -ggdb

AM_CPPFLAGS=-DSS_DOCID_BITS=256 -I../include -I../index/index_da/ -I../segment  -I../doc_search -I../index/inv_index 


#AM_CPPFLAGS+=-I../sys

