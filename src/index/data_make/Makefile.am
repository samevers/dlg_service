include $(top_srcdir)/common.mk

bin_PROGRAMS=
lib_LIBRARIES=
lib_LTLIBRARIES=
SUBDIRS=

lib_LTLIBRARIES+=libdlg_log_index.la
libdlg_log_index_la_SOURCES=dlg_log_index.cpp
libdlg_log_index_la_LIBADD=-lm -lpthread -lssplatform -lencoding -ldicmap -lCoreSegmentor -L../index_da -lindex_da
AM_CPPFLAGS+= -I../../include -I../../segment -I../index_da

#bin_PROGRAMS+=dlg_log_index_test
#dlg_log_index_test_SOURCES=dlg_log_index_test.cpp
#dlg_log_index_test_LDADD=-ldlg_log_index -lgtest -lpthread

bin_PROGRAMS+=dlg_log_index_make_index
dlg_log_index_make_index_SOURCES=dlg_log_index_make_index.cpp
dlg_log_index_make_index_LDADD=-ldlg_log_index -lpthread -lgflags

#bin_PROGRAMS+=dlg_index_test_file
#dlg_index_test_file_SOURCES=dlg_index_test_file.cpp
#dlg_index_test_file_LDADD=-ldlg_index -lpthread
#
#
#bin_PROGRAMS+=get_postfix
#get_postfix_file_SOURCES=get_postfix.cpp
#get_postfix_LDADD=-ldlg_index -lpthread
