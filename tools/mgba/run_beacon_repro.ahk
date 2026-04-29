#Requires AutoHotkey v2.0
#SingleInstance Force
SendMode "Event"
SetTitleMatchMode 2

scriptDir := A_ScriptDir
repoRoot := scriptDir "\..\.."
gMgbaPath := "C:\Program Files\mGBA\mGBA.exe"
gRomPath := repoRoot "\pokeemerald.gba"
gBeaconScript := scriptDir "\mgba_read_beacon.ps1"
gCaptureScript := scriptDir "\mgba_capture_window.ps1"
gOutputRoot := repoRoot "\build\mgba_beacon"
gLogPath := gOutputRoot "\run.log"
gFailurePath := gOutputRoot "\failure.png"
gTruckProofPath := gOutputRoot "\truck_female_a.png"
gStarterProofPath := gOutputRoot "\starter_choose_ready.png"
gLocalRoot := "C:\Temp\mgba_beacon"
gLocalRomPath := gLocalRoot "\pokeemerald.gba"
gMode := "starter"
gMgbaPid := ""
gFastForward := 0
gLastBeaconJson := ""
gWindowPrepared := false

OnExit Cleanup

if (A_Args.Length >= 1)
    gMode := Trim(A_Args[1])
if (A_Args.Length >= 2 && Trim(A_Args[2]) = "speed")
    gFastForward := 1

if !FileExist(gRomPath)
{
    repoRoot := A_WorkingDir
    gRomPath := repoRoot "\pokeemerald.gba"
    gOutputRoot := repoRoot "\build\mgba_beacon"
    gLogPath := gOutputRoot "\run.log"
    gFailurePath := gOutputRoot "\failure.png"
    gTruckProofPath := gOutputRoot "\truck_female_a.png"
    gStarterProofPath := gOutputRoot "\starter_choose_ready.png"
}

if !FileExist(gBeaconScript)
    gBeaconScript := A_WorkingDir "\tools\mgba\mgba_read_beacon.ps1"
if !FileExist(gCaptureScript)
    gCaptureScript := A_WorkingDir "\tools\mgba\mgba_capture_window.ps1"

PrepareOutput()
Log("mode=" gMode " fastForward=" gFastForward)

if (gMode = "validate")
{
    Log("validation only")
    ExitApp 0
}

if !FileExist(gMgbaPath)
    Fail("mGBA not found: " gMgbaPath)
if !FileExist(gRomPath)
    Fail("ROM not found: " gRomPath)
if !FileExist(gBeaconScript)
    Fail("Beacon reader not found: " gBeaconScript)
if !FileExist(gCaptureScript)
    Fail("Capture helper not found: " gCaptureScript)

LaunchMgba()
if (gFastForward)
{
    Send "{Tab down}"
    Log("fast-forward key held")
}

RunToTruck()
if (gMode != "truck")
    RunToStarterChoose()

Log("completed")
ExitApp 0

RunToTruck()
{
    global gTruckProofPath

    Log("phase=boot_to_main_menu")
    WaitForMainMenuFromBoot()
    SendGameKey("A")

    Log("phase=birch_intro")
    AdvanceDialogueUntilStage(3, 120000)

    beacon := WaitForBeacon(Map("stageId", 3, "inputReady", 1), 30000, "gender prompt")
    if (beacon["substageId"] != 1)
    {
        SendGameKey("Down")
        WaitForBeacon(Map("stageId", 3, "substageId", 1, "inputReady", 1), 30000, "female cursor")
    }
    SendGameKey("A")
    WaitForBeacon(Map("gender", 2), 30000, "female confirmed")

    Log("phase=name_prompt")
    AdvanceDialogueUntilStage(5, 120000)
    WaitForBeacon(Map("stageId", 5, "inputReady", 1), 30000, "naming ready")
    SendGameKey("A")
    WaitForBeacon(Map("stageId", 5, "nameLen", 1, "nameChar0", 1), 30000, "name A entered")
    SendGameKey("Start")
    WaitForBeacon(Map("stageId", 5, "flags", 3, "inputReady", 1), 30000, "naming ok ready")
    SendGameKey("A")
    WaitForBeacon(Map("stageId", 6, "gender", 2, "nameLen", 1, "nameChar0", 1), 30000, "name confirmed")

    Log("phase=name_confirm")
    WaitForBeacon(Map("stageId", 6, "inputReady", 1), 30000, "name yes/no ready")
    SendGameKey("A")

    Log("phase=post_name")
    AdvanceDialogueUntilStage(9, 180000)
    WaitForBeacon(Map("stageId", 9, "gender", 2, "nameLen", 1, "nameChar0", 1, "mapKind", 1, "inputReady", 1), 180000, "truck control ready")
    CaptureWindow(gTruckProofPath)
    Log("artifact truck=" gTruckProofPath)
}

RunToStarterChoose()
{
    global gStarterProofPath

    Log("phase=truck_to_starter")
    MoveUntilStage("Right", 10, 4)
    PressReadyAUntil(Map("stageId", 10, "substageId", 4), 90000, "moving-in intro complete")

    MoveUntilSubstage("Up", 10, 5, 5)
    WaitForBeacon(Map("stageId", 10, "substageId", 5, "mapKind", 2, "inputReady", 1), 60000, "bedroom ready")
    MoveRepeated("Right", 2, 10, 5)
    SendGameKey("Up")
    WaitForBeacon(Map("stageId", 10, "substageId", 5, "inputReady", 1), 15000, "clock facing ready")
    SendGameKey("A")

    PressReadyAUntil(Map("stageId", 10, "substageId", 6, "mapKind", 2, "inputReady", 1), 60000, "clock set screen ready")
    SendGameKey("A")
    WaitForBeacon(Map("stageId", 10, "substageId", 7, "mapKind", 2, "inputReady", 1), 30000, "clock confirm ready")
    SendGameKey("Up")
    SendGameKey("A")

    PressReadyAUntil(Map("stageId", 10, "substageId", 6, "mapKind", 2, "inputReady", 1), 90000, "bedroom after clock")
    SendGameKey("Left")
    PressReadyAUntil(Map("stageId", 10, "substageId", 7), 120000, "tv report complete")

    MoveRepeated("Left", 4, 10, 7)
    MoveRepeated("Down", 4, 10, 7)
    WaitForBeacon(Map("stageId", 10, "substageId", 7, "mapKind", 2, "inputReady", 1), 60000, "outside home")

    MoveRepeated("Left", 9, 10, 7)
    SendGameKey("Up")
    PressReadyAUntil(Map("stageId", 10, "substageId", 7), 90000, "rival mom complete")
    MoveRepeated("Up", 6, 10, 7)
    PressReadyAUntil(Map("stageId", 10, "substageId", 7), 90000, "rival intro complete")
    MoveRepeated("Down", 6, 10, 7)
    MoveRepeated("Right", 5, 10, 7)
    MoveRepeated("Up", 9, 10, 7)

    PressReadyAUntil(Map("stageId", 11, "mapKind", 3), 120000, "route 101 approach")
    MoveRepeated("Up", 8, 11, "")
    MoveRepeated("Right", 4, 11, "")
    SendGameKey("A")

    WaitForBeacon(Map("stageId", 12, "mapKind", 4, "inputReady", 1), 120000, "starter choose ready")
    CaptureWindow(gStarterProofPath)
    Log("artifact starter=" gStarterProofPath)
}

WaitForMainMenuFromBoot()
{
    global gLastBeaconJson

    start := A_TickCount
    pressToggle := 0
    Loop
    {
        beacon := ReadBeacon(7000)
        if (beacon["found"] && beacon["stageId"] = 1 && beacon["inputReady"] = 1)
        {
            LogBeacon("accepted main menu", beacon)
            return beacon
        }
        if (A_TickCount - start > 90000)
            Fail("Timed out waiting for main menu. Last beacon: " gLastBeaconJson)

        if (pressToggle)
            SendGameKey("A")
        else
            SendGameKey("Start")
        pressToggle := !pressToggle
        Sleep 500
    }
}

AdvanceDialogueUntilStage(targetStage, timeoutMs)
{
    global gLastBeaconJson

    start := A_TickCount
    Loop
    {
        beacon := ReadBeacon()
        if (beacon["found"] && beacon["stageId"] >= targetStage)
        {
            LogBeacon("accepted target stage", beacon)
            return beacon
        }
        if (A_TickCount - start > timeoutMs)
            Fail("Timed out advancing dialogue to stage " targetStage ". Last beacon: " gLastBeaconJson)
        if (beacon["found"] && beacon["inputReady"] = 1)
            SendGameKey("A")
        else
            Sleep 100
    }
}

PressReadyAUntil(criteria, timeoutMs, label)
{
    global gLastBeaconJson

    start := A_TickCount
    Loop
    {
        beacon := ReadBeacon()
        if (BeaconMatches(beacon, criteria))
        {
            LogBeacon("accepted " label, beacon)
            return beacon
        }
        if (A_TickCount - start > timeoutMs)
            Fail("Timed out waiting for " label ". Last beacon: " gLastBeaconJson)
        if (beacon["found"] && beacon["inputReady"] = 1)
            SendGameKey("A")
        else
            Sleep 100
    }
}

MoveUntilStage(keyName, targetStage, maxPresses)
{
    Loop maxPresses
    {
        beacon := ReadBeacon()
        if (beacon["found"] && beacon["stageId"] = targetStage)
            return beacon
        if (beacon["found"] && beacon["inputReady"] = 1)
            SendGameKey(keyName)
        else
            Sleep 100
    }
    return WaitForBeacon(Map("stageId", targetStage), 30000, "move until stage " targetStage)
}

MoveUntilSubstage(keyName, stageId, targetSubstage, maxPresses)
{
    Loop maxPresses
    {
        beacon := ReadBeacon()
        if (beacon["found"] && beacon["stageId"] = stageId && beacon["substageId"] = targetSubstage)
        {
            LogBeacon("accepted target substage", beacon)
            return beacon
        }
        if (beacon["found"] && beacon["stageId"] = stageId && beacon["inputReady"] = 1)
            SendGameKey(keyName)
        else
            Sleep 100
    }
    return WaitForBeacon(Map("stageId", stageId, "substageId", targetSubstage), 30000, "move until substage " targetSubstage)
}

MoveRepeated(keyName, count, stageId, substageId)
{
    Loop count
    {
        if (substageId = "")
            WaitForBeacon(Map("stageId", stageId, "inputReady", 1), 30000, "move ready")
        else
            WaitForBeacon(Map("stageId", stageId, "substageId", substageId, "inputReady", 1), 30000, "move ready")
        SendGameKey(keyName)
    }
}

WaitForBeacon(criteria, timeoutMs, label)
{
    global gLastBeaconJson

    start := A_TickCount
    Loop
    {
        beacon := ReadBeacon()
        if (BeaconMatches(beacon, criteria))
        {
            LogBeacon("accepted " label, beacon)
            return beacon
        }
        if (A_TickCount - start > timeoutMs)
            Fail("Timed out waiting for " label ". Last beacon: " gLastBeaconJson)
        Sleep 100
    }
}

BeaconMatches(beacon, criteria)
{
    if (!beacon["found"])
        return false
    for key, expected in criteria
    {
        if (expected = "")
            continue
        if (beacon[key] != expected)
            return false
    }
    return true
}

ReadBeacon(timeoutMs := 0)
{
    global gBeaconScript, gMgbaPid, gOutputRoot, gLastBeaconJson

    EnsureMgbaVisible()
    capturePath := gOutputRoot "\beacon_latest.png"
    cmd := "powershell -NoProfile -ExecutionPolicy Bypass -File " Quote(gBeaconScript) " -TargetPid " gMgbaPid " -OutPath " Quote(capturePath) " -MaxScale 2 -ScanWidth 80 -ScanHeight 110"
    json := Trim(ExecStdout(cmd, timeoutMs))
    gLastBeaconJson := json
    return ParseBeacon(json)
}

ParseBeacon(json)
{
    beacon := Map()
    beacon["found"] := InStr(json, '"found":true') ? 1 : 0
    keys := "protocol,stageId,substageId,flags,pulse,gender,nameLen,nameChar0,mapKind,starterSelection,inputReady,errorCode,x,y,scale"
    Loop Parse, keys, ","
    {
        key := A_LoopField
        beacon[key] := GetJsonNumber(json, key)
    }
    return beacon
}

GetJsonNumber(json, key)
{
    pattern := '"' key '":(-?[0-9]+)'
    if RegExMatch(json, pattern, &match)
        return match[1] + 0
    return ""
}

SendGameKey(keyName)
{
    global gMgbaPid

    key := KeyNameToAhk(keyName)
    EnsureMgbaVisible()
    Sleep 20
    Send "{" key " down}"
    Sleep 35
    Send "{" key " up}"
    Sleep 90
    Log("sent key=" keyName)
}

KeyNameToAhk(keyName)
{
    if (keyName = "A")
        return "x"
    if (keyName = "B")
        return "z"
    if (keyName = "Start")
        return "Enter"
    return keyName
}

LaunchMgba()
{
    global gMgbaPath, gRomPath, gMgbaPid, gLocalRoot, gLocalRomPath

    if !DirExist(gLocalRoot)
        DirCreate gLocalRoot
    FileCopy gRomPath, gLocalRomPath, true
    Log("staged rom=" gLocalRomPath)
    try
        Run Quote(gMgbaPath) " -2 " Quote(gLocalRomPath), , , &gMgbaPid
    catch as err
        Fail("Failed to launch mGBA: " err.Message)
    if !WinWait("ahk_pid " gMgbaPid, , 10)
        Fail("Timed out waiting for mGBA window")
    EnsureMgbaVisible()
    Log("mgba pid=" gMgbaPid)
}

CaptureWindow(path)
{
    global gCaptureScript, gMgbaPid

    EnsureMgbaVisible()
    cmd := "powershell -NoProfile -ExecutionPolicy Bypass -File " Quote(gCaptureScript) " -TargetPid " gMgbaPid " -OutPath " Quote(path)
    ExecStdout(cmd)
}

EnsureMgbaVisible()
{
    global gMgbaPid, gWindowPrepared

    if (gMgbaPid = "")
        return

    title := "ahk_pid " gMgbaPid
    if !WinExist(title)
        Fail("mGBA window not found")

    if (!gWindowPrepared)
    {
        WinRestore(title)
        width := 502
        height := 412
        WinMove(0, 0, width, height, title)
        WinSetAlwaysOnTop(1, title)
        gWindowPrepared := true
        Log("prepared mGBA window " width "x" height " topmost")
    }

    WinActivate(title)
    WinWaitActive(title, , 1)
}

ExecStdout(cmd, timeoutMs := 0)
{
    shell := ComObject("WScript.Shell")
    exec := shell.Exec(cmd)
    start := A_TickCount
    if (timeoutMs > 0)
    {
        while (exec.Status = 0)
        {
            if (A_TickCount - start > timeoutMs)
            {
                try exec.Terminate()
                Log("command timeout=" timeoutMs " cmd=" cmd)
                return '{"found":false,"error":"command timeout"}'
            }
            Sleep 50
        }
    }
    output := exec.StdOut.ReadAll()
    err := exec.StdErr.ReadAll()
    if (exec.ExitCode != 0)
        Log("command exit=" exec.ExitCode " stderr=" err)
    return output
}

PrepareOutput()
{
    global gOutputRoot, gLogPath

    if !DirExist(gOutputRoot)
        DirCreate gOutputRoot
    try FileDelete gLogPath
}

LogBeacon(prefix, beacon)
{
    Log(prefix " stage=" beacon["stageId"] " substage=" beacon["substageId"] " flags=" beacon["flags"] " gender=" beacon["gender"] " nameLen=" beacon["nameLen"] " nameChar0=" beacon["nameChar0"] " mapKind=" beacon["mapKind"] " starterSelection=" beacon["starterSelection"] " inputReady=" beacon["inputReady"] " errorCode=" beacon["errorCode"] " pulse=" beacon["pulse"])
}

Log(message)
{
    global gLogPath

    now := FormatTime(, "yyyy-MM-dd HH:mm:ss")
    FileAppend now " " message "`n", gLogPath
}

Fail(message)
{
    global gFailurePath, gMgbaPid

    details := message
    Log("FAIL " message)
    if (gMgbaPid != "")
    {
        CaptureWindow(gFailurePath)
        details := message "`nFailure screenshot: " gFailurePath
    }
    MsgBox details, "Beacon Repro Failed", 16
    ExitApp 1
}

Quote(value)
{
    return '"' value '"'
}

Cleanup(exitReason, exitCode)
{
    global gFastForward

    if (gFastForward)
        Send "{Tab up}"
}
