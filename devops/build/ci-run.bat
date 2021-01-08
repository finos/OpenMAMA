set PATH=%JAVA_HOME_OVERRIDE%\bin;%PATH%

mkdir build || goto error
cd build || goto error
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo -DWITH_CSHARP=ON -DWITH_UNITTEST=ON -DWITH_JAVA=ON -DPROTON_ROOT=C:/tools/vcpkg/installed/%PLATFORM%-windows/ -DGTEST_ROOT=C:/tools/vcpkg/installed/%PLATFORM%-windows/ -DINSTALL_RUNTIME_DEPENDENCIES=ON -DCMAKE_INSTALL_PREFIX="%OPENMAMA_INSTALL_DIR%" -G "%GENERATOR%" -DCMAKE_TOOLCHAIN_FILE=c:/tools/vcpkg/scripts/buildsystems/vcpkg.cmake -DAPR_ROOT=C:/tools/vcpkg/installed/%PLATFORM%-windows/ .. || goto error
cmake --build . --config RelWithDebInfo --target install || goto error
cd .. || goto error
python release_scripts\ci-run.py || goto error
7z a openmama-%VERSION%.win.%PLATFORM%.zip "%OPENMAMA_INSTALL_DIR%" || goto error

goto end

:error
echo Failed with error #%errorlevel%.

:end
exit /b %errorlevel
