import os
import sys
import time
import serial

PORT = "COM3"
BAUD = 115200
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))

def safe_write_chunk(ser, chunk):
    # Send chunk in small sub-blocks with a tiny delay to prevent hardware buffer overflow
    sub_block_size = 32
    for i in range(0, len(chunk), sub_block_size):
        sub = chunk[i:i+sub_block_size]
        ser.write(sub)
        ser.flush()
        time.sleep(0.002) # 2ms delay

def upload_file_serial(ser, local_path, sd_path):
    if not os.path.exists(local_path):
        print(f"  [ERROR] File not found: {local_path}")
        return False
        
    size = os.path.getsize(local_path)
    print(f"\n  Uploading  {os.path.basename(local_path)}")
    print(f"  Size       {size:,} bytes -> SD: {sd_path}")
    
    # Clean output buffer
    ser.reset_input_buffer()
    ser.reset_output_buffer()
    
    # Send UPLOAD command
    cmd = f"UPLOAD:{sd_path}:{size}\n"
    ser.write(cmd.encode('utf-8'))
    ser.flush()
    
    # Wait for READY
    response = ""
    start_time = time.time()
    while time.time() - start_time < 5:
        if ser.in_waiting > 0:
            response += ser.read(ser.in_waiting).decode('utf-8', errors='ignore')
            if "READY" in response:
                break
        time.sleep(0.01)
        
    if "READY" not in response:
        print(f"  [ERROR] Server did not respond with READY. Response: {repr(response)}")
        return False
        
    # Start sending file in chunks of 256 bytes
    with open(local_path, 'rb') as f:
        bytes_sent = 0
        chunk_num = 0
        while bytes_sent < size:
            chunk = f.read(256)
            if not chunk:
                break
            # Write chunk safely with inter-block delay
            safe_write_chunk(ser, chunk)
            
            # Wait for ACK (0x06)
            ack_received = False
            start_ack = time.time()
            while time.time() - start_ack < 5:
                if ser.in_waiting > 0:
                    ack = ser.read(1)
                    if ack == b'\x06':
                        ack_received = True
                        break
                time.sleep(0.001)
                
            if not ack_received:
                print(f"\n  [ERROR] Timeout waiting for ACK on chunk {chunk_num}")
                return False
                
            bytes_sent += len(chunk)
            chunk_num += 1
            pct = int(bytes_sent * 100 / size)
            bar = "#" * (pct // 5) + "-" * (20 - pct // 5)
            print(f"\r  [{bar}] {pct:3d}%  ({bytes_sent:,}/{size:,} bytes)", end="", flush=True)
            
    # Wait for SUCCESS
    response = ""
    start_time = time.time()
    while time.time() - start_time < 5:
        if ser.in_waiting > 0:
            response += ser.read(ser.in_waiting).decode('utf-8', errors='ignore')
            if "SUCCESS" in response:
                break
        time.sleep(0.01)
        
    if "SUCCESS" in response:
        print("\n  [OK] Successfully uploaded")
        return True
    else:
        print(f"\n  [ERROR] Server did not print SUCCESS. Response: {repr(response)}")
        return False

def main():
    print("=" * 55)
    print("  V-VARMA IVR  -  SD Card Serial Updater")
    print(f"  Port: {PORT}  Baud: {BAUD}")
    print("=" * 55)
    
    # Scan voice_sd_card directory recursively
    sd_card_dir = os.path.join(SCRIPT_DIR, "voice_sd_card")
    files_to_upload = []
    
    for root, dirs, files in os.walk(sd_card_dir):
        for file in files:
            ext = os.path.splitext(file)[1].lower()
            # We only upload gzipped web assets, skipping heavy audio files already on the SD card
            if ext == ".gz":
                local_path = os.path.join(root, file)
                rel_path = os.path.relpath(local_path, sd_card_dir)
                sd_path = "/" + rel_path.replace("\\", "/")
                files_to_upload.append((local_path, sd_path))
                
    if not files_to_upload:
        print("No .gz files found to upload.")
        return
        
    try:
        # Open serial port
        ser = serial.Serial(PORT, BAUD, timeout=1)
        print(f"Opened serial port {PORT} successfully.")
        print("Waiting 12 seconds for ESP32 to complete boot sequence...")
        time.sleep(12)
        # Flush the buffer to discard any boot logs printed by ESP32 during startup
        ser.reset_input_buffer()
        print("ESP32 ready. Starting uploads...")
    except Exception as e:
        print(f"Error opening serial port: {e}")
        return
        
    try:
        results = []
        for local, dest in files_to_upload:
            ok = upload_file_serial(ser, local, dest)
            results.append((dest, ok))
            time.sleep(0.2) # Let ESP32 settle
            
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
            print("\n  All files deployed successfully over Serial!")
        else:
            print("\n  Some files failed. Retry.")
            
    finally:
        ser.close()
        print("Serial port closed.")

if __name__ == "__main__":
    main()
