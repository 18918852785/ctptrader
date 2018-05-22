set src=W:\indicator-server\src
set src_xp=W:\indicator-server\src\wind-mac-sdk-c-unarchiver\target\wind-mac\output

set CFG=Debug
call :deploy_it

set CFG=Release
call :deploy_it
exit /b 0

:deploy_it
set dst=%src%\%CFG%-ctp
md %dst%>nul 2>&1

copy /y %src%\%CFG%\indicator-model.dll         %dst%\
copy /y %src%\%CFG%\indicator-zmq-lib.dll       %dst%\
copy /y %src%\%CFG%\Newtonsoft.Json.dll         %dst%\
copy /y %src%\%CFG%\trader-monitor.exe          %dst%\

copy /y %src%\%CFG%\ctp-market.dll          %dst%\
copy /y %src%\%CFG%\ctp-trader.dll          %dst%\
copy /y %src%\%CFG%\indicator-log.dll       %dst%\
copy /y %src%\%CFG%\nanomsg.dll             %dst%\
copy /y %src%\%CFG%\thostmduserapi.dll      %dst%\
copy /y %src%\%CFG%\thosttraderapi.dll      %dst%\
copy /y %src%\%CFG%\WindowsTraderApi.exe    %dst%\

copy /y %src_xp%\%CFG%\pthread.dll          %dst%\
copy /y %src_xp%\%CFG%\xpbase.dll           %dst%\
copy /y %src_xp%\%CFG%\zlibwapid.dll        %dst%\
copy /y %src_xp%\%CFG%\zlibwapi.dll         %dst%\
exit /b 0
