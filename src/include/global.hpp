#pragma once

inline constexpr unsigned int MAX_QUADS=5000;
inline constexpr unsigned int MAX_VERTICES=4*MAX_QUADS;
inline constexpr unsigned int MAX_INDICES=6*MAX_QUADS;

enum LightType{
    ALL_LIGHT,LIGHT_AROUND_POS,LIGHT_AROUND_POS_COLL
};