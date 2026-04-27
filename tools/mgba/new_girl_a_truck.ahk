#NoEnv
#SingleInstance Force
SendMode Event
SetBatchLines, -1
SetTitleMatchMode, 2

; Deterministic intro automation for Emerald:
; - map mode: executes a curated step timeline with 5s spacing and per-step screenshots
; - run mode: executes the same timeline with checkpoint gating to avoid overshoot

gScriptDir := A_ScriptDir
gRepoRoot := gScriptDir "\..\.."
gMgbaPath := "C:\Program Files\mGBA\mGBA.exe"
gRomPath := gRepoRoot "\pokeemerald.gba"
gLogPath := "C:\Temp\mgba_intro_automation.log"
gCaptureScript := gScriptDir "\mgba_capture_window.ps1"
gCompareScript := gScriptDir "\mgba_compare_capture.ps1"
gOutputRoot := gRepoRoot "\build\mgba_steps"
gRunCaptureDir := gOutputRoot "\run"
gMapCaptureDir := gOutputRoot "\map"
gCheckpointDir := gOutputRoot "\checkpoints"
gVerifyScreenshot := gRepoRoot "\build\mgba_truck_verify.png"
gScreenshotKey := "F12"
gUsePerStepBlockInput := 0
gBlockInputWholeRun := 1
gCheckpointThreshold := 20.0
gMode := "run"
gMgbaPid := ""
gInputBlocked := 0

OnExit, Cleanup

if (IsObject(A_Args) && A_Args.MaxIndex() >= 1)
    gMode := Trim(A_Args[1])

if (gMode != "map" && gMode != "run")
{
    MsgBox, 16, Invalid Mode, Use "map" or "run".
    ExitApp
}

; Fallback for Windows environments where UNC-hosted AHK scripts do not execute
; correctly: prefer current working directory if script-relative repo paths fail.
if !FileExist(gRomPath)
{
    gRepoRoot := A_WorkingDir
    gRomPath := gRepoRoot "\pokeemerald.gba"
    gOutputRoot := gRepoRoot "\build\mgba_steps"
    gRunCaptureDir := gOutputRoot "\run"
    gMapCaptureDir := gOutputRoot "\map"
    gCheckpointDir := gOutputRoot "\checkpoints"
    gVerifyScreenshot := gRepoRoot "\build\mgba_truck_verify.png"
}

if !FileExist(gCaptureScript)
    gCaptureScript := A_WorkingDir "\tools\mgba\mgba_capture_window.ps1"
if !FileExist(gCompareScript)
    gCompareScript := A_WorkingDir "\tools\mgba\mgba_compare_capture.ps1"

PrepareOutputDirs()
FileDelete, %gLogPath%
Log("mode=" gMode)

if !FileExist(gCaptureScript)
{
    MsgBox, 16, Missing Helper, Missing helper script: %gCaptureScript%
    ExitApp
}

if !FileExist(gCompareScript)
{
    MsgBox, 16, Missing Helper, Missing helper script: %gCompareScript%
    ExitApp
}

if !FileExist(gMgbaPath)
{
    MsgBox, 16, Missing mGBA, mGBA not found: %gMgbaPath%
    ExitApp
}

if !FileExist(gRomPath)
{
    MsgBox, 16, Missing ROM, ROM not found: %gRomPath%
    ExitApp
}

steps := BuildSteps()
LaunchMgba()

if (gBlockInputWholeRun)
{
    BlockInput, On
    gInputBlocked := 1
    Log("block input enabled for full run")
}

if (gMode = "map")
    RunMap(steps)
else
    RunDeterministic(steps)

ExitApp

RunMap(steps)
{
    global gMapCaptureDir
    index := 0
    for _, step in steps
    {
        index++
        ExecuteStep(step, true)
        capturePath := gMapCaptureDir "\" Format("{:03}", index) "_" step.id ".png"
        CaptureWindow(capturePath)
        Log("map capture: " capturePath)

        if (step.checkpoint != "")
        {
            checkpointPath := GetCheckpointPath(step.checkpoint)
            FileCopy, %capturePath%, %checkpointPath%, 1
            Log("checkpoint updated: " checkpointPath)
        }
        Sleep, 5000
    }
}

RunDeterministic(steps)
{
    global gRunCaptureDir, gVerifyScreenshot
    index := 0
    for _, step in steps
    {
        index++
        ok := ExecuteStep(step, false)
        if (!ok)
        {
            failPath := gRunCaptureDir "\" Format("{:03}", index) "_" step.id "_FAILED.png"
            CaptureWindow(failPath)
            MsgBox, 16, Checkpoint Failed, % "Step failed: " step.id "`nCapture: " failPath
            ExitApp
        }
    }

    ; Standard verify capture.
    CaptureWindow(gVerifyScreenshot)
    Log("verify capture: " gVerifyScreenshot)
}

ExecuteStep(step, isMapMode)
{
    if (step.kind = "wait")
    {
        Sleep, % step.ms
        return true
    }

    if (step.kind = "key")
    {
        SendGameKey(step.key)
        Sleep, % step.settleMs
        return CheckpointGate(step, isMapMode)
    }

    if (step.kind = "advance_until")
        return AdvanceUntil(step, isMapMode)

    Log("unknown step kind: " step.kind)
    return false
}

AdvanceUntil(step, isMapMode)
{
    ; In map mode, run the full bounded sequence to gather the timeline.
    ; In run mode, stop early as soon as checkpoint matches.
    Loop, % step.maxPresses
    {
        SendGameKey(step.key)
        Sleep, % step.intervalMs

        if (isMapMode)
            continue

        if (step.checkpoint = "")
            continue

        if (Mod(A_Index, step.checkEvery) != 0)
            continue

        if (CheckpointMatches(step.checkpoint))
        {
            Log("advance_until matched checkpoint: " step.checkpoint " at press " A_Index)
            Sleep, % step.settleMs
            return true
        }
    }

    if (isMapMode)
    {
        Sleep, % step.settleMs
        return true
    }

    ; In run mode, if no checkpoint reference exists, allow bounded fallback.
    if (!FileExist(GetCheckpointPath(step.checkpoint)))
    {
        Log("checkpoint reference missing, fallback allowed for: " step.checkpoint)
        Sleep, % step.settleMs
        return true
    }

    Log("advance_until timeout on checkpoint: " step.checkpoint)
    return false
}

CheckpointGate(step, isMapMode)
{
    if (isMapMode)
        return true
    if (step.checkpoint = "")
        return true
    return CheckpointMatches(step.checkpoint)
}

CheckpointMatches(checkpointId)
{
    global gRunCaptureDir, gCheckpointThreshold
    refPath := GetCheckpointPath(checkpointId)
    if !FileExist(refPath)
    {
        Log("checkpoint missing: " checkpointId)
        return false
    }

    curPath := gRunCaptureDir "\checkpoint_" checkpointId ".png"
    CaptureWindow(curPath)
    score := CompareImages(refPath, curPath)
    if (score < 0)
    {
        Log("checkpoint " checkpointId " compare parse failed")
        return false
    }

    Log("checkpoint " checkpointId " score=" score)
    return (score <= gCheckpointThreshold)
}

BuildSteps()
{
    steps := []

    ; Boot/title/menu
    steps.Push({id: "boot_wait", kind: "wait", ms: 12000, checkpoint: ""})
    steps.Push({id: "title_start", kind: "key", key: "Enter", settleMs: 2200, checkpoint: "title_menu"})
    steps.Push({id: "menu_new_game", kind: "key", key: "x", settleMs: 1700, checkpoint: "new_game"})

    ; Intro text -> gender select (checkpointed, no endless spam).
    steps.Push({id: "to_gender_prompt", kind: "advance_until", key: "x", maxPresses: 860, intervalMs: 220, checkEvery: 2, settleMs: 500, checkpoint: "gender_prompt"})

    ; Gender (prefer GIRL).
    steps.Push({id: "gender_select_down", kind: "key", key: "Right", settleMs: 120, checkpoint: ""})
    steps.Push({id: "gender_confirm", kind: "key", key: "x", settleMs: 700, checkpoint: "gender_confirm"})

    ; Name entry: ensure exactly A then accept.
    steps.Push({id: "name_choose_new", kind: "advance_until", key: "x", maxPresses: 30, intervalMs: 180, checkEvery: 2, settleMs: 350, checkpoint: "name_screen"})
    steps.Push({id: "name_pick_A", kind: "key", key: "x", settleMs: 350, checkpoint: "name_char_selected"})
    steps.Push({id: "name_accept", kind: "key", key: "Enter", settleMs: 500, checkpoint: "name_confirmed"})

    ; Intro tail -> truck scene.
    steps.Push({id: "to_truck_scene", kind: "advance_until", key: "x", maxPresses: 700, intervalMs: 180, checkEvery: 3, settleMs: 900, checkpoint: "truck_scene"})

    ; mGBA local screenshot for convenience, plus our own capture at end.
    steps.Push({id: "mgba_shot", kind: "key", key: gScreenshotKey, settleMs: 120, checkpoint: ""})
    return steps
}

SendGameKey(key)
{
    global gMgbaPid, gUsePerStepBlockInput
    if (gMgbaPid = "")
        return

    ; Keep focus deterministic while the key is being sent.
    WinActivate, ahk_pid %gMgbaPid%
    WinWaitActive, ahk_pid %gMgbaPid%,, 2

    if (gUsePerStepBlockInput)
        BlockInput, On

    ; Foreground send is the most reliable path for mGBA gameplay input.
    Send, {%key%}
    Sleep, 35

    if (gUsePerStepBlockInput)
        BlockInput, Off
}

LaunchMgba()
{
    global gMgbaPath, gRomPath, gMgbaPid
    Run, "%gMgbaPath%" "%gRomPath%",, UseErrorLevel, gMgbaPid
    if ErrorLevel
    {
        MsgBox, 16, Launch Failed, Failed to launch mGBA.
        ExitApp
    }

    WinWait, ahk_pid %gMgbaPid%,, 15
    if ErrorLevel
    {
        MsgBox, 16, Launch Failed, mGBA window did not appear.
        ExitApp
    }
}

CaptureWindow(outPath)
{
    global gCaptureScript, gMgbaPid
    tmpLocal := A_Temp "\mgba_cap_" A_TickCount ".png"
    tmpErr := A_Temp "\mgba_capture_err.txt"
    FileDelete, %tmpErr%
    FileDelete, %tmpLocal%
    cmd := ComSpec " /c powershell -NoProfile -ExecutionPolicy Bypass -File """ gCaptureScript """ -TargetPid " gMgbaPid " -OutPath """ tmpLocal """ >nul 2>""" tmpErr """"
    RunWait, %cmd%,, Hide

    if !FileExist(tmpLocal)
    {
        Log("capture failed, no temp image: " tmpLocal)
        return false
    }

    outDir := RegExReplace(outPath, "\\[^\\]+$")
    FileCreateDir, %outDir%
    FileCopy, %tmpLocal%, %outPath%, 1
    if !FileExist(outPath)
    {
        Log("capture copy failed: " outPath)
        return false
    }
    return true
}

CompareImages(refPath, curPath)
{
    global gCompareScript
    tmpRef := A_Temp "\mgba_ref_" A_TickCount ".png"
    tmpCur := A_Temp "\mgba_cur_" A_TickCount ".png"
    tmpOut := A_Temp "\mgba_compare_out.txt"

    FileCopy, %refPath%, %tmpRef%, 1
    FileCopy, %curPath%, %tmpCur%, 1
    if (!FileExist(tmpRef) || !FileExist(tmpCur))
        return -1.0

    FileDelete, %tmpOut%
    cmd := ComSpec " /c powershell -NoProfile -ExecutionPolicy Bypass -File """ gCompareScript """ -ReferencePath """ tmpRef """ -CurrentPath """ tmpCur """ >""" tmpOut """ 2>&1"
    RunWait, %cmd%,, Hide
    FileRead, content, %tmpOut%
    content := Trim(content)
    if (content = "")
    {
        Log("compare output empty")
        return -1.0
    }

    if RegExMatch(content, "[-+]?\d+(?:\.\d+)?", m)
        return m + 0.0

    Log("compare output unparsable: " content)
    return -1.0
}

GetCheckpointPath(checkpointId)
{
    global gCheckpointDir
    return gCheckpointDir "\" checkpointId ".png"
}

PrepareOutputDirs()
{
    global gOutputRoot, gRunCaptureDir, gMapCaptureDir, gCheckpointDir
    FileCreateDir, %gOutputRoot%
    FileCreateDir, %gRunCaptureDir%
    FileCreateDir, %gMapCaptureDir%
    FileCreateDir, %gCheckpointDir%
}

Log(msg)
{
    global gLogPath
    FormatTime, nowIso,, yyyy-MM-dd HH:mm:ss
    FileAppend, %nowIso% - %msg%`n, %gLogPath%
}

Cleanup:
if (gInputBlocked)
{
    BlockInput, Off
    gInputBlocked := 0
}
Return
