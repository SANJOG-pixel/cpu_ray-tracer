#ifndef HITABLE_H
#define HITABLE_H
#include "ray.h"

class material; 

struct hit_record{
    float t;
    vec P;
    vec N;
    material *mat_type ; // tells what type of  material is the obj
};    
class hitable{
    public:
    virtual bool hit(const ray& r , float t_min , float t_max , hit_record& rec) const = 0;
};
#endif