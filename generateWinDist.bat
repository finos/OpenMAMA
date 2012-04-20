
SetLocal

set DISTDIR=%CD%\install

REM set this to x64 for 64bit distributions
set 64BITFOLDER=

REM Build C Ref Docs
echo             ... Building MAMA C docs ...
cd mama\c_cpp
doxygen doxyconfig-c.in
mkdir %DISTDIR%\doc\mama\images
mkdir %DISTDIR%\doc\mama\c\html
xcopy /Y /Q /E doc\c\html\* %DISTDIR%\doc\mama\c\html\
xcopy /Y /Q /E doc\images\* %DISTDIR%\doc\mama\images\

echo             ... Building MAMA CPP docs ...
doxygen doxyconfig-cpp.in
mkdir %DISTDIR%\doc\mama\cpp\html
xcopy /Y /Q /E mama\doc\cpp\html\* %DISTDIR%\doc\mama\cpp\html\
xcopy /Y /Q /E mama\doc\images\* %DISTDIR%\doc\mama\images\

cd ..\..\mamda\c_cpp

echo             ... Building MAMDA CPP docs ...
doxygen doxyconfig-cpp.in
mkdir %DISTDIR%\doc\mama\cpp\html
mkdir %DISTDIR%\doc\mama\cpp\images
xcopy /Y /Q /E doc\cpp\html\* %DISTDIR%\doc\mamda\cpp\html\
xcopy /Y /Q /E doc\images\* %DISTDIR%\doc\mamda\images\

cd ..\..\

REM C/C++
if not exist %DISTDIR%\bin\dynamic-debug mkdir %DISTDIR%\bin\dynamic-debug
xcopy /E /I /Y %64BITFOLDER%debug\*.dll %DISTDIR%\bin\dynamic-debug
xcopy /E /I /Y %64BITFOLDER%debug\*.exe %DISTDIR%\bin\dynamic-debug
if not exist %DISTDIR%\lib mkdir %DISTDIR%\lib\dynamic-debug
xcopy /E /I /Y %64BITFOLDER%debug\*.lib %DISTDIR%\lib\dynamic-debug
if not exist %DISTDIR%\pdb mkdir %DISTDIR%\pdb
xcopy /E /I /Y %64BITFOLDER%debug\*.pdb %DISTDIR%\pdb

if not exist %DISTDIR%\bin\dynamic mkdir %DISTDIR%\bin\dynamic
xcopy /E /I /Y %64BITFOLDER%release\*.dll %DISTDIR%\bin\dynamic
xcopy /E /I /Y %64BITFOLDER%release\*.exe %DISTDIR%\bin\dynamic
if not exist %DISTDIR%\lib mkdir %DISTDIR%\lib\dynamic
xcopy /E /I /Y %64BITFOLDER%release\*.lib %DISTDIR%\lib\dynamic

REM examples
if not exist %DISTDIR%\examples\mama mkdir %DISTDIR%\examples\mama
xcopy /E /I /Y mama\c_cpp\src\examples\* %DISTDIR%\examples\mama

if not exist %DISTDIR%\examples\mamajni mkdir %DISTDIR%\examples\mamajni
xcopy /E /I /Y mama\jni\src\com\wombat\mama\examples\* %DISTDIR%\examples\mamajni

if not exist %DISTDIR%\examples\mamda mkdir %DISTDIR%\examples\mamda
xcopy /E /I /Y mamda\c_cpp\src\examples\* %DISTDIR%\examples\mamda

REM mamajni
if not exist %DISTDIR%\doc\mamajni mkdir %DISTDIR%\doc\mamajni
xcopy /E /I /Y mama\jni\mamajni\doc %DISTDIR%\doc\mamajni
if not exist %DISTDIR%\examples\mamajni mkdir %DISTDIR%\examples\mamajni
xcopy /E /I /Y mama\jni\mamajni\examples\* %DISTDIR%\examples\mamajni
if not exist %DISTDIR%\lib mkdir %DISTDIR%\lib
xcopy /E /I /Y mama\jni\mamajni\lib\*.jar %DISTDIR%\lib

REM mamda java
if not exist %DISTDIR%\bin mkdir %DISTDIR%\bin
xcopy /Y /Q /E mamda\java\mamda\bin\* %DISTDIR%\bin
if not exist %DISTDIR%\doc\mamda\java mkdir %DISTDIR%\doc\mamda\java
xcopy /Y /Q /E mamda\java\mamda\doc\java %DISTDIR%\doc\mamda\java
if not exist %DISTDIR%\examples\mamda mkdir %DISTDIR%\examples\mamda
xcopy /Y /Q /E mamda\java\mamda\examples\* %DISTDIR%\examples\mamda
if not exist %DISTDIR%\lib mkdir %DISTDIR%\lib
xcopy /Y /Q /E mamda\java\mamda\lib\* %DISTDIR%\lib

