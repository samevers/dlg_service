include $(top_srcdir)/common.mk

SUBDIRS=@PACKAGE_DEPENDENCIES@ 
lib_LTLIBRARIES=libquerymatch.la
libquerymatch_la_SOURCES=QueryMatch.cpp QueryMatch.h WordSegmentor.cpp GraphSegmentor.cpp
libquerymatch_la_LIBADD=-lACE -lssplatform -lencoding -lIQSegmentor -lboost_regex  -lpoolbuf_allocator -ldicmap  -lCoreSegmentor -lpthread

libquerymatch_la_LDFLAGS= -no-install -DHAVE_CXX_STDHEADERS -DLINUX -DACE_HAS_EVENT_POLL -D_FILE_OFFSET_BITS=64 -fPIC -ggdb

AM_CPPFLAGS+=-L/usr/lib64  -I$(UTILS_ROOT)  -I$(SSENCODING_ROOT)   -I/usr/include/thrift/ -I. -I/usr/include/thrift/fb303

libquerymatch_la_CPPFLAGS=$(AM_CPPFLAGS) -DSS_LOG_DEFAULT_MODULE=queryclass -DSS_DOCID_BITS=256
