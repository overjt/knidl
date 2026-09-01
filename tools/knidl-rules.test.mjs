/**
 * Assertions for the harness policy rules. Run with plain `node`:
 *
 *   node tools/knidl-rules.test.mjs
 *
 * Every case here is a rule someone can silently break by editing a regex.
 * The `git checkout -b` case in particular is a regression test: the original
 * destructive-checkout rule blocked the exact command the workflow requires
 * for starting a feature branch.
 */
import assert from "node:assert/strict";
import { blockedBash, isProtectedPath } from "./knidl-rules.mjs";

let failures = 0;

function allowed(command) {
  const reason = blockedBash(command);
  if (reason !== undefined) {
    console.error(`FAIL: expected ALLOWED but blocked: ${command}\n      reason: ${reason}`);
    failures += 1;
  }
}

function blocked(command) {
  if (blockedBash(command) === undefined) {
    console.error(`FAIL: expected BLOCKED but allowed: ${command}`);
    failures += 1;
  }
}

// The workflow must remain possible.
allowed("git checkout -b overjt/bulk-m22");
allowed("git checkout -B overjt/bulk-m22");
allowed("git switch -c overjt/bulk-m22");
allowed("git commit -m 'Carve M22 batch 1'");
allowed("git push -u origin overjt/bulk-m22");
allowed("gh pr create --base master --title x --body y");
allowed("make clean && make compare");
allowed("rm -rf build/scratch/fn_x && make clean");
allowed("./tools/fnmatch.sh 0x080988F8 0x08099B20 src/enemy_988f8.c --newpb");
allowed("docker run --rm -v \"$PWD\":/src -w /src knidl-builder arm-none-eabi-objdump -D x");
allowed("python3 tools/carve.py 0x080988F8 0x08099B20 enemy_988f8 --write");

// The contract must remain enforced.
blocked("git checkout -- src/main.c");
blocked("git checkout master");
blocked("git reset --hard HEAD~1");
blocked("git clean -fd");
blocked("git restore src/main.c");
blocked("git stash");
blocked("git merge master");
blocked("gh pr merge 109 --squash");
blocked("gh issue close 68");
blocked("git push origin master");
blocked("git commit --no-verify -m x");
blocked("git commit -n -m x");
blocked("git -c core.hooksPath=/dev/null commit -m x");
blocked("git config core.hooksPath /dev/null");
blocked("rm -rf src");
blocked("agbcc -O2 -mthumb-interwork < x.c");
blocked("git add baserom.gba");

// Protected write targets.
const cwd = "/repo";
assert.equal(isProtectedPath(cwd, "baserom.gba"), true);
assert.equal(isProtectedPath(cwd, "knidl.gba"), true);
assert.equal(isProtectedPath(cwd, ".git/config"), true);
assert.equal(isProtectedPath(cwd, "build/knidl.map"), true);
assert.equal(isProtectedPath(cwd, "src/main.c"), false);
assert.equal(isProtectedPath(cwd, "docs/analysis/rom-map.md"), false);

if (failures) {
  console.error(`${failures} rule failure(s)`);
  process.exit(1);
}
console.log("knidl-rules selftest passed");
