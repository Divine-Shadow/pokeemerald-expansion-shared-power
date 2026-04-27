param(
    [Parameter(Mandatory = $true)]
    [string]$ReferencePath,
    [Parameter(Mandatory = $true)]
    [string]$CurrentPath,
    [int]$Stride = 8,
    [int]$SkipTop = 70
)

Add-Type -AssemblyName System.Drawing

if (!(Test-Path $ReferencePath) -or !(Test-Path $CurrentPath)) {
    Write-Output "999"
    exit 0
}

$ref = [System.Drawing.Bitmap]::new($ReferencePath)
$cur = [System.Drawing.Bitmap]::new($CurrentPath)

try {
    $width = [Math]::Min($ref.Width, $cur.Width)
    $height = [Math]::Min($ref.Height, $cur.Height)
    if ($width -lt 1 -or $height -lt 1) {
        Write-Output "999"
        exit 0
    }

    $startY = [Math]::Min([Math]::Max($SkipTop, 0), $height - 1)
    $sum = 0.0
    $samples = 0

    for ($y = $startY; $y -lt $height; $y += $Stride) {
        for ($x = 0; $x -lt $width; $x += $Stride) {
            $a = $ref.GetPixel($x, $y)
            $b = $cur.GetPixel($x, $y)
            $sum += [Math]::Abs([int]$a.R - [int]$b.R)
            $sum += [Math]::Abs([int]$a.G - [int]$b.G)
            $sum += [Math]::Abs([int]$a.B - [int]$b.B)
            $samples++
        }
    }

    if ($samples -eq 0) {
        Write-Output "999"
        exit 0
    }

    $score = $sum / ($samples * 3.0)
    Write-Output ("{0:N3}" -f $score)
}
finally {
    $ref.Dispose()
    $cur.Dispose()
}
