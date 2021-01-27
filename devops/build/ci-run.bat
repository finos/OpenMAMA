set PATH=%JAVA_HOME_OVERRIDE%\bin;%PATH%
set /p VERSION=<%GITHUB_WORKSPACE%\VERSION
set OPENMAMA_INSTALL_DIR=%GITHUB_WORKSPACE%\openmama-%VERSION%.win.%PLATFORM%

choco install -y unzip gow wget gradle nunit-console-runner nunit-extension-nunit-v2-driver cmake || goto error
vcpkg install qpid-proton:%PLATFORM%-windows libevent:%PLATFORM%-windows apr:%PLATFORM%-windows gtest:%PLATFORM%-windows || goto error
msiexec.exe /I https://github.com/nunit-legacy/nunitv2/releases/download/2.7.1/NUnit-2.7.1.msi /QN || goto error
call RefreshEnv.cmd
mkdir %GITHUB_WORKSPACE%\build || goto error
cd %GITHUB_WORKSPACE%\build || goto error
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo -DWITH_CSHARP=ON -DWITH_UNITTEST=ON -DWITH_JAVA=ON -DPROTON_ROOT=C:/vcpkg/installed/%PLATFORM%-windows/ -DGTEST_ROOT=C:/vcpkg/installed/%PLATFORM%-windows/ -DINSTALL_RUNTIME_DEPENDENCIES=ON -DCMAKE_INSTALL_PREFIX="%OPENMAMA_INSTALL_DIR%" -G "%GENERATOR%" -DAPR_ROOT=C:/vcpkg/installed/%PLATFORM%-windows/ -DCMAKE_TOOLCHAIN_FILE=c:/vcpkg/scripts/buildsystems/vcpkg.cmake %EXTRA_ARGS% .. || goto error
call build_dll_path_RelWithDebInfo.bat
cmake --build . --config RelWithDebInfo --target install || goto error
ctest . -C RelWithDebInfo
cd %GITHUB_WORKSPACE%\mama\jni || goto error
gradle test
cd %GITHUB_WORKSPACE% || goto error
7z a openmama-%VERSION%.win.%PLATFORM%.zip "%OPENMAMA_INSTALL_DIR%" || goto error

goto end

:error
echo Failed with error #%errorlevel%.

:end
exit /b %errorlevel%
