#####################
#      Made by      #
#      Caste00      #
#####################

import json
from pathlib import Path

byte_befor_opcode = False
label = {}
pc = 0x50       # My chip-8 implementation start at index 0x50 (not 0x200)

def load_instruction_set():
    with open("instruction_set.json", "r", encoding="utf-8") as f:
        data = json.load(f)
    return data
    

def parse_argument(arg):
    global label
    parsed_arg = int()

    if arg.startswith("V"):
        parsed_arg = int(arg[1:], 16)
    elif arg.startswith("0x"):
        parsed_arg = int(arg[2:], 16)
    elif arg.startswith("%"):
        parsed_arg = int(arg[1:], 2)
    elif arg in label:
        parsed_arg = label[arg]
    else:
        parsed_arg = int(arg)
    
    return parsed_arg


def assemble_line(line, instruction_set):
    global byte_befor_opcode
    parts = line.strip().split()
    if not parts:
        return None

    mnemonic = parts[0]

    if mnemonic.endswith(":"):
        return None
    if mnemonic not in instruction_set:
        raise ValueError(f"Instruction {mnemonic} don't recognise")
    
    meta = instruction_set[mnemonic]
    args = []
    if meta["args"] > 0:
        args_str = " ".join(parts[1:])
        args = [parse_argument(i) for i in args_str.split(",")]

    if meta["type"] != "raw" and byte_befor_opcode:
        raise ValueError("Sprite must be at the end of the code")
    
    opcode = meta["code"]
    match meta["type"]:
        case "no_arg":
            op = opcode
        case "raw":
            op = args[0]
            byte_befor_opcode = True
        case "nnn": 
            op = opcode | args[0]
        case "x":
            op = opcode | (args[0] << 8)
        case "xnn":
            op = opcode | (args[0] << 8) | args[1]
        case "xy":
            op = opcode | (args[0] << 8) | (args[1] << 4)
        case "xyn": 
            op = opcode | (args[0] << 8) | (args[1] << 4) | args[2]
        case _:
            raise ValueError(f"Instruction {mnemonic} don't recognise")
    
    return op


def load_label(line, instruction_set):
    global pc
    global label
    parts = line.strip().split()
    mnemonic = parts[0]

    if mnemonic.endswith(":"):
        label[mnemonic[:-1].strip()] = pc
    elif mnemonic in instruction_set:
        pc += 2
    

def assemble_file(filename):
    instruction_set = load_instruction_set()
    opcodes = []
    
    with open(filename, "r") as f:
        lines = f.readlines()
        
    for raw_line in lines:
        # Scan for tabel
        line = raw_line.split(";")[0].strip()
        if not line: 
            continue
        
        label = load_label(line, instruction_set)
    
    for raw_line in lines:
        # make the opcode
        line = raw_line.split(";")[0].strip()
        if not line:
            continue

        opcode = assemble_line(line, instruction_set)
        if opcode is not None:
            opcodes.append(opcode)

    return opcodes


def save_binary(namefile, opcodes):
    with open(namefile, "wb") as f:
        for op in opcodes:
            if op <= 0xFF:
                f.write(bytes([op]))
            else:
                f.write(op.to_bytes(2, byteorder="big"))


if __name__ == "__main__":
    filename = Path(input("File name: "))
    ops = assemble_file(filename)
    save_binary(filename.with_suffix(".ch8"), ops)
    print("Operation completed successfully")