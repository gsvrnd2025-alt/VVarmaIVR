import os

def merge_assets():
    # Relative path calculations
    script_dir = os.path.dirname(os.path.abspath(__file__))
    web_dir = os.path.join(script_dir, "web")
    
    # Read files
    html_path = os.path.join(web_dir, "index.html")
    css_path = os.path.join(web_dir, "style.css")
    js_path = os.path.join(web_dir, "script.js")
    
    with open(html_path, "r", encoding="utf-8") as f:
        html = f.read()
        
    with open(css_path, "r", encoding="utf-8") as f:
        css = f.read()
        
    with open(js_path, "r", encoding="utf-8") as f:
        js = f.read()
        
    # Replace style.css link with inline style block
    css_placeholder = '<link rel="stylesheet" href="style.css">'
    if css_placeholder in html:
        html = html.replace(css_placeholder, f"<style>\n{css}\n</style>")
    else:
        print("Warning: style.css placeholder not found in index.html")
        
    # Replace script.js link with inline script block
    js_placeholder = '<script src="script.js"></script>'
    if js_placeholder in html:
        html = html.replace(js_placeholder, f"<script>\n{js}\n</script>")
    else:
        print("Warning: script.js placeholder not found in index.html")
        
    # Format as C++ index_html.h
    header_content = f"""#ifndef INDEX_HTML_H
#define INDEX_HTML_H

#include <pgmspace.h>

const char index_html[] PROGMEM = R"rawliteral(
{html}
)rawliteral";

#endif
"""
    
    # Write to target paths
    path = os.path.join(script_dir, "GSV_VARMA_IVR", "index_html.h")
    with open(path, "w", encoding="utf-8") as f:
        f.write(header_content)
    print(f"Successfully wrote merged header to: {path}")

if __name__ == "__main__":
    merge_assets()
