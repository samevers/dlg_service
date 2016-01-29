include $(top_srcdir)/common.mk

bin_PROGRAMS=
lib_LIBRARIES=
lib_LTLIBRARIES=
SUBDIRS=

lib_LTLIBRARIES+=libjsoncpp.la
libjsoncpp_la_SOURCES=json_reader.cpp  json_value.cpp json_writer.cpp
libjsoncpp_la_LIBADD=-lm -lpthread 

#bin_PROGRAMS+=translation_table_test
#translation_table_test_SOURCES=translation_table_test.cpp 
#translation_table_test_LDADD=-ltranslation_table -lgtest -lgflags -lglog
#
