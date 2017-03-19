SRCS=$(wildcard *.svg)

OBJS=$(SRCS:.svg=.png)

all: $(OBJS)
	echo $(OBJS)


.svg.png:

	@IMAGE_CONVERT_FLAG@ $@

clean:
	rm *.png -f


