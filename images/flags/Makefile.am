SRCS=$(wildcard *.svg)


OBJS=$(SRCS:.svg=.png)

all: $(OBJS)
	echo $(OBJS)

make_dir:
	mkdir -p ../48x32

%.png : %.svg make_dir
	@IMAGE_CONVERT_FLAG@ ../48x32/$@
	
clean:
	rm ../48x32 -rf


