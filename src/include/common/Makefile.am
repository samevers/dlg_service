include $(top_srcdir)/common.mk

bin_PROGRAMS=
lib_LIBRARIES=
lib_LTLIBRARIES=
SUBDIRS=

lib_LTLIBRARIES+=libeditdist.la
libeditdist_la_SOURCES=EditDist.cpp
libeditdist_la_LIBADD=-lm -lpthread -lencoding -L../../segment -lSegment
AM_CPPFLAGS+=-DSS_DOCID_BITS=256 -I../../ -I../

lib_LTLIBRARIES+=libtfidf.la
libtfidf_la_SOURCES=tf-idf.cpp
libtfidf_la_LIBADD=-lm -lpthread -lencoding

bin_PROGRAMS+=tf_idf
tf_idf_SOURCES=tfidf.cpp
tf_idf_LDADD= -lpthread -lssplatform -lencoding -L./ -ltfidf
AM_CPPFLAGS += -I./

#bin_PROGRAMS+=editDist
#editDist_SOURCES=main.cpp
#editDist_LDADD= -lpthread -lssplatform -lencoding -L./ -L../../ -lSegment -leditdist
#AM_CPPFLAGS+=- -I./ -I../../ -I../
