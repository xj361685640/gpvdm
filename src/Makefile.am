CFLAGS=-Wall -g -D dos_bin
AROPT=

inc=-I../include/
OBJS= solver_interface.o sim_find_n0.o sim_run.o newton_update.o pos.o inital.o config.o memory.o anal.o ntricks.o ntricks_externalv.o complex_solver.o thermal.o mesh.o newton_interface.o dll_interface.o remesh.o run_electrical_dll.o


LDLIBS=-L../lib/ -L../libdump/ -L../liblight/ -L../libmeasure/ -L../libserver/  -L../libdos/ -L../libmesh/ -L../libfit/

inc+= `pkg-config --cflags dbus-1` -I/usr/include/suitesparse/
CFLAGS+= -D dbus
LDLIBS+= `pkg-config --libs dbus-1`
LDLIBS+=-lumfpack -lcrypto -lm -lgsl -lgslcblas 
LDLIBS+= -rdynamic -export-dynamic -ldl -lzip -lz -lmatheval

all: main.c $(OBJS)
	$(CC) main.c $(OBJS) -o ../go.o $(inc) -L.  $(DEFINE_FLAGS) $(CFLAGS) $(LDLIBS) -Wl,--whole-archive -lgpvdm_dos -lgpvdm_lib -lgpvdm_dump -lgpvdm_light  -lgpvdm_measure -lgpvdm_server -lgpvdm_mesh -lgpvdm_fit -Wl,--no-whole-archive


%.o : %.c
	$(CC) -c $(DEFINE_FLAGS) $(inc) $(CFLAGS) $(warn) $< -o $@

clean:
	rm *.o *.a *.dll -f
