#!/usr/bin/env python3
"""
Simple HTTP server for IntelliEditor Help Documentation
Serves the help files on http://localhost:8000
"""

import http.server
import socketserver
import os
import sys
from pathlib import Path

# Get the directory where the docs are located
DOCS_DIR = Path(__file__).parent.absolute()
PORT = 8000

class HelpRequestHandler(http.server.SimpleHTTPRequestHandler):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, directory=str(DOCS_DIR), **kwargs)
    
    def log_message(self, format, *args):
        """Override to show friendly messages"""
        sys.stderr.write("[%s] %s\n" % (self.log_date_time_string(), format % args))

def start_server():
    """Start the help server"""
    try:
        with socketserver.TCPServer(("", PORT), HelpRequestHandler) as httpd:
            print(f"🚀 IntelliEditor Help Server started")
            print(f"📖 Visit: http://localhost:{PORT}")
            print(f"📁 Serving: {DOCS_DIR}")
            print(f"Press Ctrl+C to stop\n")
            httpd.serve_forever()
    except KeyboardInterrupt:
        print("\n✓ Server stopped")
    except OSError as e:
        if e.errno == 48:  # Port already in use
            print(f"❌ Error: Port {PORT} is already in use")
            print(f"Another instance may be running. Try: lsof -i :{PORT}")
        else:
            print(f"❌ Error: {e}")
        sys.exit(1)

if __name__ == "__main__":
    start_server()
