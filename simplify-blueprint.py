import re
from pathlib import Path

# === Input and Output Paths ===
input_path = Path("junk convert to psuedo")   # Replace with actual path to your .blueprintcopy file
output_path = Path("simplified_output.txt")

# === Utility to Clean Flags ===
def flag(val): return 'y' if val == 'True' else 'n'

# === Extract Function-Like Representation from Node Text ===
def extract_function_call(node_text):
    # Try to extract from FunctionReference or Function fields
    fn_match = re.search(r'MemberName="(.*?)"', node_text)
    if not fn_match:
        fn_match = re.search(r'Function=(\w+)', node_text)
    if not fn_match:
        fn_match = re.search(r'f=NSLOCTEXT\("K2Node",.*?"(.*?)"', node_text)

    fn_name = fn_match.group(1) if fn_match else "UnknownFunction"

    # Pull named pins (skip UUIDs and RETURNVALUE)
    pins = re.findall(r'\s+([A-F0-9]{32}) (\w+)', node_text)
    labels = [label for _, label in pins]

    args = [label for label in labels if label.upper() not in ("RETURNVALUE", "THEN") and not label.startswith('__')]
    ret = next((label for label in labels if "RETURNVALUE" in label.upper()), None)

    arg_str = ", ".join(args)
    return f"{fn_name}({arg_str}) -> {ret}" if ret else f"{fn_name}({arg_str})"

# === Parse Blueprint into Simplified Function-Style Output ===
def parse_blueprint_to_pseudocode(text):
    node_blocks = re.findall(r"Begin Object .*?End Object", text, re.DOTALL)
    pseudo_lines = []

    for block in node_blocks:
        if "K2Node_CallFunction" in block:
            pseudo_lines.append(extract_function_call(block))
        elif "K2Node_MakeStruct" in block:
            struct_name = re.search(r'UserDefinedStruct\"/(.*?)(\"|$)', block)
            if struct_name:
                fields = re.findall(r'f=\"?(\w+)', block)
                field_str = ", ".join(fields)
                pseudo_lines.append(f"MakeStruct({field_str}) -> {struct_name.group(1)}")
        elif "K2Node_IfThenElse" in block:
            pseudo_lines.append("if CONDITION then ... else ...")
        elif "K2Node_VariableSet" in block:
            sets = re.findall(r'\s+([A-F0-9]{32}) (\w+)', block)
            for _, name in sets:
                if name.upper() != "THEN":
                    pseudo_lines.append(f"Set {name}")
        elif "K2Node_VariableGet" in block:
            gets = re.findall(r'\s+([A-F0-9]{32}) (\w+)', block)
            for _, name in gets:
                pseudo_lines.append(f"Get {name}")

    return "\n".join(pseudo_lines)

# === Main Entry ===
if __name__ == '__main__':
    if not input_path.exists():
        raise FileNotFoundError(f"File not found: {input_path}")

    with open(input_path, 'r') as file:
        blueprint_text = file.read()

    simplified = parse_blueprint_to_pseudocode(blueprint_text)

    with open(output_path, 'w') as out_file:
        out_file.write(simplified)

    print("Simplified pseudocode saved to", output_path)
