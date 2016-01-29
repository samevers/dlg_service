include $(top_srcdir)/common.mk

bin_PROGRAMS=httpserver
httpserver_SOURCES=main.cpp http_server.cpp task_base.cpp configuration.cpp HttpResponse.cpp HttpHandler.cpp 
httpserver_LDADD=-lpthread -lssplatform  -L"../jsoncpp" -ljsoncpp -L"../locate_getresult" -llocateresult -lgflags -lglog -lgtest 
AM_CPPFLAGS+=-DSS_DOCID_BITS=256
AM_CPPFLAGS+=-I../sys 
