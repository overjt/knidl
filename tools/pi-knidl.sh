#!/usr/bin/env bash
# Launch Pi with the project-local KnIDL skill, prompts, extensions, and a
# repository-local session store.
#
# Usage:
#   ./tools/pi-knidl.sh                  interactive session
#   ./tools/pi-knidl.sh start M27        interactive session aimed at one module
#   ./tools/pi-knidl.sh autopilot        unattended run of the module queue
#   ./tools/pi-knidl.sh autopilot M22    unattended run starting at one module
#
# Environment overrides:
#   KNIDL_PROVIDER / KNIDL_MODEL   pass --provider/--model to Pi
#   KNIDL_THINKING                 reasoning level (default: high)
#   KNIDL_MAX_MODULES              autopilot: stop after N landed modules
#   KNIDL_MAX_ATTEMPTS             autopilot: attempts per module before parking
set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$repo_root"

if ! command -v pi >/dev/null 2>&1; then
  echo "error: Pi is not installed or not on PATH" >&2
  exit 127
fi

mkdir -p .pi/sessions .pi/state

# Activate the repository-owned merge gate. The value is RELATIVE on purpose:
# Git resolves a relative core.hooksPath against each working tree's own root,
# so this never reaches into a sibling worktree of the same repository. Writing
# an absolute path here is what previously made unrelated worktrees fail every
# commit with "can't open file tools/pi_harness.py".
git config --local core.hooksPath .githooks

if ! python3 tools/pi_harness.py selftest >/dev/null; then
  echo "error: harness selftest failed; run: python3 tools/pi_harness.py selftest" >&2
  exit 1
fi

pi_args=(--approve --thinking "${KNIDL_THINKING:-high}" --session-dir "$repo_root/.pi/sessions")
[[ -n "${KNIDL_PROVIDER:-}" ]] && pi_args+=(--provider "$KNIDL_PROVIDER")
[[ -n "${KNIDL_MODEL:-}" ]] && pi_args+=(--model "$KNIDL_MODEL")

mode="${1:-}"
case "$mode" in
  start)
    shift
    exec env PI_OFFLINE=1 PI_TELEMETRY=0 pi "${pi_args[@]}" \
      --name "knidl ${*:-next}" "/knidl-start $*"
    ;;
  autopilot)
    shift
    # KNIDL_AUTOPILOT is the switch the autopilot extension looks for; without
    # it the extension loads but stays dormant, so interactive sessions are
    # never driven by the loop.
    exec env PI_OFFLINE=1 PI_TELEMETRY=0 KNIDL_AUTOPILOT=1 \
      KNIDL_MAX_MODULES="${KNIDL_MAX_MODULES:-0}" \
      KNIDL_MAX_ATTEMPTS="${KNIDL_MAX_ATTEMPTS:-3}" \
      pi "${pi_args[@]}" --name "knidl autopilot" "/knidl-autopilot $*"
    ;;
  *)
    exec env PI_OFFLINE=1 PI_TELEMETRY=0 pi "${pi_args[@]}" "$@"
    ;;
esac
