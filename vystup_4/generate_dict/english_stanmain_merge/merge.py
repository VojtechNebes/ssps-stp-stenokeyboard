import os
import json


OUTPUT = "merged_stanmain.json"
STANMAIN = "stanmain.json"
ENGLISH_LISTS = [
    "english_1k.json",
    "english_5k.json",
    "english_10k.json"
]


os.chdir(os.path.dirname(__file__))

english_words = set()

for english_list in ENGLISH_LISTS:
    with open(english_list, "r") as f:
        english_words.update(set(json.load(f)["words"]))

with open(STANMAIN, "r") as f:
    stanmain_dict: dict[str, str] = json.load(f)

filtered_stanmain = {k: v for k, v in stanmain_dict.items() if v in english_words}

with open(OUTPUT, "w") as f:
    json.dump(filtered_stanmain, f, indent=4, ensure_ascii=False)