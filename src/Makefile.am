OBJS= solver_interface.o sim_find_n0.o sim_run.o newton_update.o pos.o initial.o config.o memory.o anal.o ntricks.o ntricks_externalv.o complex_solver.o thermal.o mesh.o newton_interface.o dll_interface.o remesh.o run_electrical_dll.o

AROPT=

inc=-I../include/ -L../

all: main.c $(OBJS)
	$(CC) main.c $(OBJS) -o ../gpvdm_core.@EXE_EXT@ $(inc) @COMPILE_FLAG@ -Wl,--whole-archive @LOCAL_LINK@ -Wl,--no-whole-archive @LDFLAGS@ @EXE_LINK@
	ln -s -f gpvdm_core.@EXE_EXT@ ../go.@EXE_EXT@
	ln -s -f ./gui/gpvdm ../gpvdm


.c.o : 
	$(CC) -c $(DEFINE_FLAGS) $(inc) $(CPPFLAGS) $(warn) @COMPILE_FLAG@ $< -o $@

clean:
	rm *.o *.a *.dll *.exe -f
