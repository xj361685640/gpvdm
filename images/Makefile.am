OBJS = fit.png refresh.png asi.png forward2.png oled.png star.png book.png forward.png optics.png scale.png start.png close.png pause.png scan2.png command.png inp_file.png pen.png scan.png time.png dat_file.png laser.png play.png select.png video.png device.png lasers.png plot.png server.png plot_time.png  si.png www.png dir_file.png psi.png spectra_file.png doping.png mesh.png qe.png icon.png spectrum.png tab.png add.png save.png new.png minus.png delete.png clone.png rename.png down.png
#convert -transparent white  $< $@
all: $(OBJS)
	echo $(OBJS)

.svg.png:

	rsvg-convert $< > $@
clean:
	rm $(OBJS) -f

