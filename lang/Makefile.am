OBJS= ./de_DE/LC_MESSAGES/gpvdm.mo

all: $(OBJS)
	echo "po files generated"

./de_DE/LC_MESSAGES/gpvdm.mo : de.po
	mkdir ./de_DE
	mkdir ./de_DE/LC_MESSAGES
	msgfmt  de.po -o ./de_DE/LC_MESSAGES/gpvdm.mo

clean:
	rm de_DE -rf
	rm *.mo -f
	rm gpvdm.pot -f
