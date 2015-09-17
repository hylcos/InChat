

CC = gcc
OPT_DBG_FLAGS = -g -ggdb
INCFILES=-I include -I include/tibems -I $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))


#
# 64-bit libs
#
TIBEMS_LIB64=-ltibems64 -ltibemslookup64 -ltibemsufo64
TIBEMSADMIN_LIB64=-ltibemsadmin64
TIBEMS_SSL_LIB64=-lssl -lcrypto
TIBEMS_LDAP_LIB64=-lldap -llber
TIBEMS_XML_LIB64=-lxml2
TIBEMS_ZLIB64=-lz
TLIBS=-lpthread
#
# uncomment these overrides for x86_64 (non-ia64) platforms
#
CFLAGS=$(OPT_DBG_FLAGS) $(INCFILES) -m64
LIBS=  $(TIBEMS_LIB64) $(TIBEMSADMIN_LIB64) $(TIBEMS_LDAP_LIB64) $(TIBEMS_XML_LIB64) $(TIBEMS_SSL_LIB64) $(TIBEMS_ZLIB64) $(TLIBS)
LDFLAGS= -L lib/linux/64 -L lib/linux -m64



all: Main 


Main: main.c
	$(CC) $(LDFLAGS) $(CFLAGS) main.c  $(LIBS) -o $@
#	LD_LIBRARY_PATH=$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))/lib/64
#	export @LD_LIBRARY_PATH
#	@echo $(LD_LIBRARY_PATH) or LD_LIBRARY_PATH

