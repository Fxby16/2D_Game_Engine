#pragma once

inline constexpr unsigned int MAX_QUADS=5000; ///<Max number of quads per draw call
inline constexpr unsigned int MAX_VERTICES=4*MAX_QUADS; ///<Max number of vertices per draw call
inline constexpr unsigned int MAX_INDICES=6*MAX_QUADS; ///<Max number of indices per draw call

enum LightType{
    ALL_LIGHT,LIGHT_AROUND_POS,LIGHT_AROUND_POS_COLL
};

#ifdef EDITOR

extern bool ANIMATIONS_PREVIEW;

#endif