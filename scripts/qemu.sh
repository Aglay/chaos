#!/usr/bin/env bash

##############################################################################
##
##  This file is part of the Chaos Kernel, and is made available under
##  the terms of the GNU General Public License version 2.
##
##  Copyright (C) 2017 - Benjamin Grange <benjamin.grange@epitech.eu>
##
##############################################################################

set -e -u

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$SCRIPT_DIR/../"

function print_usage {
	echo -e "Usage: $0 [OPTIONS]"
	echo -e "\t-a <arch>		architecture (Valid values are 'x86' and 'x86_64') (Default: x86)"
	echo -e "\t-d			debug mode"
	echo -e "\t-t			monitor mode"
	echo -e "\t-k			enables kvm (if available)"
	echo -e "\t-m <MB> 		memory (in MB) (Default: 512MB)"
	echo -e "\t-h			print this help menu"
	exit 1
}

DEBUG=0
KVM=0
MONITOR=0
MEMORY=512
ARCH="x86"

while getopts dtkhm:a: FLAG; do
	case $FLAG in
		d) DEBUG=1;;
		k) KVM=1;;
		t) MONITOR=1;;
		m) MEMORY="$OPTARG";;
		a) ARCH="$OPTARG";;
		h) print_usage;;
		\?)
			echo "Unknown option"
			print_usage
	esac
done

shift $((OPTIND-1))

QEMU=""
case $ARCH in
	"x86")		QEMU="qemu-system-i386";;
	"x86_64")	QEMU="qemu-system-x86_64";;
	*)
		echo "Unknown architecture $ARCH"
		print_usage
esac

ISO="$PROJECT_DIR/chaos.iso"

if [ ! -f "$ISO" ]; then
	RULES="chaos.iso"
	echo -e "  MAKE\t $RULES"
	make -C "$PROJECT_DIR" --no-print-directory $RULES
fi

ARGS="-m $MEMORY -cdrom $ISO"

if [ $DEBUG == 1 ]; then
	ARGS+=" -s -d int,cpu_reset,guest_errors,unimp"
fi

if [ $KVM == 1 ]; then
	ARGS+=" --enable-kvm"
fi

if [ $MONITOR == 1 ]; then
	ARGS+=" -monitor stdio"
else
	ARGS+=" -serial stdio"
fi

if ! which "$QEMU" &> /dev/null; then
	echo -e "  ERROR\t You must install $QEMU first"
	exit 1
fi

echo -e "  QEMU\t chaos.iso"
$QEMU $ARGS $@
