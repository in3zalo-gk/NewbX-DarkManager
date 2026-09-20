#ifndef LIGHTING_H
#define LIGHTING_H

#include "detection.h"
#include "sky.h"
#include "utils.h"
#include "noise.h"
#include "clouds.h"

vec3 sunLightTint(float dayFactor, float rain) {
  float nightFactor = step(dayFactor, 0.0);
  float dawnFactor = 1.0-dayFactor*dayFactor;
  dawnFactor *= dawnFactor*dawnFactor;
  dawnFactor *= mix(1.0, dawnFactor*dawnFactor, nightFactor);
  vec3 tint = mix(NL_NOON_SUNLIGHT_COL, NL_NIGHT_MOONLIGHT_COL*NL_MOON_INTENSITY, nightFactor);
  tint = mix(tint, NL_DAWN_SUNLIGHT_COL, dawnFactor);
  tint = mix(tint, vec3_splat(dot(tint, vec3_splat(0.33))), rain);
  return tint;
}

#ifdef NL_LEAF_LIGHT
float nlValueNoise(vec2 p) {
  vec2 i = floor(p);
  vec2 f = fract(p);
  f = f*f*(3.0-2.0*f);
  float a = fastRand(i);
  float b = fastRand(i + vec2(1.0, 0.0));
  float c = fastRand(i + vec2(0.0, 1.0));
  float d = fastRand(i + vec2(1.0, 1.0));
  return mix(mix(a, b, f.x), mix(c, d, f.x), f.y);
}

// patches of sunlight that filter through the canopy.
// they slide as the sun moves and shimmer with the wind
float nlLeafDapple(vec3 gPos, vec3 sunDir, highp float t) {
  vec2 p = mod(gPos.xz, 512.0)*0.6 + 6.0*sunDir.xz + t*vec2(0.08, 0.05);
  float n = mix(nlValueNoise(p), nlValueNoise(p*2.3 + 7.7 + t*0.12), 0.35);
  return smoothstep(0.42, 0.68, n);
}
#endif

vec3 nlLighting(
  sampler2D tex, nl_skycolor skycol, nl_environment env, vec3 wPos, out vec3 torchColor, vec3 COLOR,
  vec2 uv1, vec2 lit, bool isTree, float shade, highp float t, float renderdistance, float TIME_OF_DAY, vec3 CAMERA_POS
) {
  vec3 light;

  // 0.0 at day -> NL_NIGHT_DARKNESS at night: scales sky ambient + min brightness
  float nightDark = 1.0 - NL_NIGHT_DARKNESS*smoothstep(-0.1, 0.25, -env.dayFactor);

  if (env.underwater) {
    torchColor = NL_UNDERWATER_TORCH_COL;
  } else if (env.end) {
    torchColor = NL_END_TORCH_COL;
  } else if (env.nether) {
    torchColor = NL_NETHER_TORCH_COL;
  } else {
    torchColor = NL_OVERWORLD_TORCH_COL;
  }

  float torchAttenuation = (NL_TORCHLIGHT_INTENSITY*uv1.x)/(0.5-0.45*lit.x);

  #ifdef NL_BLINKING_TORCH
    torchAttenuation *= 1.0 - 0.19*noise1D(t*8.0);
  #endif

  vec3 torchLight = torchColor*torchAttenuation;
  float gameBrightness = texelFetch(tex, ivec2(0,0), 0).g;
  float lum = 0.0;

  if (env.nether || env.end) {
    // nether & end lighting

    light = env.end ? NL_END_AMBIENT : NL_NETHER_AMBIENT;
    light *= gameBrightness;

    lum = luminance(light);
    light += skycol.horizon/(1.0+lum);

  } else {
    // overworld lighting
    float nightFactor = step(env.dayFactor, 0.0);
    float dawnFactor = 1.0-env.dayFactor*env.dayFactor;
    dawnFactor *= dawnFactor*dawnFactor;
    dawnFactor *= mix(1.0, dawnFactor*dawnFactor, nightFactor);
    float nightIntensity = 1.0-(0.5+0.5*env.dayFactor);
    nightIntensity *= nightIntensity;

    float sunLightAttenuation = clamp(0.5*(((2.0*step(TIME_OF_DAY, 0.5)-1.0)*(wPos.x*cos(NL_SUN_PATH_YAW)+wPos.y*sin(NL_SUN_PATH_YAW))/renderdistance) + 1.0), 0.0, 1.0);
    sunLightAttenuation = mix(1.0, sunLightAttenuation*sunLightAttenuation, dawnFactor);
    sunLightAttenuation *= 1.0-0.55*env.rainFactor;

    // shadow cast by sun light
    float shadow = step(0.93, uv1.y);
    shadow = max(shadow, (1.0 - NL_SHADOW_INTENSITY + (0.6*NL_SHADOW_INTENSITY*nightIntensity))*lit.y);
    shadow *= shade > 0.8 ? 1.0 : 0.8;
    #if defined(NL_CLOUD_SHADOW) && (NL_CLOUD_TYPE == 1 || NL_CLOUD_TYPE == 2)
      vec3 mainLightDir = env.sunDir.y > 0.0 ? env.sunDir : env.moonDir;
      vec3 gPos = wPos + CAMERA_POS;
      float cloudRelativeHeight = gPos.y-187.0;
      vec2 projectionOffset = cloudRelativeHeight*mainLightDir.xz/mainLightDir.y;
      vec2 projectedPos = gPos.xz + projectionOffset;
      float cloudFade = smoothstep(1.0, 0.5, length(0.002*(wPos.xz + projectionOffset)));
      cloudFade *= (1.0-dawnFactor*dawnFactor)*clamp(-0.12*(cloudRelativeHeight-7.0), 0.0, 1.0);
      float cmask;
      #if NL_CLOUD_TYPE == 1
        // shadow cast by simple clouds
        cmask = cloudNoise2D(projectedPos*NL_CLOUD1_SCALE, t, env.rainFactor)*cloudFade;
      #elif NL_CLOUD_TYPE == 2
        // shadow cast by rounded clouds  
        projectedPos = NL_CLOUD2_SCALE * (projectedPos + vec2(1.0, 0.5) * (t * NL_CLOUD2_VELOCITY));
        cmask = cloudDf(vec3(projectedPos.x, 0.5, projectedPos.y), env.rainFactor, NL_CLOUD2_SHAPE)*cloudFade;
      #endif
      shadow *= 0.3 + 0.7*smoothstep(0.6, 0.0, cmask);
    #endif

    // sun through leaves: under a canopy the skylight is a bit lower (uv1.y < 0.93)
    float leafSun = 0.0;
    #ifdef NL_LEAF_LIGHT
      float leafBand = smoothstep(0.5, 0.7, uv1.y)*(1.0-step(0.93, uv1.y));
      float leafSunUp = max(env.dayFactor, 0.0)*(1.0-env.rainFactor)*step(0.8, shade);
      if (leafBand*leafSunUp > 0.0) {
        leafSun = leafBand*leafSunUp*nlLeafDapple(wPos + CAMERA_POS, env.sunDir, t);
        shadow = mix(shadow, 1.0, NL_LEAF_LIGHT*leafSun);
      }
    #endif

    // direct light from top
    light = (NL_SUNLIGHT_INTENSITY*shadow*sunLightAttenuation)*sunLightTint(env.dayFactor, env.rainFactor);
    #ifdef NL_LEAF_LIGHT
      light += vec3(0.30, 0.20, 0.09)*(NL_LEAF_LIGHT*leafSun); // warm glow of the beam
    #endif

    // sky ambient
    lum = luminance(light);
    light += (skycol.horizon + skycol.zenith)*(uv1.y*NL_AMBIENT_LIGHT*nightDark/(1.0+lum));

  }

  // torch light
  lum = luminance(light);
  light += torchLight/(1.0+lum);

  // game min brightness
  if (!(env.nether || env.end)) {
    lum = luminance(light);
    light += vec3_splat(gameBrightness*(NL_MIN_LIGHTING_BOOST*nightDark/(1.0+lum)));
  }

  // darken at crevices
  light *= COLOR.g > 0.35 ? 1.0 : 0.8;

  // brighten tree leaves
  if (isTree) {
    light *= 1.25;
  }

  return light;
}

void nlUnderwaterLighting(inout vec3 light, inout vec3 pos, vec2 lit, vec2 uv1, vec3 tiledCpos, vec3 cPos, highp float t, vec3 horizonCol) {
  if (uv1.y < 0.9) {
    float caustics = disp(tiledCpos, NL_WATER_WAVE_SPEED*t);
    caustics *= 3.0*caustics;
    light += NL_UNDERWATER_BRIGHTNESS + NL_CAUSTIC_INTENSITY*caustics*(0.15 + lit.y + lit.x*0.7);
  }
  light *= mix(normalize(horizonCol), vec3_splat(0.6), lit.y*0.6);
  #ifdef NL_UNDERWATER_WAVE
    pos.xy += NL_UNDERWATER_WAVE*min(0.05*pos.z,0.6)*sin(t*1.2 + dot(cPos,vec3_splat(PI_HALF)));
  #endif
}

vec3 nlEntityLighting(nl_skycolor skycol, nl_environment env, vec3 pos, vec4 normal, vec3 wPos, mat4 world, vec4 tileLightCol, vec4 overlayCol, vec3 horizonEdgeCol, float t, float TIME_OF_DAY, float renderdistance, vec3 CAMERA_POS) {
  float l = tileLightCol.b;
  float tl = tileLightCol.r;
  float lum;
  vec3 light;
  float nightDark = 1.0 - NL_NIGHT_DARKNESS*smoothstep(-0.1, 0.25, -env.dayFactor);
  if (env.nether || env.end) {
    tl = max(tl-0.6, 0.0);
    tl *= 21.0*tl;

    // nether & end lighting
    light = env.end ? NL_END_AMBIENT : NL_NETHER_AMBIENT;
    light *= min(tileLightCol.b, 0.25);

    lum = luminance(light);
    light += skycol.horizon/(1.0+lum);
  } else {
    tl = max(tl-0.08, 0.0);
    tl *= 4.0*tl;

    float nightFactor = step(env.dayFactor, 0.0);
    float dawnFactor = 1.0-env.dayFactor*env.dayFactor;
    dawnFactor *= dawnFactor*dawnFactor;
    dawnFactor *= mix(1.0, dawnFactor*dawnFactor, nightFactor);
    float nightIntensity = 1.0-(0.5+0.5*env.dayFactor);
    nightIntensity *= nightIntensity;

    float sunLightAttenuation = clamp(0.5*(((2.0*step(TIME_OF_DAY, 0.5)-1.0)*(wPos.x*cos(NL_SUN_PATH_YAW)+wPos.y*sin(NL_SUN_PATH_YAW))/renderdistance) + 1.0), 0.0, 1.0);
    sunLightAttenuation = mix(1.0, sunLightAttenuation*sunLightAttenuation, dawnFactor);
    sunLightAttenuation *= 1.0-0.5*env.rainFactor;

    // direct light from top
    light = (NL_SUNLIGHT_INTENSITY*l*sunLightAttenuation)*sunLightTint(env.dayFactor, env.rainFactor);
    vec3 N = normalize(mul(world, normal)).xyz;
    light *= 0.9 + max(N.y, 0.0);

    // sky ambient
    lum = luminance(light);
    light += (skycol.horizon + skycol.zenith)*(l*NL_AMBIENT_LIGHT*nightDark/(1.0+lum));
  }

  // torch light
  vec3 torchColor;
  if (env.underwater) {
    torchColor = NL_UNDERWATER_TORCH_COL;
  } else if (env.end) {
    torchColor = NL_END_TORCH_COL;
  } else if (env.nether) {
    torchColor = NL_NETHER_TORCH_COL;
  } else {
    torchColor = NL_OVERWORLD_TORCH_COL;
  }

  lum = luminance(light);
  light += torchColor*(smoothstep(0.1, 0.0, tileLightCol.b-tileLightCol.r)*NL_TORCHLIGHT_INTENSITY*tl/(1.0+lum));

  // game min brightness
  lum = luminance(light);
  if (!(env.nether || env.end)) {
    lum = luminance(light);
    light += vec3_splat(min(tileLightCol.r, 0.15)*(NL_MIN_LIGHTING_BOOST*nightDark/(1.0+lum)));
  }

  if (env.underwater) {
    vec3 gPos = wPos + CAMERA_POS;
    float caustics = 0.2 + 0.2*sin(dot(gPos, vec3(1.8, 2.4, 2.1)) + 0.8*t);
    light += 0.8*NL_UNDERWATER_BRIGHTNESS + NL_CAUSTIC_INTENSITY*caustics*(0.1 + tl);
    light *= mix(normalize(skycol.horizon), vec3_splat(0.5), tileLightCol.b*0.2);
  }

  lum = luminance(light);
  light += vec3_splat(overlayCol.a*(1.5/(1.0+lum)));

  return light;
}

float nlEntityEdgeHighlight(vec4 edgemap) {
  #ifdef NL_ENTITY_EDGE_HIGHLIGHT
    vec2 len = min(abs(edgemap.xy),abs(edgemap.zw));
    len *= len;
    len *= len;
    float ambient = len.x + len.y*(1.0-len.x);
    return NL_ENTITY_BRIGHTNESS + ambient*NL_ENTITY_EDGE_HIGHLIGHT;
  #else
    return 1.0;
  #endif
}

vec4 nlEntityEdgeHighlightPreprocess(vec2 texcoord) {
  vec4 edgeMap = fract(vec4(texcoord*128.0, texcoord*256.0));
  return 2.0*step(edgeMap, vec4_splat(0.5)) - 1.0;
}

vec4 nlLavaNoise(vec3 gPos, float t) {
  float n = movingNoise2D(gPos.xz + gPos.yy, NL_LAVA_NOISE_SPEED*t, 0.9);
  n *= n;
  return vec4(mix(vec3(0.7, 0.4, 0.0)*smoothstep(-0.1, 0.5, n), vec3_splat(1.5), n*n),n);
}

#endif
