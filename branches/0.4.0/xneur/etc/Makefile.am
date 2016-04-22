## Process this file with automake to produce Makefile.in

SUBDIRS = 	en \
		ua \
		ru

etcdir = $(sysconfdir)/xneur
etc_DATA = xneurrc xneurrc-ua

EXTRA_DIST = $(etc_DATA)

install-data-hook:
	rm ~/.xneur.backup -Rf
	if test -d ~/.xneur; then	\
		mv ~/.xneur ~/.xneur.backup -f;	\
		chmod -R a+r+w ~/.xneur.backup;	\
		rm ~/.xneur -rf;		\
	fi; 
	cp $(DESTDIR)$(sysconfdir)/xneur ~/.xneur -rf;
	chmod -R a+r+w ~/.xneur ;
