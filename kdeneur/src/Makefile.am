INCLUDES = 	-DPACKAGE_PIXMAPS_DIR=\""${pkgdatadir}/pixmaps"\" 	\
		-DLOCALEDIR=\""${pkgdatadir}/i18n"\" 				\
		-DXNEUR_PLUGIN_DIR=\""@libdir@/xneur"\"	\
		-DSHAREDIR=\""${pkgdatadir}"\" 				
		
bin_PROGRAMS = kdeneur

kdeneur_SOURCES =	\	
		addrules.cpp \
		adduseraction.cpp \
		edithotkey.cpp \
		frmabout.cpp \
		frmaddabbreviature.cpp \
		frmsettings.cpp \
		getnameapp.cpp \
		kdeneur.cpp \
		kxneurtray.cpp \
		main.cpp \
		ruleschange.cpp \
		xkb.c \
		xneurconfig.cpp \
		xkb.h	\
		xneurconfig.h \
		tabbar.h

nodist_kdeneur_SOURCES =	\	
		moc_addrules.cpp \
		moc_adduseraction.cpp \
		moc_edithotkey.cpp \
		moc_frmabout.cpp \
		moc_frmaddabbreviature.cpp \
		moc_frmsettings.cpp \
		moc_getnameapp.cpp \
		moc_kdeneur.cpp \
		moc_kxneurtray.cpp \
		moc_ruleschange.cpp \
		moc_xneurconfig.cpp \
		ui_addrules.h \
		ui_adduseraction.h 	\
		ui_edithotkey.h	\
		ui_frmabout.h	\
		ui_frmaddabbreviature.h	\
		ui_frmsettings.h	\
		ui_getnameapp.h	\
		ui_ruleschange.h	

EXTRA_DIST = \
	$(nodist_kdeneur_SOURCES:moc_%.cpp=%.h) 

# This rule lets GNU make create any moc_*.cpp from the equivalent *.h
moc_%.cpp: %.h
	moc-qt4 $< -o $@

ui_%.h: %.ui
	uic-qt4 $< -o $@

# Adjust this line according to your resources.qrc
resources.cpp : resources.qrc octave-gui.png
	rcc resources.qrc -o resources.cpp

# This line ensures that generated moc_*.cpp and resources.cpp files are
# deleted when we make clean.
CLEANFILES = $(filter moc_%.cpp,$(nodist_kdeneur_SOURCES)) resources.cpp $(filter ui_%.h,$(nodist_kdeneur_SOURCES))

kdeneur_CXXFLAGS = @X11_CFLAGS@ @QTCORE_CFLAGS@ @QTGUI_CFLAGS@ @QTUITOOLS_CFLAGS@ @QTDBUS_CFLAGS@ @XNEURCONF_CFLAGS@ @DEFAULT_CFLAGS@
kdeneur_LDFLAGS = @X11_LIBS@ @QTCORE_LIBS@ @QTGUI_LIBS@ @QTUITOOLS_LIBS@ @QTDBUS_LIBS@ @XNEURCONF_LIBS@ 

