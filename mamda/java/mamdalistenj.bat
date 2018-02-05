@ECHO OFF


set mama_provider=tibrv
set demo_type=MamdaListen
set no_ent=
set add_jars=
set args=

IF NOT DEFINED WOMBAT_HOME set WOMBAT_HOME=C:\Wombat
IF NOT DEFINED WOMBAT_PATH set WOMBAT_PATH=C:\Wombat\config
IF NOT DEFINED MAMA_HOME set MAMA_HOME=%WOMBAT_HOME%
IF NOT DEFINED WOMBAT_MSG_HOME set WOMBAT_MSG_HOME=%WOMBAT_HOME%
IF NOT DEFINED MAMDA_HOME set MAMDA_HOME=%WOMBAT_HOME%


:Loop

IF "%1"=="" GOTO Continue

IF "%1"=="-trades" (
set demo_type=MamdaTradeTicker
GOTO 1SHIFT
)

IF "%1"=="-quotes" (
set demo_type=MamdaQuoteTicker
GOTO 1SHIFT
)

IF "%1"=="-books" (
set demo_type=MamdaBookTicker
set add_jars=%MAMDA_HOME%/lib/mamda_book.jar
GOTO 1SHIFT
)

IF "%1"=="-bookcheck" (
set demo_type=MamdaOrderBookChecker
set add_jars=%MAMDA_HOME%/lib/mamda_book.jar
GOTO 1SHIFT
)

IF "%1"=="-atomicbooks" (
set demo_type=MamdaAtomicBookTicker
set add_jars=%MAMDA_HOME%/lib/mamda_book.jar
GOTO 1SHIFT
)

IF "%1"=="-multipart" (
set demo_type=MamdaMultiPartTicker
GOTO 1SHIFT
)

IF "%1"=="-multisecurity" (
set demo_type=MamdaMultiSecurityTicker
GOTO 1SHIFT
)

IF "%1"=="-options" (
set demo_type=MamdaOptionChainExample
GOTO 1SHIFT
)

IF "%1"=="-optionviewer" (
set demo_type=MamdaOptionChainViewExample
GOTO 1SHIFT
)

IF "%1"=="-p" (
set mama_provider=%2
GOTO 2SHIFTS
)

IF "%1"=="-noent" (
set no_ent=_noent
GOTO 1SHIFT
)

set args=%args% %1
GOTO 1SHIFT

:2SHIFTS
SHIFT
:1SHIFT
SHIFT
GOTO Loop

:Continue


if "%mama_provider%"=="tibrv" (
REM TIBRV-specific files
    IF NOT DEFINED TIBRV_HOME set TIBRV_HOME=C:\Wombat\Tibrv
    set PATH=%PATH%;%TIBRV_HOME%\bin;%TIBRV_HOME%\lib
    set CLASSPATH=%MAMA_HOME%\lib\mama_tibrv.jar;%TIBRV_HOME%\lib\tibrvnative.jar
    set mama_provider_class="com.wombat.mama.bridge.tibrv.RvProvider"
) else (
    echo no provider specified.
    GOTO END
)

set CLASSPATH=%MAMDA_HOME%\lib\mamda.jar;%MAMDA_HOME%\lib\mamda_book.jar;%MAMDA_HOME%\lib\oea.jar;%MAMA_HOME%\lib\mama%no_ent%.jar;%WOMBAT_MSG_HOME%\lib\wombatmsg.jar;%CLASSPATH%

echo java -cp %CLASSPATH% -Dwombat.dir=%WOMBAT_PATH% -Dmama.provider.class=%mama_provider_class% com.wombat.mama.examples.%demo_type% %args%

call java -cp %CLASSPATH% -Dwombat.dir=%WOMBAT_PATH% -Dmama.provider.class=%mama_provider_class% com.wombat.mamda.examples.%demo_type% %args%

GOTO END

:syntax
echo Please specify provider within batch file (tibrv / elvin)

:END
