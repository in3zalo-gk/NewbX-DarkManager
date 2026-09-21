#ifndef NL_CONFIG_H
#define NL_CONFIG_H

/*
  NEWB SHADER MAIN CONFIG
  This part contains base configuration options for the shader.

  TOGGLES
  - If [toggle] is mentioned, then
    options can be commented to disable (by adding '//')
  - eg: #define PLANTS_WAVE    -> this is ON
        //#define PLANTS_WAVE  -> this is OFF

  COLOR VALUES
  - Color format: vec3(red,green,blue)
  - 1.0 means 100%, 0.0 means 0%
  - eg: vec3(1.0,1.0,0.0) = yellow

  VALUES
  - Values must be decimal
  - eg. 32 is wrong, 32.0 is correct

  TYPES
  - Should be integer. options to choose will be mentioned there
  - eg: #define NL_CLOUD_TYPE 2

  Remember to rebuild the shader after making changes.
*/

/*
  NEWB X DARK MANAGER - main tuning knobs
  These are the "easy" options. Names map to the requested settings:
    FOG_DENSITY -> NL_FOG_DENSITY        FOG_DISTANCE -> NL_FOG_DISTANCE
    NIGHT_DARKNESS -> NL_NIGHT_DARKNESS  SUN_INTENSITY -> NL_SUNLIGHT_INTENSITY
    MOON_INTENSITY -> NL_MOON_INTENSITY  AMBIENT_LIGHT -> NL_AMBIENT_LIGHT
    TORCH_LIGHT -> NL_TORCHLIGHT_INTENSITY
    NETHER_FOG -> NL_NETHER_FOG          END_FOG -> NL_END_FOG
    UNDERWATER_FOG -> NL_UNDERWATER_FOG  COLOR_SATURATION -> NL_SATURATION
    CONTRAST -> NL_CONTRAST              BLOOM_STRENGTH -> NL_BLOOM_STRENGTH
*/
#define NL_FOG_DENSITY    1.9  // 0.3 thin ~ 2.0 very dense - matched to reference screenshots
#define NL_FOG_DISTANCE   0.45 // closer = denser, matched to thick fog references
#define NL_NIGHT_DARKNESS 0.55 // 0.0 vanilla-like night ~ 0.9 very dark night (keep < 0.9 to stay playable)
#define NL_MOON_INTENSITY 1.0  // 0.3 dim ~ 2.0 bright (cold moonlight strength)
#define NL_AMBIENT_LIGHT  0.85 // 0.4 dark shadows ~ 1.5 bright shadows (overworld sky ambient)
#define NL_NETHER_FOG     1.5  // 0.5 thin ~ 3.0 very dense
#define NL_END_FOG        1.3  // 0.5 thin ~ 3.0 very dense
#define NL_NETHER_FOG_BRIGHTNESS 0.8 // 0.4 very dark ~ 1.2 bright (nether fog color, keeps vanilla per-biome hue)
#define NL_END_FOG_START  0.28 // 0.05 fog starts very close ~ 0.9 far (End only, fraction of render distance)
#define NL_UNDERWATER_FOG 1.1  // 0.5 clear water ~ 2.0 murky water
#define NL_CONTRAST       1.12 // 1.0 vanilla ~ 1.4 harsh contrast
#define NL_BIOME_TINT     0.9  // [toggle] 0.0 off ~ 1.0 strong. Tints sky/fog by the biome fog color (fogs/ + biomes_client.json) - strong tint
#define NL_LEAF_LIGHT     0.9  // [toggle] 0.3 subtle ~ 1.0 strong. Dappled sunlight through tree leaves (moves with sun + wind)
#define NL_BLOOM_STRENGTH 0.10 // [toggle] 0.05 subtle ~ 0.4. NOT real bloom: cheap highlight bleed (no post-process pass exists in materials)

/* Color correction */
#define NL_TONEMAP_TYPE 3              // 1:Exponential, 2:Reinhard, 3:Extended Reinhard, 4:ACES
#define NL_GAMMA 1.30                  // 0.3 low ~ 2.0 high
//#define NL_EXPOSURE 1.3              // [toggle] 0.5 dark ~ 3.0 bright
#define NL_SATURATION 0.86             // [toggle] 0.0 grayscale ~ 4.0 super saturated
#define NL_TINT                        // [toggle] enable light/dark tone tinting
#define NL_TINT_LOW  vec3(0.84,0.95,1.18) // color tint for dark tone (cold shadows)
#define NL_TINT_HIGH vec3(1.10,1.0,0.88) // color tint for light tone (warm highlights)

/* Lighting */
#define NL_SUNLIGHT_INTENSITY   3.0  // 1.0 weak ~ 5.0 bright
#define NL_TORCHLIGHT_INTENSITY 1.2  // 0.5 weak ~ 3.0 bright
#define NL_SHADOW_INTENSITY     0.95  // 0.0 no shadow ~ 1.0 strong shadow (High Windows inspired)
#define NL_MIN_LIGHTING_BOOST   1.1  // 1.0 minimal lighting boost for dark areas ~ 3.0 brighter dark areas
#define NL_BLINKING_TORCH    // [toggle] flickering light
#define NL_CLOUD_SHADOW      // [toggle] cloud shadow (simple clouds only)

/* Ambient light for nether/end */
#define NL_NETHER_AMBIENT vec3(2.5,1.05,0.75)
#define NL_END_AMBIENT    vec3(1.25,0.75,1.9)

/* Sun/moon light color */
#define NL_DAWN_SUNLIGHT_COL   vec3(1.0,0.34,0.09)
#define NL_NOON_SUNLIGHT_COL   vec3(1.0,0.79,0.62)
#define NL_NIGHT_MOONLIGHT_COL vec3(0.03,0.07,0.22)

/* Torch colors */
#define NL_OVERWORLD_TORCH_COL  vec3(1.0,0.42,0.12)  // warmer orange-red, closer to true torch
#define NL_UNDERWATER_TORCH_COL vec3(1.0,0.47,0.13)
#define NL_NETHER_TORCH_COL     vec3(1.0,0.42,0.12)
#define NL_END_TORCH_COL        vec3(1.0,0.5,0.25)

/* Fog */
#define NL_FOG 1.0                // [toggle] 0.1 subtle ~ 1.0 blend with sky completely
#define NL_MIST_DENSITY 0.65      // stronger atmospheric mist
#define NL_RAIN_MIST_OPACITY 0.22 // [toggle] 0.04 very subtle ~ 0.5 thick rain mist blow
#define NL_CLOUDY_FOG 0.22        // [toggle] 0.0 subtle - 0.8 dense fog clouds

/* Sky */
#define NL_SKY_VOID_FACTOR     0.5
#define NL_SKY_VOID_DARKNESS   0.3
#define NL_SKY_RAIN_MIX_FACTOR 0.9

/* Sky colors - zenith=top, horizon=bottom */
#define NL_DAWN_ZENITH_COL   vec3(0.07,0.2,0.46)
#define NL_DAWN_HORIZON_COL  vec3(2.8,0.55,0.25)
#define NL_DAWN_EDGE_COL     vec3(1.9,0.75,0.5)
#define NL_DAY_ZENITH_COL    vec3(0.2,0.6,1.5)
#define NL_DAY_HORIZON_COL   vec3(0.8,1.2,1.5)
#define NL_DAY_EDGE_COL      vec3(1.05,1.2,1.35)
#define NL_NIGHT_ZENITH_COL  vec3(0.003,0.011,0.03)
#define NL_NIGHT_HORIZON_COL vec3(0.008,0.02,0.045)
#define NL_NIGHT_EDGE_COL    vec3(0.014,0.03,0.06)
#define NL_RAIN_ZENITH_COL   vec3(0.34,0.38,0.44)
#define NL_RAIN_HORIZON_COL  vec3(0.45,0.48,0.52)

#define NL_END_ZENITH_COL    vec3(0.03,0.0,0.06)
#define NL_END_HORIZON_COL   vec3(0.28,0.03,0.4)


/* Rainbow */
#define NL_RAINBOW           // [toggle] enable rainbow in sky
#define NL_RAINBOW_CLEAR 0.0 // 0.3 subtle ~ 1.0 bright during clear weather
#define NL_RAINBOW_RAIN  0.15 // 0.3 subtle ~ 1.0 bright during rain weather

/* Ore glow intensity */
#define NL_GLOW_TEX 2.3           // 0.4 weak ~ 8.0 bright
#define NL_GLOW_SHIMMER 0.8       // [toggle] 0.1 subtle ~ 1.0 100% shimmer
#define NL_GLOW_SHIMMER_SPEED 0.9 // 0.5 slow - 2.0 fast
//#define NL_GLOW_LEAK 0.6        // [toggle] 0.08 subtle ~ 1.0 100% brightness of NL_GLOW_TEX

/* Waving */
#define NL_PLANTS_WAVE 0.05    // [toggle] 0.02 gentle ~ 0.4 violent
#define NL_LANTERN_WAVE 0.16   // [toggle] 0.05 subtle ~ 0.4 large swing
#define NL_WAVE_SPEED 2.8      // 0.5 slow wave ~ 5.0 very fast wave
//#define NL_EXTRA_PLANTS_WAVE // [toggle] !dont use! wave using texture coords (1.21.0 vanilla 2048x1024)
#define NL_WAVE_RANGE 13.0     // 6.0 upto 6 blocks ~ 32.0 upto 32 blocks away (slower)

/* Water */
#define NL_WATER_TRANSPARENCY 0.9 // 0.0 transparent ~ 1.0 normal
#define NL_WATER_BUMP 0.09        // 0.001 plain ~ 0.2 bumpy water
#define NL_WATER_WAVE_SPEED  0.8  // 0.2 calm ~ 2.0 turbulent
#define NL_WATER_TEX_OPACITY 0.3  // 0.0 plain water ~ 1.0 vanilla water texture
#define NL_WATER_WAVE             // [toggle] wave effect
//#define NL_WATER_REFL_MASK      // [toggle] fake water reflection mask
#define NL_WATER_TINT vec3(0.45,0.78,0.6)

/* Underwater */
#define NL_UNDERWATER_BRIGHTNESS 0.65        // 0.0 dark ~ 3.0 bright
#define NL_CAUSTIC_INTENSITY 1.9             // 0.5 weak ~ 5.0 bright
#define NL_UNDERWATER_WAVE 0.1               // [toggle] 0.02 subtle ~ 0.6 trippy
#define NL_UNDERWATER_STREAKS 1.0            // [toggle] 0.8 subtle - 2.0 bright streaks from top
#define NL_UNDERWATER_TINT vec3(0.75,0.92,1.0) // fog tint color when underwater

/* Cloud type */
#define NL_CLOUD_TYPE 1 // 0:vanilla, 1:soft, 2:rounded, 3:realistic

/* Vanilla cloud settings - make sure to remove clouds.png when using this */
#define NL_CLOUD0_THICKNESS 2.1      // 0.5 slim ~ 8.0 fat
#define NL_CLOUD0_RAIN_THICKNESS 4.0 // 0.5 slim ~ 8.0 fat
#define NL_CLOUD0_OPACITY 0.9        // 0.0 invisible ~ 1.0 opaque
#define NL_CLOUD0_MULTILAYER         // [toggle] extra cloud layer

/* Soft cloud settings */
#define NL_CLOUD1_SCALE vec2(0.016, 0.022) // 0.003 large ~ 0.2 tiny
#define NL_CLOUD1_DEPTH 1.3                // 0.0 no bump ~ 10.0 large bumps
#define NL_CLOUD1_SPEED 0.04               // 0.0 static ~ 0.4 fast moving
#define NL_CLOUD1_DENSITY 0.54             // 0.1 less clouds ~ 0.8 more clouds
#define NL_CLOUD1_OPACITY 0.9              // 0.0 invisible ~ 1.0 opaque

/* Rounded cloud settings */
#define NL_CLOUD2_THICKNESS 2.1            // 0.5 slim ~ 5.0 fat
#define NL_CLOUD2_RAIN_THICKNESS 2.5       // 0.5 slim ~ 5.0 fat
#define NL_CLOUD2_STEPS 5                  // 3 low quality ~ 16 high quality
#define NL_CLOUD2_SCALE vec2(0.033, 0.033) // 0.003 large ~ 0.3 tiny
#define NL_CLOUD2_SHAPE vec2(0.5, 0.4)     // 0.0 round ~ 1.0 box vec2(horizontal shape, vertical shape)
#define NL_CLOUD2_DENSITY 25.0             // 1.0 blurry ~ 100.0 sharp
#define NL_CLOUD2_VELOCITY 0.8             // 0.0 static ~ 4.0 very fast
//#define NL_CLOUD2_LAYER2                      // [toggle] extra cloud layer
#define NL_CLOUD2_LAYER2_OFFSET 143.0           // 30.0 near ~ 300.0 very high
#define NL_CLOUD2_LAYER2_THICKNESS 2.5          // 0.7 slim ~ 5.0 fat
#define NL_CLOUD2_LAYER2_RAIN_THICKNESS 3.0     // 0.7 slim ~ 5.0 fat
#define NL_CLOUD2_LAYER2_STEPS 3                // 3 low quality ~ 16 high quality
#define NL_CLOUD2_LAYER2_SCALE vec2(0.03, 0.03) // 0.003 large ~ 0.3 tiny
#define NL_CLOUD2_LAYER2_SHAPE vec2(0.5, 0.4)   // 0.0 round ~ 1.0 box vec2(horizontal shape, vertical shape)
#define NL_CLOUD2_LAYER2_DENSITY 25.0           // 1.0 blurry ~ 100.0 sharp
#define NL_CLOUD2_LAYER2_VELOCITY 0.8           // 0.0 static ~ 4.0 very fast

/* Realistic cloud settings */
#define NL_CLOUD3_SCALE vec2(0.03, 0.03) // 0.003 large ~ 1.0 tiny
#define NL_CLOUD3_SPEED 0.005            // 0.0 static ~ 4.0 fast moving
#define NL_CLOUD3_SHADOW 0.9             // 0.1 subtle ~ 1.0 dark
#define NL_CLOUD3_SHADOW_OFFSET 0.3      // 0.05 minimal ~ 1.0 large

/* Aurora settings */
#define NL_AURORA 1.6           // [toggle] 0.4 dim ~ 4.0 very bright
#define NL_AURORA_EVERY_DAYS 3.0 // [toggle] aurora shows one night every N days (needs the game Day uniform). Remove line = every night
#define NL_AURORA_DAY_OFFSET 2.0 // 0..N-1 shifts which day the aurora shows
#define NL_AURORA_VELOCITY 0.03 // 0.0 static ~ 0.3 very fast
#define NL_AURORA_SCALE 0.04    // 0.002 large ~ 0.4 tiny
#define NL_AURORA_WIDTH 0.18    // 0.04 thin line ~ 0.4 thick lines
#define NL_AURORA_COL1 vec3(0.1,1.0,0.0)
#define NL_AURORA_COL2 vec3(0.1,0.0,1.0)

#define NL_CLOUD_AURORA_REFLECTION // [toggle] simple clouds/aurora reflection

/* Shooting star */
#define NL_SHOOTING_STAR 0.8        // [toggle] 0.2 dim ~ 1.0 bright
#define NL_SHOOTING_STAR_PERIOD 6.0 // 0.4 fast ~ 12.0 slow (12 secs)
#define NL_SHOOTING_STAR_DELAY 64.0 // 0.0 no delay ~ 100.0 long time (100 secs)

/* Galaxy */
//#define NL_GALAXY_STARS 2.0           // [toggle] 0.1 dim - 4.0 bright
#define NL_GALAXY_VIBRANCE 0.7          // 0.0 white - 1.0 colorful
#define NL_GALAXY_SPEED 0.03            // 0.01 slow motion - 0.2 fast motion
#define NL_GALAXY_DAY_VISIBILITY 0.0    // 0.0 invisible - 1.0 visible

/* Chunk loading slide in animation */
//#define NL_CHUNK_LOAD_ANIM 100.0 // [toggle] -600.0 fall from top ~ 600.0 rise from bottom

/* Sun/Moon */
#define NL_SUN_SIZE  1.0           // 0.3 tiny ~ 4.0 massive
#define NL_MOON_SIZE 1.0           // 0.3 tiny ~ 4.0 massive
#define NL_SUN_PATH_YAW    15.0 //
#define NL_MOON_PATH_YAW   17.0 //
#define NL_SUN_PATH_TILT   31.0 //
#define NL_MOON_PATH_TILT -28.0 //
#define NL_SUN_TILT        45.0 // 0.0 no tilt ~ 90.0 tilt of 90 degrees
#define NL_MOON_TILT       45.0 // 0.0 no tilt ~ 90.0 tilt of 90 degrees

/* Fake godrays during sunrise/sunset */
//#define NL_GODRAY 0.3 // [toggle] 0.1 subtle ~ 0.8 strong

/* Sky reflection */
//#define NL_GROUND_REFL 0.4       // [toggle] 0.2 slightly reflective ~ 1.0 fully reflect sky
#define NL_GROUND_RAIN_WETNESS 1.0 // 0.0 no wetness ~ 1.0 fully wet blocks when raining
#define NL_WET_DARKEN 0.5          // 0.0 none ~ 0.8 very dark wet blocks while raining
#define NL_RAIN_SHIMMER 0.3        // [toggle] 0.1 subtle ~ 0.6 puddle reflections flicker as rain hits them
#define NL_GROUND_RAIN_PUDDLES 0.85 // 0.0 no puddles ~ 1.0 puddles

/* Entity (Actor, ItemInHand) */
#define NL_ENTITY_BRIGHTNESS     0.65 // 0.1 dark ~ 1.6 bright
#define NL_ENTITY_EDGE_HIGHLIGHT 0.41 // [toggle] 0.0 no highlight ~ 1.6 bright highlight

/* Weather particles */
#define NL_WEATHER_SPECK 0.6         // [toggle] 0.0 vanilla texture ~ 1.0 soft speck
#define NL_WEATHER_RAIN_SLANT 5.0    // 1.0 minimal ~ 8.0 violent
#define NL_WEATHER_PARTICLE_SIZE 1.0 // 0.5 tiny ~ 4.0 large

/* Lava effects */
#define NL_LAVA_NOISE            // [toggle] darken lava in certain regions
//#define NL_LAVA_NOISE_BUMP 0.2 // [toggle] 0.1 subtle ~ 0.8 massive waves
#define NL_LAVA_NOISE_SPEED 0.2  // 0.0 still ~ 0.8 fast

/*
  NEWB SHADER SUBPACK CONFIG
  This part contains custom configuration options for each subpack.

  If a value is already defined,
  then you must undefine it before modifying:
  eg: #undef OPTION_NAME

  Subpack names and flags are inside `pack_config.toml`.
  Build tool will enable corresponding flags when compiling.
*/

#ifdef LITE
  #define NO_WAVE
  #undef NL_GLOW_SHIMMER
  #undef NL_LAVA_NOISE
  #undef NL_WEATHER_SPECK
  #undef NL_SHOOTING_STAR
  #undef NL_CLOUD_AURORA_REFLECTION
  #undef NL_UNDERWATER_STREAKS
  #undef NL_RAIN_MIST_OPACITY
  #undef NL_CLOUDY_FOG
  #undef NL_ENTITY_EDGE_HIGHLIGHT
#endif

/*
  "Dark Fantasy - Low" subpack (define DARK_LOW).
  Turns off: pseudo-bloom, godrays, water waves/reflections extras, cloud extras,
  animations and other secondary effects.
  Keeps: fog (incl. Nether/End/underwater), dark colors, lighting, sky, atmosphere.
*/
#ifdef DARK_LOW
  #undef NL_BLOOM_STRENGTH
  #undef NL_GODRAY
  // water
  #undef NL_WATER_WAVE
  #undef NL_UNDERWATER_WAVE
  #undef NL_UNDERWATER_STREAKS
  #undef NL_CLOUD_AURORA_REFLECTION
  // clouds / sky extras
  #undef NL_CLOUD_SHADOW
  #undef NL_CLOUD0_MULTILAYER
  #undef NL_CLOUD2_LAYER2
  #undef NL_AURORA
  #undef NL_SHOOTING_STAR
  #undef NL_RAINBOW
  #undef NL_CLOUDY_FOG
  #undef NL_RAIN_MIST_OPACITY
  #undef NL_LEAF_LIGHT
  #undef NL_RAIN_SHIMMER
  #undef NL_CLOUD_TYPE
  #define NL_CLOUD_TYPE 1
  // animations / secondary
  #define NO_WAVE
  #undef NL_BLINKING_TORCH
  #undef NL_GLOW_SHIMMER
  #undef NL_LAVA_NOISE
  #undef NL_WEATHER_SPECK
  #undef NL_ENTITY_EDGE_HIGHLIGHT
#endif

#ifdef NO_WAVE_NO_FOG
  #define NO_WAVE
  #define NO_FOG
#endif

#ifdef NO_FOG
  #undef NL_FOG
#endif

#ifdef NO_WAVE
  #undef NL_PLANTS_WAVE
  #undef NL_LANTERN_WAVE
  #undef NL_UNDERWATER_WAVE
  #undef NL_WATER_WAVE
  #undef NL_RAIN_MIST_OPACITY
#endif

#ifdef CHUNK_ANIM
  #define NL_CHUNK_LOAD_ANIM 100.0
#endif

#ifdef ROUNDED_CLOUDS
  #undef NL_CLOUD_TYPE
  #define NL_CLOUD_TYPE 2
  #undef NL_CLOUD_SHADOW // TODO: Cloud shadow for rounded, realistic clouds
#endif

#ifdef BOX_CLOUDS
  #undef NL_CLOUD_TYPE
  #define NL_CLOUD_TYPE 0
  #undef NL_CLOUD_SHADOW
#endif

#ifdef REALISTIC_CLOUDS
  #undef NL_CLOUD_TYPE
  #define NL_CLOUD_TYPE 3
  #undef NL_CLOUD_SHADOW
#endif

#endif
