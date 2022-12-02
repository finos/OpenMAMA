set /p VERSION=<%GITHUB_WORKSPACE%\VERSION
set OPENMAMA_INSTALL_DIR=%GITHUB_WORKSPACE%\openmama-%VERSION%.win.%PLATFORM%

echo "VCPKG_DEFAULT_BINARY_CACHE=%VCPKG_DEFAULT_BINARY_CACHE%"
cmake --version
choco install --no-progress -y unzip gow cmake || goto error
REM Set PATH before adding to it since it's absurdly long in github actions and complains if we try and append to it
set PATH=C:\Program Files (x86)\Gow\bin;%JAVA_HOME%;C:\Program Files\CMake\bin;C:\Program Files\Docker;C:\Program Files\Git\bin;C:\Program Files\Git\cmd;C:\Program Files\Git\mingw64\bin;C:\Program Files\Git\usr\bin;C:\Program Files\Java\jdk8u275-b01\bin;C:\Program Files\OpenSSL\bin;C:\Program Files\PowerShell\7\;C:\Program Files\TortoiseSVN\bin;C:\Program Files\dotnet;C:\ProgramData\Chocolatey\bin;C:\ProgramData\chocolatey\lib\ghc.8.10.2.2\tools\ghc-8.10.2\bin;C:\ProgramData\chocolatey\lib\maven\apache-maven-3.6.3\bin;C:\ProgramData\chocolatey\lib\pulumi\tools\Pulumi\bin;C:\ProgramData\kind;C:\hostedtoolcache\windows\Python\3.7.9\x64;C:\hostedtoolcache\windows\Python\3.7.9\x64\Scripts;C:\hostedtoolcache\windows\Ruby\2.5.8\x64\bin;C:\hostedtoolcache\windows\go\1.14.13\x64\bin;C:\hostedtoolcache\windows\stack\2.5.1\x64;C:\mysql-5.7.21-winx64\bin;C:\npm\prefix;C:\vcpkg;C:\windows;C:\windows\System32\Wbem;C:\windows\System32\WindowsPowerShell\v1.0\;C:\windows\system32;c:\tools\php
mkdir %GITHUB_WORKSPACE%\build || goto error
cd %GITHUB_WORKSPACE%\build || goto error
echo Directory cache contents:
dir %VCPKG_DEFAULT_BINARY_CACHE%
echo Before install: %time%
vcpkg --version
vcpkg install --triplet %PLATFORM%-windows
dir %VCPKG_DEFAULT_BINARY_CACHE%
echo After install: %time%
REM cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo -DWITH_CSHARP=ON -DWITH_UNITTEST=ON -DWITH_JAVA=ON -DINSTALL_RUNTIME_DEPENDENCIES=ON -DCMAKE_INSTALL_PREFIX="%OPENMAMA_INSTALL_DIR%" -G "%GENERATOR%" -DCMAKE_TOOLCHAIN_FILE=c:/vcpkg/scripts/buildsystems/vcpkg.cmake %EXTRA_ARGS% .. || goto error
REM call build_dll_path_RelWithDebInfo.bat
REM cmake --build . --config RelWithDebInfo --target install || goto error
REM ctest . -C RelWithDebInfo -E java_unittests --timeout 240 --output-on-failure || goto error
REM cd %GITHUB_WORKSPACE% || goto error
REM 7z a openmama-%VERSION%.win.%VCVER%.%PLATFORM%.zip "%OPENMAMA_INSTALL_DIR%" || goto error

goto end

:error
echo Failed with error #%errorlevel%.

:end
exit /b %errorlevel%
