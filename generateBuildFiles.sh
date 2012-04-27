
# COMMON
cd common/c_cpp
rm -rf autom4te.cache etc
mkdir etc
libtoolize -c -f
cp -f ltmain.sh etc/
aclocal
autoheader
autoconf
automake -ca -f
cd -

# MAMA C/CPP
cd mama/c_cpp

rm -rf autom4te.cache etc
mkdir etc
libtoolize -c -f
cp -f ltmain.sh etc/
aclocal
autoheader
autoconf
automake -ca -f
cd -

# MAMA JNI
cd mama/jni

rm -rf autom4te.cache etc
mkdir etc
libtoolize -c -f
cp -f ltmain.sh etc/
aclocal
autoheader
autoconf
automake -ca -f
cd -

# MAMDA CPP
cd mamda/c_cpp

rm -rf autom4te.cache etc
mkdir etc
libtoolize -c -f
cp -f ltmain.sh etc/
aclocal
autoheader
autoconf
automake -ca -f
cd -
