set PATH=%JAVA_HOME_OVERRIDE%\bin;%PATH%

mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo -DWITH_CSHARP=ON -DWITH_UNITTEST=ON -DWITH_JAVA=ON -DPROTON_ROOT=C:/tools/vcpkg/installed/%PLATFORM%-windows/ -DGTEST_ROOT=C:/tools/vcpkg/installed/%PLATFORM%-windows/ -DINSTALL_RUNTIME_DEPENDENCIES=ON -DCMAKE_INSTALL_PREFIX="%OPENMAMA_INSTALL_DIR%" -G "%GENERATOR%" -DCMAKE_TOOLCHAIN_FILE=c:/tools/vcpkg/scripts/buildsystems/vcpkg.cmake -DAPR_ROOT=C:/tools/vcpkg/installed/%PLATFORM%-windows/ .. 
cmake --build . --config RelWithDebInfo --target install
cd ..
python release_scripts\ci-run.py
7z a openmama-%VERSION%.win.%PLATFORM%.zip "%OPENMAMA_INSTALL_DIR%"

goto end

:error
echo Failed with error #%errorlevel%.

:end
exit /b %errorlevel
