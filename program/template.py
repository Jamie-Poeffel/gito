from gitoparser import parse_gito_file
import os

def main():
    template_file = os.path.join(os.getcwd(), ".gito", "template.gito")

    with open(template_file, "r", encoding="utf-8") as f:
        parsed = parse_gito_file(f.readlines())
    
    answers = {}
    for quest in parsed["questions"]:
        answer = ask(quest)
        if answer["id"]: 
            answers[answer["id"]] = answer["answer"]
    
    print(build(parsed["build"], answers))

def ask(question):
    answer = {}

    inpu = None
    while inpu == "" or inpu == None:
        options = question.get("options", [])
        default = question.get("default")

        if options:
            formatted_options = []
            for opt in options:
                if default and opt == default:
                    formatted_options.append(opt.upper())
                else:
                    formatted_options.append(opt)
            options_str = f" ({', '.join(formatted_options)})"
        else:
            options_str = ""

        inpu = input(question.get("question").strip().strip('"') + options_str + ": ")

        if inpu == "":
            if question.get("optional") != "":
                if question.get("optional") == True:
                    break
                else:
                    if default is None or default == "":
                        continue
                    inpu = default

                break
        
            continue

        if type(inpu) != question.get("type"):
            inpu = ""
            continue

        if inpu != "" and question.get("includes") == "brackets":
            inpu = f"({inpu})"
            break



    answer["answer"] = inpu
    answer["id"] = question.get("id")
    return answer


def build(build_str, answers):
    def replacer(match):
        key = match.group(1)
        return str(answers.get(key, ""))

    import re
    result = re.sub(r"\{([^}]+)\}", replacer, build_str)
    # Strip leading and trailing double quotes if present
    if result.startswith('"') and result.endswith('"'):
        result = result[1:-1]
    return result.strip()

if __name__ == "__main__":
    main()