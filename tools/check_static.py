#!/usr/bin/env python3
"""
Static checks for the Newb X Dark Manager sources. No Lazurite / shaderc needed,
so it can run anywhere (Termux, CI, laptop). It does NOT replace a real build.

For every subpack define it:
  1. runs the C preprocessor over newb/main.sh (catches broken #include / unbalanced #if)
  2. reports any NL_* identifier that survives preprocessing (= macro used but never defined)
Then it checks project structure: every material listed in pack_config.toml has
vertex/fragment/varying/config files, and every #include <newb/...> in .sc files exists.
"""
import os, re, subprocess, sys, tomllib

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
os.chdir(ROOT)
errors, warnings = [], []

with open("src/newb/pack_config.toml", "rb") as f:
    cfg = tomllib.load(f)

# --- structure
for m in cfg["materials"]:
    for fn in ("vertex.sc", "fragment.sc", "varying.def.sc", "config.json"):
        if not os.path.isfile(f"src/materials/{m}/{fn}"):
            errors.append(f"missing src/materials/{m}/{fn}")
for sp in cfg.get("subpack", []):
    for m in sp["materials"]:
        if m not in cfg["materials"]:
            errors.append(f"subpack {sp['define']} lists unknown material {m}")

inc_re = re.compile(r'#\s*include\s*[<"]([^>"]+)[>"]')
for dp, _, fns in os.walk("src"):
    for fn in fns:
        if not fn.endswith((".sc", ".h", ".sh")):
            continue
        path = os.path.join(dp, fn)
        for i, line in enumerate(open(path, encoding="utf-8"), 1):
            mt = inc_re.search(line)
            if not mt:
                continue
            target = mt.group(1)
            cands = [os.path.join("src", target), os.path.join("include", target),
                     os.path.join(dp, target)]
            if target in ("bgfx_shader.sh",) or target.startswith("MinecraftRenderer"):
                cands.append(os.path.join("include", target))
            if not any(os.path.isfile(c) for c in cands):
                errors.append(f"{path}:{i}: include not found: {target}")

# --- preprocess every variant
variants = [None] + [sp["define"] for sp in cfg.get("subpack", []) if sp["define"] != "DEFAULT"]
probe = "#include \"newb/main.sh\"\n"
tok = re.compile(r"\bNL_[A-Z0-9_]+\b")
for v in variants:
    cmd = ["cpp", "-P", "-x", "c", "-I", "src", "-I", "include"]
    if v:
        cmd.append(f"-D{v}")
    cmd.append("-")
    r = subprocess.run(cmd, input=probe, capture_output=True, text=True)
    label = v or "default"
    if r.returncode != 0:
        errors.append(f"[{label}] preprocessor failed:\n{r.stderr.strip()}")
        continue
    left = sorted(set(tok.findall(r.stdout)))
    if left:
        errors.append(f"[{label}] undefined NL_ macros used in live code: {', '.join(left)}")
    print(f"  preprocess [{label}] ok")

# --- material sources use NL_ macros? (per-material preprocess needs bgfx headers, so grep only)
cfgtxt = open("src/newb/config.h", encoding="utf-8").read()
defined = set(re.findall(r"#\s*define\s+(NL_[A-Z0-9_]+)", cfgtxt))
for dp, _, fns in os.walk("src"):
    for fn in fns:
        if fn.endswith((".sc", ".h")):
            txt = open(os.path.join(dp, fn), encoding="utf-8").read()
            for m in set(tok.findall(txt)):
                if m not in defined and not re.search(r"#\s*define\s+" + m + r"\b", txt):
                    warnings.append(f"{dp}/{fn}: {m} is not defined in config.h (ok only if intentionally optional)")


# --- assets: fog files, biome links, textures
import json, glob
fog_ids = set()
for f in glob.glob("assets/fogs/*.json"):
    try:
        d = json.load(open(f, encoding="utf-8"))
        fog_ids.add(d["minecraft:fog_settings"]["description"]["identifier"])
    except Exception as e:
        errors.append(f"{f}: invalid fog json ({e})")
try:
    bj = json.load(open("assets/biomes_client.json", encoding="utf-8"))["biomes"]
    for b, v in bj.items():
        fid = v.get("fog_identifier")
        if fid and fid not in fog_ids:
            errors.append(f"biomes_client.json: biome {b} uses unknown fog {fid}")
    print(f"  assets ok: {len(fog_ids)} fog settings, {sum(1 for v in bj.values() if 'fog_identifier' in v)} biomes with fog")
except Exception as e:
    errors.append(f"biomes_client.json invalid ({e})")
for t in ("torch_on", "soul_torch", "redstone_torch_on", "redstone_lamp_on"):
    if not os.path.isfile(f"assets/textures/blocks/{t}.png"):
        errors.append(f"missing texture {t}.png")
if not os.path.isfile("assets/pack_icon.png"):
    errors.append("missing assets/pack_icon.png")

for w in warnings:
    print("WARN ", w)
for e in errors:
    print("ERROR", e)
print(f"\nstatic check: {len(errors)} error(s), {len(warnings)} warning(s)")
sys.exit(1 if errors else 0)
