
echo set path for ctp.dll ...
set path=%path%;%~dps0..\CTP636Common

pushd %~dps0

echo ���� debug �汾
"devenv.com"  indicator-ctp.sln /rebuild "Debug|Win32"
"devenv.com"  indicator-ctp.sln /build   "Debug|Win32"
if errorlevel 1 echo FAILED & exit 1

echo ���� release �汾
"devenv.com"  indicator-ctp.sln /rebuild "Release|Win32"
"devenv.com"  indicator-ctp.sln /build   "Release|Win32"
if errorlevel 1 echo FAILED & exit 1


popd