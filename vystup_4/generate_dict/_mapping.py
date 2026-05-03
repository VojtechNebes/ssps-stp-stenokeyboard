LHS_mapping: dict[str, int] = {
    "S": 0,
    "T": 1,
    "P": 2,
    "H": 3,
    "K": 4,
    "W": 5,
    "R": 6
}

Middle_mapping: dict[str, int] = {
    "A": 7,
    "O": 8,
    "E": 9,
    "U": 10
}

RHS_mapping: dict[str, int] = {
    "F": 11,
    "P": 12,
    "L": 13,
    "T": 14,
    "D": 15,
    "R": 16,
    "B": 17,
    "G": 18,
    "S": 19,
    "Z": 20
}

plover_index_mapping: dict[int, str] = {
    0: "S",
    1: "T",
    2: "K",
    3: "P",
    4: "W",
    5: "H",
    6: "R",
    7: "A",
    8: "O",
    9: "*",
    10: "E",
    11: "U",
    12: "-F",
    13: "-R",
    14: "-P",
    15: "-B",
    16: "-L",
    17: "-G",
    18: "-T",
    19: "-S",
    20: "-D",
    21: "-Z"
}