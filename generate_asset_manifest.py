import os
import sys

def main(asset_dir, output_manifest):
    with open(output_manifest, 'w') as out:
        for root, _, files in os.walk(asset_dir):
            for f in files:
                full_path = os.path.join(root, f)
                out.write(full_path.replace("\\", "/") + '\n')

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: generate_asset_manifest.py <asset_dir> <output_file>")
        sys.exit(1)

    main(sys.argv[1], sys.argv[2])