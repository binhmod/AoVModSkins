# pip3 install aov-zstd
import os
import zipfile
from AoV_Zstd import XL

def is_zip(file_path):
    try:
        with open(file_path, "rb") as f:
            return f.read(2) == b'PK'
    except:
        return False

def ensure_dir(path):
    os.makedirs(path, exist_ok=True)

def process_file(mode, in_path, out_path):
    try:
        print(f"[+] Processing: {in_path}")

        # ZIP → giải nén
        if is_zip(in_path):
            extract_dir = out_path + "_zip"
            ensure_dir(extract_dir)

            with zipfile.ZipFile(in_path, 'r') as zip_ref:
                zip_ref.extractall(extract_dir)

            print(f"[+] Extracted ZIP -> {extract_dir}")

            process_path(mode, extract_dir, extract_dir)
            return

        # File thường
        ensure_dir(os.path.dirname(out_path))
        XL(mode, in_path)

    except Exception as e:
        print(f"[!] Error: {in_path} -> {e}")

def process_path(mode, in_root, out_root):
    if os.path.isfile(in_root):
        process_file(mode, in_root, out_root)
        return

    for root, dirs, files in os.walk(in_root):
        print(f"[DEBUG] {root} ({len(files)} files)")

        rel = os.path.relpath(root, in_root)
        out_dir = os.path.join(out_root, rel)

        for f in files:
            in_file = os.path.join(root, f)
            out_file = os.path.join(out_dir, f)

            process_file(mode, in_file, out_file)

# ===== MAIN =====
mode = input("Mode (d = decompress, c = compress): ").strip()
input_path = input("Input path: ").strip()
output_path = input("Output path: ").strip()

if mode not in ["d", "c"]:
    print("[!] Mode phải là d hoặc c")
    exit()

if not os.path.exists(input_path):
    print("[!] Input không tồn tại")
    exit()

ensure_dir(output_path)

process_path(mode, input_path, output_path)

print("[✓] Done")
