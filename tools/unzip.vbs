' unzip.vbs
pathToZipFile=Wscript.Arguments(0)
extractTo=Wscript.Arguments(1)
set sa = CreateObject("Shell.Application")
set filesInzip=sa.NameSpace(pathToZipFile).items
sa.NameSpace(extractTo).CopyHere(filesInzip)