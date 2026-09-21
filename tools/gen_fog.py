#!/usr/bin/env python3
"""
Generates per-biome fog settings (assets/fogs/dm_*.json) and links them in assets/biomes_client.json.

How it works with the shader:
  - air fog_start  -> becomes FOG_CONTROL.x  -> lower = denser fog in that biome (fog_end stays 1.0)
  - air fog_color  -> becomes FOG_COLOR      -> its hue tints sky/fog (NL_BIOME_TINT in config.h)
  - water block    -> underwater fog color/distance per biome
  - weather block  -> kept at 0.23/0.70 (the values the shader uses to detect rain/snow)
Safety rules (the shader detects Nether/End/underwater from these values):
  - air fog_start must stay OUT of 0.10-0.14 (Nether window) -> we use 0.18-0.55
  - air fog_end = 1.0; water fog_start = 0.0 and water color must have blue or green > red
  - air color must not have red == blue (that is how the End is detected)
Nether biomes are NOT touched (vanilla fog keeps the Nether detection intact).
Edit the GROUPS table and rerun:  python3 tools/gen_fog.py
"""
import json, os, sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
FOG_DIR = os.path.join(ROOT, "assets", "fogs")
BIOMES_JSON = os.path.join(ROOT, "assets", "biomes_client.json")

# name: (air_start, air_color, water_end_blocks, water_color, [biome ids])
# air_start LOWER = denser fog (safe range ~0.18-0.55, avoid 0.10-0.15 Nether)
# air_color tints sky/fog via NL_BIOME_TINT (must NOT have red==blue or End detection breaks)
# Colors matched to reference screenshots (dense atmospheric fog)
GROUPS = {
  # Plains: thick gray mist (ref: white-gray dense fog over trees)
  "plains":      (0.20, "#9a9c98", 12, "#1f4d80", ["plains", "sunflower_plains"]),
  # Meadow: soft cool gray
  "meadow":      (0.22, "#a0a8b0", 12, "#2a5a86", ["meadow"]),
  # Sakura / Cherry: pink-purple haze (ref: purple cherry grove)
  "sakura":      (0.18, "#b87898", 10, "#5a3a70", ["cherry_grove"]),
  # Forest: heavy gray-green, trees fade into fog
  "forest":      (0.18, "#6a7a70", 10, "#1f5a5c", ["forest", "forest_hills", "flower_forest", "birch_forest", "birch_forest_hills", "birch_forest_mutated", "birch_forest_hills_mutated"]),
  # Dark forest: almost black-green, very dense
  "dark_forest": (0.18, "#2e3a32",  8, "#1c4a48", ["roofed_forest", "roofed_forest_mutated"]),
  # Swamp: murky olive, thick
  "swamp":       (0.18, "#4a5a42",  8, "#3a5d3a", ["swampland", "swampland_mutated"]),
  # Mangrove: gray-green over water (ref: mangrove island shot)
  "mangrove":    (0.18, "#5a6a58",  8, "#3a6040", ["mangrove_swamp"]),
  # Jungle: humid dark teal, dense
  "jungle":      (0.18, "#2a6a58", 10, "#1d6a66", ["jungle", "jungle_hills", "jungle_mutated", "jungle_edge", "jungle_edge_mutated", "bamboo_jungle", "bamboo_jungle_hills"]),
  # Taiga: cool blue-gray
  "taiga":       (0.20, "#708898", 12, "#1f4f7a", ["taiga", "taiga_hills", "taiga_mutated", "mega_taiga", "mega_taiga_hills", "redwood_taiga_mutated", "redwood_taiga_hills_mutated"]),
  # Cold taiga: colder blue-gray
  "cold_taiga":  (0.18, "#8098a8", 12, "#2a5578", ["cold_taiga", "cold_taiga_hills", "cold_taiga_mutated"]),
  # Snow: pale icy, strong
  "snow":        (0.18, "#b0c4d8", 10, "#2f5f8f", ["ice_plains", "ice_plains_spikes", "ice_mountains", "frozen_river", "cold_beach"]),
  # Snow peaks: colder, denser
  "snow_peaks":  (0.18, "#98b0c8", 10, "#2f5f8f", ["snowy_slopes", "grove", "frozen_peaks", "jagged_peaks"]),
  # Desert: warm dusty orange (ref: orange sunset fog)
  "desert":      (0.22, "#c89860", 14, "#2a6a78", ["desert", "desert_hills", "desert_mutated"]),
  # Mesa / Badlands: rusty orange-brown dust
  "mesa":        (0.20, "#b88050", 14, "#2a6472", ["mesa", "mesa_bryce", "mesa_plateau", "mesa_plateau_mutated", "mesa_plateau_stone", "mesa_plateau_stone_mutated"]),
  # Savanna: dry yellow-brown
  "savanna":     (0.22, "#b89060", 14, "#2a6474", ["savanna", "savanna_mutated", "savanna_plateau", "savanna_plateau_mutated"]),
  # Mountains: cool blue-gray
  "mountains":   (0.20, "#6880a0", 12, "#1f4f78", ["extreme_hills", "extreme_hills_edge", "extreme_hills_mutated", "extreme_hills_plus_trees", "extreme_hills_plus_trees_mutated", "stony_peaks", "stone_beach"]),
  # Ocean: deep blue-gray
  "ocean":       (0.22, "#5080a8", 16, "#1f4d80", ["ocean", "deep_ocean"]),
  "cold_ocean":  (0.20, "#6088a8", 14, "#234d7a", ["cold_ocean", "deep_cold_ocean"]),
  "warm_ocean":  (0.22, "#50a0a8", 16, "#1f7a86", ["lukewarm_ocean", "deep_lukewarm_ocean", "warm_ocean", "deep_warm_ocean"]),
  "frozen_ocean":(0.18, "#90b0c8", 10, "#2a5a88", ["frozen_ocean", "deep_frozen_ocean", "legacy_frozen_ocean"]),
  # River / Beach
  "river":       (0.22, "#6088a0", 12, "#1f5580", ["river"]),
  "beach":       (0.24, "#98a8b0", 14, "#2a6a88", ["beach"]),
  # Mushroom: purple-gray mystical
  "mushroom":    (0.18, "#8070a0", 10, "#4a3a78", ["mushroom_island", "mushroom_island_shore"]),
  # Pale Garden: ash gray, very dense
  "pale_garden": (0.18, "#808890",  8, "#3a4a55", ["pale_garden"]),
  # Deep Dark: near black (ref: dark cave fog)
  "deep_dark":   (0.18, "#12181c",  6, "#10303a", ["deep_dark"]),
  # Caves
  "dripstone":   (0.18, "#5a5048", 10, "#1f4a6a", ["dripstone_caves"]),
  "lush_caves":  (0.18, "#2a5a40", 10, "#1d6a5a", ["lush_caves"]),
}
WEATHER_COLOR = "#6b7480"

def rgb(h):
    h = h.lstrip("#"); return tuple(int(h[i:i+2], 16) for i in (0, 2, 4))

def check(name, start, air, wend, water):
    r, g, b = rgb(air)
    if r == b: sys.exit(f"{name}: air color red==blue would be detected as End")
    if 0.09 <= start <= 0.15: sys.exit(f"{name}: air start {start} is inside the Nether detection window")
    wr, wg, wb = rgb(water)
    if not (wb > wr or wg > wr): sys.exit(f"{name}: water color must have blue or green > red (underwater detection)")
    if wend > 24: sys.exit(f"{name}: water end too far (underwater detection needs end/renderDistance < 0.8)")

os.makedirs(FOG_DIR, exist_ok=True)
for f in os.listdir(FOG_DIR):
    if f.startswith("dm_"):
        os.remove(os.path.join(FOG_DIR, f))

biomes = json.load(open(BIOMES_JSON, encoding="utf-8"))["biomes"]
seen = {}
for name, (start, air, wend, water, ids) in GROUPS.items():
    check(name, start, air, wend, water)
    ident = f"newb:dm_{name}"
    data = {
      "format_version": "1.16.100",
      "minecraft:fog_settings": {
        "description": {"identifier": ident},
        "distance": {
          "air":     {"fog_start": start, "fog_end": 1.0, "fog_color": air, "render_distance_type": "render"},
          "water":   {"fog_start": 0.0, "fog_end": float(wend), "fog_color": water, "render_distance_type": "fixed"},
          "weather": {"fog_start": 0.23, "fog_end": 0.7, "fog_color": WEATHER_COLOR, "render_distance_type": "render"},
        },
      },
    }
    json.dump(data, open(os.path.join(FOG_DIR, f"dm_{name}_fog_setting.json"), "w", encoding="utf-8"), indent=2)
    for b in ids:
        if b in seen: sys.exit(f"biome {b} listed twice ({seen[b]} and {name})")
        seen[b] = name
        entry = biomes.setdefault(b, {})
        entry["fog_identifier"] = ident

json.dump({"biomes": biomes}, open(BIOMES_JSON, "w", encoding="utf-8"), indent=2)
print(f"{len(GROUPS)} fog settings, {len(seen)} biomes linked (Nether biomes untouched, the_end keeps newb:fog_the_end)")
