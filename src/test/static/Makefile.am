DEFAULT_INCLUDES	=	-I$(srcdir)/../../.. \
										-I$(srcdir)/../../libeurocode \
										-I$(srcdir)/../../libprojet \
										-I$(srcdir)/../../libsouvenir \
										-I$(srcdir)/../../macro

TESTS	=	projet

AM_LDFLAGS      = ../../libsouvenir/libsouvenir.la ../../libprojet/libprojet.la

projet_SOURCES	=	projet.cpp

EXTRA_PROGRAMS	= $(TESTS)
EXTRA_DIST			= projet.cpp

CLEANFILES = *.xml projet *.gcno *.gcda
