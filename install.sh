#!/bin/bash
set -e

echo "🚀 Installing CNav..."

# GitHub release URL for the Linux version
DOWNLOAD_URL="https://github.com/Raiza04/cnav/releases/latest/download/cnav-linux-amd64"
BIN_DIR="$HOME/.local/bin"

mkdir -p "$BIN_DIR"
echo "⬇️ Downloading pre-compiled CNav binary..."

# Downloads the binary and saves it as 'n'
curl -sSL -f "$DOWNLOAD_URL" -o "$BIN_DIR/n"

# Makes the binary executable
chmod +x "$BIN_DIR/n"
echo "✅ CNav has been installed to $BIN_DIR."

# Automatically add shell hooks to .bashrc / .zshrc
SHELL_RC="$HOME/.bashrc"
if [[ "$SHELL" == *"zsh"* ]]; then
  SHELL_RC="$HOME/.zshrc"
fi

if ! grep -q "n --init" "$SHELL_RC"; then
  echo "" >>"$SHELL_RC"
  echo "# CNav (Command Navigation) Auto-Init" >>"$SHELL_RC"
  echo 'eval "$(n --init)"' >>"$SHELL_RC"
  echo "✅ Hooks have been added to $SHELL_RC!"
else
  echo "⚡ Hooks were already present in $SHELL_RC."
fi

echo ""
echo "🎉 Installation successful!"
echo "👉 Restart your terminal or run: source $SHELL_RC"
