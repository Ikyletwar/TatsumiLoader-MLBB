import os
import re

def convert_header_to_png(file_path):
    if not os.path.exists(file_path):
        print(f"Error: File {file_path} tidak ditemukan.")
        return

    output_dir = "Restored_Spells"
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)

    print(f"Membaca {file_path}...")
    with open(file_path, 'r') as f:
        content = f.read()

    # Regex untuk mencari: unsigned char NAMA_VARIABEL[] = { DATA_HEX }
    pattern = r'unsigned char\s+(\w+)\[\]\s*=\s*\{([\s\S]*?)\};'
    matches = re.findall(pattern, content)

    if not matches:
        print("Tidak ditemukan data array 'unsigned char' dalam file tersebut.")
        return

    for var_name, hex_data in matches:
        # --- PERBAIKAN NAMA DISINI ---
        # Jika nama variabel berakhiran _png atau _PNG, kita hapus agar tidak double
        file_name_clean = var_name
        if file_name_clean.lower().endswith("_png"):
            file_name_clean = file_name_clean[:-4] # Hapus 4 karakter terakhir (_png)
        
        # Bersihkan data hex
        clean_hex = hex_data.replace('0x', '').replace(',', '').replace(' ', '').replace('\n', '').replace('\t', '').replace('\r', '')
        
        try:
            byte_data = bytes.fromhex(clean_hex)
            
            # Simpan dengan nama yang sudah dibersihkan
            output_file = os.path.join(output_dir, f"{file_name_clean}.png")
            with open(output_file, 'wb') as png_file:
                png_file.write(byte_data)
            
            print(f"Berhasil: {file_name_clean}.png")
        except Exception as e:
            print(f"Gagal mengonversi {var_name}: {e}")

if __name__ == "__main__":
    target_file = "RankIcons.h"
    convert_header_to_png(target_file)