#
# Makefile.am
#

EXTRA_DIST                         =
DISTCLEANFILES                     =
bin_PROGRAMS                       =
AM_INSTALLCHECK_STD_OPTIONS_EXEMPT =


#Server install
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
	
bin_PROGRAMS                       += rina-server
AM_INSTALLCHECK_STD_OPTIONS_EXEMPT += rina-server
	
#Voice install
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

bin_PROGRAMS                       += rina-voice
AM_INSTALLCHECK_STD_OPTIONS_EXEMPT += rina-voice
