@echo off

del /S *.wrn *.err *.dat *.sdf *.exp *.exe *.sys *.tmp *.dll *.ncb *.obj *.ilk *.idb *.dep *.manifest *.res *.exe *.pch *.log *.xml *.aps *.htm *.html *.mac *.pdb *.map *.lib *.tlog *.unsuccessfulbuild *.db *.lastbuildstate *_manifest.rc *.ipch *.opensdf *.i

for /f "usebackq delims=" %%i in (`dir /ad /b`) do if not "%%i" == "batch" rd /s/q "%%i"
echo on
exit