

cd %MAMDA_HOME%

if not exist %DISTDIR%\bin mkdir %DISTDIR%\bin

xcopy /Y /Q /E bin\* %DISTDIR%\bin


if not exist %DISTDIR%\doc\mamda\java mkdir %DISTDIR%\doc\mamda\java

xcopy /Y /Q /E doc\java %DISTDIR%\doc\mamda\java

if not exist %DISTDIR%\examples\mamda mkdir %DISTDIR%\examples\mamda

xcopy /Y /Q /E examples\* %DISTDIR%\examples\mamda

if not exist %DISTDIR%\lib mkdir %DISTDIR%\lib

xcopy /Y /Q /E lib\* %DISTDIR%\lib

