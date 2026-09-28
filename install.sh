#!/bin/bash
set -e

echo "🚀 Installing CNav..."

# GitHub Release URL for the Linux binary
DOWNLOAD_URL="https://github.com/Raiza04/cnav/releases/latest/download/cnav-linux-amd64"
BIN_DIR="$HOME/.local/bin"

mkdir -p "$BIN_DIR"
echo "⬇️ Downloading pre-compiled CNav binary..."

# Download and save the binary
curl -sSL -f "$DOWNLOAD_URL" -o "$BIN_DIR/n"

# Make it executable
chmod +x "$BIN_DIR/n"
echo "✅ CNav has been installed to $BIN_DIR."

# Determine which shell config to update
SHELL_RC="$HOME/.bashrc"
if [[ "$SHELL" == *"zsh"* ]]; then
    SHELL_RC="$HOME/.zshrc"
fi

UPDATED=0

# 1. Check and inject PATH
if ! grep -q "$BIN_DIR" "$SHELL_RC"; then
    echo "" >> "$SHELL_RC"
    echo "# CNav PATH" >> "$SHELL_RC"
    echo "export PATH=\"$BIN_DIR:\$PATH\"" >> "$SHELL_RC"
    echo "✅ Path was added to your profile."
    UPDATED=1
fi

# 2. Check and inject Auto-Init Hook
if ! grep -q "n --init" "$SHELL_RC"; then
    echo "# CNav Auto-Init" >> "$SHELL_RC"
    echo 'eval "$(n --init)"' >> "$SHELL_RC"
    echo "✅ Hooks were added to your profile."
    UPDATED=1
fi

if [ $UPDATED -eq 0 ]; then
    echo "⚡ Hooks and Path were already present in your profile."
fi

echo ""
echo "🎉 Installation completed successfully!"
echo "👉 Restart your terminal or run: source $SHELL_RC"
