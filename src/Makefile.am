CFLAGS=-Wall -g -D dos_bin
AROPT=

local_link=@LOCAL_LINK@

inc=-I../include/
OBJS= solver_interface.o sim_find_n0.o sim_run.o newton_update.o pos.o initial.o config.o memory.o anal.o ntricks.o ntricks_externalv.o complex_solver.o thermal.o mesh.o newton_interface.o dll_interface.o remesh.o run_electrical_dll.o


LDLIBS=-L../

inc+= `pkg-config --cflags dbus-1` -I/usr/include/suitesparse/
CFLAGS+= -D dbus
LDLIBS+= `pkg-config --libs dbus-1`
LDLIBS+=-lumfpack -lm -lgsl -lgslcblas 
LDLIBS+= -rdynamic -export-dynamic -ldl -lzip -lz -lmatheval

all: main.c $(OBJS)
	$(CC) main.c $(OBJS) -o ../gpvdm_core $(inc) -L.  $(DEFINE_FLAGS) $(CFLAGS) $(LDLIBS) -Wl,--whole-archive $(local_link) -Wl,--no-whole-archive
	ln -s -f gpvdm_core ../go.o


%.o : %.c
	$(CC) -c $(DEFINE_FLAGS) $(inc) $(CFLAGS) $(warn) $< -o $@

clean:
	rm *.o *.a *.dll -f
