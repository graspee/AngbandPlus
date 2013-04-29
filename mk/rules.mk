

srcdir = .
top_srcdir = ..

pkgdatadir = $(datadir)/faangband
pkglibdir = $(libdir)/faangband
pkgincludedir = $(includedir)/faangband
top_builddir = .
am__cd = CDPATH="$${ZSH_VERSION+.}$(PATH_SEPARATOR)" && cd
INSTALL = /usr/bin/install -c
install_sh_DATA = $(install_sh) -c -m 644
install_sh_PROGRAM = $(install_sh) -c
install_sh_SCRIPT = $(install_sh) -c
INSTALL_HEADER = $(INSTALL_DATA)
transform = $(program_transform_name)
NORMAL_INSTALL = :
PRE_INSTALL = :
POST_INSTALL = :
NORMAL_UNINSTALL = :
PRE_UNINSTALL = :
POST_UNINSTALL = :
subdir = .
ACLOCAL_M4 = $(top_srcdir)/aclocal.m4
CONFIG_HEADER = autoconf.h
SOURCES =
DIST_SOURCES =
man1dir = $(mandir)/man1
am__installdirs = "$(DESTDIR)$(man1dir)" "$(DESTDIR)$(pkgconfigdir)"
NROFF = nroff
MANS = $(man_MANS)
am__vpath_adj_setup = srcdirstrip=`echo "$(srcdir)" | sed 's|.|.|g'`;
am__vpath_adj = case $$p in \
    $(srcdir)/*) f=`echo "$$p" | sed "s|^$$srcdirstrip/||"`;; \
    *) f=$$p;; \
  esac;
am__strip_dir = `echo $$p | sed -e 's|^.*/||'`;
pkgconfigDATA_INSTALL = $(INSTALL_DATA)
DATA = $(pkgconfig_DATA)
ETAGS = etags
CTAGS = ctags
DIST_SUBDIRS = $(SUBDIRS)
DISTFILES = $(DIST_COMMON) $(DIST_SOURCES) $(TEXINFOS) $(EXTRA_DIST)
distdir = $(PACKAGE)-$(VERSION)
top_distdir = $(distdir)
am__remove_distdir = \
  { test ! -d $(distdir) \
    || { find $(distdir) -type d ! -perm -200 -exec chmod u+w {} ';' \
         && rm -fr $(distdir); }; }
DIST_ARCHIVES = $(distdir).tar.gz
GZIP_ENV = --best
distuninstallcheck_listfiles = find . -type f -print
distcleancheck_listfiles = find . -type f -print
CC = gcc
CFLAGS += -g -O2 -Wall -pipe -g -fno-strength-reduce  -I/usr/include/SDL -D_GNU_SOURCE=1 -D_REENTRANT -pthread -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/gtk-2.0 -I/usr/lib/gtk-2.0/include -I/usr/include/atk-1.0 -I/usr/include/cairo -I/usr/include/pango-1.0 -I/usr/include/freetype2 -I/usr/include/libpng12 -I/usr/include/pixman-1 -I/usr/include/libglade-2.0 -I/usr/include/libxml2  
CPP = gcc -E
CPPFLAGS += 
CYGPATH_W = @CYGPATH_W@
DATA_PATH = ./lib/
DATADIRNAME = @DATADIRNAME@
DCT64 = @DCT64@
DEFS = -DHAVE_CONFIG_H
DEPDIR = @DEPDIR@
ECHO = @ECHO@
ECHO_C = 
ECHO_N = -n
ECHO_T = 
EFFECT_PLUGINS = @EFFECT_PLUGINS@
EFFECT_PLUGIN_DIR = @EFFECT_PLUGIN_DIR@
EGREP = /bin/grep -E
EXEEXT = 
GENCAT = @GENCAT@
INSTALL_DATA = ${INSTALL} -m 644
INSTALL_PROGRAM = ${INSTALL}
INSTALL_SCRIPT = ${INSTALL}
INSTALL_STRIP_PROGRAM = @INSTALL_STRIP_PROGRAM@
INSTOBJEXT = @INSTOBJEXT@
LDFLAGS =  -lncurses  -lSM -lICE -lX11  -L/usr/lib -lSDL -lSDL_image -lSDL_ttf -pthread -lgthread-2.0 -lrt -lglade-2.0 -lgtk-x11-2.0 -lxml2 -lgdk-x11-2.0 -latk-1.0 -lgdk_pixbuf-2.0 -lm -lpangocairo-1.0 -lpango-1.0 -lcairo -lgobject-2.0 -lgmodule-2.0 -ldl -lglib-2.0   -rdynamic -export-dynamic -lSDL_mixer
LIBOBJS = 
LIBNAME = @LIBNAME@
LIBS = 
LIBTOOL = @LIBTOOL@
LN_S = ln -s
LTLIBOBJS = 
MAKEINFO = @MAKEINFO@
MKINSTALLDIRS = @MKINSTALLDIRS@
MSGFMT = @MSGFMT@
MSGMERGE = @MSGMERGE@
OBJEXT = o
PACKAGE = faangband
PACKAGE_BUGREPORT = nckmccnnll@yahoo.com.au
PACKAGE_NAME = FAangband
PACKAGE_STRING = FAangband 0.3.5
PACKAGE_TARNAME = faangband
PACKAGE_VERSION = 0.3.5
PATH_SEPARATOR = :
PC_REQUIRES = @PC_REQUIRES@
PKG_CONFIG = /usr/bin/pkg-config
PLUGIN_LDFLAGS = @PLUGIN_LDFLAGS@
POSUB = @POSUB@
RANLIB = @RANLIB@
SET_MAKE = 
SETEGID = 
SHELL = /bin/bash
STRIP = @STRIP@
VERSION = 0.3.5
XGETTEXT = @XGETTEXT@
ac_ct_AR = @ac_ct_AR@
ac_ct_CC = gcc
ac_ct_CXX = @ac_ct_CXX@
ac_ct_RANLIB = @ac_ct_RANLIB@
ac_ct_STRIP = @ac_ct_STRIP@
am__fastdepCC_FALSE = @am__fastdepCC_FALSE@
am__fastdepCC_TRUE = @am__fastdepCC_TRUE@
am__fastdepCXX_FALSE = @am__fastdepCXX_FALSE@
am__fastdepCXX_TRUE = @am__fastdepCXX_TRUE@
am__include = @am__include@
am__leading_dot = @am__leading_dot@
am__quote = @am__quote@
am__tar = @am__tar@
am__untar = @am__untar@
bindir = ..
datadir = ${datarootdir}
datarootdir = ${prefix}/share
exec_prefix = ${prefix}
includedir = ${prefix}/include
infodir = ${datarootdir}/info
install_sh = @install_sh@
libdir = ${exec_prefix}/lib
libexecdir = ${exec_prefix}/libexec
localedir = ${datarootdir}/locale
localstatedir = ${prefix}/var
mandir = ${datarootdir}/man
mkdir_p = @mkdir_p@
oldincludedir = /usr/include
plugindir = @plugindir@
pluginsubs = @pluginsubs@
prefix = /usr/local
program_transform_name = s,x,x,
sbindir = ${exec_prefix}/sbin
sharedstatedir = ${prefix}/com
sysconfdir = ${prefix}/etc
SHARED_SUFFIX = @SHARED_SUFFIX@
LIBLDFLAGS = @LIBLDFLAGS@
