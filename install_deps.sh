#!/bin/sh
# Determine the operating system distribution that is being run and install the build dependencies for it.

if [ "$(whoami)" != "root" ]; then
	echo "Run this as root! sudo should do the trick."
	exit
fi

if [ -f /etc/debian_version ]; then # Debian/Ubuntu
	echo "\033[1;31mDebian/Ubuntu detected!\033[0m"
	DEPS="autoconf automake binutils bison build-essential cmake flex gcc gperf libasound2-dev libboost-dev libboost-filesystem-dev libboost-system-dev libbz2-dev libcups2-dev libcurl4-openssl-dev libdbus-glib-1-dev libevent-dev libflac-dev libgconf2-dev libgnome-keyring-dev libgtk2.0-dev libjpeg62-dev libnotify-dev libnss3-dev libpng12-dev libspeex-dev libsqlite3-dev libtinyxml2-dev libtool libwxgtk3.0-dev libx11-dev libxml2-dev libxpm-dev libxt-dev libxslt1-dev m4 scons wx-common xdg-utils yasm"
	if [ `uname -m` = 'x86_64' ]; then # 64 bit dependencies for 32BIT_SUPPORT
		DEPS=${DEPS}" gcc-multilib libc6-dev-i386"
	fi
	ST=`dpkg -l ${DEPS} 2>/dev/null | grep "^ii\ "  | cut -d' ' -f2`
	for PACK in ${DEPS}
	do
		CHECK=`echo "${ST}" | grep ${PACK}`
		if [ "${CHECK}" = "" ] ; then
			MUST_BE_INSTALLED="${MUST_BE_INSTALLED} ${PACK}"
		fi
	done
	if [ "${MUST_BE_INSTALLED}" != "" ]; then
		apt-get install ${MUST_BE_INSTALLED}
	else
		echo "Dependencies already installed."
	fi
elif [ -f /etc/redhat-release ]; then #Redhat/Fedora (not tested)
	echo "\033[1;31mRedhat/Fedora detected!\033[0m"
	DEPS="alsa-lib-devel autoconf binutils bison boost-devel boost-static bzip2-devel cups-devel dbus-glib-devel flex gcc-c++ GConf2-devel glibc-devel gperf gtk2-devel install libcurl-devel libevent-devel libgnome-keyring-devel libjpeg-turbo-devel libnotify-devel libstdc++-static libtool libX11-devel libXpm-devel libxslt-devel m4 nss-devel scons sqlite-devel xdg-user-dirs yasm-devel"
	for PACK in ${DEPS}
	do
		CHECK=`rpm -qa | grep ${PACK}`
		if [ "${CHECK}" = "" ] ; then
			MUST_BE_INSTALLED="${MUST_BE_INSTALLED} ${PACK}"
		fi
	done
	if [ "${MUST_BE_INSTALLED}" != "" ]; then
		yum install ${MUST_BE_INSTALLED}
	else
		echo "Dependencies already installed."
elif [ -f /etc/arch-release ]; then # Arch Linux (not tested)
	echo "\033[1;31mArch Linux detected!\033[0m"
	echo "\033[1;31mNote: there is a pkgbuild in ./distro/archlinux/\033[0m"
	# By using "pacman -T" to find out needed dependencies, we don't get
	# conflicts if a package we have installed provides one of the dependencies.
	DEPS="alsa-lib autoconf bison boost bzip2 cmake cups curl dbus-glib flac flex gcc gconf glib2 glibc gperf gtk2 libevent libgnome-keyring libjpeg-turbo libnotify libpng libtool libx11 libxml2 libxpm libxslt libxxf86vm m4 make nss pkg-config scons speex sqlite xdg-user-dirs xdg-utils yasm zlib"
	CHECK=`pacman -T ${DEPS} | sed -e 's/\n/ /g'`
	if [ `uname -m` == 'x86_64' ]; then # 64 bit dependencies for 32BIT_SUPPORT
		DEPS=${DEPS}" binutils-multilib gcc-multilib lib32-fakeroot libtool-multilib"
	else
		DEPS=${DEPS}" binutils"
	fi
	CHECK=`pacman -T ${DEPS} | sed -e 's/\n/ /g'`
	if [ -z "${CHECK}" ]; then
		echo "Dependencies already installed."
	else
		pacman -S --asdeps ${DEPS}
	fi
elif [ -f /usr/bin/emerge ]; then # Gentoo
	echo "\033[1;31mGentoo detected!\033[0m"
	echo "Please copy the ebuild under gentoo/portage into a local overlay and emerge via emerge --onlydeps desura"
else # unknown system
	echo "Unsupported operating system."
fi
