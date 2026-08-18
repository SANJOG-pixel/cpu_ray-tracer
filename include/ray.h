#ifndef RAY_H
#define RAY_H
#include "vec.h"

class ray{
public:
    vec orig;
    vec dir;
public:
    ray() {};
    ray(const vec& origin , const vec& dirn){
        orig = origin;  
        dir = dirn;
    }
    vec origin() const{return orig;}  // return the camera centre
    vec direction() const{return dir;}
    vec parametric_eqn(float t) const{return (orig + t*dir) ;}  // equation : O + D*t

};
#endif