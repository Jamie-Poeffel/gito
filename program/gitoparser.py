import re

type_map = {
    "string": str,
    "number": int,    
    "boolean": bool,
}

def parse_gito_file(lines):
    questions = []
    current = {}
    in_options = False
    in_build = False
    options_accum = []
    build_lines = []

    def flush_options():
        nonlocal options_accum, current
        if options_accum:
            opts_raw = " ".join(options_accum)
            options = re.findall(r'"([^"]+)"', opts_raw)
            current["options"] = options
            options_accum = []

    def flush_question():
        nonlocal current
        if current:
            questions.append(current)
            current = {}

    build_string = None

    for line in lines:
        line = line.strip()
        if not line or line.startswith("#"):
            continue

        # End build if another field begins
        if in_build:
            if re.match(r"^(QUESTION|INCLUDES|OPTIONAL|TYPE|OPTIONS|DEFAULT|ID|BUILD):", line):
                in_build = False
            else:
                build_lines.append(line)
                continue

        # End options if new field starts
        if in_options:
            if re.match(r"^(QUESTION|INCLUDES|OPTIONAL|TYPE|OPTIONS|DEFAULT|ID|BUILD):", line):
                flush_options()
                in_options = False
            else:
                options_accum.append(line)
                continue

        if line.startswith("QUESTION:"):
            flush_options()
            flush_question()
            current["question"] = line[len("QUESTION:"):].strip()

        elif line.startswith("OPTIONAL:"):
            flush_options()
            val = line[len("OPTIONAL:"):].strip().lower()
            if val in ("true", "yes", "1"):
                current["optional"] = True
            else:
                current["optional"] = False


        elif line.startswith("TYPE:"):
            flush_options()
            type_str = line[len("TYPE:"):].strip().lower()
            current["type"] = type_map.get(type_str, str)  

        
        elif line.startswith("INCLUDES:"):
            flush_options()
            current["includes"] = line[len("INCLUDES:"):].strip()

        elif line.startswith("OPTIONS:"):
            flush_options()
            opts_part = line[len("OPTIONS:"):].strip()
            if opts_part:
                current["options"] = re.findall(r'"([^"]+)"', opts_part)
            else:
                in_options = True
                options_accum = []

        elif line.startswith("DEFAULT:"):
            flush_options()
            val = line[len("DEFAULT:"):].strip()
            if val.lower() == "true":
                current["default"] = True
            elif val.lower() == "false":
                current["default"] = False
            elif val.isdigit():
                current["default"] = int(val)
            elif re.match(r'^".*"$', val):
                current["default"] = val[1:-1]
            else:
                current["default"] = val

        elif line.startswith("ID:"):
            flush_options()
            current["id"] = line[len("ID:"):].strip()

        elif line.startswith("BUILD:"):
            flush_options()
            flush_question()
            build_part = line[len("BUILD:"):].strip()
            if build_part:
                build_lines = [build_part]
            else:
                build_lines = []
            in_build = True

    flush_options()
    flush_question()

    if build_lines:
        build_string = "\n".join(build_lines)

    return {
        "questions": questions,
        "build": build_string
    }
