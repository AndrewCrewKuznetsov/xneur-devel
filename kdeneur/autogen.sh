#!/bin/sh
# Run this to generate all the initial makefiles, etc.

args=$@

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

if [ ! -f $srcdir/configure.ac ]; then
	echo -n "**Error**: Directory '"$srcdir"' does not look like the top-level package directory"
	exit 1
fi

autoconf --version > /dev/null 2>&1
if [ $? -ne 0 ]; then
	echo
	echo "**Error**: You need 'autoconf' version 2.5 or newer installed."
	echo "Download the appropriate package for your distribution, or get the source tarball at ftp://ftp.gnu.org/pub/gnu/"
	exit 1
fi

automake --version > /dev/null 2>&1
if [ $? -ne 0 ]; then
	echo
	echo "**Error**: You need 'automake' version 1.9 or newer installed."
	echo "Download the appropriate package for your distribution, or get the source tarball at ftp://ftp.gnu.org/pub/gnu/"
	exit 1
fi

aclocal --version > /dev/null 2>&1
if [ $? -ne 0 ]; then
	echo
	echo "**Error**: Missing 'aclocal'. You need 'automake' version 1.9 or newer installed."
	echo "Download the appropriate package for your distribution, or get the source tarball at ftp://ftp.gnu.org/pub/gnu/"
	exit 1
fi

libtoolize --version > /dev/null 2>&1
if [ $? -ne 0 ]; then
	echo
	echo "**Error**: Missing 'libtoolize'. You need 'libtoolize' version 1.5.2 or newer installed."
	echo "Download the appropriate package for your distribution, or get the source tarball at ftp://ftp.gnu.org/pub/gnu/"
	exit 1
fi

echo "Creating aclocal.m4..."
test -r aclocal.m4 || touch aclocal.m4

echo "Making aclocal.m4 writable..."
test -r aclocal.m4 && chmod u+w aclocal.m4

echo "Running aclocal..."
aclocal -I m4 --force

if [ $? -ne 0 ]; then
	echo
	echo "Error while running aclocal"
	exit 1
fi

echo "Running autoheader..."
autoheader --force

if [ $? -ne 0 ]; then
	echo
	echo "Error while running autoheader"
	exit 1
fi

echo "Running autoconf..."
autoconf --force

if [ $? -ne 0 ]; then
	echo
	echo "Error while running autoconf"
	exit 1
fi

echo "Running libtoolize..."
libtoolize --force

if [ $? -ne 0 ]; then
	echo
	echo "Error while running libtoolize"
	exit 1
fi

echo "Running automake..."
automake --gnu --add-missing --copy --force-missing

if [ $? -ne 0 ]; then
	echo
	echo "Error while running automake"
	exit 1
fi

echo Running $srcdir/configure $args ...
$srcdir/configure $args

if [ $? -ne 0 ]; then
	echo
	echo "Error while running configure"
	exit 1
fi

echo "Now type 'make' to compile."
