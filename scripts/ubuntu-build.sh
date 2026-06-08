#!/usr/bin/env bash
set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$repo_root"

cmake --preset ubuntu-release
cmake --build --preset ubuntu-release --target SkipAdClicker

echo "Built: $repo_root/out/build/ubuntu-release/bin/skipadclicker"
