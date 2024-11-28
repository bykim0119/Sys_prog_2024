SRCS = main.c
LIBSRCS = open.c close.c put.c get.c
TARG = kvs_static
TARG_DYNAMIC = kvs_dynamic

CC = gcc
OPTS = -Wall -g -pg
LIBS = -L. -lkvs -Wl,-rpath=.

OBJS = $(SRCS:.c=.o)
LOBJS = $(LIBSRCS:.c=.o)
LARS = libkvs.a
SLARS = libkvs.so
AOBJS = kvs.img answer.dat

all: 
#static
static: $(TARG)

$(TARG) : $(OBJS) $(LARS)
	$(CC) -o $(TARG) $(OBJS) $(LIBS) -static

$(LARS) : $(LOBJS)
	ar rs $(LARS) $(LOBJS)

%.o: %.c
	$(CC) $(OPTS) -fPIC -c $< -o $@

#dynamic
dynamic: $(TARG_DYNAMIC)
$(TARG_DYNAMIC) : $(OBJS) $(SLARS)
	$(CC) -o $(TARG_DYNAMIC) $(OBJS) $(LIBS)
	ldd $(TARG_DYNAMIC)

$(SLARS) : $(LOBJS)
	$(CC) -shared -o $(SLARS) $(LOBJS)

clean:
	rm -f $(OBJS) $(LOBJS) $(LARS) $(SLARS) $(TARG) $(TARG_DYNAMIC) $(AOBJS)


