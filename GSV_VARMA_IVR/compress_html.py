import gzip
import os

def main():
    html_path = 'voice_sd_card/index.html'
    h_path = 'index_html_gzip.h'

    if not os.path.exists(html_path):
        # try relative parent paths
        html_path = 'GSV_VARMA_IVR/voice_sd_card/index.html'
        h_path = 'GSV_VARMA_IVR/index_html_gzip.h'

    if not os.path.exists(html_path):
        print(f"Error: Could not find {html_path}")
        return

    with open(html_path, 'rb') as f_in:
        data = f_in.read()

    compressed = gzip.compress(data)

    hex_bytes = []
    for b in compressed:
        hex_bytes.append(f'0x{b:02x}')

    # Format as 16 bytes per line
    lines = []
    for i in range(0, len(hex_bytes), 16):
        chunk = hex_bytes[i:i+16]
        lines.append('  ' + ', '.join(chunk))

    content = f"""#ifndef INDEX_HTML_GZIP_H
#define INDEX_HTML_GZIP_H

#include <pgmspace.h>

const uint8_t index_html_gzip[] PROGMEM = {{
{",\n".join(lines)}
}};
const uint32_t index_html_gzip_len = {len(compressed)};

#endif
"""

    # Determine absolute directory of this script to output in same folder
    script_dir = os.path.dirname(os.path.abspath(__file__))
    output_path = os.path.join(script_dir, 'index_html_gzip.h')
    input_path = os.path.join(script_dir, 'voice_sd_card', 'index.html')

    if os.path.exists(input_path):
        with open(input_path, 'rb') as f_in:
            data = f_in.read()
        compressed = gzip.compress(data)
        hex_bytes = [f'0x{b:02x}' for b in compressed]
        lines = []
        for i in range(0, len(hex_bytes), 16):
            chunk = hex_bytes[i:i+16]
            lines.append('  ' + ', '.join(chunk))
        content = f"""#ifndef INDEX_HTML_GZIP_H
#define INDEX_HTML_GZIP_H

#include <pgmspace.h>

const uint8_t index_html_gzip[] PROGMEM = {{
{",\n".join(lines)}
}};
const uint32_t index_html_gzip_len = {len(compressed)};

#endif
"""

    with open(output_path, 'w') as f_out:
        f_out.write(content)

    print(f"Successfully compressed {len(data)} bytes of index.html into {len(compressed)} bytes of gzip data.")
    print(f"Generated header file: {output_path}")

if __name__ == '__main__':
    main()
