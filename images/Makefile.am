OBJS = perovskite.png parasitic.png hints.png man.png license.png youtube.png export.png cigs.png alpha.png n.png hdd_high.png hdd_low.png hdd_custom.png fit.png duplicate.png vars.png view-refresh.png asi.png forward2.png oled.png star.png book.png forward.png optics.png scale.png start.png close.png pause.png scan2.png utilities-terminal.png inp_file.png pen.png scan.png time.png dat_file.png laser.png play.png select.png video.png device.png lasers.png plot.png server.png plot_time.png  si.png www.png folder.png psi.png spectra_file.png doping.png mesh.png qe.png icon.png spectrum.png tab.png list-add.png document-save-as.png document-save.png document-new.png list-remove.png edit-delete.png clone.png rename.png go-down.png document-open.png edit-undo.png server_get_data.png server_get_info.png server_copy_src.png server_make.png server_clean.png off.png sync.png server_jobs.png ball_green.png ball_red.png ball_green4.png ball_red4.png jv.png preferences-system.png help.png import.png contact.png 1d.png 2d.png 3d.png go-previous.png go-next.png go-up.png user-home.png xls_file.png info_file.png rotate.png ofet.png p3ht_pcbm.png warning.png organic_material.png cost.png connected.png not_connected.png ref.png
#convert -transparent white  $< $@

all: $(OBJS)
	echo $(OBJS)

.svg.png:
	@IMAGE_CONVERT_FULL_SIZE@ $@
	@IMAGE_CONVERT_32@ 32_$@
	@IMAGE_CONVERT_16@ 16_$@

	
clean:
	rm 16_*.png 32_*.png $(OBJS) -f


