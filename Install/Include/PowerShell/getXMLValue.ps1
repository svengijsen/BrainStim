#call this function from cmd by:
#PowerShell -NoProfile -ExecutionPolicy Bypass -Command "& 'Include\PowerShell\getXMLValue.ps1'"

param([string]$inputfile, [string]$tagname)

#[string]$inputfile = "D:\Sven.Gijsen\Projects\BrainStim\Install\base.props"
#[string]$tagname = "LIBRARY_DIR"
#[string]$expressionstring = '$xmlDoc.GetElementsByTagName("' + $tagname + '")'
#normal call would be: 
#$xmlDoc.Project.PropertyGroup.BRAINSTIM_HOME
#Invoke-Expression $expressionstring



[xml]$xmlDoc = Get-Content $inputfile
$xmlDoc.GetElementsByTagName($tagname)


