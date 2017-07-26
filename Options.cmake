## This file will be included by the CMAKE main control file.
## It sets several build-options, but is ignored by git.
## This enables user-local configurations independent from
## the commited stuff.

set(BUILD_LIBCRYPTOPP_LOCAL OFF CACHE BOOL "Use local libcryptopp copy to build, not the github version")
set(LIBCRYPTOPP_LOCAL_PATH "/some/where" CACHE PATH "Libcryptopp local git repository path")

## debug options.
## -DDEBUG_QRE -DDEBUG_QR  : QueryResultElement, QueryResult . TODO: move this into cmake include,
## configurable via ccmake

set(DBG_STORAGE_CLASSES OFF CACHE BOOL "compile storage classes with debug output")
set(DBG_DB OFF CACHE BOOL "enable db stuff debug output")

