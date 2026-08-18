#ifndef SPHERE_H
#define SPHERE_H
#include "hitable.h"
class sphere : public hitable{
    public:
    vec centre;
    float radius ;
    material *mat_type;
    sphere() {}
    sphere(vec cen , float r,material* m) : centre(cen) , radius(r) , mat_type(m) {};
    virtual bool hit(const ray& r , float t_min ,float t_max , hit_record& rec)const;   
};

bool sphere::hit(const ray& r ,float t_min ,float t_max , hit_record& rec) const{
    vec oc = r.origin() - centre;
    float a = dot(r.direction(), r.direction());
    float b = 2 * dot(r.direction(),oc);
    float c  = dot(oc,oc) - radius*radius ;
    float discriminant = b*b - 4*a*c ;
    if(discriminant > 0){
        float temp = (-b - sqrt(discriminant)) / (2.0 * a) ;
        if(temp < t_max && temp > t_min){
            rec.t  = temp;
            rec.P = r.parametric_eqn(rec.t) ;
            rec.N = (rec.P - centre) / radius ;
            rec.mat_type = mat_type;
            return true;
        }
        temp = (-b + sqrt(discriminant)) / (2.0 * a) ;
        if(temp < t_max && temp > t_min){
            rec.t  = temp;
            rec.P = r.parametric_eqn(rec.t) ;
            rec.N = (rec.P - centre) / radius ;
            rec.mat_type = mat_type;
            return true;
        }
    }
    return false; // if ray missed completely
}
#endif