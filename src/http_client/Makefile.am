include $(top_srcdir)/common.mk

#bin_PROGRAMS=
#lib_LIBRARIES=
#lib_LTLIBRARIES=
#SUBDIRS=

bin_PROGRAMS=httpclient
httpclient_SOURCES=main.cpp HttpClient.cpp HttpResponse.cpp HttpRequest.cpp
httpclient_LDADD=-lpthread -L"../jsoncpp"  -lssplatform -ljsoncpp 
#bin_PROGRAMS+=httpclientfile
#httpclientfile_SOURCES=main_test_file.cpp HttpClient.cpp HttpResponse.cpp HttpRequest.cpp
#httpclientfile_LDADD=-lpthread -L"../jsoncpp" -L"../chaoli_utility" -lssplatform -ljsoncpp -lchaoli_utility

AM_CPPFLAGS+=-I../sys
AM_CPPFLAGS+=-DSS_DOCID_BITS=256
