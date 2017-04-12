#    General-purpose Photovoltaic Device Model - a drift diffusion base/Shockley-Read-Hall
#    model for 1st, 2nd and 3rd generation solar cells.
#    Copyright (C) 2012-2016 Roderick C. I. MacKenzie r.c.i.mackenzie at googlemail.com
#
#	https://www.gpvdm.com
#	Room B86 Coates, University Park, Nottingham, NG7 2RD, UK
#
#    This program is free software; you can redistribute it and/or modify
#    it under the terms of the GNU General Public License v2.0, as published by
#    the Free Software Foundation.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License along
#    with this program; if not, write to the Free Software Foundation, Inc.,
#    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

from code_ctrl import enable_betafeatures
import i18n
_ = i18n.language.gettext

class my_data():
	token=""
	units=""
	info=""
	def __init__(self,file_name,a,b,c,e,f,widget,defaults=None):
		self.file_name=file_name
		self.token=a
		self.units=b
		self.info=c
		self.defaults=defaults
		self.number_type=e
		self.number_mul=f
		self.widget=widget


lib=[]

#light.inp
lib.append(my_data("light.inp","#lpoints","au",_("Mesh points (lambda)"),"e",1.0,"QLineEdit"))
lib.append(my_data("light.inp","#lstart","m",_("Lambda start"),"e",1.0,"QLineEdit"))
lib.append(my_data("light.inp","#lstop","m",_("Lambda stop"),"e",1.0,"QLineEdit"))
lib.append(my_data("light.inp","#laserwavelength","m",_("Laser wavelength"),"e",1.0,"QLineEdit"))
lib.append(my_data("light.inp","#spotx","m",_("Spot size x"),"e",1.0,"QLineEdit"))
lib.append(my_data("light.inp","#spoty","m",_("Spot size y"),"e",1.0,"QLineEdit"))
lib.append(my_data("light.inp","#pulseJ","J",_("Energy in pulse"),"e",1.0,"QLineEdit"))
lib.append(my_data("light.inp","#gather","au",_("#gather"),"e",1.0,"QLineEdit"))
lib.append(my_data("light.inp","#laser_pulse_width","s",_("Length of pulse"),"e",1.0,"QLineEdit"))
lib.append(my_data("light.inp","#electron_eff","0-1",_("Electron generation efficiency"),"e",1.0,"QLineEdit"))
lib.append(my_data("light.inp","#hole_eff","0-1",_("Hole generation efficiency"),"e",1.0,"QLineEdit"))
lib.append(my_data("light.inp","#sun",_("filename"),_("Sun's spectra"),"e",1.0,"QLineEdit"))
lib.append(my_data("light.inp","#light_file_generation","file_name",_("File containing generation rate"),"e",1.0,"gpvdm_select"))
lib.append(my_data("light.inp","#Dphotoneff","0-1",_("Photon efficiency"),"e",1.0,"QLineEdit"))


#dos.inp
lib.append(my_data("","#dostype","au",_("DoS distribution"),"s",1.0,"QComboBoxLang",defaults=[[("exponential"),_("exponential")],["complex",_("complex")]]))
lib.append(my_data("","#Nc","m^{-3}",_("Effective density of free electron states"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#Nv","m^{-3}",_("Effective density of free hole states"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#mueffe","m^{2}V^{-1}s^{-1}",_("Electron mobility"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#mueffh","m^{2}V^{-1}s^{-1}",_("Hole mobility"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#Ntrape","m^{-3} eV^{-1}",_("Electron trap density"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#Ntraph","m^{-3} eV^{-1}",_("Hole trap density"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#Etrape","eV",_("Electron tail slope"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#Etraph","eV",_("Hole tail slope"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#epsilonr","au",_("Relative permittivity"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#srhsigman_e","m^{-2}",_("Free electron to Trapped electron"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#srhsigmap_e","m^{-2}",_("Trapped electron to Free hole"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#srhsigman_h","m^{-2}",_("Trapped hole to Free electron"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#srhsigmap_h","m^{-2}",_("Free hole to Trapped hole"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#free_to_free_recombination","m^{3}s^{-1}",_("n_{free} to p_{free} Recombination rate constant"),"e",1.0,"QLineEdit"))

#stark.inp
lib.append(my_data("","#stark_startime","s",_("startime"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#stark_ea_factor","au",_("ea_factor"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#stark_Np","1/0",_("Np"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#stark_den","1/0",_("den"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#stark_externalv","V",_("externalv"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#stark_dt_neg_time","s",_("dt_neg_time"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#stark_dt","s",_("dt"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#stark_dt_mull","au",_("dt_mull"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#stark_stop","s",_("stop"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#stark_stark","1/0",_("stark"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#stark_lasereff","1/0",_("lasereff"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#stark_probe_wavelength","nm",_("wavelength"),"e",1e9,"QLineEdit"))
lib.append(my_data("","#stark_sim_contacts","1/0",_("sim_contacts"),"e",1.0,"QLineEdit"))

#ref
lib.append(my_data("","#ref_website","au",_("Website"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#ref_research_group","au",_("Research group"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#ref_autors","au",_("Authors"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#ref_jounral","au",_("Journal"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#ref_volume","au",_("Volume"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#ref_pages","au",_("Pages"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#ref_year","au",_("Year"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#ref_md5","au",_("md5 sum"),"e",1.0,"QLineEdit"))

#pulse
lib.append(my_data("","#Rshort_pulse","Ohms",_("R_{short}"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#pulse_bias","V",_("V_{bias}"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#pulse_light_efficiency","au",_("Efficiency of light"),"e",1.0,"QLineEdit"))

#mat.inp
lib.append(my_data("","#material_type","type",_("Material type"),"e",1.0,"QComboBoxLang",defaults=[[("organic"),_("Organic")],["inorganic",_("Inorganic")],["metal",_("Metal")],["other",_("Other")]]))
lib.append(my_data("","#mat_alpha","0-1.0",_("Alpha channel"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#red_green_blue","rgb",_("Color"),"e",1.0,"QColorPicker"))
lib.append(my_data("","#mat_alpha","0-1",_("Transparency"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#status","type",_("Publish material data?"),"e",1.0,"QComboBoxLang",defaults=[[("public"),_("Public")],[("public_all"),_("Public all")],["private",_("Private")]]))
lib.append(my_data("","#changelog","au",_("Change log"),"e",1.0,"QChangeLog"))

#jv.inp
lib.append(my_data("jv.inp","#jv_step_mul","0-2.0",_("JV voltage step multiplyer"),"e",1.0,"QLineEdit"))
lib.append(my_data("jv.inp","#jv_max_j","A m^{-2}",_("Maximum current density"),"e",1.0,"QLineEdit"))
lib.append(my_data("jv.inp","#jv_light_efficiency","au",_("JV curve photon generation efficiency"),"e",1.0,"QLineEdit"))
lib.append(my_data("jv.inp","#jv_pmax_n","m^{-3}",_("Average carrier density at P_{max}"),"e",1.0,"QLineEdit"))
lib.append(my_data("jv.inp","#jv_pmax_tau","m^{-1}",_("Recombination time constant"),"e",1.0,"QLineEdit"))
lib.append(my_data("jv.inp","#Vstart","V",_("Start voltage"),"e",1.0,"QLineEdit"))
lib.append(my_data("jv.inp","#Vstop","V",_("Stop voltage"),"e",1.0,"QLineEdit"))
lib.append(my_data("jv.inp","#Vstep","V",_("Voltage step"),"e",1.0,"QLineEdit"))

#sim_info.dat
lib.append(my_data("","#voc","V",_("V_{oc}"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#pce","Percent",_("Power conversion efficiency"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#ff","a.u.",_("Fill factor"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#Pmax","Watts",_("Max power"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#voc_nt","m^{-3}",_("Trapped electrons at Voc"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#voc_pt","m^{-3}",_("Trapped holes at Voc"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#voc_nf","m^{-3}",_("Free electrons at Voc"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#voc_pf","m^{-3}",_("Free holes at Voc"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#voc_np_tot","m^{-3}",_("Total carriers (n+p)/2 at Voc"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#voc_tau","s",_("Recombination time constant at Voc"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#voc_R","m^{-3}s^{-1}",_("Recombination rate at Voc"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#voc_J","A m^{-2}",_("Current density at Voc"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#jsc","A m^{-2}",_("J_{sc}"),"e",1.0,"QLineEdit"))

#server.inp
lib.append(my_data("","#server_cpus","au",_("Number of CPUs"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#server_stall_time","","","e",1.0,"QLineEdit"))
lib.append(my_data("","#server_exit_on_dos_error","","","e",1.0,"QLineEdit"))
lib.append(my_data("","#server_max_run_time","","","e",1.0,"QLineEdit"))
lib.append(my_data("","#server_auto_cpus","","","e",1.0,"QLineEdit"))
lib.append(my_data("","#server_min_cpus","","","e",1.0,"QLineEdit"))
lib.append(my_data("","#server_steel","","","e",1.0,"QLineEdit"))
lib.append(my_data("","#server_ip","au","Cluster IP","e",1.0,"QLineEdit"))
lib.append(my_data("","#port","au","Cluster port","e",1.0,"QLineEdit"))
lib.append(my_data("","#path_to_src","au","Path to source code","e",1.0,"QLineEdit"))
lib.append(my_data("","#path_to_libs","au","Path to compiled libs for cluster","e",1.0,"QLineEdit"))
lib.append(my_data("","#make_command","au","Make command","e",1.0,"QLineEdit"))
lib.append(my_data("","#exe_name","au","exe name","e",1.0,"QLineEdit"))

#math.inp
lib.append(my_data("math.inp","#maxelectricalitt_first","au",_("Max Electrical itterations (first step)"),"e",1.0,"QLineEdit"))
lib.append(my_data("math.inp","#electricalclamp_first","au",_("Electrical clamp (first step)"),"e",1.0,"QLineEdit"))
lib.append(my_data("math.inp","#math_electrical_error_first","au",_("Desired electrical solver error (first step)"),"e",1.0,"QLineEdit"))
lib.append(my_data("math.inp","#math_enable_pos_solver",_("True/False"),_("Enable poisson solver"),"e",1.0,"gtkswitch"))
lib.append(my_data("math.inp","#maxelectricalitt","au",_("Max electrical itterations"),"e",1.0,"QLineEdit"))
lib.append(my_data("math.inp","#electricalclamp","au",_("Electrical clamp"),"e",1.0,"QLineEdit"))
lib.append(my_data("math.inp","#posclamp","au",_("Poisson clamping"),"e",1.0,"QLineEdit"))
lib.append(my_data("math.inp","#electricalerror","au",_("Minimum electrical error"),"e",1.0,"QLineEdit"))
lib.append(my_data("math.inp","#pos_max_ittr","au",_("Poisson solver max itterations"),"e",1.0,"QLineEdit"))
lib.append(my_data("math.inp","#newton_clever_exit",_("True/False"),"Newton solver clever exit","e",1.0,"gtkswitch"))
lib.append(my_data("math.inp","#newton_min_itt","au",_("Newton minimum iterations"),"e",1.0,"QLineEdit"))
lib.append(my_data("math.inp","#solver_name",_("dll name"),_("Matrix solver to use"),"e",1.0,"QLineEdit"))
lib.append(my_data("math.inp","#newton_name",_("dll name"),_("Newton solver to use"),"e",1.0,"QLineEdit"))
lib.append(my_data("math.inp","#math_t0","au",_("Slotboom T0"),"e",1.0,"QLineEdit"))
lib.append(my_data("math.inp","#math_d0","au",_("Slotboom D0"),"e",1.0,"QLineEdit"))
lib.append(my_data("math.inp","#math_n0","au",_("Slotboom n0"),"e",1.0,"QLineEdit"))


#fit.inp
lib.append(my_data("fit.inp","#fit_error_mul","au",_("Fit error multiplyer"),"e",1.0,"QLineEdit"))
lib.append(my_data("fit.inp","#fit_randomize",_("True/False"),_("Randomize fit"),"e",1.0,"gtkswitch"))
lib.append(my_data("fit.inp","#fit_random_reset_ittr","au",_("Number of iterations between random reset"),"e",1.0,"QLineEdit"))
lib.append(my_data("fit.inp","#fit_stall_steps","au",_("Stall steps"),"e",1.0,"QLineEdit"))
lib.append(my_data("fit.inp","#fit_disable_reset_at","au",_("Disable reset at level"),"e",1.0,"QLineEdit"))
lib.append(my_data("fit.inp","#fit_converge_error","au",_("Fit define convergence"),"e",1.0,"QLineEdit"))
lib.append(my_data("fit.inp","#fit_enable_simple_reset","au",_("Enable simplex reset"),"e",1.0,"gtkswitch"))
lib.append(my_data("fit.inp","#fit_enable_simple_reset","au",_("Simplex reset steps"),"e",1.0,"gtkswitch"))
lib.append(my_data("fit.inp","#fit_method","au",_("Fiting method"),"e",1.0,"QComboBox",defaults=["simplex","newton"]))
lib.append(my_data("fit.inp","#fit_simplexmul","au",_("Start simplex step multiplication"),"e",1.0,"QLineEdit"))
lib.append(my_data("fit.inp","#fit_simplex_reset","au",_("Simplex reset steps"),"e",1.0,"QLineEdit"))

#fit?.inp
lib.append(my_data("","#fit_subtract_lowest_point",_("True/False"),_("Subtract lowest point"),"e",1.0,"gtkswitch"))

#thermal.inp
lib.append(my_data("","#Tll","Kelvin",_("Device temperature on left"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#Tlr","Kelvin",_("Device temperature on right"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#thermal",_("True/False"),_("Enable thermal solver"),"e",1.0,"gtkswitch"))
lib.append(my_data("","#thermal_l",_("True/False"),_("Lattice heat model"),"e",1.0,"gtkswitch"))
lib.append(my_data("","#thermal_e",_("True/False"),_("Electron heat model"),"e",1.0,"gtkswitch"))
lib.append(my_data("","#thermal_h",_("True/False"),_("Hole heat model"),"e",1.0,"gtkswitch"))
lib.append(my_data("","#thermal_kl","W m^{-1} C^{-1}",_("Thermal conductivity"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#Tliso",_("True/False"),_("Isothermal boundary on left"),"e",1.0,"gtkswitch"))
lib.append(my_data("","#Triso",_("True/False"),_("Isothermal boundary on right"),"e",1.0,"gtkswitch"))
lib.append(my_data("","#thermal_tau_e","s",_("Electron relaxation time"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#thermal_tau_h","s",_("Hole relaxation time"),"e",1.0,"QLineEdit"))

#dump.inp
lib.append(my_data("dump.inp","#newton_dump",_("True/False"),_("Dump from newton solver"),"e",1.0,"gtkswitch"))
lib.append(my_data("dump.inp","#plot",_("True/False"),_("Plot bands etc.. "),"e",1.0,"gtkswitch"))
lib.append(my_data("dump.inp","#dump_band_structure","","","e",1.0,"QLineEdit"))
lib.append(my_data("dump.inp","#dump_slices_by_time",_("True/False"),_("dump slices by time"),"e",1.0,"gtkswitch"))
lib.append(my_data("dump.inp","#dump_1d_slices",_("True/False"),_("Dump 1D slices"),"e",1.0,"gtkswitch"))
lib.append(my_data("dump.inp","#dump_slices",_("True/False"),_("Dump slices"),"e",1.0,"gtkswitch"))
lib.append(my_data("dump.inp","#dump_dynamic",_("True/False"),_("Dump dynamic"),"e",1.0,"gtkswitch"))
lib.append(my_data("dump.inp","#dump_zip_files",_("True/False"),_("Dump zip files"),"e",1.0,"gtkswitch"))
lib.append(my_data("dump.inp","#dump_write_out_band_structure",_("True/False"),_("Write out band structure"),"e",1.0,"gtkswitch"))
lib.append(my_data("dump.inp","#dump_iodump","","","e",1.0,"QLineEdit"))
lib.append(my_data("dump.inp","#dump_movie","","","e",1.0,"QLineEdit"))
lib.append(my_data("dump.inp","#dump_optics",_("True/False"),_("Dump optical information"),"e",1.0,"gtkswitch"))
lib.append(my_data("dump.inp","#dump_optics_verbose","","","e",1.0,"QLineEdit"))
lib.append(my_data("dump.inp","#dump_print_newtonerror",_("True/False"),_("Print newton error"),"e",1.0,"gtkswitch"))
lib.append(my_data("dump.inp","#dump_print_converge",_("True/False"),_("Print solver convergence"),"e",1.0,"gtkswitch"))
lib.append(my_data("dump.inp","#dump_write_converge",_("True/False"),_("Write newton solver convergence to disk"),"e",1.0,"gtkswitch"))
lib.append(my_data("dump.inp","#dump_print_pos_error",_("True/False"),_("Print poisson solver convergence"),"e",1.0,"gtkswitch"))
lib.append(my_data("dump.inp","#dump_pl",_("True/False"),_("Dump PL spectra"),"e",1.0,"gtkswitch"))
lib.append(my_data("dump.inp","#dump_norm_time_to_one",_("True/False"),_("Normalize output x-time to one"),"e",1.0,"gtkswitch"))
lib.append(my_data("dump.inp","#dump_built_in_voltage",_("True/False"),_("Dump the built in voltage."),"e",1.0,"gtkswitch"))
lib.append(my_data("dump.inp","#dump_optical_probe_spectrum",_("True/False"),_("Dump optical probe spectrum"),"e",1.0,"gtkswitch"))
lib.append(my_data("dump.inp","#dump_optics_summary",_("True/False"),_("Dump optical summary"),"e",1.0,"gtkswitch"))
lib.append(my_data("dump.inp","#dump_ray_trace_map",_("True/False"),_("Dump raytrace plots"),"e",1.0,"gtkswitch"))
lib.append(my_data("dump.inp","#dumpitdos","","","e",1.0,"QLineEdit"))
lib.append(my_data("dump.inp","#dump_workbook",_("True/False"),_("Dump an excel workbook for each simulation run congaing the results."),"e",1.0,"gtkswitch"))
lib.append(my_data("dump.inp","#dump_file_access_log",_("True/False"),_("Write file access log to disk."),"e",1.0,"gtkswitch"))

#led.inp
lib.append(my_data("","#led_on",_("True/False"),_("Turn on LED"),"e",1.0,"gtkswitch"))
lib.append(my_data("","#led_wavelength","m",_("LED emission wavelength"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#led_extract_eff","m",_("LED extraction efficiency"),"e",1.0,"QLineEdit"))

#device.inp
lib.append(my_data("","#invert_applied_bias","au",_("Invert applied bias"),"e",1.0,"gtkswitch"))
lib.append(my_data("","#lcharge","m^{-3}",_("Charge on left contact"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#rcharge","m^{-3}",_("Charge on right contact"),"e",1.0,"QLineEdit"))

#parasitic.inp
lib.append(my_data("parasitic.inp","#Rshunt","Ohms",_("Shunt resistance"),"e",1.0,"QLineEdit"))
lib.append(my_data("parasitic.inp","#Rcontact","Ohms",_("Series resistance"),"e",1.0,"QLineEdit"))
lib.append(my_data("parasitic.inp","#otherlayers","m",_("Other layers"),"e",1.0,"QLineEdit"))

#pl?.inp
lib.append(my_data("","#pl_enabled",_("True/False"),_("Turn on luminescence"),"e",1.0,"gtkswitch"))
lib.append(my_data("","#pl_fe_fh","0.0-1.0",_("n_{free} to p_{free} photon generation efficiency"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#pl_fe_te","0.0-1.0",_("n_{free} to n_{trap} photon generation efficiency"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#pl_te_fh","0.0-1.0",_("n_{trap} to p_{free} photon generation efficiency"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#pl_th_fe","0.0-1.0",_("p_{trap} to n_{free} photon generation efficiency"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#pl_fh_th","0.0-1.0",_("p_{free} to p_{trap} photon generation efficiency"),"e",1.0,"QLineEdit"))

#fxdomain?.inp
lib.append(my_data("","#fxdomain_Rload","Ohms",_("Load resistor"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#fxdomain_points","au",_("fx domain mesh points"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#fxdomain_n","au",_("Cycles to simulate"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#fxdomain_Vexternal","au",_("V_{external}"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#fxdomain_voltage_modulation_max","au",_("Voltage modulation depth"),"e",1.0,"QLineEdit"))

if enable_betafeatures()==True:
	lib.append(my_data("","#fxdomain_light_modulation_max","au",_("Light modulation depth"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#fxdomain_do_fit","au",_("Run fit after simulation"),"e",1.0,"gtkswitch"))
lib.append(my_data("","#periods_to_fit","au",_("Periods to fit"),"e",1.0,"QLineEdit"))




lib.append(my_data("","#I0","Apms",_("I0"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#nid","(a.u.)",_("ideality factor"),"e",1.0,"QLineEdit"))
#lib.append(my_data("","#Psun","Sun","Intensity of the sun"),"e",1.0,"QLineEdit"))

lib.append(my_data("","#saturation_n0","#saturation_n0",_("#saturation_n0"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#saturation_rate","#saturation_rate",_("#saturation_rate"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#imps_saturate","#imps_saturate",_("#imps_saturate"),"e",1.0,"QLineEdit"))


lib.append(my_data("","#simplephotondensity","m^{-2}s^{-1}",_("Photon density"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#simple_alpha","m^{-1}",_("Absorption of material"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#simmode","au",_("#simmode"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#meshpoints","au",_("Mesh points (x)"),"e",1.0,"QLineEdit"))

lib.append(my_data("","#function","au",_("#function"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#lr_pcontact",_("left/right"),_("Hole majority contact on left/right of device."),"s",1.0,"leftright"))
lib.append(my_data("","#Vexternal","V",_("start voltage"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#Vmax","V",_("Max voltage"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#Eg","eV",_("Eg"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#Xi","eV",_("Xi"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#start_stop_time","s",_("Time of pause"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#stopstart","1/0",_("Pause between iterations"),"e",1.0,"QComboBox",defaults=["1","0"]))
lib.append(my_data("","#invert_current",_("True/False"),_("Invert output"),"e",1.0,"QLineEdit"))


lib.append(my_data("","#use_capacitor","1/0",_("Use capacitor"),"e",1.0,"QComboBox",defaults=["1","0"]))


#
lib.append(my_data("","#Rshort_imps","Ohms",_("R_{short}"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#imps_sun","1=1 Sun",_("Backgroud light bias"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#imps_modulation_max","1=1 Sun",_("Modulation depth"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#imps_modulation_fx","Hz",_("Modulation frequency"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#high_sun_scale","au",_("High light multiplyer"),"e",1.0,"QLineEdit"))



lib.append(my_data("","#imps_r","Amps",_("Re(i)"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#imps_i","Amps",_("Im(i)"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#imps_Jr","Amps $m^{-2}$",_("Re(J)"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#imps_Ji","Amps $m^{-2}$",_("Im(J)"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#imps_fx","Hz",_("Frequency"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#imps_delta_i","s",_("Phase shift"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#imps_delta_g","s",_("Phase shift"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#imps_delta_phase","s",_("Phase shift"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#imps_points","s",_("points"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#imps_n","s",_("Wavelengths to simulate"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#imps_Vexternal","Volts",_("External voltage"),"e",1.0,"QLineEdit"))

lib.append(my_data("","#Cext","C",_("External C"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#Rext","Ohms",_("External R"),"e",1.0,"QLineEdit"))

lib.append(my_data("","#Rscope","Ohms",_("Resistance of scope"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#srh_bands","bands",_("Number of traps"),"s",1.0,"QLineEdit"))

lib.append(my_data("","#sun_voc_single_point","1/0",_("Single point"),"e",1.0,"QComboBox",defaults=["1","0"]))
lib.append(my_data("","#sun_voc_Psun_start","Suns",_("Start intensity"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#sun_voc_Psun_stop","Suns",_("Stop intensity"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#sun_voc_Psun_mul","au",_("step multiplier"),"e",1.0,"QLineEdit"))


lib.append(my_data("","#simplephotondensity","m^{-2}s^{-1}",_("Photon Flux"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#simple_alpha","m^{-1}",_("Absorption"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#xlen","m",_("device width"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#zlen","m",_("device breadth"),"e",1.0,"QLineEdit"))


lib.append(my_data("","#ver","","","e",1.0,"QLineEdit"))
lib.append(my_data("","#dostype","","","e",1.0,"QLineEdit"))
lib.append(my_data("","#me","","","e",1.0,"QLineEdit"))
lib.append(my_data("","#mh","","","e",1.0,"QLineEdit"))
lib.append(my_data("","#gendos","","","e",1.0,"QLineEdit"))
lib.append(my_data("","#notused","","","e",1.0,"QLineEdit"))
lib.append(my_data("","#notused","","","e",1.0,"QLineEdit"))
lib.append(my_data("","#Tstart","","","e",1.0,"QLineEdit"))
lib.append(my_data("","#Tstop","","","e",1.0,"QLineEdit"))
lib.append(my_data("","#Tpoints","","","e",1.0,"QLineEdit"))
lib.append(my_data("","#nstart","","","e",1.0,"QLineEdit"))
lib.append(my_data("","#nstop","","","e",1.0,"QLineEdit"))
lib.append(my_data("","#npoints","","","e",1.0,"QLineEdit"))
lib.append(my_data("","#nstart","","","e",1.0,"QLineEdit"))
lib.append(my_data("","#nstop","","","e",1.0,"QLineEdit"))
lib.append(my_data("","#npoints","","","e",1.0,"QLineEdit"))
lib.append(my_data("","#srhbands","","","e",1.0,"QLineEdit"))
lib.append(my_data("","#srh_start","","","e",1.0,"QLineEdit"))
lib.append(my_data("","#srhvth_e","","","e",1.0,"QLineEdit"))
lib.append(my_data("","#srhvth_h","","","e",1.0,"QLineEdit"))
lib.append(my_data("","#srh_cut","","","e",1.0,"QLineEdit"))
lib.append(my_data("","#lumodelstart","","","e",1.0,"QLineEdit"))
lib.append(my_data("","#lumodelstop","","","e",1.0,"QLineEdit"))
lib.append(my_data("","#homodelstart","","","e",1.0,"QLineEdit"))
lib.append(my_data("","#homodelstop","","","e",1.0,"QLineEdit"))
lib.append(my_data("","#gaus_mull","","","e",1.0,"QLineEdit"))
lib.append(my_data("","#Esteps","","","e",1.0,"QLineEdit"))





lib.append(my_data("","#Rshort","","","e",1.0,"QLineEdit"))
lib.append(my_data("","#Dphoton","","","e",1.0,"QLineEdit"))
lib.append(my_data("","#interfaceleft","","","e",1.0,"QLineEdit"))
lib.append(my_data("","#interfaceright","","","e",1.0,"QLineEdit"))
lib.append(my_data("","#phibleft","","","e",1.0,"QLineEdit"))
lib.append(my_data("","#phibright","","","e",1.0,"QLineEdit"))
lib.append(my_data("","#vl_e","","","e",1.0,"QLineEdit"))
lib.append(my_data("","#vl_h","","","e",1.0,"QLineEdit"))
lib.append(my_data("","#vr_e","","","e",1.0,"QLineEdit"))
lib.append(my_data("","#vr_h","","","e",1.0,"QLineEdit"))
lib.append(my_data("","#light_model","","","e",1.0,"QLineEdit"))
lib.append(my_data("","#NDfilter","","","e",1.0,"QLineEdit"))
lib.append(my_data("","#plottime","","","e",1.0,"QLineEdit"))
lib.append(my_data("","#startstop","","","e",1.0,"QLineEdit"))
lib.append(my_data("","#plotfile","","","e",1.0,"QLineEdit"))


lib.append(my_data("","#Rshort","","","e",1.0,"QLineEdit"))
lib.append(my_data("","#solve_at_Vbi","","","e",1.0,"QLineEdit"))

lib.append(my_data("","#remesh","","","e",1.0,"QLineEdit"))
lib.append(my_data("","#newmeshsize","","","e",1.0,"QLineEdit"))
lib.append(my_data("","#epitaxy","","","e",1.0,"QLineEdit"))
lib.append(my_data("","#alignmesh","","","e",1.0,"QLineEdit"))
lib.append(my_data("","#stark_start_time","","","e",1.0,"QLineEdit"))


lib.append(my_data("","#voc_J_to_Jr","au","Ratio of conduction current to recombination current","e",1.0,"QLineEdit"))

lib.append(my_data("","#voc_i","au",_("Current"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#kl_in_newton","1/0",_("Solve Kirchhoff's current law in Newton solver"),"e",1.0,"QComboBox",defaults=["1","0"]))

lib.append(my_data("","#simplexmul","au","simplex mull","e",1.0,"QLineEdit"))
lib.append(my_data("","#simplex_reset","au","Reset steps","e",1.0,"QLineEdit"))

lib.append(my_data("","#max_nfree_to_ptrap","m^{-3}s^{-1}","nfree_to_ptrap","e",1.0,"QLineEdit"))
lib.append(my_data("","#max_pfree_to_ntrap","m^{-3}s^{-1}","max_pfree_to_ntrap","e",1.0,"QLineEdit"))
lib.append(my_data("","#max_nrelax","m^{-3}s^{-1}","max_nrelax","e",1.0,"QLineEdit"))
lib.append(my_data("","#max_prelax","m^{-3}s^{-1}","max_prelax","e",1.0,"QLineEdit"))

lib.append(my_data("","#max_nfree","m^{-3}","max_nfree","e",1.0,"QLineEdit"))
lib.append(my_data("","#max_pfree","m^{-3}","max_pfree","e",1.0,"QLineEdit"))
lib.append(my_data("","#max_ntrap","m^{-3}","max_ntrap","e",1.0,"QLineEdit"))
lib.append(my_data("","#max_ptrap","m^{-3}","max_ptrap","e",1.0,"QLineEdit"))
lib.append(my_data("","#alpha_max_reduction","m^{-1}","alpha_max_reduction","e",1.0,"QLineEdit"))
lib.append(my_data("","#alpha_max_increase","m^{-1}","alpha_max_increase","e",1.0,"QLineEdit"))

lib.append(my_data("","#srh_n_r1","m^{-3}s^{-1}","srh electron rate 1","e",1.0,"QLineEdit"))
lib.append(my_data("","#srh_n_r2","m^{-3}s^{-1}","srh electron rate 2","e",1.0,"QLineEdit"))
lib.append(my_data("","#srh_n_r3","m^{-3}s^{-1}","srh electron rate 3","e",1.0,"QLineEdit"))
lib.append(my_data("","#srh_n_r4","m^{-3}s^{-1}","srh electron rate 4","e",1.0,"QLineEdit"))

lib.append(my_data("","#srh_p_r1","m^{-3}s^{-1}","srh hole rate 1","e",1.0,"QLineEdit"))
lib.append(my_data("","#srh_p_r2","m^{-3}s^{-1}","srh hole rate 2","e",1.0,"QLineEdit"))
lib.append(my_data("","#srh_p_r3","m^{-3}s^{-1}","srh hole rate 3","e",1.0,"QLineEdit"))
lib.append(my_data("","#srh_p_r4","m^{-3}s^{-1}","srh hole rate 4","e",1.0,"QLineEdit"))

lib.append(my_data("","#band_bend_max","percent","band bend max","e",1.0,"QLineEdit"))

#config.inp
lib.append(my_data("","#gui_config_3d_enabled",_("True/False"),_("Enable 3d effects"),"e",1.0,"gtkswitch"))
lib.append(my_data("","#gui_use_icon_theme",_("True/False"),_("Use icons from OS"),"e",1.0,"gtkswitch"))

#fit
lib.append(my_data("","#enabled",_("True/False"),_("Enable fit"),"e",1.0,"gtkswitch"))
lib.append(my_data("","#time_shift","s","time shift","e",1.0,"QLineEdit"))
lib.append(my_data("","#start","s","start","e",1.0,"QLineEdit"))
lib.append(my_data("","#stop","s","stop","e",1.0,"QLineEdit"))
lib.append(my_data("","#log_x",_("True/False"),_("log x"),"e",1.0,"gtkswitch"))
lib.append(my_data("","#sim_data",_("filename"),"Fit file name","e",1.0,"QLineEdit"))
lib.append(my_data("","#fit_invert_simulation_y",_("True/False"),_("Invert simulated data (y)"),"e",1.0,"gtkswitch"))


#
lib.append(my_data("","#layer0","m",_("Active layer width"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#stark_saturate","au","Stark saturate","e",1.0,"QLineEdit"))

lib.append(my_data("","#n_mul","au","n mul","e",1.0,"QLineEdit"))
lib.append(my_data("","#alpha_mul","m^{-1}","Alpha mul","e",1.0,"QLineEdit"))

lib.append(my_data("","#stark_point0","au","DR/R","e",1.0,"QLineEdit"))
lib.append(my_data("","#stark_point1","au","DR/R","e",1.0,"QLineEdit"))
lib.append(my_data("","#stark_point2","au","DR/R","e",1.0,"QLineEdit"))
lib.append(my_data("","#stark_point3","au","DR/R","e",1.0,"QLineEdit"))
lib.append(my_data("","#stark_point4","au","DR/R","e",1.0,"QLineEdit"))
lib.append(my_data("","#stark_subtracted_value","s","subtracted value","e",1.0,"QLineEdit"))
lib.append(my_data("","#light_energy","eV","Energy","e",1.0,"QLineEdit"))
lib.append(my_data("","#sim_id","au","sim id","e",1.0,"QLineEdit"))


lib.append(my_data("","#Rload","Ohms",_("External load resistor"),"e",1.0,"QLineEdit"))
lib.append(my_data("","#pulse_shift","s","Shift of TPC signal","e",1.0,"QLineEdit"))

lib.append(my_data("","#flip_field","au",_("Filp the opticl field"),"e",1.0,"QComboBox",defaults=["1","0"]))


class tokens:

	def find(self,token):
		global lib
		for i in range(0, len(lib)):
			if lib[i].token==token.strip():
				if lib[i].units=="" and lib[i].info=="":
					return False
				else:
					return lib[i]

		#sys.stdout.write("Add -> lib.append(my_data(\""+token+"\",\"\",\"\",[\"text\"]))\n")
		return False

	def dump_lib(self):
		global lib
		for i in range(0, len(lib)):
			print(">",lib[i].token,"<>",lib[i].info,"<")
			
	def get_lib(self):
		global lib
		return lib
