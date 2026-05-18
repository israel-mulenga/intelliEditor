# IntelliEditor Help Server Setup

This directory contains the help documentation for IntelliEditor.

## Quick Start

### Option 1: Using Python (Recommended)

```bash
cd docs
python3 server.py
```

Then visit: **http://localhost:8000**

### Option 2: Using Python's built-in server

```bash
cd docs
python3 -m http.server 8000
```

### Option 3: Using Node.js http-server

```bash
cd docs
npm install -g http-server
http-server
```

### Option 4: Using PHP

```bash
cd docs
php -S localhost:8000
```

### Option 5: Using Ruby

```bash
cd docs
ruby -run -ehttpd . -p8000
```

## Accessing the Help

Once the server is running:

1. Open your browser to: **http://localhost:8000**
2. Or from IntelliEditor: **Help > Help Contents** (or press F1)

The help page includes:
- ✨ Feature overview
- ⌨️ Keyboard shortcuts reference
- 📋 Complete menu guide
- ❓ FAQ section
- 💡 Tips and tricks

## File Structure

```
docs/
├── index.html          # Main help page
├── style.css          # Styling
├── server.py          # Python server script
└── README.md          # This file
```

## Features

- 📱 Responsive design (works on desktop and mobile)
- 🎨 Modern styling with gradient themes
- ⚡ No external dependencies (pure HTML/CSS)
- 🔍 Easy navigation with smooth scrolling
- 📖 Comprehensive documentation

## Customization

To customize the help page:

1. Edit `index.html` to change content
2. Edit `style.css` to change styling
3. Refresh your browser (Ctrl+R)

## Troubleshooting

- **Port 8000 already in use?** Use a different port: `python3 server.py 8080` or change `PORT` variable
- **Browser shows "Cannot GET /"?** Make sure `index.html` exists in the docs directory
- **CSS not loading?** Clear browser cache (Ctrl+Shift+Delete) or use Ctrl+Shift+R

## Integration with IntelliEditor

To link the help from IntelliEditor application:

1. Make sure the server is running
2. Update `callbacks.c` to open: `http://localhost:8000`
3. Recompile the application

```c
// In on_help_contents_clicked():
system("xdg-open http://localhost:8000");  // Linux
system("open http://localhost:8000");       // macOS
system("start http://localhost:8000");      // Windows
```

---

Created for IntelliEditor v1.0 © 2026
