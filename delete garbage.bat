del /S *.dat *.sdf *.exp *.exe *.sys *.tmp *.dll *.ncb *.obj *.ilk *.idb *.dep *.manifest *.res *.exe *.pch *.log *.xml *.aps *.htm *.html *.mac *.pdb *.map *.lib *.tlog *.unsuccessfulbuild *.db *.lastbuildstate *_manifest.rc *.ipch *.opensdf *.i

for /f "delims=" %%i in ('dir /ad /b /s') do call :rd_dir_empty "%%i"
goto :eof
rem ======= ɾ����Ŀ¼ =========
:rd_dir_empty
rd %1 2>nul||goto :eof
set dir_route=%1
for /f "delims=" %%i in (%dir_route%) do (
     set dir_route="%%~dpi"
     for /f "delims=" %%j in ('dir /ad /b "%%~dpi"')do rd "%%~dpi%%j" 2>nul||goto :eof
)
:: ��·������\ȥ�����Ա�set route_deepest="%%~dpi"��ȡ����һ��·��
if "%dir_route:~-2,1%"=="\" set dir_route="%dir_route:~1,-2%"
if /i not "%cd%"==%dir_route% call :rd_dir_empty %dir_route%
goto :eof
