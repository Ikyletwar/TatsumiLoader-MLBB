import os

# Script untuk mengonversi PNG menjadi array C++ (seperti xxd -i)
# Dibuat otomatis untuk semua file di folder ini

def convert_to_header():
    # Folder saat ini
    current_folder = os.path.dirname(os.path.realpath(__file__))
    output_header = os.path.join(current_folder, "SpellIcons.h")

    png_files = [f for f in os.listdir(current_folder) if f.lower().endswith(".png")]

    if not png_files:
        print("Tidak ada file PNG ditemukan di folder ini.")
        return

    with open(output_header, 'w') as out:
        out.write("#pragma once\n\n")
        out.write("// File ini di-generate otomatis oleh convert_to_cpp.py\n\n")

        for filename in png_files:
            # Buat nama variabel: misal "Flicker.png" -> "flicker_png"
            base_name = os.path.splitext(filename)[0].lower().replace(" ", "_")
            var_name = f"{base_name}_png"
            filepath = os.path.join(current_folder, filename)

            with open(filepath, 'rb') as f:
                data = f.read()

            print(f"Mengonversi: {filename} -> {var_name}")

            out.write(f"// Icon: {filename}\n")
            out.write(f"static unsigned char {var_name}[] = {{\n")

            # Format ke HEX (12 byte per baris)
            for i in range(0, len(data), 12):
                chunk = data[i:i+12]
                hex_line = ", ".join([f"0x{b:02x}" for b in chunk])
                comma = "," if i + 12 < len(data) else ""
                out.write(f"    {hex_line}{comma}\n")

            out.write("};\n")
            out.write(f"static unsigned int {var_name}_len = {len(data)};\n\n")

    print(f"\nSelesai! File '{output_header}' telah dibuat.")
    print("Silakan #include \"icon/Spell/SpellIcons.h\" di main.cpp Anda.")

if __name__ == "__main__":
    convert_to_header()
