/**
 * Unattended driver for the KnIDL module queue.
 *
 * The loop is deliberately not model-driven. After every time the agent goes
 * quiet, the autopilot re-reads the objective state from
 * `tools/pi_harness.py` — which derives "is this landed" from segments.txt,
 * the same file the linker and the build use — and decides the next step from
 * that. A model claiming success it did not achieve therefore cannot advance
 * the queue, and a model that quietly gives up cannot stall it either.
 *
 * Dormant unless KNIDL_AUTOPILOT=1, so interactive sessions are never driven.
 *
 * Environment:
 *   KNIDL_MAX_MODULES   stop after N modules land (0 = until the queue empties)
 *   KNIDL_MAX_ATTEMPTS  settles per module before it is parked (default 3)
 */
import type { ExtensionAPI, ExtensionContext } from "@earendil-works/pi-coding-agent";
import { execFile } from "node:child_process";
import { promisify } from "node:util";

const execFileAsync = promisify(execFile);

const ENABLED = process.env.KNIDL_AUTOPILOT === "1";
const MAX_MODULES = Number(process.env.KNIDL_MAX_MODULES || "0");
const MAX_ATTEMPTS = Math.max(1, Number(process.env.KNIDL_MAX_ATTEMPTS || "3"));
/** Compact once the context passes this share of the window. */
const COMPACT_AT = 0.7;

type Assignment = {
  id: string;
  issue: number | null;
  start: string;
  end: string;
  name: string;
  recipe: string;
  batches: string;
  uncovered_bytes: number;
  functions_left: number;
  attempts: number;
};

type State = {
  running: boolean;
  module: string | null;
  /** uncovered_bytes when the current module's last attempt started. */
  baseline: number;
  attempts: number;
  landed: number;
  settles: number;
};

const state: State = { running: false, module: null, baseline: -1, attempts: 0, landed: 0, settles: 0 };

async function oracle(cwd: string, args: string[]): Promise<any> {
  const { stdout } = await execFileAsync("python3", ["tools/pi_harness.py", ...args], {
    cwd,
    timeout: 120 * 1000,
    maxBuffer: 4 * 1024 * 1024,
  });
  const text = stdout.trim();
  if (!text) return null;
  try {
    return JSON.parse(text);
  } catch {
    return text;
  }
}

async function record(cwd: string, module: string, result: string, note: string): Promise<void> {
  try {
    await execFileAsync("python3", ["tools/pi_harness.py", "record", module, "--result", result, "--note", note], {
      cwd,
      timeout: 30 * 1000,
    });
  } catch {
    // Bookkeeping must never take the loop down.
  }
}

function say(ctx: ExtensionContext, text: string, level: "info" | "warning" | "error" = "info"): void {
  if (ctx.hasUI) ctx.ui.notify(text, level);
  else console.log(`[knidl-autopilot] ${text}`);
}

function briefing(assignment: Assignment): string {
  const batches = assignment.batches ? `\nSuggested batches: ${assignment.batches}` : "";
  const issue = assignment.issue ? `issue #${assignment.issue}` : "no filed issue";
  return [
    `Use the knidl-decomp skill. Work module ${assignment.id} (${issue}): ${assignment.name}.`,
    `Range [${assignment.start}, ${assignment.end}), ${assignment.uncovered_bytes} bytes of assembly left in ${assignment.functions_left} function(s).`,
    `Recipe: ${assignment.recipe}.${batches}`,
    "",
    "Land one verified batch at a time: fnmatch must report MATCH before carve,",
    "then `make symbols && make split`, then `make clean && make compare`, then commit.",
    "Commit only at batch boundaries — the merge gate runs `make clean`, which deletes build/scratch.",
    "When the whole module is C, update the maps and lessons, then say DONE and stop.",
  ].join("\n");
}

function nudge(assignment: Assignment, before: number): string {
  const moved = before >= 0 && assignment.uncovered_bytes < before;
  const progress = moved
    ? `You landed ${before - assignment.uncovered_bytes} bytes since the last check.`
    : "Nothing landed since the last check, according to segments.txt.";
  return [
    `${progress} Module ${assignment.id} still has ${assignment.uncovered_bytes} bytes of assembly`,
    `in ${assignment.functions_left} function(s), so it is not finished.`,
    "",
    "Continue with the next smallest uncovered function.",
    "If a function will not match after two informed attempts, leave it, note why,",
    "and move to the next one rather than looping on it.",
    "Run `python3 tools/pi_harness.py assignment " + assignment.id + "` for the current list.",
  ].join("\n");
}

export default function knidlAutopilot(pi: ExtensionAPI) {
  pi.registerCommand("knidl-autopilot", {
    description: "Start the unattended module queue loop",
    handler: async (input, ctx) => {
      if (!ENABLED) {
        say(ctx, "Autopilot is dormant. Launch it with ./tools/pi-knidl.sh autopilot.", "warning");
        return;
      }
      if (state.running) {
        say(ctx, "Autopilot is already running.", "warning");
        return;
      }
      const requested = input.trim();
      const target = requested
        ? await oracle(ctx.cwd, ["assignment", requested, "--json"])
        : await oracle(ctx.cwd, ["next", "--json"]);
      if (!target) {
        say(ctx, "Queue is empty: every module is landed or parked.", "warning");
        return;
      }
      const assignment = target as Assignment;
      state.running = true;
      state.module = assignment.id;
      state.baseline = assignment.uncovered_bytes;
      state.attempts = 0;
      state.landed = 0;
      state.settles = 0;
      pi.appendEntry("knidl-autopilot", { event: "start", module: assignment.id });
      say(ctx, `Autopilot started on ${assignment.id} [${assignment.start}, ${assignment.end}).`);
      pi.sendUserMessage(briefing(assignment));
    },
  });

  pi.on("session_start", (_event, ctx) => {
    if (ENABLED && ctx.hasUI) {
      ctx.ui.setStatus("knidl-autopilot", `autopilot armed (max ${MAX_ATTEMPTS} attempts/module)`);
    }
  });

  pi.on("agent_settled", async (_event, ctx) => {
    if (!ENABLED || !state.running || !state.module) return;
    state.settles += 1;

    // Ground truth first. Nothing the model said is trusted here.
    let assignment: Assignment;
    try {
      assignment = (await oracle(ctx.cwd, ["assignment", state.module, "--json"])) as Assignment;
    } catch (error: any) {
      say(ctx, `Oracle failed: ${error?.message || error}. Stopping.`, "error");
      state.running = false;
      return;
    }

    if (assignment && assignment.uncovered_bytes === 0) {
      state.landed += 1;
      await record(ctx.cwd, state.module, "landed", `landed after ${state.attempts + 1} attempt(s)`);
      pi.appendEntry("knidl-autopilot", { event: "landed", module: state.module });
      say(ctx, `${state.module} is fully C. ${state.landed} module(s) landed this run.`);

      if (MAX_MODULES > 0 && state.landed >= MAX_MODULES) {
        say(ctx, `Reached KNIDL_MAX_MODULES=${MAX_MODULES}. Stopping.`);
        state.running = false;
        return;
      }
      const next = (await oracle(ctx.cwd, ["next", "--json"])) as Assignment | null;
      if (!next) {
        say(ctx, "Queue empty: the bulk code is fully decompiled. Stopping.");
        state.running = false;
        return;
      }
      state.module = next.id;
      state.baseline = next.uncovered_bytes;
      state.attempts = 0;
      // A fresh module deserves a fresh context; the summary carries the
      // lessons forward without the previous module's transcript.
      const usage = ctx.getContextUsage?.();
      if (usage && usage.tokens > 0) {
        ctx.compact({
          customInstructions:
            "Keep the agbcc source-shape lessons and the repository workflow. Drop the finished module's per-function detail.",
          onComplete: () => pi.sendUserMessage(briefing(next)),
          onError: () => pi.sendUserMessage(briefing(next)),
        });
      } else {
        pi.sendUserMessage(briefing(next));
      }
      return;
    }

    // Not finished. Decide between nudging and parking on measured progress.
    state.attempts += 1;
    const before = state.baseline;
    const moved = before >= 0 && assignment.uncovered_bytes < before;
    if (moved) state.attempts = 0; // real progress buys a fresh budget
    state.baseline = assignment.uncovered_bytes;

    if (state.attempts >= MAX_ATTEMPTS) {
      await record(
        ctx.cwd,
        state.module,
        "blocked",
        `no progress in ${MAX_ATTEMPTS} settles; ${assignment.uncovered_bytes} bytes left`,
      );
      pi.appendEntry("knidl-autopilot", { event: "parked", module: state.module });
      say(ctx, `Parking ${state.module}: no measurable progress in ${MAX_ATTEMPTS} attempts.`, "warning");
      const next = (await oracle(ctx.cwd, ["next", "--json"])) as Assignment | null;
      if (!next) {
        say(ctx, "Nothing left that is not parked. Stopping.");
        state.running = false;
        return;
      }
      state.module = next.id;
      state.baseline = next.uncovered_bytes;
      state.attempts = 0;
      pi.sendUserMessage(briefing(next));
      return;
    }

    const usage = ctx.getContextUsage?.();
    if (usage && usage.contextWindow && usage.tokens / usage.contextWindow > COMPACT_AT) {
      ctx.compact({
        customInstructions: "Keep the current module's verified ranges, failures, and next step.",
        onComplete: () => pi.sendUserMessage(nudge(assignment, before)),
        onError: () => pi.sendUserMessage(nudge(assignment, before)),
      });
      return;
    }
    pi.sendUserMessage(nudge(assignment, before));
  });
}
