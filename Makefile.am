SUBDIRS = src

clean: distclean
	-rm *.m4
	-rm autom4te.cache/*
	-rm -r autom4te.cache
	-rm *.tar.gz
	-find . -name "*.in" -or -name "*.trs" -or -name "*.log" -delete
	-rm configure
	-rm conf/*
	-rm -r conf
	-rm config.h.in~
	-rm src/libregex src/libregextest