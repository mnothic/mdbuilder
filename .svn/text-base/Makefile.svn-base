###
# Rustic Makefile mdbuilder package.
##
CC          = gcc
CFLAGS      = -O1 -g
LDFLAGS     = -L/usr/local/lib
INCLUDES    = -I/usr/local/include
BIN_DIR     = /usr/local/bin
RC_DIR      = /usr/local/etc/rc.d
SCRIPT      = mdbuilder
SCRIPT_DIR  = script
GROUP       = vmail
OWNER       = vmail
LIBS        = -lpq -lconfuse
BIN         = mdbuilder
PID_DIR     = /var/run/mdbuilder
SOURCE_DIR  = src
CONFIG_DIR  = conf
ETC         = /usr/local/etc
CONFIG      = mdbuilder.conf
WORK_DIR    = /var/vmail

default: $(BIN)

main.o:		$(SOURCE_DIR)/main.c $(SOURCE_DIR)/mdbuilder.h $(SOURCE_DIR)/parser.h $(SOURCE_DIR)/util.h
	$(CC) $(CFLAGS) -c $(SOURCE_DIR)/main.c  $(LDFLAGS) $(INCLUDES)

mdbuilder.o:	$(SOURCE_DIR)/mdbuilder.c $(SOURCE_DIR)/mdbuilder.h $(SOURCE_DIR)/util.h
	$(CC) $(CFLAGS) -c $(SOURCE_DIR)/mdbuilder.c $(LDFLAGS) $(INCLUDES)

parser.o:	$(SOURCE_DIR)/parser.c $(SOURCE_DIR)/parser.h
	 $(CC) $(CFLAGS) -c $(SOURCE_DIR)/parser.c $(LDFLAGS) $(INCLUDES)

util.o:	$(SOURCE_DIR)/util.c $(SOURCE_DIR)/util.h
	 $(CC) $(CFLAGS) -c $(SOURCE_DIR)/util.c $(LDFLAGS) $(INCLUDES)

$(BIN):	main.o mdbuilder.o parser.o util.o
	$(CC) $(CFLAGS) -o $(BIN) main.o mdbuilder.o parser.o util.o $(LDFLAGS) $(INCLUDES) $(LIBS)

install: $(BIN)
	mv $(BIN) $(BIN_DIR)
	chmod 755 $(BIN_DIR)/$(BIN)
	chown $(OWNER):$(GROUP) $(BIN_DIR)/$(BIN)
	chown -R $(OWNER):$(GROUP) $(WORK_DIR)
	cp $(SCRIPT_DIR)/$(SCRIPT) $(RC_DIR)
	chmod 755 $(RC_DIR)/$(SCRIPT)
	mkdir -p $(PID_DIR)
	chown -R $(OWNER):$(GROUP)  $(PID_DIR)
	cp $(CONFIG_DIR)/$(CONFIG) $(ETC)/$(CONFIG)
clean:
	rm -f count *.o *~ *.bak

deinstall:
	rm -rf $(BIN_DIR)/$(BIN)  $(RC_DIR)/$(SCRIPT) $(PID_DIR) $(ETC)/$(CONFIG)
