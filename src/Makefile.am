#
# Makefile.am
#

EXTRA_DIST                         =
DISTCLEANFILES                     =
bin_PROGRAMS                       =
AM_INSTALLCHECK_STD_OPTIONS_EXEMPT =


#Ping
ping_SOURCES  =				\
	ping.cc	\
	simple_server.cc	\
	simple_client.cc	\
    simple_ap.cc	\
    ping_main.cc

ping_LDADD    = $(LIBRINA_LIBS) -lrt 
ping_CPPFLAGS =	 $(LIBRINA_CFLAGS) $(CPPFLAGS_EXTRA) -I$(srcdir)/tclap
ping_CXXFLAGS =	$(CPPFLAGS_EXTRA)

#Clocksincro
clocksincro_SOURCES  =				\
	clocksincro.cc	\
	simple_server.cc	\
	simple_client.cc	\
    simple_ap.cc	\
    clocksincro_main.cc

clocksincro_LDADD    = $(LIBRINA_LIBS) -lrt 
clocksincro_CPPFLAGS =	 $(LIBRINA_CFLAGS) $(CPPFLAGS_EXTRA) -I$(srcdir)/tclap
clocksincro_CXXFLAGS =	$(CPPFLAGS_EXTRA)

#Server
server_SOURCES  =				\
	server.cc	\
	simple_server.cc	\
    simple_ap.cc	\
    common.cc	\
    server_main.cc

server_LDADD    = $(LIBRINA_LIBS) -lrt 
server_CPPFLAGS =	 $(LIBRINA_CFLAGS) $(CPPFLAGS_EXTRA) -I$(srcdir)/tclap
server_CXXFLAGS =	$(CPPFLAGS_EXTRA)

	
#Voice
voice_SOURCES  =				\
    common.cc	\
    simple_ap.cc	\
	simple_client.cc	\
    base_client.cc	\
	voice_client.cc	\
    voice_main.cc
	
voice_LDADD    = $(LIBRINA_LIBS) -lrt 
voice_CPPFLAGS =	 $(LIBRINA_CFLAGS) $(CPPFLAGS_EXTRA) -I$(srcdir)/tclap
voice_CXXFLAGS =	$(CPPFLAGS_EXTRA)
	
#Data
data_SOURCES  =				\
    common.cc	\
    simple_ap.cc	\
	simple_client.cc	\
    base_client.cc	\
	data_client.cc	\
    data_main.cc
	
data_LDADD    = $(LIBRINA_LIBS) -lrt 
data_CPPFLAGS =	 $(LIBRINA_CFLAGS) $(CPPFLAGS_EXTRA) -I$(srcdir)/tclap
data_CXXFLAGS =	$(CPPFLAGS_EXTRA)
	
#Readrop
readrop_SOURCES  =				\
	simple_server.cc	\
    simple_ap.cc	\
    readrop.cc	\
    bcommon.cc	\
    readrop_main.cc
	
readrop_LDADD    = $(LIBRINA_LIBS) -lrt 
readrop_CPPFLAGS =	 $(LIBRINA_CFLAGS) $(CPPFLAGS_EXTRA) -I$(srcdir)/tclap
readrop_CXXFLAGS =	$(CPPFLAGS_EXTRA)
	
#Logger
logger_SOURCES  =				\
	simple_server.cc	\
    simple_ap.cc	\
    logger.cc	\
    bcommon.cc	\
    logger_main.cc
	
logger_LDADD    = $(LIBRINA_LIBS) -lrt 
logger_CPPFLAGS =	 $(LIBRINA_CFLAGS) $(CPPFLAGS_EXTRA) -I$(srcdir)/tclap
logger_CXXFLAGS =	$(CPPFLAGS_EXTRA)
	
#DataLogger
datalogger_SOURCES  =				\
	simple_server.cc	\
    simple_ap.cc	\
    datalogger.cc	\
    bcommon.cc	\
    datalogger_main.cc
	
datalogger_LDADD    = $(LIBRINA_LIBS) -lrt 
datalogger_CPPFLAGS =	 $(LIBRINA_CFLAGS) $(CPPFLAGS_EXTRA) -I$(srcdir)/tclap
datalogger_CXXFLAGS =	$(CPPFLAGS_EXTRA)
	
#VideoLogger
videologger_SOURCES  =				\
	simple_server.cc	\
    simple_ap.cc	\
    videologger.cc	\
    bcommon.cc	\
    videologger_main.cc
	
videologger_LDADD    = $(LIBRINA_LIBS) -lrt 
videologger_CPPFLAGS =	 $(LIBRINA_CFLAGS) $(CPPFLAGS_EXTRA) -I$(srcdir)/tclap
videologger_CXXFLAGS =	$(CPPFLAGS_EXTRA)
	
#bVoice
bvoice_SOURCES  =				\
    simple_ap.cc \
	simple_client.cc \
	bach_voice.cc \
    bcommon.cc	\
    bvoice_main.cc
	
bvoice_LDADD    = $(LIBRINA_LIBS) -lrt 
bvoice_CPPFLAGS =	 $(LIBRINA_CFLAGS) $(CPPFLAGS_EXTRA) -I$(srcdir)/tclap
bvoice_CXXFLAGS =	$(CPPFLAGS_EXTRA)
	
#dbVoice
dbvoice_SOURCES  =				\
    simple_ap.cc \
	simple_client.cc \
	dbach_voice.cc \
    bcommon.cc	\
    dbvoice_main.cc
	
dbvoice_LDADD    = $(LIBRINA_LIBS) -lrt 
dbvoice_CPPFLAGS =	 $(LIBRINA_CFLAGS) $(CPPFLAGS_EXTRA) -I$(srcdir)/tclap
dbvoice_CXXFLAGS =	$(CPPFLAGS_EXTRA)
	
#bCRate
bcrate_SOURCES  =				\
    simple_ap.cc \
	simple_client.cc \
	batch_crate.cc \
    bcommon.cc	\
    bcrate_main.cc
	
bcrate_LDADD    = $(LIBRINA_LIBS) -lrt 
bcrate_CPPFLAGS =	 $(LIBRINA_CFLAGS) $(CPPFLAGS_EXTRA) -I$(srcdir)/tclap
bcrate_CXXFLAGS =	$(CPPFLAGS_EXTRA)
	
## BINs

bin_PROGRAMS                       += ping
bin_PROGRAMS                       += clocksincro
bin_PROGRAMS                       += server
bin_PROGRAMS                       += voice
bin_PROGRAMS                       += data
bin_PROGRAMS                       += readrop	
bin_PROGRAMS                       += bvoice	
bin_PROGRAMS                       += dbvoice		
bin_PROGRAMS                       += bcrate		
bin_PROGRAMS                       += logger		
bin_PROGRAMS                       += datalogger		
bin_PROGRAMS                       += videologger				

AM_INSTALLCHECK_STD_OPTIONS_EXEMPT += ping
AM_INSTALLCHECK_STD_OPTIONS_EXEMPT += clocksincro
AM_INSTALLCHECK_STD_OPTIONS_EXEMPT += server
AM_INSTALLCHECK_STD_OPTIONS_EXEMPT += voice
AM_INSTALLCHECK_STD_OPTIONS_EXEMPT += data
AM_INSTALLCHECK_STD_OPTIONS_EXEMPT += readrop
AM_INSTALLCHECK_STD_OPTIONS_EXEMPT += bvoice
AM_INSTALLCHECK_STD_OPTIONS_EXEMPT += dbvoice
AM_INSTALLCHECK_STD_OPTIONS_EXEMPT += bcrate
AM_INSTALLCHECK_STD_OPTIONS_EXEMPT += logger
AM_INSTALLCHECK_STD_OPTIONS_EXEMPT += datalogger
AM_INSTALLCHECK_STD_OPTIONS_EXEMPT += videologger