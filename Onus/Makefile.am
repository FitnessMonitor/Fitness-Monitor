## Process this file with automake to produce Makefile.in
## Created by Anjuta

SUBDIRS = src 

onusdocdir = ${prefix}/doc/onus
onusdoc_DATA = \
	README\
	COPYING\
	AUTHORS\
	ChangeLog\
	INSTALL\
	NEWS


EXTRA_DIST = $(onusdoc_DATA)


# Remove doc directory on uninstall
uninstall-local:
	-rm -r $(onusdocdir)
