OBJS= ./de_DE/LC_MESSAGES/gpvdm.mo

all: $(OBJS)
	xgettext --language=Python --keyword=_ --output=gpvdm.pot `find ../ \( -name "*.py" -or -name "*.c" \)|grep -v pub` --msgid-bugs-address=roderick.mackenzie@nottingham.ac.uk  --copyright-holder="Roderick C. I. MacKenzie" --package-name=gpvdm --package-version=4.80

./de_DE/LC_MESSAGES/gpvdm.mo : de.po
	mkdir ./de_DE
	mkdir ./de_DE/LC_MESSAGES
	msgfmt  de.po -o ./de_DE/LC_MESSAGES/gpvdm.mo

clean:
	rm de_DE -rf
	rm *.mo -f
	rm gpvdm.pot -f
