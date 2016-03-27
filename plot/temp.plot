set terminal wxt title 'Sun - Voc - www.opvdm.com'
set key top left
set xlabel 'Intensity'
set ylabel 'Voc (Volts)
set format y '%.1le%T'
set logscale x
plot \
'/home/rod/brian/play//sun_voc.dat' using ($1*1.0):($2*1.0) with lp lw 2 title 'play'