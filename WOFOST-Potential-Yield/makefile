SRCS = $(wildcard *.c)
OBJS = $(SRCS:%.o=%.c)
EXECUTABLE = wofost_Yp
CC       = gcc
CFLAGS  = -g -ggdb -O0 -Wall -Wextra -std=c99 -lm -lnetcdf -fcommon\
#CFLAGS   = -Wall -O3

all: $(EXECUTABLE)

$(EXECUTABLE) : $(OBJS)
	$(CC) -o $(EXECUTABLE) $(OBJS) -lm $(CFLAGS)


%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

TAGS:	$(SRCS)
	etags $(SRCS)
clean::
	\rm -f TAGS $(EXECUTABLE)