include $(top_srcdir)/common.mk

bin_PROGRAMS=
lib_LIBRARIES=
lib_LTLIBRARIES=
SUBDIRS=

lib_LTLIBRARIES+=libkmeans.la
libkmeans_la_SOURCES=kmeans.cpp general.cpp 
libkmeans_la_LIBADD=-lm -lpthread
AM_CPPFLAGS+= -I./


bin_PROGRAMS+=makeCorpus
makeCorpus_SOURCES=makeCorpus.cpp
makeCorpus_LDADD=-lpthread -L/search/yonghuahu/hack/dlg_service/src/include/common -leditdist -L./ -lkmeans
AM_CPPFLAGS= -I/search/yonghuahu/hack/dlg_service/src/ -I/search/yonghuahu/hack/dlg_service/src/include -I./


