/* knidl-boottest: the boot test of a shifted ROM (issue #36, docs/data.md
 * section 8.4).
 *
 * Runs a reference ROM (the matching knidl.gba) and one or more test ROMs
 * (the same objects linked with padding inserted, tools/boottest.py) in
 * lockstep in mGBA's core, with the same scripted input and a fresh, empty
 * save held in memory, and compares every frame: the video buffer, the audio
 * samples, and the work RAM up to relocation (a RAM word that differs by
 * exactly the padding and points at or after the insertion point is a
 * pointer the linker moved, not a divergence).  The first frame at which a
 * test ROM differs points at what the shift test cannot see: a pointer the
 * census missed or misclassified, a relative branch written as raw bytes, or
 * behaviour that depends on an address.  Exit status: 0 when every test ROM
 * matches for the whole run, 1 on any difference, 2 on a usage or load
 * error.
 *
 * A script can name its scenes (`mark`) and check that the reference really
 * reaches them (`expect`, a RAM cell of the reference at that frame); a
 * failed expectation fails the run, so a script cannot drift silently away
 * from what its comments claim (docs/data.md section 8.4).
 *
 * Nothing is written except what --shot asks for (PNG frames of the
 * reference, for designing the input script; they are game assets and must
 * never be committed) and what --coverage asks for (the ROM address ranges
 * the reference executed, for a coverage count; never committed either):
 * hashes are compared at run time only.
 *
 * Built against mGBA's core library (MPL-2.0) in tools/boottest/Dockerfile.
 */

#include <mgba/core/blip_buf.h>
#include <mgba/core/config.h>
#include <mgba/core/core.h>
#include <mgba/core/log.h>
#include <mgba/gba/core.h>
#include <mgba/internal/arm/arm.h>
#include <mgba-util/vfs.h>

#include <ctype.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WIDTH 240
#define HEIGHT 160
#define MAX_ROMS 16
#define AUDIO_RATE 32768
#define REGION_EWRAM 2
#define REGION_IWRAM 3

/* ---------------------------------------------------------------- symbols */

/* --syms: `nm` output ("<hex address> <type> <name>" per line), so that
 * expect/--peek can name a RAM cell instead of writing its address. */
struct Sym {
	char* name;
	uint32_t addr;
};

static struct Sym* syms;
static size_t nsyms;

static void load_syms(const char* path) {
	FILE* f = fopen(path, "r");
	char line[512];
	size_t cap = 1024;
	if (!f) {
		fprintf(stderr, "%s: %s\n", path, strerror(errno));
		exit(2);
	}
	syms = malloc(cap * sizeof(*syms));
	while (fgets(line, sizeof(line), f)) {
		char addr[64], type[8], name[256];
		if (sscanf(line, "%63s %7s %255s", addr, type, name) != 3) {
			continue;
		}
		if (nsyms == cap) {
			cap *= 2;
			syms = realloc(syms, cap * sizeof(*syms));
		}
		syms[nsyms].name = strdup(name);
		syms[nsyms].addr = strtoul(addr, NULL, 16);
		++nsyms;
	}
	fclose(f);
}

/* A RAM cell: "<symbol or 0xADDR>[+<offset>]:<size>", size 1, 2 or 4.  The
 * values are read from the reference (rawRead, no side effects). */
struct Cell {
	char spec[96];
	uint32_t addr;
	int size;
};

static int parse_cell(const char* spec, struct Cell* c) {
	char buf[96];
	char* colon;
	char* plus;
	uint32_t off = 0;
	snprintf(c->spec, sizeof(c->spec), "%s", spec);
	snprintf(buf, sizeof(buf), "%s", spec);
	colon = strrchr(buf, ':');
	if (!colon) {
		return 0;
	}
	*colon = '\0';
	c->size = (int) strtol(colon + 1, NULL, 0);
	if (c->size != 1 && c->size != 2 && c->size != 4) {
		return 0;
	}
	plus = strchr(buf, '+');
	if (plus) {
		*plus = '\0';
		off = strtoul(plus + 1, NULL, 0);
	}
	if (isdigit((unsigned char) buf[0])) {
		c->addr = strtoul(buf, NULL, 0);
	} else {
		size_t i;
		for (i = 0; i < nsyms && strcmp(syms[i].name, buf) != 0; ++i) {
		}
		if (i == nsyms) {
			return 0;
		}
		c->addr = syms[i].addr;
	}
	c->addr += off;
	return 1;
}

static uint32_t read_cell(struct mCore* core, const struct Cell* c) {
	uint32_t v = 0;
	for (int i = c->size - 1; i >= 0; --i) {
		v = v << 8 | (core->rawRead8(core, c->addr + i, -1) & 0xFF);
	}
	return v;
}

/* ---------------------------------------------------------------- input */

static const char* const KEY_NAMES[] = {
	"A", "B", "SELECT", "START", "RIGHT", "LEFT", "UP", "DOWN", "R", "L"
};

struct Step {
	long frames;
	uint32_t keys;
	int line;
};

static struct Step* steps;
static size_t nsteps;
static long script_frames;

/* "mark <name>" and "expect <cell> <op> <value>" lines: checkpoints at the
 * frame where the next step starts (after every frame above them ran). */
enum { OP_EQ, OP_NE, OP_LT, OP_LE, OP_GT, OP_GE };
static const char* const OP_NAMES[] = { "==", "!=", "<", "<=", ">", ">=" };

struct Mark {
	long frame;
	int line;
	int expect;          /* 0: a scene name; 1: a RAM check */
	char name[400];      /* the scene (mark) or the line as written (expect) */
	struct Cell cell;
	int op;
	uint32_t value;
};

static struct Mark* marks;
static size_t nmarks, cap_marks;
static long expects_failed, expects_passed;

static uint32_t parse_keys(const char* spec, const char* path, int line) {
	uint32_t keys = 0;
	char buf[256];
	if (strcmp(spec, "-") == 0) {
		return 0;
	}
	snprintf(buf, sizeof(buf), "%s", spec);
	for (char* tok = strtok(buf, "+"); tok; tok = strtok(NULL, "+")) {
		size_t k;
		for (k = 0; k < sizeof(KEY_NAMES) / sizeof(*KEY_NAMES); ++k) {
			if (strcmp(tok, KEY_NAMES[k]) == 0) {
				break;
			}
		}
		if (k == sizeof(KEY_NAMES) / sizeof(*KEY_NAMES)) {
			fprintf(stderr, "%s:%d: unknown key '%s'\n", path, line, tok);
			exit(2);
		}
		keys |= 1u << k;
	}
	return keys;
}

static struct Mark* new_mark(const char* path, int lineno, long rep) {
	if (rep) {
		fprintf(stderr, "%s:%d: mark/expect inside a repeat block\n", path, lineno);
		exit(2);
	}
	if (nmarks == cap_marks) {
		cap_marks = cap_marks ? cap_marks * 2 : 64;
		marks = realloc(marks, cap_marks * sizeof(*marks));
	}
	memset(&marks[nmarks], 0, sizeof(*marks));
	marks[nmarks].frame = script_frames;
	marks[nmarks].line = lineno;
	return &marks[nmarks++];
}

/* One step per line: "<frames> <keys>", keys "-" (none) or names joined by
 * '+' (A B SELECT START RIGHT LEFT UP DOWN R L).  "#" starts a comment;
 * "repeat <n>" ... "end" repeats a block (not nested).  Outside a repeat
 * block, "mark <name>" names the scene that starts there and "expect <cell>
 * <op> <value>" checks a RAM cell of the reference there (<cell> as
 * parse_cell, <op> one of == != < <= > >=, unsigned; a failed check fails
 * the run).  Both take effect at the frame where the next step starts. */
static void load_script(const char* path) {
	FILE* f = fopen(path, "r");
	char line[512];
	int lineno = 0;
	size_t cap = 256;
	long rep = 0;
	size_t rep_start = 0;
	long rep_frames = 0;
	if (!f) {
		fprintf(stderr, "%s: %s\n", path, strerror(errno));
		exit(2);
	}
	steps = malloc(cap * sizeof(*steps));
	while (fgets(line, sizeof(line), f)) {
		char* hash = strchr(line, '#');
		char a[64], b[256], c[64], d[64];
		int n;
		++lineno;
		if (hash) {
			*hash = '\0';
		}
		n = sscanf(line, "%63s %255s %63s %63s", a, b, c, d);
		if (n <= 0) {
			continue;
		}
		if (strcmp(a, "mark") == 0 && n >= 2) {
			struct Mark* m = new_mark(path, lineno, rep);
			char* name = strstr(line, "mark") + 4;
			size_t len;
			while (isspace((unsigned char) *name)) {
				++name;
			}
			len = strlen(name);
			while (len && isspace((unsigned char) name[len - 1])) {
				name[--len] = '\0';
			}
			snprintf(m->name, sizeof(m->name), "%s", name);
			continue;
		}
		if (strcmp(a, "expect") == 0) {
			struct Mark* m;
			int op;
			if (n != 4) {
				fprintf(stderr, "%s:%d: expected 'expect <cell> <op> <value>'\n",
				        path, lineno);
				exit(2);
			}
			for (op = 0; op < 6 && strcmp(OP_NAMES[op], c) != 0; ++op) {
			}
			m = new_mark(path, lineno, rep);
			if (op == 6 || !parse_cell(b, &m->cell)) {
				fprintf(stderr, "%s:%d: bad expect '%s %s %s' (a cell is "
				        "<symbol|0xADDR>[+off]:<1|2|4>; symbols need --syms)\n",
				        path, lineno, b, c, d);
				exit(2);
			}
			m->expect = 1;
			m->op = op;
			m->value = strtoul(d, NULL, 0);
			snprintf(m->name, sizeof(m->name), "%s %s %s", b, c, d);
			continue;
		}
		if (strcmp(a, "repeat") == 0 && n == 2) {
			rep = strtol(b, NULL, 0);
			rep_start = nsteps;
			rep_frames = script_frames;
			continue;
		}
		if (strcmp(a, "end") == 0 && n == 1) {
			size_t len = nsteps - rep_start;
			long body = script_frames - rep_frames;
			for (long r = 1; r < rep; ++r) {
				for (size_t i = 0; i < len; ++i) {
					if (nsteps == cap) {
						cap *= 2;
						steps = realloc(steps, cap * sizeof(*steps));
					}
					steps[nsteps++] = steps[rep_start + i];
				}
				script_frames += body;
			}
			rep = 0;
			continue;
		}
		if (n != 2) {
			fprintf(stderr, "%s:%d: expected '<frames> <keys>'\n", path, lineno);
			exit(2);
		}
		if (nsteps == cap) {
			cap *= 2;
			steps = realloc(steps, cap * sizeof(*steps));
		}
		steps[nsteps].frames = strtol(a, NULL, 0);
		steps[nsteps].keys = parse_keys(b, path, lineno);
		steps[nsteps].line = lineno;
		if (steps[nsteps].frames <= 0) {
			fprintf(stderr, "%s:%d: frame count must be positive\n", path, lineno);
			exit(2);
		}
		script_frames += steps[nsteps].frames;
		++nsteps;
	}
	fclose(f);
}

/* The scene running at frame f: the last mark at or before it, or NULL. */
static const char* scene_at(long f) {
	const char* name = NULL;
	for (size_t i = 0; i < nmarks && marks[i].frame <= f; ++i) {
		if (!marks[i].expect) {
			name = marks[i].name;
		}
	}
	return name;
}

/* Run the marks and expects due before frame f (the reference's state
 * after frame f - 1). */
static void check_marks(struct mCore* ref, long f, int last) {
	static size_t next = 0;
	while (next < nmarks && (marks[next].frame <= f || last)) {
		struct Mark* m = &marks[next++];
		if (m->frame > f) {
			printf("script line %d: %s %s: not reached (the run ends at frame %ld)\n",
			       m->line, m->expect ? "expect" : "mark", m->name, f);
			continue;
		}
		if (!m->expect) {
			printf("frame %6ld: %s\n", m->frame, m->name);
			continue;
		}
		uint32_t v = read_cell(ref, &m->cell);
		uint32_t w = m->value;
		int ok = m->op == OP_EQ ? v == w : m->op == OP_NE ? v != w
		    : m->op == OP_LT ? v < w : m->op == OP_LE ? v <= w
		    : m->op == OP_GT ? v > w : v >= w;
		if (ok) {
			++expects_passed;
		} else {
			++expects_failed;
			printf("frame %6ld: SCRIPT DRIFT: expect %s failed on the reference "
			       "(value 0x%X, script line %d)\n", m->frame, m->name, v, m->line);
		}
	}
	fflush(stdout);
}

/* keys for frame `frame` (0-based); the script's line through *line */
static uint32_t keys_at(long frame, int* line) {
	static size_t cur = 0;
	static long cur_start = 0;
	if (frame < cur_start) {
		cur = 0;
		cur_start = 0;
	}
	while (cur < nsteps && frame >= cur_start + steps[cur].frames) {
		cur_start += steps[cur].frames;
		++cur;
	}
	if (cur >= nsteps) {
		*line = 0;
		return 0;
	}
	*line = steps[cur].line;
	return steps[cur].keys;
}

/* ---------------------------------------------------------------- logging */

/* The core a thread is running (each core runs on one thread per frame). */
static __thread int current_rom = -1;
static long current_frame;
static long game_errors[MAX_ROMS];
static char first_errors[MAX_ROMS][3][160];
static int quiet_log;

static void log_cb(struct mLogger* logger, int category, enum mLogLevel level,
                   const char* format, va_list args) {
	(void) logger;
	if (level & (mLOG_GAME_ERROR | mLOG_ERROR | mLOG_FATAL)) {
		if (current_rom >= 0 && current_rom < MAX_ROMS) {
			long n = game_errors[current_rom]++;
			if (n < 3) {
				char msg[128];
				vsnprintf(msg, sizeof(msg), format, args);
				snprintf(first_errors[current_rom][n], sizeof(first_errors[0][0]),
				         "frame %ld: %s: %s", current_frame,
				         mLogCategoryName(category), msg);
			}
		}
	}
}

static struct mLogger logger = { .log = log_cb };

/* ---------------------------------------------------------------- hashes */

static uint64_t fnv(uint64_t h, const void* data, size_t n) {
	const uint8_t* p = data;
	for (size_t i = 0; i < n; ++i) {
		h ^= p[i];
		h *= 0x100000001B3ULL;
	}
	return h;
}

#define FNV_INIT 0xCBF29CE484222325ULL

/* ---------------------------------------------------------------- PNG */

static uint32_t crc_table[256];

static void crc_init(void) {
	for (uint32_t n = 0; n < 256; ++n) {
		uint32_t c = n;
		for (int k = 0; k < 8; ++k) {
			c = c & 1 ? 0xEDB88320u ^ (c >> 1) : c >> 1;
		}
		crc_table[n] = c;
	}
}

static uint32_t crc(uint32_t c, const uint8_t* p, size_t n) {
	c ^= 0xFFFFFFFFu;
	while (n--) {
		c = crc_table[(c ^ *p++) & 0xFF] ^ (c >> 8);
	}
	return c ^ 0xFFFFFFFFu;
}

static void be32(uint8_t* p, uint32_t v) {
	p[0] = v >> 24; p[1] = v >> 16; p[2] = v >> 8; p[3] = v;
}

static void chunk(FILE* f, const char* type, const uint8_t* data, uint32_t n) {
	uint8_t hdr[8];
	uint32_t c;
	be32(hdr, n);
	memcpy(hdr + 4, type, 4);
	fwrite(hdr, 1, 8, f);
	fwrite(data, 1, n, f);
	c = crc(crc(0, hdr + 4, 4), data, n);
	be32(hdr, c);
	fwrite(hdr, 1, 4, f);
}

/* An uncompressed (stored-deflate) RGB PNG of one frame. */
static void write_png(const char* path, const color_t* px) {
	const uint32_t row = 1 + WIDTH * 3;
	const uint32_t raw_len = row * HEIGHT;
	uint8_t* raw = malloc(raw_len);
	uint32_t nblocks = (raw_len + 65534) / 65535;
	uint32_t z_len = 2 + raw_len + nblocks * 5 + 4;
	uint8_t* z = malloc(z_len);
	uint8_t ihdr[13];
	uint32_t a = 1, b = 0, o = 0;
	FILE* f = fopen(path, "wb");
	if (!f) {
		fprintf(stderr, "%s: %s\n", path, strerror(errno));
		exit(2);
	}
	for (int y = 0; y < HEIGHT; ++y) {
		raw[y * row] = 0;
		for (int x = 0; x < WIDTH; ++x) {
			uint32_t c = px[y * WIDTH + x];
			raw[y * row + 1 + x * 3 + 0] = c & 0xFF;
			raw[y * row + 1 + x * 3 + 1] = (c >> 8) & 0xFF;
			raw[y * row + 1 + x * 3 + 2] = (c >> 16) & 0xFF;
		}
	}
	z[o++] = 0x78;
	z[o++] = 0x01;
	for (uint32_t i = 0; i < raw_len; i += 65535) {
		uint32_t n = raw_len - i < 65535 ? raw_len - i : 65535;
		z[o++] = i + n == raw_len;
		z[o++] = n & 0xFF;
		z[o++] = n >> 8;
		z[o++] = ~n & 0xFF;
		z[o++] = (~n >> 8) & 0xFF;
		memcpy(z + o, raw + i, n);
		o += n;
	}
	for (uint32_t i = 0; i < raw_len; ++i) {
		a = (a + raw[i]) % 65521;
		b = (b + a) % 65521;
	}
	be32(z + o, b << 16 | a);
	o += 4;
	fwrite("\x89PNG\r\n\x1a\n", 1, 8, f);
	be32(ihdr, WIDTH);
	be32(ihdr + 4, HEIGHT);
	ihdr[8] = 8;  /* bit depth */
	ihdr[9] = 2;  /* RGB */
	ihdr[10] = ihdr[11] = ihdr[12] = 0;
	chunk(f, "IHDR", ihdr, 13);
	chunk(f, "IDAT", z, o);
	chunk(f, "IEND", NULL, 0);
	fclose(f);
	free(raw);
	free(z);
}

/* ---------------------------------------------------------------- cores */

struct Rom {
	const char* path;
	uint32_t point;       /* insertion point (VMA), 0 for the reference */
	uint32_t pad;         /* bytes inserted there */
	uint32_t rom_end;     /* VMA end of the reference image */
	struct mCore* core;
	color_t* video;
	uint64_t vhash, ahash;
	long first_diff;      /* first frame with a video/audio difference, -1 */
	const char* what;
	long first_ram_diff;  /* first frame with a RAM difference beyond relocation */
	uint32_t ram_addr[8], ram_ref[8], ram_test[8];
	long ram_frames;      /* frames with a RAM difference */
	long ram_last;        /* the last one */
	long ram_max;         /* the most words differing in one frame */
	long ram_n;           /* this frame's count (worker thread) */
	long allowed;         /* --ram-allow hits */
	int done;
};

static struct Rom roms[MAX_ROMS];
static int nroms;

/* --ram-allow: RAM words whose difference is explained and not a pointer */
static uint32_t ram_allow[32];
static int nallow;

static int16_t audio_scratch[0x4000];

static const char* const REG_NAMES[] = {
	"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9",
	"r10", "r11", "r12", "sp", "lr", "pc", "cpsr"
};

/* A register value of the test ROM equals the reference's up to relocation */
static int same_value(const struct Rom* r, uint32_t va, uint32_t vb) {
	return va == vb || ((va & ~1u) >= r->point && (va & ~1u) < r->rom_end
	                    && vb == va + r->pad);
}


static void open_rom(struct Rom* r) {
	struct VFile* vf = VFileOpen(r->path, O_RDONLY);
	struct mCore* core;
	if (!vf) {
		fprintf(stderr, "%s: cannot open\n", r->path);
		exit(2);
	}
	core = GBACoreCreate();
	if (!core || !core->init(core)) {
		fprintf(stderr, "cannot create an mGBA core\n");
		exit(2);
	}
	mCoreInitConfig(core, NULL);
	/* no idle-loop skipping: the same cycles for both images */
	mCoreConfigSetValue(&core->config, "idleOptimization", "ignore");
	core->opts.mute = false;
	core->opts.volume = 0x100;
	core->opts.frameskip = 0;
	core->loadConfig(core, &core->config);
	r->video = calloc(WIDTH * HEIGHT, sizeof(color_t));
	core->setVideoBuffer(core, r->video, WIDTH);
	core->setAudioBufferSize(core, 4096);
	if (!core->loadROM(core, vf)) {
		fprintf(stderr, "%s: not a GBA ROM\n", r->path);
		exit(2);
	}
	/* a fresh, empty save kept in memory only (never a .sav file) */
	core->loadSave(core, VFileMemChunk(NULL, 0));
	core->reset(core);
	blip_set_rates(core->getAudioChannel(core, 0), core->frequency(core), AUDIO_RATE);
	blip_set_rates(core->getAudioChannel(core, 1), core->frequency(core), AUDIO_RATE);
	r->core = core;
	r->first_diff = -1;
	r->first_ram_diff = -1;
}

static void hash_frame(struct Rom* r) {
	struct mCore* core = r->core;
	struct blip_t* left = core->getAudioChannel(core, 0);
	struct blip_t* right = core->getAudioChannel(core, 1);
	int n;
	r->vhash = fnv(FNV_INIT, r->video, WIDTH * HEIGHT * sizeof(color_t));
	r->ahash = FNV_INIT;
	while ((n = blip_samples_avail(left)) > 0) {
		if (n > (int) (sizeof(audio_scratch) / sizeof(*audio_scratch) / 2)) {
			n = sizeof(audio_scratch) / sizeof(*audio_scratch) / 2;
		}
		blip_read_samples(left, audio_scratch, n, 1);
		blip_read_samples(right, audio_scratch + 1, n, 1);
		r->ahash = fnv(r->ahash, audio_scratch, n * 2 * sizeof(int16_t));
	}
}

static void run_frame(struct Rom* r, uint32_t keys) {
	r->core->setKeys(r->core, keys);
	r->core->runFrame(r->core);
	hash_frame(r);
}

/* --coverage: the reference runs one instruction at a time, and every ROM
 * or IWRAM halfword an instruction starts at (both halfwords of an ARM one)
 * is set in a bitmap; instructions run elsewhere (the BIOS, EWRAM) are only
 * counted.  IWRAM holds the code AgbInit and the sound driver copy there. */
#define IWRAM_SIZE 0x8000

static uint8_t* cov;
static uint8_t cov_iwram[IWRAM_SIZE / 16];
static uint32_t cov_size;    /* ROM bytes covered by the bitmap */
static uint64_t cov_rom, cov_iw, cov_other;

static void cov_set(uint8_t* map, uint32_t off, int thumb) {
	map[off >> 4] |= 1u << ((off >> 1) & 7);
	if (!thumb) {
		off += 2;
		map[off >> 4] |= 1u << ((off >> 1) & 7);
	}
}

static void run_frame_stepped(struct Rom* r, uint32_t keys) {
	struct mCore* core = r->core;
	struct ARMCore* cpu = core->cpu;
	uint32_t start = core->frameCounter(core);
	core->setKeys(core, keys);
	while (core->frameCounter(core) == start) {
		int thumb = cpu->executionMode == MODE_THUMB;
		uint32_t pc = (uint32_t) cpu->gprs[ARM_PC] - (thumb ? 2 : 4);
		if (!cpu->halted) {
			if (pc - 0x08000000u < cov_size) {
				cov_set(cov, pc - 0x08000000u, thumb);
				++cov_rom;
			} else if (pc - 0x03000000u < IWRAM_SIZE) {
				cov_set(cov_iwram, pc - 0x03000000u, thumb);
				++cov_iw;
			} else {
				++cov_other;
			}
		}
		core->step(core);
	}
	hash_frame(r);
}

/* One "<start> <end>" line (hex VMAs, end exclusive) per run of set
 * halfwords of `map` (`size` bytes from `base`); returns the bytes. */
static uint64_t write_ranges(FILE* f, const uint8_t* map, uint32_t size, uint32_t base,
                             long* runs) {
	uint64_t bytes = 0;
	for (uint32_t h = 0; h < size / 2;) {
		uint32_t e;
		if (!(map[h >> 3] & (1u << (h & 7)))) {
			++h;
			continue;
		}
		for (e = h; e < size / 2 && (map[e >> 3] & (1u << (e & 7))); ++e) {
		}
		fprintf(f, "0x%08X 0x%08X\n", base + h * 2, base + e * 2);
		bytes += (e - h) * 2;
		++*runs;
		h = e;
	}
	return bytes;
}

static void write_coverage(const char* path) {
	FILE* f = fopen(path, "w");
	uint64_t bytes, iw;
	long runs = 0;
	if (!f) {
		fprintf(stderr, "%s: %s\n", path, strerror(errno));
		exit(2);
	}
	fprintf(f, "# knidl-boottest --coverage: executed ranges (start end, end exclusive);"
	        " %llu instructions in ROM, %llu in IWRAM, %llu elsewhere\n",
	        (unsigned long long) cov_rom, (unsigned long long) cov_iw,
	        (unsigned long long) cov_other);
	bytes = write_ranges(f, cov, cov_size, 0x08000000u, &runs);
	iw = write_ranges(f, cov_iwram, IWRAM_SIZE, 0x03000000u, &runs);
	fclose(f);
	printf("coverage: %llu ROM and %llu IWRAM bytes executed in %ld range(s), "
	       "written to %s\n", (unsigned long long) bytes, (unsigned long long) iw,
	       runs, path);
}

/* EWRAM + IWRAM of a test ROM against the reference, up to relocation.
 * Returns the number of differing words; the first eight go to r->ram_*. */
static long ram_compare(struct Rom* ref, struct Rom* r) {
	static const int regions[] = { REGION_EWRAM, REGION_IWRAM };
	static const uint32_t bases[] = { 0x02000000, 0x03000000 };
	long n = 0;
	for (int k = 0; k < 2; ++k) {
		size_t sa, sb;
		const uint32_t* a = ref->core->getMemoryBlock(ref->core, regions[k], &sa);
		const uint32_t* b = r->core->getMemoryBlock(r->core, regions[k], &sb);
		if (!a || !b || sa != sb) {
			continue;
		}
		if (memcmp(a, b, sa) == 0) {
			continue;
		}
		for (size_t i = 0; i < sa / 4; ++i) {
			uint32_t va = a[i], vb = b[i];
			if (va == vb) {
				continue;
			}
			/* a pointer (bit 0 = Thumb) into the moved part, moved */
			if (same_value(r, va, vb)) {
				continue;
			}
			if (nallow) {
				uint32_t addr = bases[k] + i * 4;
				int j;
				for (j = 0; j < nallow && ram_allow[j] != addr; ++j) {
				}
				if (j < nallow) {
					++r->allowed;
					continue;
				}
			}
			if (n < 8) {
				r->ram_addr[n] = bases[k] + i * 4;
				r->ram_ref[n] = va;
				r->ram_test[n] = vb;
			}
			++n;
		}
	}
	return n;
}

static void read_regs(struct Rom* r, uint32_t* regs) {
	char name[8];
	for (int i = 0; i < 16; ++i) {
		snprintf(name, sizeof(name), "r%d", i);
		r->core->readRegister(r->core, name, &regs[i]);
	}
	r->core->readRegister(r->core, "cpsr", &regs[16]);
}

/* --step: run frame `f` of the reference and ONE test ROM one instruction at
 * a time and report the first instruction after which their registers differ
 * beyond relocation, with the instructions before it.  (Diagnostics only.) */
static void step_compare(struct Rom* ref, struct Rom* r, uint32_t keys, long f) {
	enum { RING = 48 };
	uint32_t ring_a[RING], ring_b[RING];
	uint32_t ra[17], rb[17];
	uint32_t start = ref->core->frameCounter(ref->core);
	long n;
	ref->core->setKeys(ref->core, keys);
	r->core->setKeys(r->core, keys);
	for (n = 0; n < 4000000; ++n) {
		int bad = -1;
		read_regs(ref, ra);
		read_regs(r, rb);
		ring_a[n % RING] = ra[15];
		ring_b[n % RING] = rb[15];
		for (int i = 0; i < 17; ++i) {
			if (!same_value(r, ra[i], rb[i])) {
				bad = i;
				break;
			}
		}
		if (bad >= 0) {
			long k0 = n >= RING ? n - RING + 1 : 0;
			printf("%s: frame %ld, instruction %ld: r%d differs (ref 0x%08X, test 0x%08X)\n",
			       r->path, f, n, bad, ra[bad], rb[bad]);
			printf("  last pcs (prefetch pc; ref / test):\n");
			for (long k = k0; k <= n; ++k) {
				printf("    %7ld  0x%08X  0x%08X\n", k, ring_a[k % RING], ring_b[k % RING]);
			}
			printf("  registers ref / test:\n");
			for (int i = 0; i < 17; ++i) {
				printf("    %-4s 0x%08X  0x%08X%s\n", REG_NAMES[i],
				       ra[i], rb[i], same_value(r, ra[i], rb[i]) ? "" : "  <--");
			}
			return;
		}
		if (ref->core->frameCounter(ref->core) != start) {
			printf("%s: frame %ld: %ld instructions, registers equal up to relocation\n",
			       r->path, f, n);
			return;
		}
		ref->core->step(ref->core);
		r->core->step(r->core);
	}
	printf("%s: frame %ld: no frame end after %ld instructions\n", r->path, f, n);
}

static uint32_t read_pc(struct Rom* r) {
	uint32_t pc = 0;
	r->core->readRegister(r->core, "pc", &pc);
	return pc;
}

/* ---------------------------------------------------------------- threads */

/* Every frame runs in two phases on `jobs` threads (core i on thread
 * i % jobs): all cores run the frame, then each test ROM's RAM is compared
 * with the reference's; the main thread (thread 0) then checks the hashes. */
static int jobs = 0;  /* 0: the online CPUs */
static int use_ram = 1;
static uint32_t frame_keys;
static volatile int stop_threads;
static pthread_barrier_t barrier;

static void phase_run(int t) {
	for (int i = t; i < nroms; i += jobs) {
		if (i > 0 && roms[i].done) {
			continue;
		}
		current_rom = i;
		if (i == 0 && cov) {
			run_frame_stepped(&roms[i], frame_keys);
		} else {
			run_frame(&roms[i], frame_keys);
		}
	}
	current_rom = -1;
}

static void phase_ram(int t) {
	for (int i = t; i < nroms; i += jobs) {
		roms[i].ram_n = 0;
		if (i > 0 && !roms[i].done && use_ram) {
			roms[i].ram_n = ram_compare(&roms[0], &roms[i]);
		}
	}
}

static void* worker(void* arg) {
	int t = (int) (intptr_t) arg;
	for (;;) {
		pthread_barrier_wait(&barrier);
		if (stop_threads) {
			return NULL;
		}
		phase_run(t);
		pthread_barrier_wait(&barrier);
		phase_ram(t);
		pthread_barrier_wait(&barrier);
	}
}

/* ---------------------------------------------------------------- main */

static void usage(void) {
	fprintf(stderr,
	    "usage: knidl-boottest [options] REF.gba [TEST.gba@POINT+PAD ...]\n"
	    "  --frames N       frames to run (default: the script's length + 600)\n"
	    "  --input FILE     input script (\"<frames> <keys>\" per line, with\n"
	    "                   mark/expect checkpoints; see load_script)\n"
	    "  --syms FILE      nm output: symbol names for expect and --peek\n"
	    "  --no-ram         compare video and audio only\n"
	    "  --ram-allow A[,A...]  RAM words (addresses) whose difference is\n"
	    "                   explained and not a pointer (counted, not failed)\n"
	    "  --trace          print the reference's per-frame hashes (stdout)\n"
	    "  --shot F[,F...]  write PNGs of the reference at these frames\n"
	    "  --shot-every N   ... and every N frames\n"
	    "  --shot-dir DIR   where the PNGs go (default: .)\n"
	    "  --peek C[,C...]  print these RAM cells of the reference (SYM[+off]:size\n"
	    "                   or 0xADDR:size) at the --shot frames and the marks\n"
	    "  --peek-every N   ... and every N frames\n"
	    "  --coverage FILE  run the reference alone an instruction at a time and\n"
	    "                   write the ROM ranges it executed to FILE\n"
	    "  --step F         diagnostics: run frame F of the reference and the one\n"
	    "                   test ROM an instruction at a time, report where their\n"
	    "                   registers first differ beyond relocation, and stop\n"
	    "  --jobs N         threads (default: the CPUs, at most one per ROM)\n"
	    "  --quiet-log      do not print the emulator's game errors\n"
	    "TEST.gba@POINT+PAD: a test ROM with PAD bytes inserted at VMA POINT;\n"
	    "RAM words pointing at or after POINT may differ by exactly PAD.\n");
	exit(2);
}

int main(int argc, char** argv) {
	long frames = -1;
	const char* input = NULL;
	const char* shot_dir = ".";
	char* shots = NULL;
	long shot_every = 0;
	int trace = 0;
	long step_frame = -1;
	const char* syms_path = NULL;
	char* peek_list = NULL;
	long peek_every = 0;
	struct Cell peeks[32];
	int npeeks = 0;
	const char* cov_path = NULL;
	pthread_t threads[MAX_ROMS];
	int status = 0;
	long distinct = 0;
	uint64_t last_vhash = 0;

	for (int i = 1; i < argc; ++i) {
		const char* a = argv[i];
		if (strcmp(a, "--frames") == 0 && i + 1 < argc) {
			frames = strtol(argv[++i], NULL, 0);
		} else if (strcmp(a, "--input") == 0 && i + 1 < argc) {
			input = argv[++i];
		} else if (strcmp(a, "--syms") == 0 && i + 1 < argc) {
			syms_path = argv[++i];
		} else if (strcmp(a, "--peek") == 0 && i + 1 < argc) {
			peek_list = argv[++i];
		} else if (strcmp(a, "--peek-every") == 0 && i + 1 < argc) {
			peek_every = strtol(argv[++i], NULL, 0);
		} else if (strcmp(a, "--coverage") == 0 && i + 1 < argc) {
			cov_path = argv[++i];
		} else if (strcmp(a, "--no-ram") == 0) {
			use_ram = 0;
		} else if (strcmp(a, "--trace") == 0) {
			trace = 1;
		} else if (strcmp(a, "--shot") == 0 && i + 1 < argc) {
			shots = argv[++i];
		} else if (strcmp(a, "--shot-every") == 0 && i + 1 < argc) {
			shot_every = strtol(argv[++i], NULL, 0);
		} else if (strcmp(a, "--shot-dir") == 0 && i + 1 < argc) {
			shot_dir = argv[++i];
		} else if (strcmp(a, "--ram-allow") == 0 && i + 1 < argc) {
			char* list = strdup(argv[++i]);
			for (char* tok = strtok(list, ","); tok; tok = strtok(NULL, ",")) {
				if (nallow == (int) (sizeof(ram_allow) / sizeof(*ram_allow))) {
					usage();
				}
				ram_allow[nallow++] = strtoul(tok, NULL, 0) & ~3u;
			}
		} else if (strcmp(a, "--step") == 0 && i + 1 < argc) {
			step_frame = strtol(argv[++i], NULL, 0);
		} else if (strcmp(a, "--jobs") == 0 && i + 1 < argc) {
			jobs = strtol(argv[++i], NULL, 0);
		} else if (strcmp(a, "--quiet-log") == 0) {
			quiet_log = 1;
		} else if (a[0] == '-') {
			usage();
		} else {
			char* spec;
			char* at;
			if (nroms == MAX_ROMS) {
				fprintf(stderr, "at most %d ROMs\n", MAX_ROMS);
				exit(2);
			}
			spec = strdup(a);
			at = strchr(spec, '@');
			if (at) {
				char* plus = strchr(at, '+');
				*at = '\0';
				if (!plus) {
					usage();
				}
				roms[nroms].point = strtoul(at + 1, NULL, 0);
				roms[nroms].pad = strtoul(plus + 1, NULL, 0);
			}
			roms[nroms].path = spec;
			++nroms;
		}
	}
	if (nroms < 1) {
		usage();
	}
	if (syms_path) {
		load_syms(syms_path);
	}
	if (input) {
		load_script(input);
	}
	if (peek_list) {
		char* list = strdup(peek_list);
		for (char* tok = strtok(list, ","); tok; tok = strtok(NULL, ",")) {
			if (npeeks == (int) (sizeof(peeks) / sizeof(*peeks)) || !parse_cell(tok, &peeks[npeeks])) {
				fprintf(stderr, "--peek: bad cell '%s' (<symbol|0xADDR>[+off]:<1|2|4>; "
				        "symbols need --syms)\n", tok);
				exit(2);
			}
			++npeeks;
		}
	}
	if (cov_path && nroms != 1) {
		fprintf(stderr, "--coverage runs the reference alone (no test ROMs)\n");
		exit(2);
	}
	if (frames < 0) {
		frames = script_frames + 600;
	}
	crc_init();
	mLogSetDefaultLogger(&logger);
	if (jobs <= 0) {
		jobs = (int) sysconf(_SC_NPROCESSORS_ONLN);
		if (jobs <= 0) {
			jobs = 1;
		}
	}

	for (int i = 0; i < nroms; ++i) {
		open_rom(&roms[i]);
	}
	for (int i = 1; i < nroms; ++i) {
		roms[i].rom_end = 0x08000000u + (uint32_t) roms[0].core->romSize(roms[0].core);
		if (roms[i].pad == 0) {
			fprintf(stderr, "%s: no @POINT+PAD given; RAM compared exactly\n",
			        roms[i].path);
		}
	}

	if (step_frame >= 0) {
		jobs = 1;
	}
	if (cov_path) {
		cov_size = (uint32_t) roms[0].core->romSize(roms[0].core);
		cov = calloc(cov_size / 16 + 1, 1);
	}
	if (jobs > nroms) {
		jobs = nroms;
	}
	printf("boot test: %ld frames (%.1f min of game time), script %s (%ld frames), "
	       "%d test ROM(s), %d thread(s)\n", frames, frames / 59.7275 / 60,
	       input ? input : "(none)", script_frames, nroms - 1, jobs);
	fflush(stdout);
	pthread_barrier_init(&barrier, NULL, jobs);
	for (int t = 1; t < jobs; ++t) {
		pthread_create(&threads[t], NULL, worker, (void*) (intptr_t) t);
	}

	long ran = 0;
	for (long f = 0; f < frames; ++f) {
		int line;
		uint32_t keys = keys_at(f, &line);
		int alive = 0;
		const char* scene;
		current_frame = f;
		check_marks(roms[0].core, f, 0);
		scene = scene_at(f);
		if (f == step_frame) {
			if (nroms != 2) {
				fprintf(stderr, "--step needs exactly one test ROM\n");
				exit(2);
			}
			step_compare(&roms[0], &roms[1], keys, f);
			return 1;
		}
		frame_keys = keys;
		if (jobs > 1) {
			pthread_barrier_wait(&barrier);
		}
		phase_run(0);
		if (jobs > 1) {
			pthread_barrier_wait(&barrier);
		}
		phase_ram(0);
		if (jobs > 1) {
			pthread_barrier_wait(&barrier);
		}
		ran = f + 1;
		if (roms[0].vhash != last_vhash) {
			++distinct;
			last_vhash = roms[0].vhash;
		}
		if (trace) {
			printf("%ld %016llx %016llx %03x\n", f, (unsigned long long) roms[0].vhash,
			       (unsigned long long) roms[0].ahash, keys);
		}
		if (shots || shot_every > 0) {
			int want = shot_every > 0 && f % shot_every == 0;
			if (shots) {
				char list[1024], key[32];
				snprintf(list, sizeof(list), ",%s,", shots);
				snprintf(key, sizeof(key), ",%ld,", f);
				if (strstr(list, key)) {
					want = 1;
				}
			}
			if (want) {
				char path[1024];
				snprintf(path, sizeof(path), "%s/f%06ld.png", shot_dir, f);
				write_png(path, roms[0].video);
			}
		}
		if (npeeks) {
			int want = peek_every > 0 && f % peek_every == 0;
			if (shots) {
				char list[1024], key[32];
				snprintf(list, sizeof(list), ",%s,", shots);
				snprintf(key, sizeof(key), ",%ld,", f);
				want |= strstr(list, key) != NULL;
			}
			want |= shot_every > 0 && f % shot_every == 0;
			if (want) {
				printf("peek %6ld:", f);
				for (int k = 0; k < npeeks; ++k) {
					uint32_t v = read_cell(roms[0].core, &peeks[k]);
					int32_t sv = peeks[k].size == 1 ? (int8_t) v
					    : peeks[k].size == 2 ? (int16_t) v : (int32_t) v;
					printf(" %s=%d", peeks[k].spec, sv);
					if (peeks[k].size == 4 || sv < 0) {
						printf("(0x%X)", v);
					}
				}
				printf("\n");
			}
		}
		for (int i = 1; i < nroms; ++i) {
			struct Rom* r = &roms[i];
			if (r->done) {
				continue;
			}
			if (use_ram) {
				long n = r->ram_n;
				if (n) {
					++r->ram_frames;
					r->ram_last = f;
					if (n > r->ram_max) {
						r->ram_max = n;
					}
				}
				if (n && r->first_ram_diff < 0) {
					r->first_ram_diff = f;
					printf("%s: RAM differs beyond relocation at frame %ld "
					       "(script line %d%s%s): %ld word(s); pc ref 0x%08X test 0x%08X\n",
					       r->path, f, line, scene ? ", scene: " : "", scene ? scene : "",
					       n, read_pc(&roms[0]), read_pc(r));
					for (long k = 0; k < n && k < 8; ++k) {
						printf("  0x%08X: ref 0x%08X test 0x%08X\n", r->ram_addr[k],
						       r->ram_ref[k], r->ram_test[k]);
					}
				}
			}
			if (r->vhash != roms[0].vhash || r->ahash != roms[0].ahash) {
				r->first_diff = f;
				r->what = r->vhash != roms[0].vhash
				    ? (r->ahash != roms[0].ahash ? "video+audio" : "video")
				    : "audio";
				r->done = 1;
				printf("%s: first difference at frame %ld (%s; script line %d%s%s); "
				       "pc ref 0x%08X test 0x%08X\n", r->path, f, r->what, line,
				       scene ? ", scene: " : "", scene ? scene : "",
				       read_pc(&roms[0]), read_pc(r));
				continue;
			}
			++alive;
		}
		fflush(stdout);
		if (nroms > 1 && !alive && !trace && !shots && shot_every <= 0) {
			break;
		}
	}
	check_marks(roms[0].core, ran, 1);
	stop_threads = 1;
	if (jobs > 1) {
		pthread_barrier_wait(&barrier);
	}
	for (int t = 1; t < jobs; ++t) {
		pthread_join(threads[t], NULL);
	}

	printf("\n%-48s %-10s %-24s %s\n", "rom", "result", "first difference", "game errors");
	printf("%-48s %-10s %-24s %ld\n", roms[0].path, "reference", "-", game_errors[0]);
	for (int i = 1; i < nroms; ++i) {
		struct Rom* r = &roms[i];
		char diff[64];
		int bad = r->first_diff >= 0 || (use_ram && r->first_ram_diff >= 0)
		    || game_errors[i] != game_errors[0];
		if (r->first_diff >= 0) {
			snprintf(diff, sizeof(diff), "frame %ld (%s)", r->first_diff, r->what);
		} else if (use_ram && r->first_ram_diff >= 0) {
			snprintf(diff, sizeof(diff), "frame %ld (RAM only)", r->first_ram_diff);
		} else {
			snprintf(diff, sizeof(diff), "none");
		}
		printf("%-48s %-10s %-24s %ld\n", r->path, bad ? "DIFFERS" : "ok", diff,
		       game_errors[i]);
		if (r->ram_frames) {
			printf("%-48s RAM differed in %ld frame(s), last %ld, at most %ld word(s)\n",
			       "", r->ram_frames, r->ram_last, r->ram_max);
		}
		if (r->allowed) {
			printf("%-48s allowed RAM differences (--ram-allow): %ld word-frame(s)\n",
			       "", r->allowed);
		}
		if (game_errors[i] != game_errors[0] && !quiet_log) {
			for (int k = 0; k < 3 && first_errors[i][k][0]; ++k) {
				printf("%-48s emulator error: %s\n", "", first_errors[i][k]);
			}
		}
		if (bad) {
			status = 1;
		}
	}
	printf("\nreference: %ld frames, %ld distinct consecutive video frames\n",
	       ran, distinct);
	if (expects_passed || expects_failed) {
		printf("script checks: %ld of %ld expectation(s) held on the reference\n",
		       expects_passed, expects_passed + expects_failed);
	}
	if (expects_failed) {
		printf("SCRIPT DRIFT: the reference no longer reaches what the script "
		       "expects (see above)\n");
		status = 1;
	}
	if (cov) {
		write_coverage(cov_path);
	}
	for (int i = 0; i < nroms; ++i) {
		roms[i].core->deinit(roms[i].core);
	}
	return status;
}
