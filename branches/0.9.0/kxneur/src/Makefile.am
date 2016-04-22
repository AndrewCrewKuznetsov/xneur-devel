# set the include path for X, qt and KDE
INCLUDES = $(all_includes)

# these are the headers for your project
noinst_HEADERS = kxneur.h kxneurtray.h kxneurconf.h kxneursettings.h \
		kxnlistbox.h kxncombobox.h kxnkeyboard.h key_convert.h

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/kxneur.pot

KDE_ICON = AUTO

#########################################################################
# APPLICATION SECTION
#########################################################################
# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables
bin_PROGRAMS = kxneur

# the application source, library search path, and link libraries
kxneur_SOURCES = main.cpp kxneur.cpp kxneurtray.cpp kxneurconf.cpp \
		kxneursettings.cpp kxnlistbox.cpp kxncombobox.cpp kxnkeyboard.cpp key_convert.cpp
kxneur_LDFLAGS = $(KDE_RPATH) $(all_libraries)
kxneur_LDADD = -lxnconfig $(LIB_KDEUI) $(LIB_KFILE)

# this is where the desktop file will go 
shelldesktopdir = $(kde_appsdir)/Utilities
shelldesktop_DATA = kxneur.desktop

# this is where the shell's XML-GUI resource file goes
shellrcdir = $(kde_datadir)/kxneur

autostartdir = $(prefix)/share/autostart
autostart_DATA =  kxneur.desktop

kxneurdatadir = $(kde_datadir)/kxneur
kxneurdata_DATA = langs
