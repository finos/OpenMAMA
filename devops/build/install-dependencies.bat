REM Output will be written to current package
git clone https://github.com/microsoft/vcpkg.git vcpkg-openmama || goto error
cd vcpkg-openmama || goto error
git checkout 2023.08.09 || goto error
call .\bootstrap-vcpkg.bat || goto error
vcpkg install --triplet %PLATFORM%-windows qpid-proton libevent apr apr-util gtest || goto error

goto end

:error
echo Failed with error #%errorlevel%.

:end
exit /b %errorlevel%
