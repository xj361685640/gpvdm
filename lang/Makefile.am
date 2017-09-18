OBJS= ./de_DE/LC_MESSAGES/gpvdm.mo ./zh_CN/LC_MESSAGES/gpvdm.mo ./fa_IR/LC_MESSAGES/gpvdm.mo ./ru_RU/LC_MESSAGES/gpvdm.mo

all: $(OBJS)
	xgettext --language=Python --keyword=_ --output=gpvdm.pot `find ../ \( -name "*.py" -or -name "*.c" \)|grep -v pub` --msgid-bugs-address=roderick.mackenzie@nottingham.ac.uk  --copyright-holder="Roderick C. I. MacKenzie" --package-name=gpvdm --package-version=4.97

./%/LC_MESSAGES/gpvdm.mo : %.po
	mkdir $(basename $<)
	mkdir $(basename $<)/LC_MESSAGES
	cp $(basename $<).ref $(basename $<)/LC_MESSAGES/ref.ref
	msgfmt  $< -o $(basename $<)/LC_MESSAGES/gpvdm.mo


clean:
	rm de_DE zh_CN fa_IR ru_RU -rf
	rm *.mo -f
	rm gpvdm.pot -f
