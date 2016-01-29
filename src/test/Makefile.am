include $(top_srcdir)/common.mk

bin_PROGRAMS=
lib_LIBRARIES=
lib_LTLIBRARIES=
SUBDIRS=

bin_PROGRAMS=test_run
test_run_SOURCES=test.cpp
test_run_LDADD= -lssplatform -lencoding -L../dialogue -ldialogue -L../da_search -lda_search -L../index/inv_index -lnormalize -L../doc_search -ldoc_search -L../include/common -leditdist
test_run_LDFLAGS=-DHAVE_CXX_STDHEADERS -DLINUX -DACE_HAS_EVENT_POLL -D_FILE_OFFSET_BITS=64 -fPIC -ggdb

AM_CPPFLAGS=-DSS_DOCID_BITS=256 -I../include -I../index/index_da -I../dialogue -I../ -I../da_search -I../doc_search -I../index/inv_index -I../include/common -I../include/gary_common -I../segment
