SRCS=$(wildcard *.svg)

OBJS=$(SRCS:.svg=.png)

all: $(OBJS)
	echo $(OBJS)


.svg.png:

	rsvg-convert -w 48 -h 32 $< > $@

clean:
	rm *.png -f


