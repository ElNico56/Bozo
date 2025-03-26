-- bozoasm.lua

local OPCODES = {
	"NOOP III", "HALT III",
	"MOV IMI", "MOV MMI", "MOV PMI",
	"MOV IPI", "MOV MPI", "MOV PPI",
	"MOV IMM", "MOV MMM", "MOV PMM",
	"MOV IPM", "MOV MPM", "MOV PPM",
	"MOV IMP", "MOV MMP", "MOV PMP",
	"MOV IPP", "MOV MPP", "MOV PPP",
	"OUT III", "OUT MII", "OUT PII",
	"OUT IMI", "OUT MMI", "OUT PMI",
	"OUT IPI", "OUT MPI", "OUT PPI",
	"INP MII", "INP PII",
	"INP MMI", "INP PMI",
	"INP MPI", "INP PPI",
	"SHOW III", "SHOW MII", "SHOW PII",
	"SHOW IMI", "SHOW MMI", "SHOW PMI",
	"SHOW IPI", "SHOW MPI", "SHOW PPI",
	"ADD IMI", "ADD MMI", "ADD PMI",
	"ADD IPI", "ADD MPI", "ADD PPI",
	"ADD IMM", "ADD MMM", "ADD PMM",
	"ADD IPM", "ADD MPM", "ADD PPM",
	"ADD IMP", "ADD MMP", "ADD PMP",
	"ADD IPP", "ADD MPP", "ADD PPP",
	"SUB IMI", "SUB MMI", "SUB PMI",
	"SUB IPI", "SUB MPI", "SUB PPI",
	"SUB IMM", "SUB MMM", "SUB PMM",
	"SUB IPM", "SUB MPM", "SUB PPM",
	"SUB IMP", "SUB MMP", "SUB PMP",
	"SUB IPP", "SUB MPP", "SUB PPP",
	"MUL IMI", "MUL MMI", "MUL PMI",
	"MUL IPI", "MUL MPI", "MUL PPI",
	"MUL IMM", "MUL MMM", "MUL PMM",
	"MUL IPM", "MUL MPM", "MUL PPM",
	"MUL IMP", "MUL MMP", "MUL PMP",
	"MUL IPP", "MUL MPP", "MUL PPP",
	"DIV IMI", "DIV MMI", "DIV PMI",
	"DIV IPI", "DIV MPI", "DIV PPI",
	"DIV IMM", "DIV MMM", "DIV PMM",
	"DIV IPM", "DIV MPM", "DIV PPM",
	"DIV IMP", "DIV MMP", "DIV PMP",
	"DIV IPP", "DIV MPP", "DIV PPP",
	"AND IMI", "AND MMI", "AND PMI",
	"AND IPI", "AND MPI", "AND PPI",
	"AND IMM", "AND MMM", "AND PMM",
	"AND IPM", "AND MPM", "AND PPM",
	"AND IMP", "AND MMP", "AND PMP",
	"AND IPP", "AND MPP", "AND PPP",
	"XOR IMI", "XOR MMI", "XOR PMI",
	"XOR IPI", "XOR MPI", "XOR PPI",
	"XOR IMM", "XOR MMM", "XOR PMM",
	"XOR IPM", "XOR MPM", "XOR PPM",
	"XOR IMP", "XOR MMP", "XOR PMP",
	"XOR IPP", "XOR MPP", "XOR PPP",
	"JEQ MII", "JEQ PII",
	"JEQ IMI", "JEQ MMI", "JEQ PMI",
	"JEQ IPI", "JEQ MPI", "JEQ PPI",
	"JEQ MIM", "JEQ PIM",
	"JEQ IMM", "JEQ MMM", "JEQ PMM",
	"JEQ IPM", "JEQ MPM", "JEQ PPM",
	"JEQ MIP", "JEQ PIP",
	"JEQ IMP", "JEQ MMP", "JEQ PMP",
	"JEQ IPP", "JEQ MPP", "JEQ PPP",
	"JNE MII", "JNE PII",
	"JNE IMI", "JNE MMI", "JNE PMI",
	"JNE IPI", "JNE MPI", "JNE PPI",
	"JNE MIM", "JNE PIM",
	"JNE IMM", "JNE MMM", "JNE PMM",
	"JNE IPM", "JNE MPM", "JNE PPM",
	"JNE MIP", "JNE PIP",
	"JNE IMP", "JNE MMP", "JNE PMP",
	"JNE IPP", "JNE MPP", "JNE PPP",
	"JLT MII", "JLT PII",
	"JLT IMI", "JLT MMI", "JLT PMI",
	"JLT IPI", "JLT MPI", "JLT PPI",
	"JLT MIM", "JLT PIM",
	"JLT IMM", "JLT MMM", "JLT PMM",
	"JLT IPM", "JLT MPM", "JLT PPM",
	"JLT MIP", "JLT PIP",
	"JLT IMP", "JLT MMP", "JLT PMP",
	"JLT IPP", "JLT MPP", "JLT PPP",
	"JGT MII", "JGT PII",
	"JGT IMI", "JGT MMI", "JGT PMI",
	"JGT IPI", "JGT MPI", "JGT PPI",
	"JGT MIM", "JGT PIM",
	"JGT IMM", "JGT MMM", "JGT PMM",
	"JGT IPM", "JGT MPM", "JGT PPM",
	"JGT MIP", "JGT PIP",
	"JGT IMP", "JGT MMP", "JGT PMP",
	"JGT IPP", "JGT MPP", "JGT PPP",
}

for i, v in ipairs(OPCODES) do
	OPCODES[v] = i - 1
end

local function parse_operand(operand)
	if operand:match"^@%x+$" then
		return "M", tonumber(operand:sub(2), 16) % 65536
	elseif operand:match"^*%x+$" then
		return "P", tonumber(operand:sub(2), 16) % 65536
	elseif tonumber(operand, 16) then
		return "I", tonumber(operand, 16) % 65536
	else
		return "I", 0
	end
end

local function resolve_label(labels, label)
	if labels[label:sub(2)] or labels[label] then
		if label:sub(1, 1) == "@" then
			return "M", labels[label:sub(2)]
		elseif label:sub(1, 1) == "*" then
			return "P", labels[label:sub(2)]
		else
			return "I", labels[label]
			-- this would return the actual address as
			-- an immediate valu, you could use this as enums
		end
	end
	return nil, nil
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
			for token in line:gmatch"%S+" do
				tokens[#tokens+1] = token:upper()
			end

			tokens[2] = tokens[2] or "0"
			tokens[3] = tokens[3] or "0"
			tokens[4] = tokens[4] or "0"

			local m1, v1 = parse_operand(tokens[2])
			local m2, v2 = parse_operand(tokens[3])
			local m3, v3 = parse_operand(tokens[4])

			-- Resolve label addresse
			local m, v = nil, nil
			m, v = resolve_label(labels, tokens[2])
			m1, v1 = m or m1, v or v1
			m, v = resolve_label(labels, tokens[3])
			m2, v2 = m or m2, v or v2
			m, v = resolve_label(labels, tokens[4])
			m3, v3 = m or m3, v or v3

			local opcode = OPCODES[tokens[1].." "..m1..m2..m3] or -1
			if opcode == -1 then
				_, opcode = parse_operand(tokens[1])
			end
			binary[#binary+1] = opcode
			binary[#binary+1] = v1
			binary[#binary+1] = v2
			binary[#binary+1] = v3
			address = address + 4
		end
	end
	return binary
end

local function read_file(filename)
	local file = io.open(filename, "r")
	if not file then error("Could not open file: "..filename) end
	local content = {}
	for line in file:lines() do content[#content+1] = line end
	file:close()
	return content
end

local function write_file(filename, data)
	local file = io.open(filename, "wb")
	if not file then error("Could not write file: "..filename) end
	for i = 1, 65536 do
		local value = data[i] or 0
		file:write(string.char(value % 256, (value // 256) % 256))
	end
	file:close()
end

local input_file = arg[1] or "input.asm"
local output_file = arg[2] or "output.bin"
local assembly_lines = read_file(input_file)
local machine_code = assemble(assembly_lines)
write_file(output_file, machine_code)
print("Assembled to "..output_file)
