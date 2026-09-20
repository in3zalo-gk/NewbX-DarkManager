#!/usr/bin/env python3
"""
Validates a built pack (run AFTER `build.sh pack`), then copies the .mcpack to dist/.

Checks:
  - every material in pack_config.toml has a non-empty <Name>.material.bin (default + each subpack)
  - manifest.json fields (name, uuid, version, min_engine_version, subpacks incl. "Dark Fantasy — Low")
  - every subpack folder listed in the manifest exists
  - the .mcpack is a valid zip with manifest.json at the archive root and no backslash paths
"""
import argparse, glob, json, os, shutil, sys, tomllib, zipfile

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
os.chdir(ROOT)

ap = argparse.ArgumentParser()
ap.add_argument("--profile", default="android")
ap.add_argument("--no-zip", action="store_true", help="only validate the pack folder")
args = ap.parse_args()

cfg = tomllib.load(open("src/newb/pack_config.toml", "rb"))
pack = os.path.join("build", "pack-" + args.profile)
errors = []

def err(m):
    errors.append(m)
    print("ERROR", m)

if not os.path.isdir(pack):
    print(f"ERROR pack folder not found: {pack}")
    sys.exit(1)

def check_mats(folder, mats, label):
    d = os.path.join(folder, "renderer", "materials")
    for m in mats:
        f = os.path.join(d, m + ".material.bin")
        if not os.path.isfile(f):
            err(f"[{label}] missing {m}.material.bin")
        elif os.path.getsize(f) < 64:
            err(f"[{label}] {m}.material.bin is suspiciously small ({os.path.getsize(f)} bytes)")
    print(f"  [{label}] {len(mats)} material(s) checked")

check_mats(pack, cfg["materials"], "default")
for sp in cfg.get("subpack", []):
    if sp["materials"]:
        check_mats(os.path.join(pack, "subpacks", sp["define"].lower()), sp["materials"], sp["define"].lower())


# assets that must be inside the pack
for rel in ("pack_icon.png", "biomes_client.json", os.path.join("textures", "blocks", "torch_on.png"),
            os.path.join("fogs", "dm_swamp_fog_setting.json")):
    if not os.path.isfile(os.path.join(pack, rel)):
        err(f"asset missing in pack: {rel}")

mf_path = os.path.join(pack, "manifest.json")
if not os.path.isfile(mf_path):
    err("manifest.json missing")
else:
    mf = json.load(open(mf_path, encoding="utf-8"))
    h = mf.get("header", {})
    if h.get("name") != cfg["name"]: err(f"manifest name {h.get('name')!r} != {cfg['name']!r}")
    if h.get("uuid") != cfg["uuid"]: err("manifest uuid differs from pack_config.toml")
    if h.get("min_engine_version") != cfg["min_supported_mc_version"]:
        err(f"min_engine_version {h.get('min_engine_version')} != {cfg['min_supported_mc_version']}")
    names = [s.get("name") for s in mf.get("subpacks", [])]
    if not any("Dark Fantasy" in (n or "") and "Low" in (n or "") for n in names):
        err("subpack 'Dark Fantasy — Low' not present in manifest")
    for s in mf.get("subpacks", []):
        if not os.path.isdir(os.path.join(pack, "subpacks", s["folder_name"])):
            err(f"subpack folder missing: {s['folder_name']}")
    print(f"  manifest ok: {h.get('name')} v{'.'.join(map(str, h.get('version', [])))} "
          f"min_engine {h.get('min_engine_version')} subpacks={len(names)}")

if not args.no_zip:
    cands = sorted(glob.glob(os.path.join("build", "*-" + args.profile + ".mcpack")))
    if not cands:
        err("no .mcpack found in build/ (did you build without --no-zip?)")
    else:
        z = cands[-1]
        with zipfile.ZipFile(z) as zf:
            bad = zf.testzip()
            if bad: err(f"corrupt entry in mcpack: {bad}")
            n = zf.namelist()
            if "manifest.json" not in n: err("manifest.json is not at the archive root")
            if any("\\" in x for x in n): err("backslash path inside archive")
            bins = [x for x in n if x.endswith(".material.bin")]
            print(f"  mcpack ok: {z} ({len(n)} files, {len(bins)} material.bin)")
        if not errors:
            os.makedirs("dist", exist_ok=True)
            out = os.path.join("dist", f"Newb-X-Dark-Manager-{args.profile}.mcpack")
            shutil.copyfile(z, out)
            print("  copied ->", out)

print(f"\nvalidation: {len(errors)} error(s)")
sys.exit(1 if errors else 0)
