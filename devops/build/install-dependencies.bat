REM Output will be written to current package
git clone https://github.com/microsoft/vcpkg.git vcpkg-openmama || goto error
cd vcpkg-openmama || goto error
call .\bootstrap-vcpkg.bat || goto error
git checkout 62d01b70df227850b728f5050418b917ad6d2b32 || goto error
vcpkg install --triplet %PLATFORM%-windows qpid-proton libevent apr apr-util gtest || goto error

goto end

:error
echo Failed with error #%errorlevel%.

:end
exit /b %errorlevel%
