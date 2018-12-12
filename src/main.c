//
//  General-purpose Photovoltaic Device Model gpvdm.com- a drift diffusion
//  base/Shockley-Read-Hall model for 1st, 2nd and 3rd generation solarcells.
// 
//  Copyright (C) 2012-2017 Roderick C. I. MacKenzie r.c.i.mackenzie at googlemail.com
//
//	https://www.gpvdm.com
//	Room B86 Coates, University Park, Nottingham, NG7 2RD, UK
//
//
// This program is free software; you can redistribute it and/or modify it
// under the terms and conditions of the GNU General Public License,
// version 2, as published by the Free Software Foundation.
//
// This program is distributed in the hope it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
// more details.



#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>


#include <time.h>
#include <unistd.h>

#include <util.h>

#include <sim.h>
#include <dos.h>
#include <server.h>
#include <light_interface.h>
#include <dump.h>
#include <complex_solver.h>
#include <inp.h>
#include <gui_hooks.h>
#include <timer.h>
#include <rand.h>
#include <hard_limit.h>
#include <patch.h>
#include <cal_path.h>
#include <lang.h>
#include <log.h>
#include <device.h>
#include <fit.h>
#include <advmath.h>
#include <plot.h>
#include <assert.h>
#include <rpn.h>

#include <sys/prctl.h>
#include <sys/types.h>
#include <signal.h>
#include <fdtd.h>
static int unused __attribute__((unused));


int main (int argc, char *argv[])
{
prctl(PR_SET_PDEATHSIG, SIGKILL);
//setlocale(LC_ALL,"");
//bindtextdomain("gpvdm","./lang/");
//textdomain("gpvdm");
//wchar_t wide[1000];
//int i=mbstowcs(wide, _("Hole generation rate"), 1000);
//exit(0);
int run=FALSE;
struct simulation sim;
sim_init(&sim);

/*struct rpn rpn_cal;
rpn_init(&sim,&rpn_cal);
rpn_add_var(&sim,&rpn_cal,"a",1e-10);
double value1=rpn_evaluate(&sim,&rpn_cal,"log(a)");
printf("rodeval: %le\n",value1);
return 0;*/

if (scanarg( argv,argc,"--gui")==TRUE)
{
	sim.gui=TRUE;
}

int log_level=0;
set_logging_level(&sim,log_level_screen);
cal_path(&sim);
char *b=NULL;
char *c=NULL;
if (scanarg( argv,argc,"--lang")==TRUE)
{
	setlocale(LC_ALL,get_arg_plusone( argv,argc,"--lang"));
	c=textdomain("gpvdm");
	b=bindtextdomain("gpvdm","lang");

}else
{
	setlocale(LC_ALL,"");
	setlocale(LC_NUMERIC, "C");
	c=textdomain("gpvdm");	
	b=bindtextdomain("gpvdm","lang");
}
if (scanarg( argv,argc,"--html")==TRUE)
{
	sim.html=TRUE;
	printf_log(&sim,"<meta charset=\"utf-8\">\n");
}else
{
	sim.html=FALSE;
}

if (scanarg( argv,argc,"--help")==TRUE)
{
	printf_log(&sim,"gpvdm_core - General-purpose Photovoltaic Device Model\n");
	printf_log(&sim,"%s\n",_("Copyright (C) 2012-2017 Roderick C. I. MacKenzie, Releced under GPLv2"));
	printf_log(&sim,"\n");
	printf_log(&sim,"Usage: gpvdm_core [%s]\n",_("options"));
	printf_log(&sim,"\n");
	printf_log(&sim,"%s:\n",_("Options"));
	printf_log(&sim,"\n");
	printf_log(&sim,"\t--outputpath\t%s\n",_("output data path"));
	printf_log(&sim,"\t--inputpath\t %s\n",_("sets the input path"));
	printf_log(&sim,"\t--version\t%s\n",_("displays the current version"));
	printf_log(&sim,"\t--zip_results\t %s\n",_("zip the results"));
	printf_log(&sim,"\t--simmode\t %s\n",_("Forces a simulation mode."));
	printf_log(&sim,"\t--cpus\t %s\n",_("sets the number of CPUs"));
	printf_log(&sim,"\n");
	printf_log(&sim,"%s\n",_("Additional information about gpvdm is available at https://www.gpvdm.com."));
	printf_log(&sim,"\n");
	printf_log(&sim,"%s\n\n",_("Report bugs to: roderick.mackenzie@nottingham.ac.uk"));
	exit(0);
}
if (scanarg( argv,argc,"--version")==TRUE)
{
	printf_log(&sim,_("gpvdm_core, Version %s\n"),gpvdm_ver);
	printf_log(&sim,"%s\n",_("Copyright (C) 2012-2017 Roderick C. I. MacKenzie, Releced under GPLv2"));
	printf_log(&sim,"%s\n",_("This is free software; see the source code for copying conditions."));
	printf_log(&sim,_("There is ABSOLUTELY NO WARRANTY; not even for MERCHANTABILITY or\n"));
	printf_log(&sim,_("FITNESS FOR A PARTICULAR PURPOSE.\n"));
	printf_log(&sim,"\n");
	exit(0);
}


timer_init(0);
timer_init(1);
dbus_init();

set_ewe_lock_file("","");

char pwd[1000];
if (getcwd(pwd,1000)==NULL)
{
	ewe(&sim,"IO error\n");
}


remove("snapshots.zip");
remove("light_dump.zip");

do_fdtd(&sim);

hard_limit_init(&sim);

dumpfiles_load(&sim);
set_plot_script_dir(pwd);

//set_plot_script_dir(char * in)



if(geteuid()==0) {
	ewe(&sim,"Don't run me as root!\n");
}



srand(time(0));
//printf_log(&sim,"%s\n",_("Token"));
//exit(0);
randomprint(&sim,_("General-purpose Photovoltaic Device Model (https://www.gpvdm.com)\n"));
randomprint(&sim,_("You should have received a copy of the GNU General Public License\n"));
randomprint(&sim,_("along with this software.  If not, see www.gnu.org/licenses/.\n"));
randomprint(&sim,"\n");
randomprint(&sim,_("If you wish to collaborate in anyway please get in touch:\n"));
randomprint(&sim,"roderick.mackenzie@nottingham.ac.uk\n");
randomprint(&sim,"www.rodmack.com/contact.html\n");
randomprint(&sim,"\n");
//getchar();
sim.server.on=FALSE;
sim.server.cpus=1;
sim.server.readconfig=TRUE;

if (scanarg( argv,argc,"--outputpath")==TRUE)
{
	strcpy(sim.output_path,get_arg_plusone( argv,argc,"--outputpath"));
}


if (scanarg( argv,argc,"--inputpath")==TRUE)
{
	strcpy(sim.input_path,get_arg_plusone( argv,argc,"--inputpath"));
}





char name[200];
struct inp_file inp;

inp_init(&sim,&inp);
if (inp_load_from_path(&sim,&inp,sim.input_path,"ver.inp")!=0)
{
	printf_log(&sim,"can't find file %s ver.inp",sim.input_path);
	exit(0);
}
inp_check(&sim,&inp,1.0);
inp_search_string(&sim,&inp,name,"#core");
inp_free(&sim,&inp);

if (strcmp(name,gpvdm_ver)!=0)
{
printf_log(&sim,"Software is version %s and the input files are version %s\n",gpvdm_ver,name);
exit(0);
}

if (scanarg( argv,argc,"--gui")==TRUE)
{
	sim.gui=TRUE;
}

gui_start(&sim);
server_init(&sim);

if (scanarg( argv,argc,"--lock")==TRUE)
{
	server_set_dbus_finish_signal(&(sim.server), get_arg_plusone( argv,argc,"--lock"));
}

if (scanarg( argv,argc,"--lockfile")==TRUE)
{
	server_set_lock_file(&(sim.server), get_arg_plusone( argv,argc,"--lockfile"));
}

int ret=0;


if (scanarg( argv,argc,"--simmode")==TRUE)
{
	strcpy(sim.force_sim_mode,get_arg_plusone( argv,argc,"--simmode"));
}

if (run==FALSE)
{
	gen_dos_fd_gaus_fd(&sim);

	server_add_job(&sim,sim.output_path,sim.input_path);
	print_jobs(&sim);

	ret=server_run_jobs(&sim,&(sim.server));

}

server_shut_down(&sim,&(sim.server));

hard_limit_free(&sim);
dumpfiles_free(&sim);

if (ret!=0)
{
	return 1;
}
return 0;
}

