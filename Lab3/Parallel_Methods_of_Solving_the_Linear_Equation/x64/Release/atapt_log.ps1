$Newlol = @()
$lol1 = Get-Content .\lol.log
$lol = $lol1 -split "="
ForEach($Line in $lol)
{
If (($Line.StartsWith("Parallel") -ne $True) -and ($Line.StartsWith("Enter") -ne $True) -and ($Line.StartsWith("****") -ne $True) -and ($Line.StartsWith("Press") -ne $True))
{
$Newlol += $Line
}
}
Set-Content .\lol.log -Value $Newlol
(gc .\lol.log) | ? {$_.trim() -ne "" } | set-content .\lol.log

