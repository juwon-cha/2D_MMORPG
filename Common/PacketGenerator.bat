rem ���� �������� ���ϵ� ����
set CLIENT_SCRIPT_PATH=..\Assets\Scripts
set SERVER_SCRIPT_PATH=..\2D_MMO_Server\GameServer
set DBSERVER_SCRIPT_PATH=..\2D_MMO_Server\DBServer

set ROOT_FBS=fbs/Protocol.fbs

rem ȯ�溯�� ���� ����
setlocal enabledelayedexpansion

del /S /Q "*.cs"
del /S /Q "*.h"
del /S /Q "*.cpp"
del /S /Q "%CLIENT_SCRIPT_PATH%\FlatBuffer\*.cs"
del /S /Q "%SERVER_SCRIPT_PATH%\*generated.h"
del /S /Q "%DBSERVER_SCRIPT_PATH%\*generated.h"

set "fileList="

rem ���� ���丮�� ��� .fbs ������ ����
for %%f in (./fbs/*.fbs) do (
    set "fileList=!fileList! fbs/%%f"
)

rem flatc ���� (������ ���� ����Ʈ�� �� ���� ����)
START ./flatc.exe --csharp %fileList%
START ./flatc.exe --cpp %fileList%
START ../2D_MMO_Server/PacketGenerator/bin/Debug/net8.0/PacketGenerator.exe %ROOT_FBS%

rem 3�� ��ٸ� �� ���� ����
timeout /t 3 /nobreak >nul
for %%f in (*.cs) do (
    copy "%%f" "%CLIENT_SCRIPT_PATH%/FlatBuffer"
)

rem ---------- Server -----------
for %%f in (*.h) do (
    XCOPY /Y ".\%%f" %SERVER_SCRIPT_PATH%
)
XCOPY /Y ".\Server\PacketManager.h" %SERVER_SCRIPT_PATH%
XCOPY /Y ".\Server\PacketManager.cpp" %SERVER_SCRIPT_PATH%
rem -----------------------------

rem ---------- DBServer -----------
for %%f in (*.h) do (
   XCOPY /Y ".\%%f" %DBSERVER_SCRIPT_PATH%
)
XCOPY /Y ".\DBServer\PacketManager.h" %DBSERVER_SCRIPT_PATH%
XCOPY /Y ".\DBServer\PacketManager.cpp" %DBSERVER_SCRIPT_PATH%
rem -------------------------------

rem ---------- Client -------------
XCOPY /Y ".\Client\PacketManager.cs" "%CLIENT_SCRIPT_PATH%\Packet"
rem -------------------------------

pause