#
# Makefile.am
#

EXTRA_DIST                         =
DISTCLEANFILES                     =
bin_PROGRAMS                       =
AM_INSTALLCHECK_STD_OPTIONS_EXEMPT =


#Ping
rina_ping_SOURCES  =				\
	ping.cc		ping.h	\
	simple_server.cc		simple_server.h	\
	simple_client.cc		simple_client.h	\
    simple_ap.cc		simple_ap.h	\
    ping_main.cc

rina_ping_LDADD    = $(LIBRINA_LIBS)		\
		     -lrt 

rina_ping_CPPFLAGS =				\
	$(LIBRINA_CFLAGS)			\
	$(CPPFLAGS_EXTRA)			\
        $(BOOST_CPPFLAGS)			\
	-I$(srcdir)/tclap

rina_ping_CXXFLAGS =				\
	$(CPPFLAGS_EXTRA)

#Clocksincro
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

#Server
rina_server_SOURCES  =				\
	server.cc		server.h	\
	simple_server.cc		simple_server.h	\
    simple_ap.cc		simple_ap.h	\
    common.cc	\
    server_main.cc

rina_server_LDADD    = $(LIBRINA_LIBS)		\
		     -lrt 

rina_server_CPPFLAGS =				\
	$(LIBRINA_CFLAGS)			\
	$(CPPFLAGS_EXTRA)			\
        $(BOOST_CPPFLAGS)			\
	-I$(srcdir)/tclap

rina_server_CXXFLAGS =				\
	$(CPPFLAGS_EXTRA)

	
#Voice
rina_voice_SOURCES  =				\
    common.cc				common.h	\
    simple_ap.cc			simple_ap.h	\
	simple_client.cc		simple_client.h	\
    base_client.cc			base_client.h	\
	voice_client.cc			voice_client.h	\
    voice_main.cc
	
rina_voice_LDADD    = $(LIBRINA_LIBS)		\
		     -lrt 

rina_voice_CPPFLAGS =				\
	$(LIBRINA_CFLAGS)			\
	$(CPPFLAGS_EXTRA)			\
        $(BOOST_CPPFLAGS)			\
	-I$(srcdir)/tclap

rina_voice_CXXFLAGS =				\
	$(CPPFLAGS_EXTRA)
	
#Data
rina_data_SOURCES  =				\
    common.cc				common.h	\
    simple_ap.cc			simple_ap.h	\
	simple_client.cc		simple_client.h	\
    base_client.cc			base_client.h	\
	data_client.cc			data_client.h	\
    data_main.cc
	
rina_data_LDADD    = $(LIBRINA_LIBS)		\
		     -lrt 

rina_data_CPPFLAGS =				\
	$(LIBRINA_CFLAGS)			\
	$(CPPFLAGS_EXTRA)			\
        $(BOOST_CPPFLAGS)			\
	-I$(srcdir)/tclap

rina_data_CXXFLAGS =				\
	$(CPPFLAGS_EXTRA)
	
#Readrop
rina_readrop_SOURCES  =				\
	simple_server.cc		simple_server.h	\
    simple_ap.cc		simple_ap.h	\
    readrop.cc		readrop.h	\
    readrop_main.cc
	
rina_readrop_LDADD    = $(LIBRINA_LIBS)		\
		     -lrt 

rina_readrop_CPPFLAGS =				\
	$(LIBRINA_CFLAGS)			\
	$(CPPFLAGS_EXTRA)			\
        $(BOOST_CPPFLAGS)			\
	-I$(srcdir)/tclap

rina_readrop_CXXFLAGS =				\
	$(CPPFLAGS_EXTRA)
	
#Voice
rina_bvoice_SOURCES  =				\
    simple_ap.cc \
	simple_client.cc \
	bach_voice.cc \
    bvoice_main.cc
	
rina_bvoice_LDADD    = $(LIBRINA_LIBS)		\
		     -lrt 

rina_bvoice_CPPFLAGS =				\
	$(LIBRINA_CFLAGS)			\
	$(CPPFLAGS_EXTRA)			\
        $(BOOST_CPPFLAGS)			\
	-I$(srcdir)/tclap

rina_bvoice_CXXFLAGS =				\
	$(CPPFLAGS_EXTRA)
	
## BINs

bin_PROGRAMS                       += rina-ping
bin_PROGRAMS                       += rina-clocksincro
bin_PROGRAMS                       += rina-server
bin_PROGRAMS                       += rina-voice
bin_PROGRAMS                       += rina-data
bin_PROGRAMS                       += rina-readrop	
bin_PROGRAMS                       += rina-bvoice				

AM_INSTALLCHECK_STD_OPTIONS_EXEMPT += rina-ping
AM_INSTALLCHECK_STD_OPTIONS_EXEMPT += rina-clocksincro
AM_INSTALLCHECK_STD_OPTIONS_EXEMPT += rina-server
AM_INSTALLCHECK_STD_OPTIONS_EXEMPT += rina-voice
AM_INSTALLCHECK_STD_OPTIONS_EXEMPT += rina-data
AM_INSTALLCHECK_STD_OPTIONS_EXEMPT += rina-readrop
AM_INSTALLCHECK_STD_OPTIONS_EXEMPT += rina-bvoice