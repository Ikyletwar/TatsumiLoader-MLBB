import os
import re

def convert_to_header():
    # Folder tempat script berada
    current_folder = os.path.dirname(os.path.realpath(__file__))
    output_header = os.path.join(current_folder, "SkillIcons.h")

    print("==================================================")
    print("   MLBB SKILL ICON CONVERTER (FIXED PYTHON)")
    print("==================================================")

    skill_mappings = []

    # Header awal file C++
    header_top = "#pragma once\n\n"
    header_top += "// File ini di-generate otomatis oleh script Python\n"
    header_top += "struct SkillIconData { int heroId; int skillIdx; const unsigned char* data; unsigned int len; };\n\n"

    body_content = ""

    # 1. Telusuri semua folder dan file
    for root, dirs, files in os.walk(current_folder):
        for filename in files:
            if filename.lower().endswith(".png"):
                filepath = os.path.join(root, filename)

                # --- LOGIKA AMBIL ID & INDEX ---
                folder_name = os.path.basename(root)
                id_match = re.search(r'(\d+)', folder_name)
                idx_match = re.search(r'(\d+)', filename)

                if id_match and idx_match:
                    hero_id = int(id_match.group(1))
                    base_idx = int(idx_match.group(1))

                    # Logika Morph: +10 jika ada kata 'morph'
                    skill_idx = base_idx
                    if "morph" in filename.lower():
                        skill_idx = base_idx + 10

                    # Buat nama variabel unik dari nama file
                    clean_name = re.sub(r'[^a-zA-Z0-9]', '_', filename.replace(".png", "").replace(".PNG", ""))
                    var_name = f"skill_{hero_id}_{clean_name}"

                    try:
                        with open(filepath, 'rb') as f:
                            data = f.read()

                        data_len = len(data)
                        print(f"[OK] Hero {hero_id} | Skill {skill_idx} ({filename})")

                        body_content += f"// Source: {folder_name}/{filename}\n"
                        body_content += f"static const unsigned char {var_name}[] = {{\n"

                        for i in range(0, data_len, 12):
                            chunk = data[i:i+12]
                            hex_line = ", ".join([f"0x{b:02x}" for b in chunk])
                            comma = "," if i + 12 < data_len else ""
                            body_content += f"    {hex_line}{comma}\n"

                        body_content += "};\n"
                        body_content += f"static const unsigned int {var_name}_len = {data_len};\n\n"

                        skill_mappings.append((hero_id, skill_idx, var_name))
                    except Exception as e:
                        print(f"[ERROR] Gagal membaca {filename}: {e}")

    if not skill_mappings:
        print("\n[ERROR] Tidak ada file PNG yang ditemukan!")
        return

    # 2. Registry Table
    footer = f"static const int TOTAL_SKILL_ICONS = {len(skill_mappings)};\n"
    footer += "static const SkillIconData skill_icon_registry[] = {\n"
    for m in skill_mappings:
        footer += f"    {{ {m[0]}, {m[1]}, {m[2]}, {m[2]}_len }},\n"
    footer += "};\n"

    with open(output_header, 'w') as out:
        out.write(header_top)
        out.write(body_content)
        out.write(footer)

    print("==================================================")
    print(f"SUKSES! File '{output_header}' telah dibuat.")
    print(f"Total Ikon: {len(skill_mappings)}")
    print("==================================================")

if __name__ == "__main__":
    convert_to_header()
