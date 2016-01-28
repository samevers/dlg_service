# -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT(hint, 3.0.1, lvjieyong@sohu-rd.com)
AC_CONFIG_SRCDIR([common.mk])
AC_CONFIG_AUX_DIR(_aux)
AM_INIT_AUTOMAKE([foreign])
AC_CONFIG_HEADER([config-dummy.h])
AC_PREFIX_DEFAULT(/opt/sogou)
#AM_MAINTAINER_MODE

# Checks for system
AC_CANONICAL_HOST
# Checks for programs.
AC_LANG(C++)
AC_PROG_CC
AC_PROG_CXX
PKG_PROG_PKG_CONFIG
#AC_PROG_RANLIB
#AC_LIBTOOL_WIN32_DLL
#AC_LIBTOOL_DLOPEN
AC_PROG_LIBTOOL
#AC_PROG_LEX
#AC_PROG_YACC
#AM_PATH_PYTHON(2.4)

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([iostream])

# Checks for typedefs, structures, and compiler characteristics.
#AC_HEADER_STDBOOL
AC_C_CONST
#AC_C_INLINE
#AC_TYPE_SIZE_T
#AC_HEADER_TIME

# Checks for library functions.
#	below removed by kirbyzhou, becauuse underall linux, they work fine.
AC_FUNC_ALLOCA
#AC_FUNC_MALLOC
#AC_FUNC_REALLOC
#AC_FUNC_SELECT_ARGTYPES
#AC_TYPE_SIGNAL
#AC_FUNC_VPRINTF
#AC_CHECK_FUNCS([gethostbyname inet_ntoa memmove memset select socket sqrt strchr strdup strerror strncasecmp strstr])
AC_CHECK_FUNCS([mbstowcs wcstombs mbtowc wctomb])

AK_STD_FEATURES

#for large file
CPPFLAGS="$CPPFLAGS -D_FILE_OFFSET_BITS=64"

##check mysql
#AK_DETECT_LIB2(mysql, [/usr],
#	[mysql.h], [mysqlclient_r], my_init,
#	[optional],
#	[include/mysql], [lib/mysql])

#check ace
#AK_DETECT_LIB2(ace, [/usr/local/src/ACE* /usr/local/ACE* /opt/ACE*],
#	[ace/ACE.h], [ACE], [ACE::init],
#	[optional],
#	[], [ace])

#check db4 
#AK_DETECT_LIB2(db4, [],
#	[db.h], [db-4.4 db-4.3 db-4.2 db], db_create,
#	[required],
#	[], [])
#AK_DETECT_LIB2(db_cxx4, [],
#	[db_cxx.h], [db_cxx-4.4 db_cxx-4.3 db_cxx-4.2 db_cxx], db_create,
#	[required],
#	[], [])
#
##check sqlite 
#AK_DETECT_LIB2(sqlite3, [],
#	[sqlite3.h], [sqlite3], sqlite3_exec,
#	[optional],
#	[], [])

AC_CACHE_SAVE

# var / condiional


AK_CHECK_MODULE_ROOT(SSPLATFORM_ROOT, [the root of module ssplatform], ssplatform, [_include], [_lib])
AK_CHECK_MODULE_ROOT(SSENCODING_ROOT, [the root of module encoding], ssplatform-encoding, [], [_lib])
AK_CHECK_MODULE_ROOT(WEBBASE_ROOT, [the root of module web_base], web_base, [_include], [_lib])
AK_CHECK_MODULE_ROOT(UTILS_ROOT, [the root of module utils], utils, [], [_lib])
AK_CHECK_MODULE_ROOT(DICMAP_ROOT, [the root of module dicmap], dicmap, [_include], [_lib])
AK_CHECK_MODULE_ROOT(WORDSEGMENT_ROOT, [the root of module word_segmentor], word_segmentor, [], [_lib])
AK_CHECK_MODULE_ROOT(WEBSEGMENT_ROOT, [the root of module web_segmentor], web_segmentor, [], [_lib])
#AK_CHECK_MODULE_ROOT(LIBVRDATAENGINE_ROOT, [the root of module vr_data_engine], vr_data_engine, [], [_lib])
#AK_CHECK_MODULE_ROOT(LIBMAPSEARCH_ROOT, [the root of module map_search], map_search, [_include], [_lib])
#AK_CHECK_MODULE_ROOT(PINYIN_ANNOTATION_ROOT, [the root of module pinyinannotation], pinyinannotation, [_include], [_lib])
#AK_CHECK_MODULE_ROOT(QUERYCORRECTOR_ROOT, [the root of module query_corrector], query_corrector, [], [Library/Linux])


#AK_CHECK_MODULE_ROOT(WEBBASE_ROOT, [the root of module web_base], web_base, [_include], [_lib])
#AK_CHECK_MODULE_ROOT(SSPLATFORM_ROOT, [the root of module ssplatform], ssplatform, [_include], [_lib])
#AK_CHECK_MODULE_ROOT(WORDSEGMENT_ROOT, [the root of module word_segmentor], word_segmentor, [_include], [_lib])
#AK_CHECK_MODULE_ROOT(UTILS_ROOT, [the root of module utils], utils, [_include], [_lib])
#AK_CHECK_MODULE_ROOT(PINYIN_ANNOTATION_ROOT, [the root of module pinyinannotation], pinyinannotation, [_include], [_lib])
#AK_CHECK_MODULE_ROOT(QUERYCORRECTOR_ROOT, [the root of module query_corrector], query_corrector, [_include], [_lib])
#AK_CHECK_MODULE_ROOT(PORNRATE_ROOT, [the root of module porn_rate], porn_rate, [_include], [_lib])
#AK_CHECK_MODULE_ROOT(HINT_ROOT, [the root of module Hint], Hint, [_include], [_lib])
#AK_CHECK_MODULE_ROOT(WEBANALYSIS_ROOT, [the root of module web_analysis], web_analysis)
#AK_CHECK_MODULE_ROOT(WEBSPIDER_ROOT, [the root of module web_spider], web_spider)
#AK_CHECK_MODULE_ROOT(SUMMARY_ROOT, [the root of module summary], summary)

AK_CHECK_BUILD_DEPENDENCIES(yes, [$MODULE_ROOT_VALUES])

# finalize
AC_CONFIG_FILES([
	Makefile
	src/jsoncpp/Makefile
	src/common_base/Makefile
	src/locate_getresult/Makefile
	src/index/index_da/Makefile
	src/index/inv_index/Makefile
	src/index/data_make/Makefile
	src/segment/Makefile
	src/QueryMatch/Makefile
	src/dialogue/Makefile
	src/da_search/Makefile
	src/doc_search/Makefile
	src/test/Makefile
	src/http_client/Makefile
	src/http_server/Makefile
	src/include/common/Makefile
	src/include/Utils/Makefile
])
AC_OUTPUT
AK_PATCH_LIBTOOL

