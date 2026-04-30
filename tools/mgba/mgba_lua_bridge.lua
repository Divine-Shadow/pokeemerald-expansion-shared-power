-- mGBA Lua automation bridge.
-- This script is intentionally small: it exposes emulator input and beacon reads
-- over a localhost line protocol so host-side orchestration can avoid GUI focus.

local bridgePort = tonumber(os.getenv("MGBA_LUA_BRIDGE_PORT") or "46510")
local bridgeHost = os.getenv("MGBA_LUA_BRIDGE_HOST") or "127.0.0.1"

local BEACON_TILE_ADDR = 0x06010000 + (1023 * 32)
local BEACON_VALUE_MAX = 14

local KEY_MASKS = {
    A = 1,
    B = 2,
    SELECT = 4,
    START = 8,
    RIGHT = 16,
    LEFT = 32,
    UP = 64,
    DOWN = 128,
    R = 256,
    L = 512,
}

local server = nil
local client = nil
local clientBuffer = ""
local activeKeys = 0
local releaseFrame = nil
local pendingRunFrames = nil

local function jsonEscape(value)
    value = tostring(value)
    value = value:gsub('\\', '\\\\')
    value = value:gsub('"', '\\"')
    value = value:gsub('\n', '\\n')
    value = value:gsub('\r', '\\r')
    value = value:gsub('\t', '\\t')
    return value
end

local function isArray(value)
    local max = 0
    local count = 0

    for key, _ in pairs(value) do
        if type(key) ~= "number" then
            return false
        end
        if key > max then
            max = key
        end
        count = count + 1
    end

    return max == count
end

local function encodeJson(value)
    local valueType = type(value)

    if valueType == "nil" then
        return "null"
    elseif valueType == "boolean" then
        return value and "true" or "false"
    elseif valueType == "number" then
        return tostring(value)
    elseif valueType == "string" then
        return '"' .. jsonEscape(value) .. '"'
    elseif valueType == "table" then
        local parts = {}

        if isArray(value) then
            for i = 1, #value do
                parts[#parts + 1] = encodeJson(value[i])
            end
            return "[" .. table.concat(parts, ",") .. "]"
        end

        for key, item in pairs(value) do
            parts[#parts + 1] = encodeJson(tostring(key)) .. ":" .. encodeJson(item)
        end
        return "{" .. table.concat(parts, ",") .. "}"
    end

    return encodeJson(tostring(value))
end

local function currentFrame()
    if emu == nil then
        return 0
    end
    return emu:currentFrame()
end

local function response(values)
    values.frame = currentFrame()
    return encodeJson(values) .. "\n"
end

local function send(values)
    if client ~= nil then
        client:send(response(values))
    end
end

local function splitWords(line)
    local words = {}

    for word in string.gmatch(line, "%S+") do
        words[#words + 1] = word
    end

    return words
end

local function memoryDomainForAddress(address)
    local region = math.floor(address / 0x01000000)
    local base = region * 0x01000000

    if emu ~= nil and emu.memory ~= nil then
        if region == 0x02 then
            return emu.memory.wram, address - base
        elseif region == 0x03 then
            return emu.memory.iwram, address - base
        elseif region == 0x04 then
            return emu.memory.io, address - base
        elseif region == 0x05 then
            return emu.memory.palette, address - base
        elseif region == 0x06 then
            return emu.memory.vram, address - base
        elseif region == 0x07 then
            return emu.memory.oam, address - base
        elseif region == 0x08 or region == 0x09 then
            return emu.memory.cart0, address - 0x08000000
        elseif region == 0x0A or region == 0x0B then
            return emu.memory.cart1, address - 0x0A000000
        elseif region == 0x0C or region == 0x0D then
            return emu.memory.cart2, address - 0x0C000000
        elseif region == 0x0E or region == 0x0F then
            return emu.memory.sram, address - 0x0E000000
        elseif region == 0x00 then
            return emu.memory.bios, address
        end
    end

    return nil, address
end

local function read8(address)
    local domain, offset = memoryDomainForAddress(address)
    local ok, value = pcall(function()
        if domain ~= nil then
            return domain:read8(offset)
        end
        return emu:read8(address)
    end)

    if not ok or value == nil then
        return nil
    end

    return value
end

local function write8(address, value)
    local domain, offset = memoryDomainForAddress(address)
    local ok, err = pcall(function()
        if domain ~= nil then
            domain:write8(offset, value)
        else
            emu:write8(address, value)
        end
    end)

    if not ok then
        return false, tostring(err)
    end

    return true, nil
end

local function readU32(address)
    local b0 = read8(address)
    local b1 = read8(address + 1)
    local b2 = read8(address + 2)
    local b3 = read8(address + 3)

    if b0 == nil or b1 == nil or b2 == nil or b3 == nil then
        return nil
    end

    return b0 + (b1 * 0x100) + (b2 * 0x10000) + (b3 * 0x1000000)
end

local function writeU32(address, value)
    local bytes = {
        value % 0x100,
        math.floor(value / 0x100) % 0x100,
        math.floor(value / 0x10000) % 0x100,
        math.floor(value / 0x1000000) % 0x100,
    }

    for i = 1, 4 do
        local ok, err = write8(address + i - 1, bytes[i])
        if not ok then
            return false, err
        end
    end

    return true, nil
end

local function decodeTileByte(byte)
    local left = (byte % 16) - 1
    local right = (math.floor(byte / 16) % 16) - 1
    return left, right
end

local function readBeacon()
    local values = {}
    local flat = {}

    if emu == nil then
        return { ok = false, found = false, error = "emu_unavailable", source = "vram" }
    end

    for row = 0, 5 do
        values[row + 1] = {}
        for colByte = 0, 3 do
            local byte = read8(BEACON_TILE_ADDR + (row * 4) + colByte)
            if byte == nil then
                return { ok = false, found = false, error = "vram_read_failed", source = "vram" }
            end

            local left, right = decodeTileByte(byte)
            values[row + 1][(colByte * 2) + 1] = left
            values[row + 1][(colByte * 2) + 2] = right
            flat[#flat + 1] = left
            flat[#flat + 1] = right
        end
    end

    local protocol = values[1][3]
    local stageId = values[1][4]
    local substageId = values[1][5]
    local flags = values[1][6]
    local pulse = values[1][7]
    local checksum = values[1][8]
    local expectedChecksum = (protocol + stageId + substageId + flags + pulse) % 15

    local found = values[1][1] == 14
        and values[1][2] == 13
        and protocol == 1
        and checksum == expectedChecksum
    local navChecksum = (values[3][1] + values[3][2] + values[3][3] + values[3][4] + values[3][5] + values[3][6] + values[3][7]) % 15
    local navFound = values[3][1] == 12
        and values[3][2] == 11
        and values[3][8] == navChecksum
    local mapChecksum = (values[4][1] + values[4][2] + values[4][3] + values[4][4] + values[4][5] + values[4][6] + values[4][7]) % 15
    local mapRowFound = values[4][1] == 10
        and values[4][2] == 9
        and values[4][8] == mapChecksum
    local semanticChecksum = (values[5][1] + values[5][2] + values[5][3] + values[5][4] + values[5][5] + values[5][6] + values[5][7]) % 15
    local semanticFound = values[5][1] == 8
        and values[5][2] == 7
        and values[5][8] == semanticChecksum
    local interactionChecksum = (values[6][1] + values[6][2] + values[6][3] + values[6][4] + values[6][5] + values[6][6] + values[6][7]) % 15
    local interactionFound = values[6][1] == 6
        and values[6][2] == 5
        and values[6][8] == interactionChecksum
    local playerX = values[3][3]
    local playerY = values[3][4]
    local frontX = values[3][6]
    local frontY = values[3][7]

    if mapRowFound then
        playerX = playerX + (15 * values[4][4])
        playerY = playerY + (15 * values[4][5])
        frontX = frontX + (15 * values[4][6])
        frontY = frontY + (15 * values[4][7])
    end

    return {
        ok = true,
        found = found,
        source = "vram",
        protocol = protocol,
        stageId = stageId,
        substageId = substageId,
        flags = flags,
        pulse = pulse,
        checksum = checksum,
        expectedChecksum = expectedChecksum,
        gender = values[2][1],
        nameLen = values[2][2],
        nameChar0 = values[2][3],
        mapKind = values[2][4],
        starterSelection = values[2][5],
        inputReady = values[2][6],
        errorCode = values[2][7],
        reserved = values[2][8],
        navFound = navFound,
        playerX = playerX,
        playerY = playerY,
        playerFacing = values[3][5],
        frontX = frontX,
        frontY = frontY,
        playerXLow = values[3][3],
        playerYLow = values[3][4],
        frontXLow = values[3][6],
        frontYLow = values[3][7],
        navChecksum = values[3][8],
        expectedNavChecksum = navChecksum,
        mapRowFound = mapRowFound,
        mapSlot = values[4][3],
        playerXHi = values[4][4],
        playerYHi = values[4][5],
        frontXHi = values[4][6],
        frontYHi = values[4][7],
        mapChecksum = values[4][8],
        expectedMapChecksum = mapChecksum,
        semanticFound = semanticFound,
        semanticVersion = values[5][3],
        movementReady = values[5][4],
        textReady = values[5][5],
        menuReady = values[5][6],
        interactReady = values[5][7],
        semanticChecksum = values[5][8],
        expectedSemanticChecksum = semanticChecksum,
        interactionFound = interactionFound,
        scriptWaitKind = values[6][3],
        interactableAhead = values[6][4],
        routeErrorCode = values[6][5],
        interactionReserved0 = values[6][6],
        interactionReserved1 = values[6][7],
        interactionChecksum = values[6][8],
        expectedInteractionChecksum = interactionChecksum,
        raw = flat,
    }
end

local function parseMask(words, startIndex)
    local mask = 0

    for i = startIndex, #words do
        local token = string.upper(words[i])
        local numeric = tonumber(token)
        if numeric ~= nil then
            mask = mask + numeric
        elseif KEY_MASKS[token] ~= nil then
            mask = mask + KEY_MASKS[token]
        else
            return nil, "unknown_key:" .. words[i]
        end
    end

    return mask, nil
end

local function setKeys(mask)
    activeKeys = mask or 0
    releaseFrame = nil
    emu:setKeys(activeKeys)
end

local function handleCommand(line)
    local words = splitWords(line)
    local command = string.lower(words[1] or "")

    if command == "" then
        return
    elseif command == "ping" then
        send({ ok = true, type = "pong" })
    elseif command == "read_beacon" then
        local beacon = readBeacon()
        beacon.type = "beacon"
        send(beacon)
    elseif command == "read_u32_array" then
        local address = tonumber(words[2])
        local count = tonumber(words[3])
        local values = {}

        if address == nil or count == nil or count < 0 then
            send({ ok = false, type = "read_u32_array", error = "bad_arguments" })
            return
        end

        for i = 0, count - 1 do
            local value = readU32(address + (i * 4))
            if value == nil then
                send({
                    ok = false,
                    type = "read_u32_array",
                    address = address,
                    count = count,
                    error = "memory_read_failed",
                    index = i,
                })
                return
            end
            values[#values + 1] = value
        end

        send({ ok = true, type = "read_u32_array", address = address, count = count, values = values })
    elseif command == "write_u32" then
        local address = tonumber(words[2])
        local value = tonumber(words[3])

        if address == nil or value == nil or value < 0 then
            send({ ok = false, type = "write_u32", error = "bad_arguments" })
            return
        end

        local ok, err = writeU32(address, value)
        if not ok then
            send({ ok = false, type = "write_u32", address = address, value = value, error = err })
            return
        end

        send({ ok = true, type = "write_u32", address = address, value = value })
    elseif command == "set_keys" then
        local mask, err = parseMask(words, 2)
        if mask == nil then
            send({ ok = false, type = "set_keys", error = err })
            return
        end
        setKeys(mask)
        send({ ok = true, type = "set_keys", keys = activeKeys })
    elseif command == "clear_keys" then
        setKeys(0)
        send({ ok = true, type = "clear_keys", keys = activeKeys })
    elseif command == "tap" then
        local frames = tonumber(words[#words]) or 4
        local keyEnd = #words
        if tonumber(words[#words]) ~= nil then
            keyEnd = #words - 1
        end

        local keyWords = {}
        for i = 2, keyEnd do
            keyWords[#keyWords + 1] = words[i]
        end
        local mask, err = parseMask(keyWords, 1)
        if mask == nil then
            send({ ok = false, type = "tap", error = err })
            return
        end

        activeKeys = mask
        releaseFrame = currentFrame() + math.max(1, frames)
        emu:setKeys(activeKeys)
        send({ ok = true, type = "tap", keys = activeKeys, releaseFrame = releaseFrame })
    elseif command == "run_frames" then
        local frames = math.max(1, tonumber(words[2]) or 1)
        pendingRunFrames = {
            startFrame = currentFrame(),
            targetFrame = currentFrame() + frames,
            frames = frames,
        }
    elseif command == "savestate" then
        local path = words[2]
        local flags = tonumber(words[3]) or 0
        if path == nil then
            send({ ok = false, type = "savestate", error = "missing_path" })
            return
        end
        local ok, result = pcall(function()
            return emu:saveStateFile(path, flags)
        end)
        if not ok then
            send({ ok = false, type = "savestate", path = path, error = tostring(result) })
            return
        end
        local method = "file"
        local size = 0
        if result ~= true then
            local bufferOk, buffer = pcall(function()
                return emu:saveStateBuffer(flags)
            end)
            if bufferOk and buffer ~= nil then
                local handle, fileError = io.open(path, "wb")
                if handle == nil then
                    send({ ok = false, type = "savestate", path = path, flags = flags, error = tostring(fileError) })
                    return
                end
                handle:write(buffer)
                handle:close()
                result = true
                method = "buffer"
                size = #buffer
            end
        end
        send({ ok = result == true, type = "savestate", path = path, flags = flags, method = method, size = size })
    elseif command == "screenshot" then
        local path = words[2]
        if path == nil then
            send({ ok = false, type = "screenshot", error = "missing_path" })
            return
        end
        local ok, result = pcall(function()
            emu:screenshot(path)
            return true
        end)
        if not ok then
            send({ ok = false, type = "screenshot", path = path, error = tostring(result) })
            return
        end
        send({ ok = result == true, type = "screenshot", path = path })
    else
        send({ ok = false, type = "unknown", error = "unknown_command", command = command })
    end
end

local function onClientData()
    local data, err = client:receive(4096)

    if data == nil then
        send({ ok = false, type = "socket", error = err or "receive_failed" })
        return
    end

    clientBuffer = clientBuffer .. data
    while true do
        local newline = string.find(clientBuffer, "\n", 1, true)
        if newline == nil then
            break
        end

        local line = string.sub(clientBuffer, 1, newline - 1)
        clientBuffer = string.sub(clientBuffer, newline + 1)
        handleCommand(line)
    end
end

local function onServerConnection()
    if client ~= nil then
        local extra = server:accept()
        if extra ~= nil then
            extra:send(response({ ok = false, type = "connect", error = "client_already_connected" }))
        end
        return
    end

    client = server:accept()
    if client == nil then
        return
    end

    client:add("received", onClientData)
    send({ ok = true, type = "connected", port = bridgePort })
end

local function onFrame()
    if releaseFrame ~= nil and currentFrame() >= releaseFrame then
        setKeys(0)
    end

    if pendingRunFrames ~= nil and currentFrame() >= pendingRunFrames.targetFrame then
        send({
            ok = true,
            type = "run_frames",
            startFrame = pendingRunFrames.startFrame,
            targetFrame = pendingRunFrames.targetFrame,
            frames = pendingRunFrames.frames,
        })
        pendingRunFrames = nil
    end

    if server ~= nil then
        server:poll()
    end
    if client ~= nil then
        client:poll()
    end
end

server = socket.bind(bridgeHost, bridgePort)
server:listen(1)
server:add("received", onServerConnection)
callbacks:add("frame", onFrame)
console:log("mGBA Lua bridge listening on " .. bridgeHost .. ":" .. tostring(bridgePort))
