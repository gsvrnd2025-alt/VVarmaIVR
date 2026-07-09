import requests, os, time

BASE = 'http://192.168.0.162'

files_to_fix = [
    ('voice_sd_card/libs/chart.umd.min.js.gz',    '/libs/chart.umd.min.js.gz',    69789),
    ('voice_sd_card/libs/jspdf.umd.min.js.gz',    '/libs/jspdf.umd.min.js.gz',    115624),
]

for local_path, sd_path, expected_size in files_to_fix:
    actual_size = os.path.getsize(local_path)
    print(f'\nUploading {os.path.basename(local_path)}  ({actual_size:,} bytes) -> {sd_path}')

    for attempt in range(1, 4):
        try:
            with open(local_path, 'rb') as f:
                resp = requests.post(
                    f'{BASE}/upload_file',
                    files={'file': (sd_path, f, 'application/octet-stream')},
                    timeout=120,
                )
            if resp.status_code == 200:
                print(f'  [OK] {resp.text}')
                break
            else:
                print(f'  [WARN] attempt {attempt}: HTTP {resp.status_code}')
        except Exception as e:
            print(f'  [WARN] attempt {attempt}: {e}')
        time.sleep(3)

# Verify sizes match expected
print('\n--- Verifying SD card sizes ---')
r = requests.get(f'{BASE}/sd_files', timeout=15)
sd_map = {item['name']: item['size'] for item in r.json()}
all_ok = True
for local_path, sd_path, expected_size in files_to_fix:
    sd_size = sd_map.get(sd_path, 'MISSING')
    ok = sd_size == expected_size
    status = 'OK  ' if ok else 'FAIL'
    print(f'  [{status}]  SD={sd_size:>8,}  expected={expected_size:>8,}  {sd_path}')
    if not ok:
        all_ok = False

if all_ok:
    print('\n  All files verified! Dashboard should now load correctly.')
else:
    print('\n  Some files still mismatch. Check logs.')
