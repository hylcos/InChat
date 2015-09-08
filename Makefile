

CC = gcc
OPT_DBG_FLAGS = -g -ggdb
LDFLAGS=-L lib -m32
INCFILES=-I include -I include/tibems
CFLAGS=$(OPT_DBG_FLAGS) $(INCFILES) -m32

#
# 32-bit libs
#
TIBEMS_LIB=-ltibems -ltibemslookup -ltibemsufo
TIBEMSADMIN_LIB=-ltibemsadmin
TLIBS=-lpthread

LIBS=  $(TIBEMS_LIB) $(TIBEMSADMIN_LIB) $(TIBEMS_LDAP_LIB) $(TIBEMS_XML_LIB) $(TIBEMS_SSL_LIB) $(TIBEMS_ZLIB) $(TLIBS)


all: Main 


Main: main.c
	$(CC) $(LDFLAGS) $(CFLAGS) main.c  $(LIBS) -o $@

