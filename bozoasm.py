# bozoasm.py

import re
import sys

OPCODES = [
    "NOOP III",
    "HALT III",
    "MOV IMI",
    "MOV MMI",
    "MOV PMI",
    "MOV IPI",
    "MOV MPI",
    "MOV PPI",
    "MOV IMM",
    "MOV MMM",
    "MOV PMM",
    "MOV IPM",
    "MOV MPM",
    "MOV PPM",
    "MOV IMP",
    "MOV MMP",
    "MOV PMP",
    "MOV IPP",
    "MOV MPP",
    "MOV PPP",
    "OUT III",
    "OUT MII",
    "OUT PII",
    "OUT IMI",
    "OUT MMI",
    "OUT PMI",
    "OUT IPI",
    "OUT MPI",
    "OUT PPI",
    "INP MII",
    "INP PII",
    "INP MMI",
    "INP PMI",
    "INP MPI",
    "INP PPI",
    "SHOW III",
    "SHOW MII",
    "SHOW PII",
    "SHOW IMI",
    "SHOW MMI",
    "SHOW PMI",
    "SHOW IPI",
    "SHOW MPI",
    "SHOW PPI",
    "ADD IMI",
    "ADD MMI",
    "ADD PMI",
    "ADD IPI",
    "ADD MPI",
    "ADD PPI",
    "ADD IMM",
    "ADD MMM",
    "ADD PMM",
    "ADD IPM",
    "ADD MPM",
    "ADD PPM",
    "ADD IMP",
    "ADD MMP",
    "ADD PMP",
    "ADD IPP",
    "ADD MPP",
    "ADD PPP",
    "SUB IMI",
    "SUB MMI",
    "SUB PMI",
    "SUB IPI",
    "SUB MPI",
    "SUB PPI",
    "SUB IMM",
    "SUB MMM",
    "SUB PMM",
    "SUB IPM",
    "SUB MPM",
    "SUB PPM",
    "SUB IMP",
    "SUB MMP",
    "SUB PMP",
    "SUB IPP",
    "SUB MPP",
    "SUB PPP",
    "MUL IMI",
    "MUL MMI",
    "MUL PMI",
    "MUL IPI",
    "MUL MPI",
    "MUL PPI",
    "MUL IMM",
    "MUL MMM",
    "MUL PMM",
    "MUL IPM",
    "MUL MPM",
    "MUL PPM",
    "MUL IMP",
    "MUL MMP",
    "MUL PMP",
    "MUL IPP",
    "MUL MPP",
    "MUL PPP",
    "DIV IMI",
    "DIV MMI",
    "DIV PMI",
    "DIV IPI",
    "DIV MPI",
    "DIV PPI",
    "DIV IMM",
    "DIV MMM",
    "DIV PMM",
    "DIV IPM",
    "DIV MPM",
    "DIV PPM",
    "DIV IMP",
    "DIV MMP",
    "DIV PMP",
    "DIV IPP",
    "DIV MPP",
    "DIV PPP",
    "AND IMI",
    "AND MMI",
    "AND PMI",
    "AND IPI",
    "AND MPI",
    "AND PPI",
    "AND IMM",
    "AND MMM",
    "AND PMM",
    "AND IPM",
    "AND MPM",
    "AND PPM",
    "AND IMP",
    "AND MMP",
    "AND PMP",
    "AND IPP",
    "AND MPP",
    "AND PPP",
    "XOR IMI",
    "XOR MMI",
    "XOR PMI",
    "XOR IPI",
    "XOR MPI",
    "XOR PPI",
    "XOR IMM",
    "XOR MMM",
    "XOR PMM",
    "XOR IPM",
    "XOR MPM",
    "XOR PPM",
    "XOR IMP",
    "XOR MMP",
    "XOR PMP",
    "XOR IPP",
    "XOR MPP",
    "XOR PPP",
    "JEQ MII",
    "JEQ PII",
    "JEQ IMI",
    "JEQ MMI",
    "JEQ PMI",
    "JEQ IPI",
    "JEQ MPI",
    "JEQ PPI",
    "JEQ MIM",
    "JEQ PIM",
    "JEQ IMM",
    "JEQ MMM",
    "JEQ PMM",
    "JEQ IPM",
    "JEQ MPM",
    "JEQ PPM",
    "JEQ MIP",
    "JEQ PIP",
    "JEQ IMP",
    "JEQ MMP",
    "JEQ PMP",
    "JEQ IPP",
    "JEQ MPP",
    "JEQ PPP",
    "JNE MII",
    "JNE PII",
    "JNE IMI",
    "JNE MMI",
    "JNE PMI",
    "JNE IPI",
    "JNE MPI",
    "JNE PPI",
    "JNE MIM",
    "JNE PIM",
    "JNE IMM",
    "JNE MMM",
    "JNE PMM",
    "JNE IPM",
    "JNE MPM",
    "JNE PPM",
    "JNE MIP",
    "JNE PIP",
    "JNE IMP",
    "JNE MMP",
    "JNE PMP",
    "JNE IPP",
    "JNE MPP",
    "JNE PPP",
    "JLT MII",
    "JLT PII",
    "JLT IMI",
    "JLT MMI",
    "JLT PMI",
    "JLT IPI",
    "JLT MPI",
    "JLT PPI",
    "JLT MIM",
    "JLT PIM",
    "JLT IMM",
    "JLT MMM",
    "JLT PMM",
    "JLT IPM",
    "JLT MPM",
    "JLT PPM",
    "JLT MIP",
    "JLT PIP",
    "JLT IMP",
    "JLT MMP",
    "JLT PMP",
    "JLT IPP",
    "JLT MPP",
    "JLT PPP",
    "JGT MII",
    "JGT PII",
    "JGT IMI",
    "JGT MMI",
    "JGT PMI",
    "JGT IPI",
    "JGT MPI",
    "JGT PPI",
    "JGT MIM",
    "JGT PIM",
    "JGT IMM",
    "JGT MMM",
    "JGT PMM",
    "JGT IPM",
    "JGT MPM",
    "JGT PPM",
    "JGT MIP",
    "JGT PIP",
    "JGT IMP",
    "JGT MMP",
    "JGT PMP",
    "JGT IPP",
    "JGT MPP",
    "JGT PPP",
]
OPCODE_MAP = {v: i for i, v in enumerate(OPCODES)}


def parse_operand(operand):
    if re.match(r"^@[0-9A-Fa-f]+$", operand):
        return "M", int(operand[1:], 16) % 65536
    elif re.match(r"^\*[0-9A-Fa-f]+$", operand):
        return "P", int(operand[1:], 16) % 65536
    elif re.match(r"^\-?[0-9A-Fa-f]+$", operand):
        return "I", int(operand, 16) % 65536
    else:
        return "I", 0


def resolve_label(labels, label):
    key = label[1:] if label[0] in ('@', '*') else label
    if key in labels:
        if label[0] == '@':
            return "M", labels[key]
        elif label[0] == '*':
            return "P", labels[key]
        else:
            return "I", labels[key]
    return None, None


def assemble(lines):
    labels = {}
    binary = [0] * 65536
    address = 0

    # First pass: Collect labels.
    for line in lines:
        line = re.sub(r"#.*", "", line).strip()
        if line:
            label_match = re.match(r"^(\w+):$", line)
            if label_match:
                label = label_match.group(1)
                labels[label] = address
            # Check for an .org directive.
            org_match = re.match(r"^\.org ([0-9A-Fa-f]+)$", line)
            ret_match = re.match(r"^\.ret @?(\w+)$", line)
            # If this line is not just a label, increment address by 4.
            if not label_match and not org_match and not ret_match:
                address += 4
            if org_match:
                address = int(org_match.group(1), 16)

    # Second pass: Generate machine code.
    address = 0
    for line in lines:
        line = re.sub(r"#.*", "", line).strip()
        if re.match(r"^\.org [0-9A-Fa-f]+$", line):
            org_val = re.match(r"^\.org ([0-9A-Fa-f]+)$", line).group(1)
            address = int(org_val, 16)
        elif re.match(r"^\.ret @?\w+$", line):
            ret_match = re.match(r"^\.ret (@?[\w]+)$", line)
            ret = ret_match.group(1)
            _, add_val = resolve_label(labels, ret)
            binary[-1] = add_val
        # Process non-empty lines that are not just a label.
        elif line and not re.match(r"^\w+:$", line):
            tokens = line.split()
            # Ensure there are at least 4 tokens.
            while len(tokens) < 4:
                tokens.append("0")
            # Parse the operands.
            m1, v1 = parse_operand(tokens[1])
            m2, v2 = parse_operand(tokens[2])
            m3, v3 = parse_operand(tokens[3])
            # If tokens reference labels, resolve them.
            m, v = resolve_label(labels, tokens[1])
            if m is not None:
                m1, v1 = m, v
            m, v = resolve_label(labels, tokens[2])
            if m is not None:
                m2, v2 = m, v
            m, v = resolve_label(labels, tokens[3])
            if m is not None:
                m3, v3 = m, v
            # Construct the opcode key.
            opcode_key = f"{tokens[0].upper()} {m1}{m2}{m3}"
            opcode = OPCODE_MAP.get(opcode_key, -1)
            if opcode == -1:
                _, opcode = parse_operand(tokens[0])

            binary[address] = opcode
            binary[address + 1] = v1
            binary[address + 2] = v2
            binary[address + 3] = v3
            address += 4
    return binary


def read_file(filename):
    with open(filename, "r") as f:
        return f.readlines()


def write_file(filename, data):
    with open(filename, "wb") as f:
        for i in range(65536):
            value = data[i] if i < len(data) else 0
            f.write(bytes([value % 256, (value // 256) % 256]))


if __name__ == "__main__":
    input_file = sys.argv[1] if len(sys.argv) > 1 else "input.asm"
    output_file = sys.argv[2] if len(sys.argv) > 2 else "output.bin"
    machine_code = assemble(read_file(input_file))
    write_file(output_file, machine_code)
    print(f"Assembled to {output_file}")
