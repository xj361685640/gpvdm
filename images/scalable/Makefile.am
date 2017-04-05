OBJS = pdf.png perovskite.png parasitic.png hints.png internet-web-browser.png license.png youtube.png document-export.png cigs.png alpha.png n.png hdd_high.png hdd_low.png hdd_custom.png fit.png duplicate.png vars.png view-refresh.png asi.png forward2.png oled.png star.png book.png forward.png optics.png scale.png start.png close.png media-playback-pause.png scan2.png utilities-terminal.png text-x-generic.png pen.png scan.png time.png dat_file.png laser.png media-playback-start.png select.png video.png device.png lasers.png plot.png server.png plot_time.png  si.png www.png folder.png psi.png spectra_file.png doping.png mesh.png qe.png icon.png spectrum.png tab.png list-add.png document-save-as.png document-save.png document-new.png list-remove.png edit-delete.png clone.png rename.png go-down.png document-open.png edit-undo.png server_get_data.png server_get_info.png server_copy_src.png server_make.png server_clean.png off.png sync.png server_jobs.png ball_green.png ball_red.png ball_green4.png ball_red4.png jv.png preferences-system.png help.png import.png contact.png 1d.png 2d.png 3d.png go-previous.png go-next.png go-up.png user-home.png wps-office-xls.png info_file.png rotate.png ofet.png p3ht_pcbm.png warning.png organic_material.png cost.png connected.png not_connected.png ref.png
#convert -transparent white  $< $@



all: $(OBJS)
	echo $(OBJS)

make_dir:
	mkdir -p ../16x16
	mkdir -p ../32x32
	mkdir -p ../64x64

%.png : %.svg make_dir
	@IMAGE_CONVERT_16@ ../16x16/$@
	@IMAGE_CONVERT_32@ ../32x32/$@
	@IMAGE_CONVERT_64@ ../64x64/$@

	
clean:
	rm ../16x16 -rf
	rm ../32x32 -rf
	rm ../64x64 -rf

