param(
    [Parameter(Mandatory = $true)]
    [int]$TargetPid,
    [Parameter(Mandatory = $true)]
    [string]$OutPath
)

Add-Type -AssemblyName System.Drawing
Add-Type -TypeDefinition @'
using System;
using System.Runtime.InteropServices;
public struct RECT { public int Left; public int Top; public int Right; public int Bottom; }
public struct POINT { public int X; public int Y; }
public static class NativeWin {
  [DllImport("user32.dll")] public static extern bool GetClientRect(IntPtr hWnd, out RECT rect);
  [DllImport("user32.dll")] public static extern bool ClientToScreen(IntPtr hWnd, ref POINT point);
}
'@

$proc = Get-Process -Id $TargetPid -ErrorAction Stop
if ($proc.MainWindowHandle -eq 0) {
    throw "Process has no main window handle."
}

$rect = New-Object RECT
[NativeWin]::GetClientRect($proc.MainWindowHandle, [ref]$rect) | Out-Null

$width = [Math]::Max(1, $rect.Right - $rect.Left)
$height = [Math]::Max(1, $rect.Bottom - $rect.Top)
$origin = New-Object POINT
$origin.X = 0
$origin.Y = 0
[NativeWin]::ClientToScreen($proc.MainWindowHandle, [ref]$origin) | Out-Null

$outDir = Split-Path -Parent $OutPath
if ($outDir -and !(Test-Path $outDir)) {
    New-Item -ItemType Directory -Path $outDir -Force | Out-Null
}

$bmp = New-Object System.Drawing.Bitmap($width, $height)
$gfx = [System.Drawing.Graphics]::FromImage($bmp)
$gfx.CopyFromScreen($origin.X, $origin.Y, 0, 0, (New-Object System.Drawing.Size($width, $height)))
$gfx.Dispose()
$bmp.Save($OutPath, [System.Drawing.Imaging.ImageFormat]::Png)
$bmp.Dispose()

Write-Output $OutPath
