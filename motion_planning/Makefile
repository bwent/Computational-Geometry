PLATFORM = $(shell uname)


## Compilation flags
##comment out one or the other
##debugging
CFLAGS = -g
##release
#CFLAGS = -O3 -DNDEBUG
LDFLAGS=

CFLAGS+= -Wall

ifeq ($(PLATFORM),Darwin)
## Mac OS X
CFLAGS += -m64 -isystem/usr/local/include  -Wno-deprecated
LDFLAGS+= -m64 -lc -framework AGL -framework OpenGL -framework GLUT -framework Foundation
else
## Linux
CFLAGS += -m64
INCLUDEPATH  = -I/usr/include/GL/
LIBPATH = -L/usr/lib64 -L/usr/X11R6/lib
LDFLAGS+=  -lGL -lglut -lrt -lGLU -lX11 -lm  -lXmu -lXext -lXi
endif


CC = g++ -O3 -Wall $(INCLUDEPATH)


PROGS = plan

default: $(PROGS)

plan: vis.o vg.o
		$(CC) -o $@ vis.o vg.o $(LDFLAGS)

vis.o: vis.cpp vg.h
		$(CC) -c $(INCLUDEPATH) $(CFLAGS)  vis.cpp -o $@

vg.o: vg.cpp vg.h
		$(CC) -c $(INCLUDEPATH)  $(CFLAGS)  vg.cpp -o $@


clean::
	rm *.o
	rm plan
