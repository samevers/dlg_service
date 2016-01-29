#include $(top_srcdir)/common.mk

bin_PROGRAMS=editDist
editDist_SOURCES=editDist.cpp
editDist_LDADD= -lpthread -lssplatform -lencoding -L../common -L../../segment -lSegment -leditdist
AM_CPPFLAGS= -I../ -I../../ -I../common
