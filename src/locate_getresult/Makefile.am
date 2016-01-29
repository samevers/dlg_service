include $(top_srcdir)/common.mk

bin_PROGRAMS=
lib_LIBRARIES=
lib_LTLIBRARIES=
SUBDIRS=

lib_LTLIBRARIES+=liblocateresult.la
liblocateresult_la_SOURCES=locate_getresult.cpp
liblocateresult_la_LIBADD=-lpthread -lssplatform -lgflags -lglog -lgtest

liblocateresult_la_CPPFLAGS=$(AM_CPPFLAGS) -DSS_DOCID_BITS=256

#AM_CPPFLAGS+=-DSS_DOCID_BITS=256


#AM_CPPFLAGS+=-I../sys

