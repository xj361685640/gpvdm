SRCS=$(wildcard *.svg)

OBJS=$(SRCS:.svg=.png)

all: $(OBJS)
	echo $(OBJS)


.svg.png:

	rsvg-convert -w 32 -h 32 $< > $@

clean:
	rm *.png -f


