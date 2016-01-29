include $(top_srcdir)/common.mk

bin_PROGRAMS=
lib_LIBRARIES=
lib_LTLIBRARIES=
SUBDIRS=

lib_LTLIBRARIES+=libcommon_base.la
libcommon_base_la_SOURCES=database.cpp
libcommon_base_la_LIBADD=-lm -lpthread -L/usr/lib64/mysql -lmysqlclient

#bin_PROGRAMS+=common_base_test
#common_base_test_SOURCES=common_base_test.cpp
#common_base_test_LDADD=-lcommon_base -lgtest -lpthread 
