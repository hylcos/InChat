TIBEMS_DIR=$(HOME)/tibco/ems/8.2

CC = gcc
OPT_DBG_FLAGS = -g
LDFLAGS=-L$(TIBEMS_DIR)/lib -m32
INCFILES=-I$(TIBEMS_DIR)/include -I $(TIBEMS_DIR)/samples/c
CFLAGS=$(OPT_DBG_FLAGS) $(INCFILES) -m32

#
# 32-bit libs
#
TIBEMS_LIB=-ltibems -ltibemslookup -ltibemsufo
TIBEMSADMIN_LIB=-ltibemsadmin
TIBEMS_SSL_LIB=-lssl -lcrypto
TIBEMS_LDAP_LIB=-lldap -llber
TIBEMS_XML_LIB=-lxml2
TIBEMS_ZLIB=-lz

TLIBS=-lpthread

LIBS=  $(TIBEMS_LIB) $(TIBEMSADMIN_LIB) $(TIBEMS_LDAP_LIB) $(TIBEMS_XML_LIB) $(TIBEMS_SSL_LIB) $(TIBEMS_ZLIB) $(TLIBS)


all: Main 


Main: main.c
	$(CC) $(LDFLAGS) $(CFLAGS) main.c  $(LIBS) -o $@
