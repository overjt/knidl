/**
 * Project-local safety guard for the KnIDL Pi harness.
 *
 * It blocks only actions that violate the repository contract in AGENTS.md.
 * Normal editing, Docker builds, branch creation, commits on feature branches,
 * and PR creation stay available to the agent and its owner.
 */
import type { ExtensionAPI } from "@earendil-works/pi-coding-agent";
import { execFile } from "node:child_process";
import { promisify } from "node:util";
// The policy rules live in plain JS so `node tools/knidl-rules.test.mjs` can
// assert them in CI without loading Pi. Keep this file to wiring only.
import { blockedBash, isProtectedPath } from "../../tools/knidl-rules.mjs";

const execFileAsync = promisify(execFile);

function display(output: string, maximum = 7000): string {
  return output.length <= maximum ? output : `${output.slice(0, maximum)}\n… output truncated`;
}

async function runHarness(args: string[], cwd: string): Promise<string> {
  try {
    const { stdout, stderr } = await execFileAsync("python3", ["tools/pi_harness.py", ...args], {
      cwd,
      timeout: args[0] === "verify" ? 30 * 60 * 1000 : 60 * 1000,
      maxBuffer: 1024 * 1024,
    });
    return display(`${stdout}${stderr}`.trim() || "Harness completed without output.");
  } catch (error: any) {
    const stdout = error?.stdout || "";
    const stderr = error?.stderr || error?.message || String(error);
    return display(`${stdout}${stderr}`.trim());
  }
}

export default function knidlGuard(pi: ExtensionAPI) {
  pi.on("session_start", (_event, ctx) => {
    if (ctx.hasUI) {
      ctx.ui.setStatus("knidl", "KnIDL guard active");
      ctx.ui.notify("KnIDL harness loaded. Start with /knidl-start or /skill:knidl-decomp.", "info");
    }
  });

  pi.on("tool_call", async (event, ctx) => {
    if (event.toolName === "bash") {
      const reason = blockedBash(String(event.input.command || ""));
      return reason ? { block: true, reason } : undefined;
    }
    if ((event.toolName === "write" || event.toolName === "edit") && isProtectedPath(ctx.cwd, event.input.path)) {
      return { block: true, reason: "Generated build output, Git metadata, ROM input, and ROM output are protected." };
    }
    return undefined;
  });

  for (const [name, description, args] of [
    ["knidl-status", "Show repository, ROM, Docker, and queue status", ["status"]],
    ["knidl-preflight", "Check prerequisites before working", ["preflight"]],
    ["knidl-next", "Show the next module in the recommended order", ["next"]],
    ["knidl-plan", "Show the whole remaining module queue", ["plan"]],
    ["knidl-verify", "Run full Docker verification (requires baserom.gba)", ["verify", "--full"]],
  ] as const) {
    pi.registerCommand(name, {
      description,
      handler: async (_input, ctx) => {
        const result = await runHarness([...args], ctx.cwd);
        if (ctx.hasUI) ctx.ui.notify(result, result.startsWith("ERROR") ? "error" : "info");
      },
    });
  }

  pi.registerCommand("knidl-assignment", {
    description: "Show range, recipe, batches, and next functions for a module",
    handler: async (input, ctx) => {
      const module = input.trim();
      if (!module) {
        if (ctx.hasUI) ctx.ui.notify("Usage: /knidl-assignment M27 (or an address/range)", "warning");
        return;
      }
      const result = await runHarness(["assignment", module], ctx.cwd);
      if (ctx.hasUI) ctx.ui.notify(result, result.startsWith("ERROR") ? "error" : "info");
    },
  });
}
