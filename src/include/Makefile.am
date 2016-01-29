include $(top_srcdir)/common.mk

bin_PROGRAMS=
lib_LIBRARIES=
lib_LTLIBRARIES=
SUBDIRS=

lib_LTLIBRARIES+=libeditdist.la
libeditdist_la_SOURCES=EditDist.cpp
libeditdist_la_LIBADD=-lm -lpthread -lencoding -L../../segment -lSegment
AM_CPPFLAGS+=-DSS_DOCID_BITS=256 -I../../segment -I../

bin_PROGRAMS+=editDist
editDist_SOURCES=main.cpp
editDist_LDADD= -lpthread -lssplatform -lencoding -L./ -L../../segment -lSegment -leditdist
AM_CPPFLAGS+=- -I./ -I../../segment -I../
