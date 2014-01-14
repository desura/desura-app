#!/bin/sh
echo 'Make sure to run \033[1;31msudo ./install-deps.sh\033[0m before compiling!\n'

PREFIX="../install"
OFFICIAL_BUILD=OFF

clean() {
	rm -rf build
	rm -rf install
	rm -rf build_package
	echo 'Cleaned'
}

build_desura() {
	if [ ! -d "build_package" ]; then
		mkdir build
	fi
	cd build
	cmake .. -DCMAKE_INSTALL_PREFIX=$PREFIX -DBUILD_CEF=OFF -DWITH_GTEST=OFF -DOFFICIAL_BUILD=$OFFICIAL_BUILD || exit 1
	make install $@
	cd ..
	echo 'Building Desura completed'
}

build_cef() {
	if [ ! -d "build" ]; then
		mkdir build
	fi
	cd build
	cmake .. -DCMAKE_INSTALL_PREFIX=$PREFIX -DBUILD_ONLY_CEF=ON || exit 1
	make install $@
	cd ..
	echo 'Building CEF completed'
}

pack() {
	if [ ! -d "build_package" ]; then
		mkdir build_package
	fi
	cd build_package
	cmake .. -DPACKAGE_TYPE=$PACKAGE -DINSTALL_DESKTOP_FILE=ON -DCMAKE_INSTALL_PREFIX="/opt/desura" -DDESKTOP_EXE="/opt/desura/desura" -DDESKTOP_ICON="/opt/desura/desura.png" || exit 1
	make package $@
	if [ $PACKAGE = "DEB" ]; then
		mv Desura-*.deb ..
	elif [ $PACKAGE = "RPM" ]; then
		mv Desura-*.rpm ..
	fi
	echo 'Building $PACKAGE completed'
	cd ..
}

if [ -z "$1" ]; then
	clean
	build_desura || exit 2
	build_cef || exit 2
elif [ "$1" = "clean" ]; then
	shift
	clean
elif [ "$1" = "pack" ]; then
	shift
	PACKAGE="$1"
	shift
	clean
	pack || exit 2
elif [ "$1" = "only_desura" ]; then
	shift
	clean
	build_desura || exit 2
elif [ "$1" = "only_cef" ]; then
	shift
	clean
	build_cef || exit 2
elif [ "$1" = "help" ]; then
	echo 'This script will allow you to easy compile Desura on Linux.'
	echo 'Type "./build_desura.sh clean" to remove build files.'
	echo 'Type "./build_desura.sh" to build Desura client.'
	echo 'Type "./build_desura.sh pack DEB" to prepare .deb package.'
	echo 'Type "./build_desura.sh pack RPM" to prepare .rpm package.'
	echo 'Type "./build_desura.sh only_desura" to compile Desura without CEF(Chormium Embedded Framework).'
	echo 'Type "./build_desura.sh only_cef" to compile CEF without Desura.'
	echo 'Type "./build_desura.sh help" to display this message.'
	echo 'Any additonal shell argument will be given to "make" command.\n'
fi

echo 'Run \033[1;31m./install/desura\033[0m to start Desura!'
