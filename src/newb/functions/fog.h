#ifndef FOG_H
#define FOG_H

#include "detection.h"

// Dark Manager fog:
// - overworld : FOG_CONTROL scaled by NL_FOG_DISTANCE, strength NL_FOG_DENSITY
// - nether    : engine FOG_CONTROL (already dense), strength NL_NETHER_FOG
// - end       : own curve starting at NL_END_FOG_START, strength NL_END_FOG
// - underwater: engine FOG_CONTROL, strength NL_UNDERWATER_FOG
// Environment is detected from FOG_COLOR/FOG_CONTROL (same as the rest of Newb),
// so no biome API is needed. This runs in vertex shaders only (cheap).
float nlRenderFogFade(float relativeDist, vec3 FOG_COLOR, vec2 FOG_CONTROL) {
  #ifdef NL_FOG
    float fade;
    float mult;
    if (detectEnd(FOG_COLOR)) {
      fade = smoothstep(NL_END_FOG_START, 1.0, relativeDist);
      mult = NL_END_FOG;
    } else if (detectNether(FOG_COLOR, FOG_CONTROL)) {
      fade = smoothstep(FOG_CONTROL.x, FOG_CONTROL.y, relativeDist);
      mult = NL_NETHER_FOG;
    } else if (detectUnderwater(FOG_COLOR, FOG_CONTROL)) {
      fade = smoothstep(FOG_CONTROL.x, FOG_CONTROL.y, relativeDist);
      mult = NL_UNDERWATER_FOG;
    } else {
      fade = smoothstep(FOG_CONTROL.x*NL_FOG_DISTANCE, FOG_CONTROL.y*NL_FOG_DISTANCE, relativeDist);
      mult = NL_FOG_DENSITY;
    }

    // misty effect
    float density = NL_MIST_DENSITY*(19.0 - 18.0*FOG_COLOR.g);
    fade += (1.0-fade)*(0.3-0.3*exp(-relativeDist*relativeDist*density));

    return min(NL_FOG*fade*mult, 1.0);
  #else
    return 0.0;
  #endif
}

float nlRenderGodRayIntensity(vec3 cPos, vec3 worldPos, float t, vec2 uv1, float relativeDist, vec3 FOG_COLOR) {
  // offset wPos (only works upto 16 blocks)
  vec3 offset = cPos - 16.0*fract(worldPos*0.0625);
  offset = abs(2.0*fract(offset*0.0625)-1.0);
  offset = offset*offset*(3.0-2.0*offset);
  //offset = 0.5 + 0.5*cos(offset*0.392699082);

  //vec3 ofPos = wPos+offset;
  vec3 nrmof = normalize(worldPos);

  float u = nrmof.z/length(nrmof.zy);
  float diff = dot(offset,vec3(0.1,0.2,1.0)) + 0.07*t;
  float mask = nrmof.x*nrmof.x;

  float vol = sin(7.0*u + 1.5*diff)*sin(3.0*u + diff);
  vol *= vol*mask*uv1.y*(1.0-mask*mask);
  vol *= relativeDist*relativeDist;

  // dawn/dusk mask
  vol *= clamp(3.0*(FOG_COLOR.r-FOG_COLOR.b), 0.0, 1.0);

  vol = smoothstep(0.0, 0.1, vol);
  return vol;
}

#endif
