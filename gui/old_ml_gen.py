
					
					ret=get_vectors(full_name,"0.0","measure_jv.dat",dolog=True)
					if ret==False:
						print("ml_input_jv_dark")
						break
					v=v+ret+"\n"

					v=v+"#ml_input_jv_light\n"
					ret=get_vectors(full_name,"1.0","measure_jv.dat",div=1e2)
					if ret==False:
						print("ml_input_jv_light")
						break
					v=v+ret+"\n"

					v=v+"#ml_input_tpc_0v_dark\n"
					ret=get_vectors(full_name,"TPC_0V_DARK","measure_tpc.dat",fabs=True,dolog=True)
					if ret==False:
						print("#ml_input_tpc_0v_dark")
						break
					v=v+ret+"\n"

					v=v+"#ml_input_tpc_400mV_dark\n"
					ret=get_vectors(full_name,"TPC_400mV_DARK","measure_tpc.dat",fabs=True,dolog=True)
					if ret==False:
						print("#ml_input_tpc_400mV_dark")
						break
					v=v+ret+"\n"

					v=v+"#ml_input_tpc_0v_dark_norm\n"
					ret=get_vectors(full_name,"TPC_0V_DARK","measure_tpc.dat",fabs=True,dolog=True,do_norm=True)
					if ret==False:
						print("#ml_input_tpc_0v_dark")
						break
					v=v+ret+"\n"

					v=v+"#ml_input_tpc_400mV_dark_norm\n"
					ret=get_vectors(full_name,"TPC_400mV_DARK","measure_tpc.dat",fabs=True,dolog=True,do_norm=True)
					if ret==False:
						print("#ml_input_tpc_400mV_dark")
						break
					v=v+ret+"\n"

					#v=v+"#ml_input_tpc_neg\n"
					#ret=get_vectors(full_name,"TPC","measure_tpc.dat",fabs=True,dolog=True)
					#if ret==False:
					#	print("#ml_input_tpc_neg")
					#	break
					#v=v+ret+"\n"

					#v=v+"#ml_input_tpc\n"
					#ret=get_vectors(full_name,"TPC_0","measure_tpc.dat",fabs=True,dolog=True)
					#if ret==False:
					#	print("#ml_input_tpc")
					#	break
					#v=v+ret+"\n"

					#v=v+"#ml_input_tpc_neg_norm\n"
					#ret=get_vectors(full_name,"TPC","measure_tpc.dat",fabs=True,dolog=True,do_norm=True)
					#if ret==False:
					#	print("#ml_input_tpc_neg_norm")
					#	break
					#v=v+ret+"\n"

					#v=v+"#ml_input_tpc_norm\n"
					#ret=get_vectors(full_name,"TPC_0","measure_tpc.dat",fabs=True,dolog=True,do_norm=True)
					#if ret==False:
					#	print("#ml_input_tpc_norm")
					#	break
					#v=v+ret+"\n"

					#v=v+"#ml_input_tpc_ideal\n"
					#ret=get_vectors(full_name,"TPC_ideal","measure_tpc.dat",fabs=True,dolog=True)
					#if ret==False:
					#	print("#ml_input_tpc_ideal")
					#	break
					#v=v+ret+"\n"

					#v=v+"#ml_input_tpc_ideal_norm\n"
					#ret=get_vectors(full_name,"TPC_ideal","measure_tpc.dat",fabs=True,dolog=True,do_norm=True)
					#if ret==False:
					#	print("#ml_input_tpc_ideal_norm")
					#	break
					#v=v+ret+"\n"

					#v=v+"#ml_input_tpv\n"
					#ret=get_vectors(full_name,"TPV","measure_tpv.dat",fabs=True)
					#if ret==False:
					#	print("#ml_input_tpv")
					#	break
					#v=v+ret+"\n"

					#v=v+"#ml_input_celiv\n"
					#ret=get_vectors(full_name,"CELIV","measure_celiv.dat",fabs=True)
					#if ret==False:
					#	break
					#v=v+ret+"\n"

					#v=v+get_vectors_binary(full_name,"1.0")

