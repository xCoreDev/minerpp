// getfile.js
var url = WScript.Arguments(0),
  filename = WScript.Arguments(1),
  fso = WScript.CreateObject('Scripting.FileSystemObject'),
  request, stream;
if (fso.FileExists(filename)) {
  WScript.Echo('Already got ' + filename);
} else {
  WScript.Echo('Downloading ' + url);
  request = WScript.CreateObject('MSXML2.ServerXMLHTTP');
  request.open('GET', url, false);
  request.send();
  if (request.status === 200) {
    WScript.Echo("Size: " + request.getResponseHeader("Content-Length") + " bytes");
    stream = WScript.CreateObject('ADODB.Stream');
    stream.Open();
    stream.Type = 1;
    stream.Write(request.responseBody);
    stream.Position = 0;
    stream.SaveToFile(filename, 1);
    stream.Close();
    WScript.Echo('Done');
  } else {
    WScript.Echo('Failed');
    WScript.Quit(1);
  }
}
WScript.Quit(0);