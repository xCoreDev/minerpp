
cd deps
cd boost

rem run this file
rem bootstrap.bat

cd ..\
cd ..\
cd test


..\deps\boost\bjam.exe link=static runtime-link=static release
