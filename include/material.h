#ifndef MATERIAL_H
#define MATERIAL_H
#include "hitable.h"
#include"ray.h"
#include<cstdlib>
inline vec reflect(const vec&v ,const vec& n){
    return v - 2.0 * dot(v,n) * n ;  
}
float schlick(float cosine, float ref_idx){     // an approximation technique for frensel effect
    float r0 = (1 - ref_idx) / (1 + ref_idx);   
    r0 = r0*r0;
    return r0 + (1 - r0) * pow((1-cosine),5) ; // if angle is steep --> acts like a mirror
}
bool refract(const vec& v ,const vec& n , float ni_over_nout , vec& refracted ){
    vec uv = unit_vector(v);
    float dt = dot(uv,n);
    float refracted_horizontal = ni_over_nout*ni_over_nout*(1 - dt*dt) ;
    float discriminant = 1.0 - (refracted_horizontal) ;     // this part calculates the vertical component of refracted ray
    // at a large incidence angle --> if horizontal component of refracted is > 1.0 --> then vertical component(discriminant) becomes less than 0 ie ; no ray crosses the boundary(no refraction) --> TIR
    if(discriminant > 0){
        refracted = ni_over_nout*(uv - n*dt) - n*sqrt(discriminant);
        return true;
    }else{
        return false;
    }
}
inline float random_double(){
    return rand() / (RAND_MAX + 1.0);
}
// THE REJECTION METHOD : keep going   until you get a point within the sphere radius 
vec random_int_unitSphere(){
    vec p;
    do{
        // pick a random point in a cube (-1 to +1)
        p = (2 * vec(random_double(), random_double(), random_double())) - vec(1.0,1.0,1.0); 
    }while(p.squared_length() >= 1); 
    return p;
}

class material{
    public:
        virtual bool scatter(const ray& r_in , const hit_record& rec , vec& attenuation ,ray& scattered)const  = 0 ;
        virtual vec emitted() const {return vec(0.0f , 0.0f ,0.0f);}
};
class lambertian : public material{
    public :
    vec albedo;
        lambertian(const vec& a) : albedo(a) {}
        virtual bool scatter(const ray& r_in , const hit_record& rec , vec& attenuation ,ray& scattered) const{
            vec target = rec.P + rec.N + random_int_unitSphere();
            scattered = ray(rec.P , target - rec.P);
            attenuation = albedo; // the color of the material
            return true;
        }  
};
class metal: public material {
    public :
        vec albedo; // amount of light reflected - amount of light that survived the hit
        float fuzz;
        metal(const vec& a , float f) : albedo(a){
            if(f < 1) fuzz = f;
            else fuzz = 1;
        }
        virtual bool scatter(const ray& r_in , const hit_record& rec , vec & attenuation , ray& scattered)const{
            vec reflected = reflect(unit_vector(r_in.direction()) , rec.N);
            scattered = ray(rec.P, reflected + fuzz * random_int_unitSphere());
            attenuation  =albedo ; 
            return (dot(rec.N , scattered.direction()) > 0);
        }
};
class dielectric : public material {
    public :
        float ref_idx;
        dielectric(float ri) : ref_idx(ri) {}

        virtual bool scatter(const ray& r_in , const hit_record& rec , vec& attenuation ,ray& scattered) const{
            
            vec outward_normal;
            float ni_over_nout;
            vec reflected  = reflect(r_in.direction(),rec.N);   
            
            attenuation = vec(1.0,1.0,1.0);  
            float cosine;
            float reflect_prob;
            vec refracted;

            // Check if the ray is inside the glass trying to get out, or outside going in
            if(dot(r_in.direction(),rec.N) > 0){
                // means ray is trapped inside glass and trying to move from glass to air (same dirn as outward normal)
                outward_normal = - rec.N;
                ni_over_nout = ref_idx;
                cosine =  ref_idx * dot(unit_vector(r_in.direction()), rec.N); // multiplied by ref_idx to convert internal refract angle to outside angle using snell's law
            }else{
                outward_normal = rec.N;
                ni_over_nout = 1 / ref_idx;
                cosine = -dot(unit_vector(r_in.direction()) , rec.N);
            }
            if(refract(r_in.direction() , outward_normal , ni_over_nout , refracted)){
                reflect_prob = schlick(cosine,ref_idx); // if it refracts then it ask schlicks for the exact probability the glass act like a mirror
            }else{
                scattered = ray(rec.P,reflected); // means TOTAL internal reflection --> ligh is reflected
                reflect_prob = 1.0;
            }
            if(random_double() < reflect_prob){
                // if random no. is less than schlick prob --> then it reflects 
                scattered = ray(rec.P,reflected);
            }else{
                // otherwise
                scattered = ray(rec.P,refracted);
            }
            return true;
        }
};

class diffuse_light : public material{
public:
    vec emit_color;
    diffuse_light(const vec& c) : emit_color(c) {}

    virtual bool scatter(const ray& r_in , const hit_record& rec , vec& attenuation ,ray& scattered)const {
        return false;
    }
    virtual vec emitted() const {return emit_color ;}
};

class glowing_metal : public material{
    vec emit_color , albedo ;
    float fuzz;
    glowing_metal(const vec& e , const vec& a , float f) : emit_color(e) , albedo(a) ,fuzz(f) {}

    virtual bool scatter(const ray& r_in , const hit_record& rec , vec& attenuation ,ray& scattered) const{
        vec reflected = reflect(unit_vector(r_in.direction()) , rec.N);
        scattered = ray(rec.P , reflected + (fuzz * random_int_unitSphere()));
        attenuation = albedo;
        return (dot(rec.N , scattered.direction()) > 0);
    }

    virtual vec emitted()const{
        return emit_color;
    }
};
#endif