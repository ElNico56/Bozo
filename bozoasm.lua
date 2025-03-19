-- bozoasm.lua

local opcode_map = {
	MOV = 0x0,
	ADD = 0x1,
	SUB = 0x2,
	JEQ = 0x3,
	JGT = 0x4,
	JLT = 0x5,
	JNE = 0x6,
	AND = 0x7,
	HALT = 0x3, -- JEQ @XX @XX 000
	NOOP = 0x0, -- MOV @XX @XX 000
	OUT = 0x0, -- MOV ?XX 000 000
	IN = 0x0, -- MOV ?XX 001 000
	SHOW = 0x0, -- MOV ?XX 002 000
}

local function parse_mode(operand)
	if operand:match"^%-%x%x$" then
		return 0, (-tonumber(operand, 16)) % 256
	elseif operand:match"^0%x%x$" then
		return 0, tonumber(operand, 16) % 256
	elseif operand:match"^@%x%x$" then
		return 1, tonumber(operand:sub(2), 16) % 256
	elseif operand:match"^%*%x%x$" then
		return 2, tonumber(operand:sub(2), 16) % 256
	else
		return 0, 0
	end
end

local function assemble(lines)
	local labels, binary, address = {}, {}, 0

	-- First pass: Collect labels
	for _, line in ipairs(lines) do
		line = line:gsub("#.*", ""):match"^%s*(.-)%s*$" -- Remove comments & trim
		if line ~= "" then
			local label = line:match"^(%w+):$"
			if label then labels[label:upper()] = address end
			address = address + (label and 0 or 4)
		end
	end

	-- Second pass: Generate machine code
	address = 0
	for _, line in ipairs(lines) do
		line = line:gsub("#.*", ""):match"^%s*(.-)%s*$"
		if line ~= "" and not line:match"^%w+:$" then
			local tokens = {}
			for token in line:gmatch"%S+" do table.insert(tokens, token:upper()) end
			local opcode = opcode_map[tokens[1]] or -1
			local m1, v1 = parse_mode(tokens[2] or "000")
			local m2, v2 = parse_mode(tokens[3] or "000")
			local m3, v3 = parse_mode(tokens[4] or "000")

			-- Resolve label addresse
			if labels[tokens[2]] then
				m1, v1 = 1, labels[tokens[2]]
			end
			if labels[tokens[3]] then
				m2, v2 = 1, labels[tokens[3]]
			end
			if labels[tokens[4]] then
				m3, v3 = 1, labels[tokens[4]]
			end

			if tokens[1] == "HALT" then
				m1, v1 = 1, v1
				m2, v2 = 1, v1
				m3, v3 = 0, 0
			elseif tokens[1] == "NOOP" then
				m1, v1 = 1, v1
				m2, v2 = 1, v1
				m3, v3 = 0, 0
			elseif tokens[1] == "OUT" then
				m1, v1 = m1, v1
				m3, v3 = m2, v2
				m2, v2 = 0, 0
			elseif tokens[1] == "IN" then
				m1, v1 = m1, v1
				m3, v3 = m2, v2
				m2, v2 = 0, 1
			elseif tokens[1] == "SHOW" then
				m1, v1 = m1, v1
				m3, v3 = m2, v2
				m2, v2 = 0, 2
			end

			local instr = (opcode << 5) + 9 * m1 + 3 * m2 + m3

			if opcode == -1 then
				_, instr = parse_mode(tokens[1])
			end

			table.insert(binary, string.char(instr, v1, v2, v3))
			address = address + 4
		end
	end

	return table.concat(binary)
end

local function read_file(filename)
	local file = io.open(filename, "r")
	if not file then error("Could not open file: "..filename) end
	local content = {}
	for line in file:lines() do table.insert(content, line) end
	file:close()
	return content
end

local function write_file(filename, data)
	for i = 1, 256 - #data do data = data.."\0" end
	local file = io.open(filename, "wb")
	if not file then error("Could not write file: "..filename) end
	file:write(data)
	file:close()
end

local input_file = arg[1] or "input.asm"
local output_file = arg[2] or "output.bin"
local assembly_lines = read_file(input_file)
local machine_code = assemble(assembly_lines)
write_file(output_file, machine_code)
print("Assembled to "..output_file)
