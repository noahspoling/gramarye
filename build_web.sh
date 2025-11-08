#!/usr/bin/env bash
set -euo pipefail

# Resolve repo root (directory of this script)
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

have_cmd() { command -v "$1" >/dev/null 2>&1; }

# Try to ensure Emscripten tools are available in this shell
if ! have_cmd emcmake; then
	EMS_ENV="$SCRIPT_DIR/emsdk/emsdk_env.sh"
	if [[ -f "$EMS_ENV" ]]; then
		# shellcheck disable=SC1090
		source "$EMS_ENV"
	elif [[ -f "$SCRIPT_DIR/install_emscripten.sh" ]]; then
		echo "Emscripten not found. Installing via install_emscripten.sh..."
		bash "$SCRIPT_DIR/install_emscripten.sh"
		if [[ -f "$EMS_ENV" ]]; then
			# shellcheck disable=SC1090
			source "$EMS_ENV"
		fi
	fi
fi

if ! have_cmd emcmake; then
	echo "Error: emcmake not found in PATH."
	echo "Hint: Open a new shell or run: source emsdk/emsdk_env.sh"
	exit 127
fi

mkdir -p build-web

# Configure with Emscripten toolchain
emcmake cmake -B build-web -S . -DCMAKE_BUILD_TYPE=Release -DBUILD_WEB=ON

# Build
cmake --build build-web -j

# Run locally (prefer emrun if available, otherwise print a simple HTTP server hint)
if have_cmd emrun; then
	emrun --no_browser build-web/hello_world.html
else
	echo "Tip: emrun not found. To serve locally, you can run:"
	echo "  python3 -m http.server -d build-web 8000"
	echo "Then open http://localhost:8000/hello_world.html"
fi