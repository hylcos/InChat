TIBEMS_DIR=/home/udingh/tibco/ems/8.2

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

#
# 64-bit libs
#
TIBEMS_LIB64=-ltibems64 -ltibemslookup64 -ltibemsufo64
TIBEMSADMIN_LIB64=-ltibemsadmin64
TIBEMS_SSL_LIB64=-lssl -lcrypto
TIBEMS_LDAP_LIB64=-lldap -llber
TIBEMS_XML_LIB64=-lxml2
TIBEMS_ZLIB64=-lz

all: ReadMessages SendMessages


ReadMessages: ReadMessages.c
	$(CC) $(LDFLAGS) $(CFLAGS) ReadMessages.c  $(LIBS) -o $@

SendMessages: SendMessages.c
	$(CC) $(LDFLAGS) $(CFLAGS) SendMessages.c  $(LIBS) -o $@