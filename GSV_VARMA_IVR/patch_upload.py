with open('GSV_VARMA_IVR.ino', 'rb') as f:
    content = f.read()

# Find the exact bytes for the section we want to replace
start_marker = b'File uploadFile;\r\nvoid handleFileUpload()'
end_marker = b'}\r\n\r\nvoid handleCaptivePortalRedirect()'

s = content.find(start_marker)
e = content.find(end_marker)

if s < 0 or e < 0:
    print(f'Markers not found: start={s}, end={e}')
    exit(1)

old_section = content[s:e+1]  # include the closing '}'
print(f'Found section at bytes {s}-{e+1} ({len(old_section)} bytes)')

new_section = b'''File uploadFile;
static bool uploadSdMutexHeld = false; // Track mutex held across START to END callbacks

void handleFileUpload() {
  if (server.uri() != "/upload_audio" && server.uri() != "/upload_file") return;
  HTTPUpload& upload = server.upload();

  if (upload.status == UPLOAD_FILE_START) {
    String filename = upload.filename;
    if (!filename.startsWith("/")) filename = "/" + filename;
    webLog("[Upload] Start: " + filename);
    createParentDirs(filename);
    webUploadActive = true; // Tell sdTake() in PeriodicSync to wait
    // Acquire SD mutex and HOLD IT for the entire upload.
    // This prevents ALL concurrent SD ops from corrupting SdFat shared sector cache.
    uploadSdMutexHeld = sdTake(30000);
    if (uploadSdMutexHeld) {
      uploadFile = myFS->open(filename, "w");
      // DO NOT call sdGive() here - hold mutex until END or ABORTED
    }

  } else if (upload.status == UPLOAD_FILE_WRITE) {
    lastSdActivityMillis = millis();
    if (uploadFile && uploadSdMutexHeld) {
      // Mutex already held since START - write directly without sdTake/sdGive
      uploadFile.write(upload.buf, upload.currentSize);
    }

  } else if (upload.status == UPLOAD_FILE_END) {
    if (uploadFile) {
      uploadFile.flush();
      uploadFile.close();
    }
    if (uploadSdMutexHeld) {
      sdGive();
      uploadSdMutexHeld = false;
    }
    webUploadActive = false;
    String filename = upload.filename;
    if (!filename.startsWith("/")) filename = "/" + filename;
    webLog("[Upload] End: " + filename + " Size: " + String(upload.totalSize));
    updateSDCache();

  } else if (upload.status == UPLOAD_FILE_ABORTED) {
    if (uploadFile) {
      uploadFile.close();
    }
    if (uploadSdMutexHeld) {
      sdGive();
      uploadSdMutexHeld = false;
    }
    webUploadActive = false;
    webLog("[Upload] ABORTED");
  }
}'''.replace(b'\n', b'\r\n')

content = content[:s] + new_section + content[e+1:]
with open('GSV_VARMA_IVR.ino', 'wb') as f:
    f.write(content)
print('OK: patch applied successfully')
