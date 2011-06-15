		 '__________________________________________________'
'STARTING: OSC.exe'
'__________________________________________________'
Set WshShell = WScript.CreateObject("WScript.Shell")
wshshell.run "ApplicationMonitor.exe",2
'wshshell.run "OfApp.bat",0
'WScript.Sleep 10000
'wshshell1.run "VideoWallGallery\VideoWallGallery.exe",1
'WScript.Sleep 100
'wshshell1.run "CoreControllUI\CoreControllUI.exe",1
'WScript.Sleep 100
'wshshell1.run "ofxAppMonitor.exe",2
'WScript.Sleep 1000
Set osh1 = wscript.CreateObject("wscript.Shell")
WshShell.AppActivate "ActionScript Interface"
WScript.Sleep 100