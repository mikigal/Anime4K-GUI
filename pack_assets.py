import os
import sys
import struct

def pack_assets(source_dir, output_file):
    files = []
    for root, _, filenames in os.walk(source_dir):
        for fname in filenames:
            full_path = os.path.join(root, fname)
            rel_path = os.path.relpath(full_path, source_dir).replace("\\", "/")
            with open(full_path, "rb") as f:
                data = f.read()
            files.append((rel_path, data))

    with open(output_file, "wb") as f:
        f.write(struct.pack("<I", len(files)))
        for rel_path, data in files:
            encoded_path = rel_path.encode('utf-8')
            f.write(struct.pack("<H", len(encoded_path)))
            f.write(encoded_path)
            f.write(struct.pack("<I", len(data)))
            f.write(data)

    print(f"Packed {len(files)} files into {output_file}")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: pack_assets.py <assets_dir> <output_file>")
        sys.exit(1)
    pack_assets(sys.argv[1], sys.argv[2])