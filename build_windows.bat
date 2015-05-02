:Make deps dir
set src_path=%CD%
mkdir deps

:Boost download
cd %src_path%\deps
cscript //e:jscript ..\tools\getfile.js http://freefr.dl.sourceforge.net/project/boost/boost/1.58.0/boost_1_58_0.zip boost_1_58_0.zip

:Boost unzip
cd %src_path%\deps
cscript ..\tools\unzip.vbs %CD%\boost_1_58_0.zip %CD%

:Boost setup
cd %src_path%\deps
ren boost_1_58_0 boost
cd %src_path%\deps\boost
echo exit >> bootstrap.bat
start /wait bootstrap.bat

:Check Architecture & Build Miner++
cd %src_path%\test
if /i "%processor_architecture%"=="x32" (
    if not defined PROCESSOR_ARCHITEW6432 (
        ..\deps\boost\bjam.exe link=static runtime-link=static release
        cd %src_path%\test\bin\msvc-*\release\link-static\runtime-link-static
    ) else (
        ..\deps\boost\bjam.exe link=static runtime-link=static address-model=64 release
        cd %src_path%\test\bin\msvc-*\release\address-model-64\link-static\runtime-link-static
    )
) else (
    ..\deps\boost\bjam.exe link=static runtime-link=static address-model=64 release
    cd %src_path%\test\bin\msvc-*\release\address-model-64\link-static\runtime-link-static
)

:Copy fresh bin to main path
xcopy minerpp.exe %src_path%
exit
