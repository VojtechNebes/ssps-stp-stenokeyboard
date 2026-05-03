import json
import os

from _mapping import LHS_mapping, Middle_mapping, RHS_mapping, plover_index_mapping


os.chdir(os.path.dirname(__file__))

PLOVER_DICT_PATH = "english_stanmain_merge/merged_stanmain.json"
# PLOVER_DICT_PATH = "test_steno_dict_2.json"
OUTPUT_BIN_PATH = "output/dict_{}.bin"
OUTPUT_O_PATH = "../src/dicts/dict_{}.o"

MAX_WORD_LEN = 18
CHORD_BITS = 23
CHORD_BYTES = (CHORD_BITS + 7) // 8
MAX_SEQUENCE_LEN = 5


def load_plover_dict(path: str) -> dict[str, str]:
    """
    Loads the Plover dict from file and returns in this format:
    ```
    {
        "AB/C/D": "word",
        ...
    }
    ```
    """

    with open(path, "r") as f:
        original_plover_dict: dict[str, str] = json.load(f)
    
    plover_dict = {}

    for i, (plover_sequence, word) in enumerate(original_plover_dict.items()):
        sequence_len = len(plover_sequence.split("/"))
        word_len = len(word)

        if sequence_len > MAX_SEQUENCE_LEN: print(f"Chord sequence in record {i} (word \"{word}\") is too long ({sequence_len} chords)")
        elif word_len > MAX_WORD_LEN: print(f"Word in record {i} (word \"{word}\") is too long ({word_len} characters)")
        else:
            plover_dict[plover_sequence] = word

    print(f"Removed {len(original_plover_dict) - len(plover_dict)} records from original dict.")

    return plover_dict

def _plover_chord_to_bools_chord(plover_chord: str) -> str:
    """
    Translates a single Plover chord (`AB`) to a bool mapping (`0011`).
    """

    bools = [False] * CHORD_BITS
    is_rhs = False

    for c in plover_chord:
        if c == "-":
            is_rhs = True
            continue

        if c == "*":
            index = 21
            is_rhs = True
        elif c == "#":
            index = 22
        
        elif c in Middle_mapping:
            index = Middle_mapping.get(c)
            is_rhs = True
        elif is_rhs and c in RHS_mapping:
            index = RHS_mapping.get(c)
        elif not is_rhs and c in LHS_mapping:
            index = LHS_mapping.get(c)
        
        else:
            raise ValueError(f"Unknown chord character: {c}")

        if index is not None:
            bools[index] = True
        else:
            raise ValueError(f"Could not translate character: {c}")

    return "".join("1" if bit else "0" for bit in bools)

def plover_dict_to_bools_dict(plover_dict: dict[str, str]) -> dict[str, str]:
    """
    Converts a Plover dict to a dict with this format:
    ```
    {
        "0011/0100/1000": "word",
        ...
    }
    ```
    """

    bools_dict = {}

    for plover_sequence, word in plover_dict.items():
        bools_sequence = []

        try:
            for plover_chord in plover_sequence.split("/"):
                bools_chord = _plover_chord_to_bools_chord(plover_chord)
                bools_sequence.append(bools_chord)
            
            bools_sequence_str = "/".join(bools_sequence)
        except ValueError as e:
            print(f"Cannot translate plover sequence {plover_sequence}: {e}")
        else:
            bools_dict[bools_sequence_str] = word
    
    return bools_dict

def _bools_to_bytes(bits: str) -> bytes:
    """Converts a single bools chord (`0011`) to bytes (`0x3`)."""

    if len(bits) != CHORD_BITS: raise ValueError(f"list of bools is wrong length: {bits}")

    # pad to full byte
    bits = bits.ljust((len(bits) + 7) // 8 * 8, '0')
    byte_array = bytearray()
    for i in range(0, len(bits), 8):
        byte_array.append(int(bits[i:i+8], 2))
    return bytes(byte_array)

def records_to_bin_file(bools_dict: dict[str, str], seq_len: int, bin_path: str):
    """Takes a bools dict, filters by seq_len and writes to a .bin file."""

    bin_records: list[tuple[bytes, bytes]] = []
    n_records = 0

    for bools_sequence, word in bools_dict.items():
        bools_sequence_split = bools_sequence.split("/")
        if len(bools_sequence_split) != seq_len: continue

        n_records += 1
        bytes_sequence: list[bytes] = []

        for bools_chord in bools_sequence_split:
            chord_bytes = _bools_to_bytes(bools_chord)
            print(f"{bools_chord} -> {chord_bytes}")
            bytes_sequence.append(chord_bytes)
        
        word_bytes = word.encode("utf-8").ljust(MAX_WORD_LEN, b'\x00')
        bin_records.append((b"".join(bytes_sequence), word_bytes))

    # Sort records and write to file

    bin_records.sort(key=lambda rec: rec[0], reverse=True)

    os.makedirs(os.path.dirname(bin_path), exist_ok=True)

    with open(bin_path, "wb") as bin_file:
        for chord_bytes, word_bytes in bin_records:
            record = chord_bytes + word_bytes
            bin_file.write(record)
            bin_file.flush()
    
    # Return information about this dict

    seq_bytes = CHORD_BYTES * seq_len
    record_len = seq_bytes + MAX_WORD_LEN

    return record_len, n_records

def bin_to_o_file(bin_path, o_path):
    """Convert a .bin file to a .o file for RP2040 processor."""

    os.system(
        f"arm-none-eabi-objcopy -I binary -O elf32-littlearm -B arm "
        "--rename-section .data=.rodata,alloc,load,readonly,data,contents "
        f"{bin_path} {o_path}"
    )

def prepare_output_dirs():
    os.makedirs(os.path.dirname(OUTPUT_BIN_PATH), exist_ok=True)

    os.makedirs(os.path.dirname(OUTPUT_O_PATH), exist_ok=True)
    os.system(f"rm -f {os.path.dirname(OUTPUT_O_PATH)}/*.o")


if __name__ == "__main__":
    plover_dict = load_plover_dict(PLOVER_DICT_PATH)
    bools_dict = plover_dict_to_bools_dict(plover_dict)

    prepare_output_dirs()
    
    for seq_len in range(1, MAX_SEQUENCE_LEN + 1):
        print(f"\n\033[92mDICT {seq_len}\033[0m\n")

        output_bin_path = OUTPUT_BIN_PATH.format(seq_len)
        output_o_path = OUTPUT_O_PATH.format(seq_len)

        record_len, n_records = records_to_bin_file(bools_dict, seq_len, output_bin_path)
        bin_to_o_file(output_bin_path, output_o_path)
        
        print(f" {record_len} B / record")
        print(f" {n_records} records")
        print(f" {os.path.getsize(output_bin_path)/1000:.0f} kB of data")

        print()
        os.system(f"xxd -b -c {record_len} {output_bin_path}")
    
    print(
        "\n\033[92mCompleted.\033[0m\n"
        f"Generated {len(bools_dict)} records.\n"
        f"MAX_SEQUENCE_LEN: {MAX_SEQUENCE_LEN}\n"
        f"MAX_WORD_LEN: {MAX_WORD_LEN}\n"
        "\nDict sizes:"
    )
    os.system(f"du -h {os.path.dirname(OUTPUT_BIN_PATH)}/*")
    print("\nTotal:")
    os.system(f"du -h {os.path.dirname(OUTPUT_BIN_PATH)}")