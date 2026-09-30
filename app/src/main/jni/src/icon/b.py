import base64
import re
import os

# 1. Konfigurasi
NAMA_FILE_SOURCE = "HeroIcons.cpp"  # Ganti dengan nama file .cpp atau .h Anda
FOLDER_OUTPUT = "hasil_ikon"         # Nama folder hasil konversi

def bulk_decode():
    # Buat folder jika belum ada
    if not os.path.exists(FOLDER_OUTPUT):
        os.makedirs(FOLDER_OUTPUT)

    print(f"Membaca file {NAMA_FILE_SOURCE}...")

    with open(NAMA_FILE_SOURCE, 'r', encoding='utf-8', errors='ignore') as f:
        content = f.read()

    # 2. Regex untuk mencari pola: ID (angka) dan String Base64 (didalam kutip)
    # Pola ini cocok untuk: { 1, "base64..." } atau [1] = "base64..."
    pattern = r'(\d+)\s*,\s*"([A-Za-z0-9+/=]{50,})"' 
    
    matches = re.findall(pattern, content)
    
    if not matches:
        print("Tidak ditemukan pola Base64. Mencoba pola alternatif...")
        # Coba pola alternatif jika formatnya berbeda: "1": "base64..."
        pattern = r'"(\d+)"\s*:\s*"([A-Za-z0-9+/=]{50,})"'
        matches = re.findall(pattern, content)

    print(f"Ditemukan {len(matches)} ikon. Mulai mengonversi...")

    count = 0
    for item_id, b64_str in matches:
        try:
            # Hapus whitespace jika ada di dalam string
            clean_b64 = b64_str.strip()
            
            # Decode Base64
            img_data = base64.b64decode(clean_b64)
            
            # Simpan file dengan nama hero_ID.png
            file_name = f"hero_{item_id}.png"
            file_path = os.path.join(FOLDER_OUTPUT, file_name)
            
            with open(file_path, 'wb') as f_out:
                f_out.write(img_data)
            
            count += 1
            if count % 50 == 0:
                print(f"Diproses: {count} file...")
        
        except Exception as e:
            print(f"Gagal mengonversi ID {item_id}: {e}")

    print("------------------------------------------")
    print(f"SELESAI! {count} file disimpan di folder: {FOLDER_OUTPUT}")

if __name__ == "__main__":
    bulk_decode()