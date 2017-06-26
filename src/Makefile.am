#
# Makefile.am
#
# Written by: Francesco Salvestrini <f DOT salvestrini AT nextworks DOT it>
#

EXTRA_DIST                         =
DISTCLEANFILES                     =
bin_PROGRAMS                       =
AM_INSTALLCHECK_STD_OPTIONS_EXEMPT =

rina_clocksincro_SOURCES  =				\
	clocksincro.cc		clocksincro.h	\
	simple_server.cc		simple_server.h	\
	simple_client.cc		simple_client.h	\
    simple_ap.cc		simple_ap.h	\
    clocksincro_main.cc

rina_clocksincro_LDADD    = $(LIBRINA_LIBS)		\
		     -lrt 

rina_clocksincro_CPPFLAGS =				\
	$(LIBRINA_CFLAGS)			\
	$(CPPFLAGS_EXTRA)			\
        $(BOOST_CPPFLAGS)			\
	-I$(srcdir)/tclap

rina_clocksincro_CXXFLAGS =				\
	$(CPPFLAGS_EXTRA)

bin_PROGRAMS                       += rina-clocksincro
AM_INSTALLCHECK_STD_OPTIONS_EXEMPT += rina-clocksincro
