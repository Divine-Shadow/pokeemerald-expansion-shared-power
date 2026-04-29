param(
    [int]$TargetPid = 0,
    [string]$ImagePath = "",
    [string]$OutPath = "",
    [int]$MaxScale = 8,
    [int]$ScanWidth = 320,
    [int]$ScanHeight = 240,
    [int]$MaxColorDistanceSq = 7000
)

Add-Type -AssemblyName System.Drawing
Add-Type -TypeDefinition @'
using System;
using System.Runtime.InteropServices;
public struct RECT { public int Left; public int Top; public int Right; public int Bottom; }
public struct POINT { public int X; public int Y; }
public static class NativeWinBeacon {
  [DllImport("user32.dll")] public static extern bool GetClientRect(IntPtr hWnd, out RECT rect);
  [DllImport("user32.dll")] public static extern bool ClientToScreen(IntPtr hWnd, ref POINT point);
}
'@

function Write-JsonLine($obj) {
    $obj | ConvertTo-Json -Compress
}

function New-Failure($message) {
    Write-JsonLine ([ordered]@{
        found = $false
        error = $message
    })
}

function Convert-GbaChannelTo8Bit($value) {
    return [int][Math]::Round($value * 255 / 31)
}

function New-ExpectedColor($r, $g, $b) {
    return [ordered]@{
        R = Convert-GbaChannelTo8Bit $r
        G = Convert-GbaChannelTo8Bit $g
        B = Convert-GbaChannelTo8Bit $b
    }
}

function Decode-BeaconColor($color, $expectedColors, $maxDistanceSq) {
    $bestValue = -1
    $bestDistance = [int]::MaxValue

    for ($i = 0; $i -lt $expectedColors.Count; $i++) {
        $expected = $expectedColors[$i]
        $dr = [int]$color.R - [int]$expected.R
        $dg = [int]$color.G - [int]$expected.G
        $db = [int]$color.B - [int]$expected.B
        $distance = $dr * $dr + $dg * $dg + $db * $db
        if ($distance -lt $bestDistance) {
            $bestDistance = $distance
            $bestValue = $i
        }
    }

    if ($bestDistance -gt $maxDistanceSq) {
        return -1
    }
    return $bestValue
}

function Read-BeaconValues($bitmap, $x, $y, $scale, $expectedColors, $maxDistanceSq) {
    $values = New-Object 'int[,]' 2,8
    $sampleOffset = [Math]::Max(0, [int][Math]::Floor($scale / 2))

    for ($row = 0; $row -lt 2; $row++) {
        for ($col = 0; $col -lt 8; $col++) {
            $sampleX = $x + $col * $scale + $sampleOffset
            $sampleY = $y + $row * $scale + $sampleOffset
            if ($sampleX -ge $bitmap.Width -or $sampleY -ge $bitmap.Height) {
                return $null
            }
            $color = $bitmap.GetPixel($sampleX, $sampleY)
            $decoded = Decode-BeaconColor -color $color -expectedColors $expectedColors -maxDistanceSq $maxDistanceSq
            if ($decoded -lt 0) {
                return $null
            }
            $values[$row,$col] = $decoded
        }
    }
    return ,$values
}

function Test-BeaconAnchor($bitmap, $x, $y, $scale, $expectedColors, $maxDistanceSq) {
    $sampleOffset = [Math]::Max(0, [int][Math]::Floor($scale / 2))
    $anchorColor0 = $bitmap.GetPixel($x + $sampleOffset, $y + $sampleOffset)
    $anchor0 = Decode-BeaconColor -color $anchorColor0 -expectedColors $expectedColors -maxDistanceSq $maxDistanceSq
    if ($anchor0 -ne 14) {
        return $false
    }

    $anchorColor1 = $bitmap.GetPixel($x + $scale + $sampleOffset, $y + $sampleOffset)
    $anchor1 = Decode-BeaconColor -color $anchorColor1 -expectedColors $expectedColors -maxDistanceSq $maxDistanceSq
    return $anchor1 -eq 13
}

function Read-RunScaledBeaconRow($bitmap, $x, $y, $expectedColors, $maxDistanceSq) {
    $values = New-Object 'int[]' 8
    $pos = $x

    for ($col = 0; $col -lt 8; $col++) {
        if ($pos -ge $bitmap.Width) {
            return $null
        }

        $color = $bitmap.GetPixel($pos, $y)
        $decoded = Decode-BeaconColor -color $color -expectedColors $expectedColors -maxDistanceSq $maxDistanceSq
        if ($decoded -lt 0) {
            return $null
        }

        $values[$col] = $decoded
        $runStart = $pos
        do {
            $pos++
            if ($pos -ge $bitmap.Width) {
                break
            }
            $nextColor = $bitmap.GetPixel($pos, $y)
            $nextDecoded = Decode-BeaconColor -color $nextColor -expectedColors $expectedColors -maxDistanceSq $maxDistanceSq
        } while ($nextDecoded -eq $decoded -and ($pos - $runStart) -le 12)

        if ($pos -eq $runStart) {
            return $null
        }
    }

    return ,$values
}

function Get-DecodedPixel($bitmap, $x, $y, $expectedColors, $maxDistanceSq) {
    if ($x -lt 0 -or $y -lt 0 -or $x -ge $bitmap.Width -or $y -ge $bitmap.Height) {
        return -1
    }

    $color = $bitmap.GetPixel($x, $y)
    return Decode-BeaconColor -color $color -expectedColors $expectedColors -maxDistanceSq $maxDistanceSq
}

function Get-VerticalRunHeight($bitmap, $x, $y, $value, $expectedColors, $maxDistanceSq) {
    $height = 0

    while (($y + $height) -lt $bitmap.Height -and $height -le 12) {
        $decoded = Get-DecodedPixel -bitmap $bitmap -x $x -y ($y + $height) -expectedColors $expectedColors -maxDistanceSq $maxDistanceSq
        if ($decoded -ne $value) {
            break
        }
        $height++
    }

    return $height
}

function New-BeaconResult($values, $x, $y, $scale, $capturePath) {
    $protocol = $values[0,2]
    if ($protocol -ne 1) {
        return $null
    }

    $stageId = $values[0,3]
    $substageId = $values[0,4]
    $flags = $values[0,5]
    $pulse = $values[0,6]
    $checksum = $values[0,7]
    $expectedChecksum = ($protocol + $stageId + $substageId + $flags + $pulse) % 15
    if ($checksum -ne $expectedChecksum) {
        return $null
    }

    return [ordered]@{
        found = $true
        protocol = $protocol
        stageId = $stageId
        substageId = $substageId
        flags = $flags
        pulse = $pulse
        checksumOk = $true
        gender = $values[1,0]
        nameLen = $values[1,1]
        nameChar0 = $values[1,2]
        mapKind = $values[1,3]
        starterSelection = $values[1,4]
        inputReady = $values[1,5]
        errorCode = $values[1,6]
        x = $x
        y = $y
        scale = $scale
        capturePath = $capturePath
    }
}

function Find-RunScaledBeacon($bitmap, $expectedColors, $maxDistanceSq, $scanWidth, $scanHeight, $capturePath) {
    $maxX = [Math]::Min($bitmap.Width - 1, $scanWidth - 1)
    $maxY = [Math]::Min($bitmap.Height - 1, $scanHeight - 1)

    for ($y = 0; $y -le $maxY; $y++) {
        for ($x = 0; $x -le $maxX; $x++) {
            $decoded = Get-DecodedPixel -bitmap $bitmap -x $x -y $y -expectedColors $expectedColors -maxDistanceSq $maxDistanceSq
            if ($decoded -ne 14) {
                continue
            }
            if ($x -gt 0) {
                $prev = Get-DecodedPixel -bitmap $bitmap -x ($x - 1) -y $y -expectedColors $expectedColors -maxDistanceSq $maxDistanceSq
                if ($prev -eq 14) {
                    continue
                }
            }

            $row0 = Read-RunScaledBeaconRow -bitmap $bitmap -x $x -y $y -expectedColors $expectedColors -maxDistanceSq $maxDistanceSq
            if ($null -eq $row0 -or $row0[0] -ne 14 -or $row0[1] -ne 13) {
                continue
            }

            $rowHeight = Get-VerticalRunHeight -bitmap $bitmap -x $x -y $y -value 14 -expectedColors $expectedColors -maxDistanceSq $maxDistanceSq
            if ($rowHeight -lt 1) {
                continue
            }

            $row1Y = $y + $rowHeight
            if ($row1Y -gt $maxY) {
                continue
            }

            $row1 = Read-RunScaledBeaconRow -bitmap $bitmap -x $x -y $row1Y -expectedColors $expectedColors -maxDistanceSq $maxDistanceSq
            if ($null -eq $row1) {
                continue
            }

            $values = New-Object 'int[,]' 2,8
            for ($col = 0; $col -lt 8; $col++) {
                $values[0,$col] = $row0[$col]
                $values[1,$col] = $row1[$col]
            }

            $result = New-BeaconResult -values $values -x $x -y $y -scale $rowHeight -capturePath $capturePath
            if ($null -ne $result) {
                return $result
            }
        }
    }

    return $null
}

function Capture-ClientArea($targetPid, $outPath) {
    $proc = Get-Process -Id $targetPid -ErrorAction Stop
    if ($proc.MainWindowHandle -eq 0) {
        throw "Process has no main window handle."
    }

    $rect = New-Object RECT
    [NativeWinBeacon]::GetClientRect($proc.MainWindowHandle, [ref]$rect) | Out-Null
    $width = [Math]::Max(1, $rect.Right - $rect.Left)
    $height = [Math]::Max(1, $rect.Bottom - $rect.Top)

    $origin = New-Object POINT
    $origin.X = 0
    $origin.Y = 0
    [NativeWinBeacon]::ClientToScreen($proc.MainWindowHandle, [ref]$origin) | Out-Null

    $outDir = Split-Path -Parent $outPath
    if ($outDir -and !(Test-Path $outDir)) {
        New-Item -ItemType Directory -Path $outDir -Force | Out-Null
    }

    $bmp = New-Object System.Drawing.Bitmap($width, $height)
    $gfx = [System.Drawing.Graphics]::FromImage($bmp)
    $gfx.CopyFromScreen($origin.X, $origin.Y, 0, 0, (New-Object System.Drawing.Size($width, $height)))
    $gfx.Dispose()
    $bmp.Save($outPath, [System.Drawing.Imaging.ImageFormat]::Png)
    $bmp.Dispose()
    return $outPath
}

$expectedColors = @(
    New-ExpectedColor 2 2 2
    New-ExpectedColor 31 31 31
    New-ExpectedColor 31 0 0
    New-ExpectedColor 0 31 0
    New-ExpectedColor 0 0 31
    New-ExpectedColor 31 31 0
    New-ExpectedColor 31 0 31
    New-ExpectedColor 0 31 31
    New-ExpectedColor 16 16 16
    New-ExpectedColor 31 16 0
    New-ExpectedColor 16 31 0
    New-ExpectedColor 0 31 16
    New-ExpectedColor 0 16 31
    New-ExpectedColor 16 0 31
    New-ExpectedColor 31 0 16
)

try {
    $capturePath = $ImagePath
    if (!$capturePath) {
        if ($TargetPid -eq 0) {
            New-Failure "TargetPid is required when ImagePath is not provided"
            exit 0
        }
        if (!$OutPath) {
            $OutPath = Join-Path $env:TEMP ("mgba_beacon_capture_{0}.png" -f $TargetPid)
        }
        $capturePath = Capture-ClientArea $TargetPid $OutPath
    }

    $bitmap = [System.Drawing.Bitmap]::FromFile($capturePath)
    try {
        for ($scale = 1; $scale -le $MaxScale; $scale++) {
            $maxY = [Math]::Min($bitmap.Height - (2 * $scale), $ScanHeight - (2 * $scale))
            $maxX = [Math]::Min($bitmap.Width - (8 * $scale), $ScanWidth - (8 * $scale))
            for ($y = 0; $y -le $maxY; $y++) {
                for ($x = 0; $x -le $maxX; $x++) {
                    if (!(Test-BeaconAnchor -bitmap $bitmap -x $x -y $y -scale $scale -expectedColors $expectedColors -maxDistanceSq $MaxColorDistanceSq)) {
                        continue
                    }
                    $values = Read-BeaconValues -bitmap $bitmap -x $x -y $y -scale $scale -expectedColors $expectedColors -maxDistanceSq $MaxColorDistanceSq
                    if ($null -eq $values) {
                        continue
                    }

                    $result = New-BeaconResult -values $values -x $x -y $y -scale $scale -capturePath $capturePath
                    if ($null -eq $result) {
                        continue
                    }

                    Write-JsonLine $result
                    exit 0
                }
            }
        }

        $runScaledResult = Find-RunScaledBeacon -bitmap $bitmap -expectedColors $expectedColors -maxDistanceSq $MaxColorDistanceSq -scanWidth $ScanWidth -scanHeight $ScanHeight -capturePath $capturePath
        if ($null -ne $runScaledResult) {
            Write-JsonLine $runScaledResult
            exit 0
        }
    }
    finally {
        $bitmap.Dispose()
    }

    New-Failure "beacon anchor not found"
}
catch {
    New-Failure $_.Exception.Message
}
