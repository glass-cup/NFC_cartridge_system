#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

echo "Installing nfc-cartridge-daemon..."

mkdir -p "$HOME/.local/bin"
cp "$SCRIPT_DIR/nfc-cartridge-daemon" "$HOME/.local/bin/nfc-cartridge-daemon"
chmod +x "$HOME/.local/bin/nfc-cartridge-daemon"

echo "Installing Python dependency (evdev)..."
pip3 install evdev 2>/dev/null || sudo pip3 install evdev 2>/dev/null || echo "  WARNING: could not install evdev — install it manually with: pip install evdev"

echo "Installing systemd user service..."
mkdir -p "$HOME/.config/systemd/user"
cp "$SCRIPT_DIR/nfc-cartridge.service" "$HOME/.config/systemd/user/nfc-cartridge.service"

echo "Reloading systemd and enabling service..."
systemctl --user daemon-reload
systemctl --user enable --now nfc-cartridge.service

echo "Done. Status:"
systemctl --user is-active nfc-cartridge.service
