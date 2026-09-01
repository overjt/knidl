/**
 * Pure policy rules for the KnIDL Pi harness.
 *
 * They live outside .pi/extensions so that plain `node` can test them without
 * loading Pi (see tools/knidl-rules.test.mjs, which CI runs on every push).
 * Keeping them dependency-free is the point: a rule that cannot be tested is a
 * rule nobody notices has stopped matching.
 */
import { relative, resolve } from "node:path";

/**
 * Decide whether a bash command violates the repository contract.
 * Returns the reason to show the agent, or undefined when the command is fine.
 */
export function blockedBash(command) {
  // Branch creation is REQUIRED by the workflow, so it has to survive the
  // destructive-checkout rule: `git checkout -b`/`-B` and `git switch` stay
  // available, every other checkout/restore/stash form discards work.
  const createsBranch = /\bgit\s+checkout\s+(?:-b|-B)\b/.test(command);
  if (!createsBranch && /\bgit\s+(?:reset\s+--hard|clean\b|checkout\b|restore\b|stash\b)/.test(command)) {
    return "Destructive Git operations are forbidden; preserve existing work. Use `git checkout -b` or `git switch -c` to start a branch.";
  }
  if (/\bgit\s+merge\b|\bgh\s+pr\s+merge\b/.test(command)) {
    return "The repository owner merges PRs. Leave the PR open for review.";
  }
  if (/\bgh\s+issue\s+close\b/.test(command)) {
    return "Issues auto-close only when the owner merges the PR into master.";
  }
  if (/\bgit\s+push\b[^\n]*(?:\bmaster\b|\binit\b)/.test(command)) {
    return "master and init must never be pushed by this harness.";
  }
  if (
    /\bgit\s+(?:-c\s+[^\n]*core\.hooksPath|config\b[^\n]*core\.hooksPath|commit\b[^\n]*(?:--no-verify|(?:^|\s)-n(?:\s|$)))/.test(
      command,
    )
  ) {
    return "Commit-hook bypasses are forbidden; the KnIDL merge gate must validate every commit.";
  }
  if (/\brm\s+(?:-\w*r\w*|--recursive)/i.test(command) && !/\bmake\s+clean\b/.test(command)) {
    return "Recursive deletion is blocked. `make clean` is the only supported build cleanup.";
  }
  const usesGbaToolchain = /(?:^|[\s;&|])(?:agbcc|old_agbcc|agbcc_arm|arm-none-eabi-(?:as|ld|objcopy))\b/.test(command);
  if (usesGbaToolchain && !/\bdocker\b/.test(command)) {
    return "GBA compilation must run inside the knidl-builder Docker image.";
  }
  if (/\bgit\s+add\b[^\n]*\bbaserom\.gba\b/.test(command)) {
    return "baserom.gba is copyrighted user input and may never be staged.";
  }
  return undefined;
}

/** Paths the agent must never write through the write/edit tools. */
export function isProtectedPath(cwd, value) {
  if (typeof value !== "string") return false;
  const path = resolve(cwd, value);
  const projectPath = relative(cwd, path).replace(/\\/g, "/");
  return (
    path.endsWith("/baserom.gba") ||
    path.endsWith("/knidl.gba") ||
    projectPath === ".git" ||
    projectPath.startsWith(".git/") ||
    projectPath === "build" ||
    projectPath.startsWith("build/")
  );
}
