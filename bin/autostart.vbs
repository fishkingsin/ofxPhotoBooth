		 '__________________________________________________'
'STARTING: OSC.exe'
'__________________________________________________'
Set WshShell = WScript.CreateObject("WScript.Shell")
'wshshell.run "yucolabPhotoBooth.exe"'
'WAIT UNTIL OSC.exe IS STARTED'
WScript.Sleep 1000
Set osh = wscript.CreateObject("wscript.Shell")
WshShell.AppActivate "Eigenschaften von Property Sheet"
'WScript.Sleep 100'
'CONFIRM THE "OK" BUTTON'
'osh.Sendkeys "{ENTER}"
'__________________________________________________'
'STARTING: FLOSC'
'__________________________________________________'
WScript.Sleep 1000
Set WshShell1 = WScript.CreateObject("WScript.Shell")
'STARTING "flosc"'
WScript.Sleep 1500
wshshell1.run "flosc_2.0.jar",0
WScript.Sleep 500
Set osh1 = wscript.CreateObject("wscript.Shell")
WshShell1.AppActivate "Flosc 2.0"
WScript.Sleep 100
osh.Sendkeys "{TAB}"
WScript.Sleep 100
osh.Sendkeys "{TAB}"
WScript.Sleep 100
osh.Sendkeys " " 
WScript.Sleep 100
'STARTING VIDEOWALL'
WScript.Sleep 1000
'_____________________________________________________'
wshshell.run "OfApp.bat",0
WScript.Sleep 10000
wshshell1.run "VideoWallGallery\VideoWallGallery.exe",1
WScript.Sleep 100
wshshell1.run "CoreControllUI\CoreControllUI.exe",1
WScript.Sleep 100
'wshshell1.run "ofxAppMonitor.exe",2
WScript.Sleep 1000
Set osh1 = wscript.CreateObject("wscript.Shell")
WshShell1.AppActivate "ActionScript Interface"
WScript.Sleep 100