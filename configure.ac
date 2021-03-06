AC_INIT(client/gdaemonvfs.h)

AM_INIT_AUTOMAKE(gvfs, 1.1.9)
AM_CONFIG_HEADER(config.h)
	
AM_SANITY_CHECK

AC_C_CONST
AC_ISC_POSIX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AM_DISABLE_STATIC
AM_PROG_LIBTOOL
AC_PATH_PROG(PKG_CONFIG, pkg-config, no)
AM_PROG_CC_C_O


# Make libtool use --silent when --silent is passed to make
changequote(,)dnl
LIBTOOL="${LIBTOOL} \$(shell echo \"\$(MFLAGS)\" | awk '/^[^ ]*s/ { print \"--silent\" }')"
changequote([,])dnl

AC_TYPE_PID_T
AC_TYPE_SIGNAL
AC_TYPE_SIZE_T
AC_TYPE_UID_T

AH_VERBATIM([_GNU_SOURCE],
[/* Enable GNU extensions on systems that have them.  */
#ifndef _GNU_SOURCE
#  define _GNU_SOURCE
#endif])
	
saved_CFLAGS=$CFLAGS
AC_CHECK_MEMBERS([struct stat.st_mtimensec, struct stat.st_mtim.tv_nsec, struct stat.st_atimensec, struct stat.st_atim.tv_nsec, struct stat.st_ctimensec, struct stat.st_ctim.tv_nsec])
CFLAGS=$saved_CFLAGS

LIBTOOL_EXPORT_OPTIONS='-export-symbols-regex "^[[^_]].*"'
AC_SUBST(LIBTOOL_EXPORT_OPTIONS)
			
GTK_DOC_CHECK
DISTCHECK_CONFIGURE_FLAGS="--enable-gtk-doc"
AC_SUBST(DISTCHECK_CONFIGURE_FLAGS)

PKG_CHECK_MODULES(GLIB, glib-2.0 >= 2.19.1 gthread-2.0 gobject-2.0 gmodule-no-export-2.0 gio-unix-2.0 gio-2.0)
AC_SUBST(GLIB_CFLAGS)
AC_SUBST(GLIB_LIBS)

PKG_CHECK_MODULES(DBUS, dbus-1)
AC_SUBST(DBUS_CFLAGS)
AC_SUBST(DBUS_LIBS)

AC_ARG_WITH(dbus_service_dir, [  --with-dbus-service-dir=PATH choose directory for dbus service files, [default=PREFIX/share/dbus-1/services]], with_dbus_service_dir="$withval", with_dbus_service_dir=$datadir/dbus-1/services)
DBUS_SERVICE_DIR=$with_dbus_service_dir
AC_SUBST(DBUS_SERVICE_DIR)

dnl **** gio module path handling ****
giomodulesdir=`pkg-config --variable=giomoduledir gio-2.0`
AC_SUBST(giomodulesdir)

dnl ****************************
dnl *** Checks for intltool  ***
dnl ****************************

IT_PROG_INTLTOOL([0.35.0])
GETTEXT_PACKAGE=gvfs

AC_SUBST([GETTEXT_PACKAGE])
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE],["$GETTEXT_PACKAGE"],[The gettext domain name])
AM_GLIB_GNU_GETTEXT

dnl ****************************
dnl *** Checks for gettext   ***
dnl ****************************

dnl *** disabled by now, using intltool ***
dnl *** we'll revert when glib-gettexttize will be able
dnl *** to manage [enconding: UTF-8] in POTIFILES.in

dnl GETTEXT_PACKAGE=gvfs
dnl AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,"$GETTEXT_PACKAGE", [The gettext domain name])
dnl AC_SUBST(GETTEXT_PACKAGE)

dnl ALL_LINGUAS=""

dnl AM_GLIB_GNU_GETTEXT

dnl LIBS="$LIBS $INTLLIBS"

dnl AM_GLIB_DEFINE_LOCALEDIR(GVFS_LOCALEDIR)

dnl ****************************
dnl *** Checks for ssh stuff ***
dnl ****************************

AC_PATH_PROG(SSH_PROGRAM, ssh, "ssh")

dnl ****************************
dnl *** Checks for pty stuff ***
dnl ****************************

AC_CHECK_HEADERS(sys/un.h stropts.h termios.h utmp.h sys/uio.h sys/param.h)

# Check for PTY handling functions.
AC_CHECK_FUNCS(getpt posix_openpt grantpt unlockpt ptsname ptsname_r)

# Pull in the right libraries for various functions which might not be
# bundled into an exploded libc.
AC_CHECK_FUNC(socketpair,[have_socketpair=1],AC_CHECK_LIB(socket,socketpair,[have_socketpair=1; LIBS="$LIBS -lsocket"]))
if test x$have_socketpair = x1 ; then
	AC_DEFINE(HAVE_SOCKETPAIR,1,[Define if you have the socketpair function.])
fi

AC_SEARCH_LIBS(login_tty, util, [AC_DEFINE([HAVE_LOGIN_TTY],[],[Whether login_tty is available])])

dnl **************************************************
dnl *** Check if we should build with http backend ***
dnl **************************************************
AC_ARG_ENABLE(http, [  --disable-http           build without http/dav backend])
msg_http=no
HTTP_CFLAGS=
HTTP_LIBS=

if test "x$enable_http" != "xno"; then
  PKG_CHECK_EXISTS(libsoup-gnome-2.4 >= 2.25.1, msg_http=yes)

  if test "x$msg_http" = "xyes"; then
    PKG_CHECK_MODULES(HTTP, libsoup-gnome-2.4 libxml-2.0)
    AC_DEFINE(HAVE_HTTP, 1, [Define to 1 if http/dav is going to be built])
  fi
fi

AC_SUBST(HTTP_CFLAGS)
AC_SUBST(HTTP_LIBS)
AM_CONDITIONAL(HAVE_HTTP, [test "$msg_http" = "yes"])

dnl **************************************************
dnl *** Check if we should build with DNS-SD backend ***
dnl **************************************************
AM_CONDITIONAL(HAVE_AVAHI, false)
			  
AC_ARG_ENABLE(avahi, [  --disable-avahi     build without avahi support])
msg_avahi=no
if test "x$enable_avahi" != "xno"; then
	PKG_CHECK_MODULES(AVAHI, avahi-glib >= 0.6 avahi-client >= 0.6,
		  	[AM_CONDITIONAL(HAVE_AVAHI, true)
		   	AC_DEFINE(HAVE_AVAHI, [], [Set if we can use avahi])]
                        msg_avahi=yes,
	          	[AM_CONDITIONAL(HAVE_AVAHI, false)])
	AC_SUBST(AVAHI_CFLAGS)
	AC_SUBST(AVAHI_LIBS)
fi

dnl **********************
dnl *** Check for FUSE ***
dnl **********************
AC_ARG_ENABLE(fuse, [  --disable-fuse           build without FUSE support])
msg_fuse=no
FUSE_LIBS=
FUSE_CFLAGS=

if test "x$enable_fuse" != "xno"; then
  PKG_CHECK_EXISTS(fuse, msg_fuse=yes)

  if test "x$msg_fuse" = "xyes"; then
    PKG_CHECK_MODULES(FUSE, fuse)
    AC_DEFINE(HAVE_FUSE, 1, [Define to 1 if FUSE is available])
  fi
fi

AC_SUBST(FUSE_LIBS)
AC_SUBST(FUSE_CFLAGS)

AM_CONDITIONAL(USE_FUSE, [test "$msg_fuse" = "yes"])

dnl **********************
dnl *** Check for GConf ***
dnl **********************
AC_ARG_ENABLE(gconf, [  --disable-gconf           build without GConf support])
msg_gconf=no
GCONF_LIBS=
GCONF_CFLAGS=

if test "x$enable_gconf" != "xno"; then
  PKG_CHECK_EXISTS(gconf-2.0, msg_gconf=yes)

  if test "x$msg_gconf" = "xyes"; then
    PKG_CHECK_MODULES(GCONF, gconf-2.0)
    AC_DEFINE(HAVE_GCONF, 1, [Define to 1 if GConf is available])
  fi
fi

AC_SUBST(GCONF_LIBS)
AC_SUBST(GCONF_CFLAGS)

AM_CONDITIONAL(USE_GCONF, [test "$msg_gconf" = "yes"])

dnl **********************
dnl *** Check for HAL ***
dnl **********************
AC_ARG_ENABLE(hal, [  --disable-hal           build without HAL support])
msg_hal=no
have_hal_fast_init=no
HAL_LIBS=
HAL_CFLAGS=
HAL_REQUIRED=0.5.10

if test "x$enable_hal" != "xno"; then
  PKG_CHECK_EXISTS([hal >= $HAL_REQUIRED], msg_hal=yes)

  if test "x$msg_hal" = "xyes"; then
    PKG_CHECK_MODULES([HAL],[hal >= $HAL_REQUIRED])
    AC_DEFINE(HAVE_HAL, 1, [Define to 1 if HAL is available])
    save_libs="$LIBS"
    LIBS="$HAL_LIBS"
    AC_CHECK_LIB(hal, libhal_get_all_devices_with_properties, have_hal_fast_init=yes)
    if test "x$have_hal_fast_init" = "xyes"; then
      AC_DEFINE(HAVE_HAL_FAST_INIT, 1, [Define to 1 if libhal_get_all_devices_with_properties is available])
    fi
    LIBS="$save_libs"
  fi
fi

AC_SUBST(HAL_LIBS)
AC_SUBST(HAL_CFLAGS)

AM_CONDITIONAL(USE_HAL, [test "$msg_hal" = "yes"])

dnl **************************************************
dnl *** Check if we should build with CDDA backend ***
dnl **************************************************
AC_ARG_ENABLE(cdda, [  --disable-cdda           build without CDDA backend])
msg_cdda=no
CDDA_LIBS=
CDDA_CFLAGS=

if test "x$enable_cdda" != "xno"; then
  PKG_CHECK_EXISTS(libcdio_paranoia >= 0.78.2 hal >= $HAL_REQUIRED, msg_cdda=yes)

  if test "x$msg_cdda" = "xyes"; then
    PKG_CHECK_MODULES(CDDA, libcdio_paranoia hal >= $HAL_REQUIRED)
    AC_DEFINE(HAVE_CDDA, 1, [Define to 1 if CDDA is going to be built])
  fi
fi

AC_SUBST(CDDA_LIBS)
AC_SUBST(CDDA_CFLAGS)

AM_CONDITIONAL(USE_CDDA, [test "$msg_cdda" = "yes"])

dnl *****************************************************
dnl *** Check if we should build with obexftp backend ***
dnl *****************************************************
AC_ARG_ENABLE(obexftp, [  --disable-obexftp           build without ObexFTP backend])
msg_obexftp=no
OBEXFTP_LIBS=
OBEXFTP_CFLAGS=

if test "x$enable_obexftp" != "xno" -a "x$msg_hal" = "xyes" ; then
  PKG_CHECK_EXISTS(dbus-glib-1 bluez >= 4.0, msg_obexftp=yes)

  dnl Make sure we have expat
  AC_CHECK_LIB(expat, XML_ParserCreate_MM,
               [ AC_CHECK_HEADERS(expat.h, have_expat=true, have_expat=false) ],
               have_expat=false)

  if test "x$msg_obexftp" = "xyes" -a "x$have_expat" = "xtrue"; then
    PKG_CHECK_MODULES(OBEXFTP, dbus-glib-1 bluez >= 4.0)
    AC_SUBST(OBEXFTP_LIBS)
    AC_SUBST(OBEXFTP_CFLAGS)

    msg_obexftp=yes
    AC_DEFINE(HAVE_OBEXFTP, 1, [Define to 1 if ObexFTP is going to be built])
    XML_CFLAGS=""
    XML_LIBS="-lexpat"
  else
    msg_obexftp=no
  fi
fi

AC_SUBST(XML_LIBS)
AC_SUBST(XML_CFLAGS)

AM_CONDITIONAL(USE_OBEXFTP, [test "$msg_obexftp" = "yes"])

dnl *************************
dnl *** Check for gphoto2 ***
dnl *************************
AC_ARG_ENABLE(gphoto2, [  --disable-gphoto2           build without gphoto2 support])
msg_gphoto2=no
GPHOTO2_LIBS=
GPHOTO2_CFLAGS=

if test "x$enable_gphoto2" != "xno" -a "x$msg_hal" = "xyes" ; then
  PKG_CHECK_EXISTS(libgphoto2, msg_gphoto2=yes)

  # Need OS tweaks in hal volume monitor backend
  case "$host" in
    *-linux*)
      use_gphoto2=yes
      ;;
    *)
      use_gphoto2=no
      ;;
  esac

  if test "x$msg_gphoto2" = "xyes"; then
    if test "x$use_gphoto2" = "xyes"; then
      PKG_CHECK_MODULES(GPHOTO2, libgphoto2 >= 2.4.0)
      AC_DEFINE(HAVE_GPHOTO2, 1, [Define to 1 if gphoto2 is available])
    else
      AC_MSG_WARN([Not building with gphoto2 support. Need OS tweaks in hal volume monitor.])
      msg_gphoto2=no
    fi
  fi
fi

AC_SUBST(GPHOTO2_LIBS)
AC_SUBST(GPHOTO2_CFLAGS)

AM_CONDITIONAL(USE_GPHOTO2, [test "$msg_gphoto2" = "yes"])

dnl *******************************
dnl *** Check for GNOME Keyring ***
dnl *******************************
AC_ARG_ENABLE(keyring, [  --disable-keyring           build without GNOME Keyring support])
msg_keyring=no
KEYRING_LIBS=
KEYRING_CFLAGS=

if test "x$enable_keyring" != "xno"; then
  PKG_CHECK_EXISTS(gnome-keyring-1, msg_keyring=yes)

  if test "x$msg_keyring" = "xyes"; then
    PKG_CHECK_MODULES(KEYRING, gnome-keyring-1)
    AC_DEFINE(HAVE_KEYRING, 1, [Define to 1 if GNOME Keyring is available])
  fi
fi

AC_SUBST(KEYRING_LIBS)
AC_SUBST(KEYRING_CFLAGS)

AM_CONDITIONAL(USE_KEYRING, [test "$msg_keyring" = "yes"])

dnl ==========================================================================
dnl Samba 3.0

AC_ARG_ENABLE(samba, [  --disable-samba       build without samba support])
msg_samba="no"
if test "x$enable_samba" != "xno"; then
  AC_ARG_WITH(samba-includes, [  --with-samba-includes=PREFIX     Location of samba includes.],
	      with_samba_includes="$withval", with_samba_includes="/usr/include")
  have_samba_includes="no"
  if test "x${with_samba_includes}" != "xno"; then
	CPPFLAGS_save="$CPPFLAGS"

	CPPFLAGS="$CPPFLAGS -I$with_samba_includes"
	AC_CHECK_HEADER(libsmbclient.h, [ samba_includes="yes" ])
	CPPFLAGS="$CPPFLAGS_save"
	
	if test "x{samba_includes}" != "xno" -a "x${samba_includes}" != "x"; then
		have_samba_includes="yes"
		if test "${with_samba_includes}" != "/usr/include" ; then
			SAMBA_CFLAGS="-I$with_samba_includes"
		else
			SAMBA_CFLAGS=""
		fi
		
		CPPFLAGS="$CPPFLAGS -I$with_samba_includes"
		AC_CHECK_MEMBER(SMBCCTX.flags,
				[AC_DEFINE(HAVE_SAMBA_FLAGS,, [Defined if flags available in SMBCCTXT])],,
				[#include <libsmbclient.h>])
		AC_CHECK_MEMBER(SMBCCTX.close,
				[AC_DEFINE(HAVE_SAMBA_OLD_CLOSE, , [Defined if old close is available in SMBCCTXT])],,
				[#include <libsmbclient.h>])
		CPPFLAGS="$CPPFLAGS_save"
	else
		SAMBA_CFLAGS=""
	fi
  fi
  AC_ARG_WITH(samba-libs, [  --with-samba-libs=PREFIX     	Location of Samba libs.],
	      with_samba_libs="$withval", with_samba_libs="/usr/lib")
  if test "x${with_samba_libs}" != "xno" -a "x${have_samba_includes}" != "xno"; then
	LDFLAGS_save="$LDFLAGS"
	
	LDFLAGS="$LDFLAGS -L$with_samba_libs"
	AC_CHECK_LIB(smbclient, smbc_option_get,samba_libs="yes", samba_libs="no")
	LDFLAGS="$LDFLAGS_save"
	if test "x${samba_libs}" != "xno"; then
		AC_DEFINE(HAVE_SAMBA,, [Define to 1 if you have the samba 3.0 libraries])
		msg_samba="yes"
                if test x$with_samba_libs != x/usr/lib; then
                        SAMBA_LIBS="-L$with_samba_libs -lsmbclient"
                else
                	SAMBA_LIBS="-lsmbclient"
                fi
	else
		AC_CHECK_LIB(smbclient, smbc_new_context,samba_old_libs="yes", samba_old_libs="no")
		if test "x${samba_old_libs}" != "xno"; then
		   msg_samba="Too old, need at least samba 3.0.22"
		fi
		SAMBA_CFLAGS=""
		SAMBA_LIBS=""
	fi
  fi
  AC_MSG_CHECKING(for Samba 3.0 libraries)
  AC_MSG_RESULT($msg_samba)
fi
AM_CONDITIONAL(HAVE_SAMBA, test "$msg_samba" = "yes")
AC_SUBST(SAMBA_CFLAGS)
AC_SUBST(SAMBA_LIBS)

dnl ==========================================================================

dnl ****************************
dnl *** Check for libarchive ***
dnl ****************************

AC_ARG_ENABLE(archive, [  --disable-archive       build without archive support])
msg_archive="no"
if test "x$enable_archive" != "xno"; then
  AC_ARG_WITH(archive-includes, [  --with-archive-includes=PREFIX     Location of archive includes.],
	      with_archive_includes="$withval", with_archive_includes="/usr/include")
  have_archive_includes="no"
  if test "x${with_archive_includes}" != "xno"; then
	CPPFLAGS_save="$CPPFLAGS"

	CPPFLAGS="$CPPFLAGS -I$with_archive_includes"
	AC_CHECK_HEADER(archive.h, [ archive_includes="yes" ])
	CPPFLAGS="$CPPFLAGS_save"
	
	if test "x{archive_includes}" != "xno" -a "x${archive_includes}" != "x"; then
		have_archive_includes="yes"
		if test "${with_archive_includes}" != "/usr/include" ; then
			ARCHIVE_CFLAGS="-I$with_archive_includes"
		else
			ARCHIVE_CFLAGS=""
		fi
	else
		ARCHIVE_CFLAGS=""
	fi
  fi
  AC_ARG_WITH(archive-libs, [  --with-archive-libs=PREFIX     	Location of Archive libs.],
	      with_archive_libs="$withval", with_archive_libs="/usr/lib")
  if test "x${with_archive_libs}" != "xno" -a "x${have_archive_includes}" != "xno"; then
	LDFLAGS_save="$LDFLAGS"
	
	LDFLAGS="$LDFLAGS -L$with_archive_libs"
	AC_CHECK_LIB(archive, archive_entry_filetype, archive_libs="yes", archive_libs="no")
	LDFLAGS="$LDFLAGS_save"
	if test "x${archive_libs}" != "xno"; then
		AC_DEFINE(HAVE_ARCHIVE,, [Define to 1 if you have the libarchive library])
		msg_archive="yes"
                if test x$with_archive_libs != x/usr/lib; then
                        ARCHIVE_LIBS="-L$with_archive_libs -larchive"
                else
                	ARCHIVE_LIBS="-larchive"
                fi
	else
		AC_CHECK_LIB(archive, archive_entry_filetype, archive_old_libs="yes", archive_old_libs="no")
		if test "x${archive_old_libs}" != "xno"; then
		   msg_archive="Too old, need at least archive 3.0.22"
		fi
		ARCHIVE_CFLAGS=""
		ARCHIVE_LIBS=""
	fi
  fi
  AC_MSG_CHECKING(for Archive 3.libraries)
  AC_MSG_RESULT($msg_archive)
fi
AM_CONDITIONAL(HAVE_ARCHIVE, test "$msg_archive" = "yes")
AC_SUBST(ARCHIVE_CFLAGS)
AC_SUBST(ARCHIVE_LIBS)

dnl Install bash-completion file?
AC_ARG_ENABLE([bash-completion],
	      AC_HELP_STRING([--disable-bash-completion],
			     [build without bash-completion support]),
	      msg_bash_completion=$enableval,
	      msg_bash_completion=yes)
if test "x$enable_bash_completion" != "xno"; then
  msg_bash_completion=yes
fi
AM_CONDITIONAL(ENABLE_BASHCOMP, test "$msg_bash_completion" = "yes")

AC_ARG_WITH([bash-completion-dir],
	    AC_HELP_STRING([--with-bash-completion-dir=PATH],
	    		   [choose directory for bash-completion file (default=SYSCONFDIR/profile.d)]),
	    with_bashcomp_dir=$withval,
	    with_bashcomp_dir=$sysconfdir/profile.d)
BASHCOMP_DIR=$with_bashcomp_dir
AC_SUBST(BASHCOMP_DIR)

dnl ==========================================================================
	
dnl Globally define_GNU_SOURCE and therefore enable the GNU extensions
AC_DEFINE(_GNU_SOURCE, 1, [Enable GNU Extensions])

dnl ==========================================================================
	
AC_DEFINE(_FILE_OFFSET_BITS, 64, [Enable LFS])
		       
dnl ==========================================================================

AC_PATH_PROG(GLIB_GENMARSHAL, glib-genmarshal)

dnl ==========================================================================
dnl Turn on the additional warnings last, so -Werror doesn't affect other tests.

AC_ARG_ENABLE(more-warnings,
[  --enable-more-warnings  Maximum compiler warnings],
set_more_warnings="$enableval",[
if test -f $srcdir/CVSVERSION; then
	is_cvs_version=true
	set_more_warnings=yes
else
	set_more_warnings=no
fi
])
AC_MSG_CHECKING(for more warnings, including -Werror)
if test "$GCC" = "yes" -a "$set_more_warnings" != "no"; then
	AC_MSG_RESULT(yes)
	CFLAGS="\
	-Wall \
	-Wchar-subscripts -Wmissing-declarations -Wmissing-prototypes \
	-Wnested-externs -Wpointer-arith \
	-Wcast-align -Wsign-compare \
	$CFLAGS"

	for option in -Wno-strict-aliasing -Wno-sign-compare; do
		SAVE_CFLAGS="$CFLAGS"
		CFLAGS="$CFLAGS $option"
		AC_MSG_CHECKING([whether gcc understands $option])
		AC_TRY_COMPILE([], [],
			has_option=yes,
			has_option=no,)
		if test $has_option = no; then
			CFLAGS="$SAVE_CFLAGS"
		fi
		AC_MSG_RESULT($has_option)
		unset has_option
		unset SAVE_CFLAGS
	done
	unset option
else
	AC_MSG_RESULT(no)
fi
			       
AC_OUTPUT([
Makefile
common/Makefile
client/Makefile
daemon/trashlib/Makefile
daemon/Makefile
monitor/Makefile
monitor/proxy/Makefile
monitor/hal/Makefile
monitor/gphoto2/Makefile
gconf/Makefile
programs/Makefile
test/Makefile
po/Makefile.in
])

echo 
echo "gvfs configuration summary:"
echo "
        gio module directory : $giomodulesdir

        FTP/HTTP/WebDAV support       $msg_http
        ObexFTP support               $msg_obexftp
	Samba support:	              $msg_samba
	FUSE support:                 $msg_fuse
        CDDA support:                 $msg_cdda
        Gphoto2 support:              $msg_gphoto2
	archive support:	      $msg_archive
        GConf support:                $msg_gconf
        DNS-SD support:               $msg_avahi
	Use HAL for volume monitor:   $msg_hal (with fast init path: $have_hal_fast_init)
	GNOME Keyring support:        $msg_keyring
	Bash-completion support:      $msg_bash_completion
"
