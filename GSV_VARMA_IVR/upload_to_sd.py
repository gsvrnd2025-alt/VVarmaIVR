"""
upload_to_sd.py  —  Push updated web files to the ESP32 SD card over WiFi.

Uses the /chunk_upload endpoint built into GSV_VARMA_IVR.ino.
POST /chunk_upload?path=<dest>&offset=<byte_offset>&total=<file_size>
Body = raw text chunk (4 KB per request to stay under ESP32 TCP limits).
"""

import os
import sys
import requests

ESP32_IP  = "192.168.0.162"
BASE_URL  = f"http://{ESP32_IP}"
CHUNK_SZ  = 3072          # 3 KB per POST — safe for ESP32 WebServer heap
TIMEOUT   = 15            # seconds per request

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))

# Files will be scanned dynamically in main()


def upload_file(local_path: str, sd_path: str) -> bool:
    if not os.path.exists(local_path):
        print(f"  [ERROR] Source file not found: {local_path}")
        return False

    total = os.path.getsize(local_path)
    print(f"\n  Uploading  {os.path.basename(local_path)}")
    print(f"  Size       {total:,} bytes  ->  SD: {sd_path}")

    max_retries = 5
    for attempt in range(1, max_retries + 1):
        try:
            # Open file fresh for each retry
            files = {
                'file': (sd_path, open(local_path, 'rb'), 'application/octet-stream')
            }
            resp = requests.post(
                f"{BASE_URL}/upload_file",
                files=files,
                timeout=TIMEOUT,
            )
            
            if resp.status_code == 200:
                try:
                    result = resp.json()
                    if result.get("success", True):
                        print(f"  [OK] Done")
                        return True
                    else:
                        print(f"    [WARN] Server returned failure on attempt {attempt}: {result.get('error')}")
                except Exception:
                    print(f"  [OK] Done")
                    return True
            else:
                print(f"    [WARN] Server returned HTTP {resp.status_code} on attempt {attempt}")
                
        except (requests.exceptions.ConnectTimeout, requests.exceptions.ReadTimeout) as e:
            print(f"    [WARN] Timeout on attempt {attempt}: {e}")
        except requests.exceptions.ConnectionError as e:
            print(f"    [WARN] Connection failed on attempt {attempt}: {e}")
        except Exception as e:
            print(f"    [WARN] Unexpected error on attempt {attempt}: {e}")
            
        if attempt < max_retries:
            time.sleep(2.0)
            
    print(f"  [ERROR] Failed to upload {os.path.basename(local_path)} after {max_retries} attempts.")
    return False


def main():
    print("=" * 55)
    print("  V-VARMA IVR  -  SD Card WiFi Updater")
    print(f"  Target ESP32: {BASE_URL}")
    print("=" * 55)

    # Quick connectivity check using a lightweight API endpoint
    try:
        r = requests.get(f"{BASE_URL}/api/get-config", timeout=20)
        if r.status_code != 200:
            print(f"\n[ERROR] ESP32 returned HTTP {r.status_code}. Device may be in a call.")
            sys.exit(1)
        print(f"\n  ESP32 reachable - config OK")
    except Exception as exc:
        print(f"\n[ERROR] Cannot reach ESP32 at {BASE_URL}: {exc}")
        sys.exit(1)

    # Scan voice_sd_card directory recursively
    sd_card_dir = os.path.join(SCRIPT_DIR, "voice_sd_card")
    files_to_upload = []
    folders_to_create = set()

    for root, dirs, files in os.walk(sd_card_dir):
        for file in files:
            ext = os.path.splitext(file)[1].lower()
            # We only upload gzipped web assets, skipping heavy audio files already on the SD card
            if ext == ".gz":
                local_path = os.path.join(root, file)
                rel_path = os.path.relpath(local_path, sd_card_dir)
                sd_path = "/" + rel_path.replace("\\", "/")
                
                files_to_upload.append((local_path, sd_path))
                
                parent_dir = os.path.dirname(sd_path)
                if parent_dir and parent_dir != "/":
                    folders_to_create.add(parent_dir)

    # Ensure all required directories exist on SD card
    print(f"\n  Creating required directories on SD card...")
    for folder in sorted(folders_to_create):
        try:
            print(f"    Ensuring folder: {folder}")
            requests.post(f"{BASE_URL}/create_folder", params={"path": folder}, timeout=5)
        except Exception as e:
            print(f"      [WARN] Folder creation request failed: {e}")

    # Fetch existing files on the SD card to skip matches
    sd_sizes = {}
    try:
        r = requests.get(f"{BASE_URL}/sd_files", timeout=10)
        if r.status_code == 200:
            sd_files = r.json()
            sd_sizes = {item["name"]: item["size"] for item in sd_files}
            print(f"  Successfully fetched SD file list (found {len(sd_sizes)} files)")
    except Exception as e:
        print(f"  [WARN] Failed to fetch SD file list: {e}. Will upload all files.")

    results = []
    for local, dest in files_to_upload:
        local_size = os.path.getsize(local)
        if dest in sd_sizes and sd_sizes[dest] == local_size:
            print(f"  [SKIP] {dest} (already exists with same size: {local_size:,} bytes)")
            results.append((dest, True))
        else:
            ok = upload_file(local, dest)
            results.append((dest, ok))

    print("\n" + "=" * 55)
    print("  UPLOAD SUMMARY")
    print("=" * 55)
    all_ok = True
    for dest, ok in results:
        status = "[OK]    " if ok else "[FAIL]  "
        print(f"  {status}  {dest}")
        if not ok:
            all_ok = False

    if all_ok:
        print("\n  All files deployed successfully!")
        print("  -> Open your browser and press Ctrl+Shift+R (hard reload)")
        print(f"  -> http://{ESP32_IP}/dashboard")
    else:
        print("\n  Some files failed. Check connection and retry.")
        sys.exit(1)


if __name__ == "__main__":
    main()
