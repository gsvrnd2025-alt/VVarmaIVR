import os
import gzip
import urllib.request

def download_file(url, dest_path):
    print(f"Downloading: {url} -> {dest_path}")
    os.makedirs(os.path.dirname(dest_path), exist_ok=True)
    try:
        urllib.request.urlretrieve(url, dest_path)
        print("  [OK]")
        return True
    except Exception as e:
        print(f"  [ERROR] Failed to download {url}: {e}")
        return False

def gzip_file(src_path, dest_path):
    print(f"Compressing: {src_path} -> {dest_path}")
    try:
        with open(src_path, 'rb') as f_in:
            data = f_in.read()
        with gzip.open(dest_path, 'wb') as f_out:
            f_out.write(data)
        print(f"  [OK] Size reduced from {len(data):,} to {os.path.getsize(dest_path):,} bytes")
        return True
    except Exception as e:
        print(f"  [ERROR] Failed to gzip {src_path}: {e}")
        return False

def main():
    script_dir = os.path.dirname(os.path.abspath(__file__))
    sd_card_dir = os.path.join(script_dir, "voice_sd_card")
    libs_dir = os.path.join(sd_card_dir, "libs")
    
    # 1. Download CDN Libraries
    libs = {
        "https://cdn.jsdelivr.net/npm/bootstrap@5.3.2/dist/css/bootstrap.min.css": os.path.join(libs_dir, "bootstrap.min.css"),
        "https://cdn.jsdelivr.net/npm/bootstrap@5.3.2/dist/js/bootstrap.bundle.min.js": os.path.join(libs_dir, "bootstrap.bundle.min.js"),
        "https://cdn.jsdelivr.net/npm/chart.js@4.4.0/dist/chart.umd.min.js": os.path.join(libs_dir, "chart.umd.min.js"),
        "https://cdnjs.cloudflare.com/ajax/libs/jspdf/2.5.1/jspdf.umd.min.js": os.path.join(libs_dir, "jspdf.umd.min.js"),
        "https://cdnjs.cloudflare.com/ajax/libs/jspdf-autotable/3.8.2/jspdf.plugin.autotable.min.js": os.path.join(libs_dir, "jspdf.plugin.autotable.min.js"),
        "https://cdn.socket.io/4.7.4/socket.io.min.js": os.path.join(libs_dir, "socket.io.min.js"),
    }
    
    print("=" * 60)
    print("  Downloading local web libraries...")
    print("=" * 60)
    for url, path in libs.items():
        download_file(url, path)
        
    # 2. Gzip all files in voice_sd_card
    print("\n" + "=" * 60)
    print("  Compiling / Gzipping all assets...")
    print("=" * 60)
    
    extensions_to_gzip = {".html", ".js", ".css", ".json"}
    
    for root, dirs, files in os.walk(sd_card_dir):
        # Skip gzipped files in walk to avoid double compression
        for file in files:
            if file.endswith(".gz"):
                continue
            
            ext = os.path.splitext(file)[1].lower()
            if ext in extensions_to_gzip:
                src_path = os.path.join(root, file)
                dest_path = src_path + ".gz"
                gzip_file(src_path, dest_path)
                
    print("\n  Asset compilation complete!")
    print("=" * 60)

if __name__ == "__main__":
    main()
