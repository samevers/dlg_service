include $(top_srcdir)/common.mk

bin_PROGRAMS=
lib_LIBRARIES=
lib_LTLIBRARIES=
SUBDIRS=

lib_LTLIBRARIES+=libSegment.la
libSegment_la_SOURCES=Segment.cpp
libSegment_la_LIBADD=-lm -lpthread -lssplatform -lencoding -ldicmap -lCoreSegmentor
AM_CPPFLAGS+= -I../include

bin_PROGRAMS+=segment
segment_SOURCES=main.cpp
segment_LDADD=-lpthread -L../index/index_da -lindex_da -L./ -lSegment
AM_CPPFLAGS= -I../index/index_da -I../include -I../index/inv_index


