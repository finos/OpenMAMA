cd common/c_cpp
rm -rf autom4te.cache etc
mkdir etc
libtoolize -c -f
cp -f ltmain.sh etc/
aclocal
autoheader
autoconf
automake -ca -f

cd - ; cd mama/c_cpp

rm -rf autom4te.cache etc
mkdir etc
libtoolize -c -f
cp -f ltmain.sh etc/
aclocal
autoheader
autoconf
automake -ca -f
