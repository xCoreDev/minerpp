:Make deps dir
set src_path=%CD%
mkdir deps
cd %src_path%\deps

:Write d/l JScript
echo var url = WScript.Arguments(0),>getfile.js
echo   filename = WScript.Arguments(1),>>getfile.js
echo   fso = WScript.CreateObject('Scripting.FileSystemObject'),>>getfile.js
echo   request, stream;>>getfile.js
echo if (fso.FileExists(filename)) {>>getfile.js
echo   WScript.Echo('Already got ' + filename);>>getfile.js
echo } else {>>getfile.js
echo   WScript.Echo('Downloading ' + url);>>getfile.js
echo   request = WScript.CreateObject('MSXML2.ServerXMLHTTP');>>getfile.js
echo   request.open('GET', url, false);>>getfile.js
echo   request.send();>>getfile.js
echo   if (request.status === 200) {>>getfile.js
echo     WScript.Echo('Size: ' + request.getResponseHeader('Content-Length') + ' bytes');>>getfile.js
echo     stream = WScript.CreateObject('ADODB.Stream');>>getfile.js
echo     stream.Open();>>getfile.js
echo     stream.Type = 1;>>getfile.js
echo     stream.Write(request.responseBody);>>getfile.js
echo     stream.Position = 0;>>getfile.js
echo     stream.SaveToFile(filename, 1);>>getfile.js
echo     stream.Close();>>getfile.js
echo     WScript.Echo('Done');>>getfile.js
echo   } else {>>getfile.js
echo     WScript.Echo('Failed');>>getfile.js
echo     WScript.Quit(1);>>getfile.js
echo   }>>getfile.js
echo }>>getfile.js
echo WScript.Quit(0);>>getfile.js

:Write unzip VBScript
echo pathToZipFile=Wscript.Arguments(0)>unzip.vbs
echo extractTo=Wscript.Arguments(1)>>unzip.vbs
echo set sa = CreateObject("Shell.Application")>>unzip.vbs
echo set filesInzip=sa.NameSpace(pathToZipFile).items>>unzip.vbs
echo sa.NameSpace(extractTo).CopyHere(filesInzip)>>unzip.vbs

:Boost download
cscript //e:jscript getfile.js http://freefr.dl.sourceforge.net/project/boost/boost/1.58.0/boost_1_58_0.zip boost_1_58_0.zip

:Boost unzip
cd %src_path%\deps
cscript unzip.vbs %CD%\boost_1_58_0.zip %CD%

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

:Clean
cd %src_path%\deps
del getfile.js
del unzip.vbs
exit
