## Process this file with automake to produce Makefile.in

SUBDIRS = en ru

etcdir = $(sysconfdir)/xneur
etc_DATA = xneurrc

EXTRA_DIST = $(etc_DATA)

install-data-hook:
	chmod -R a+r+w $(sysconfdir)/xneur
