#ifndef CAMERA_H
#define CAMERA_H
#include "ray.h"
#include "material.h"

inline vec random_in_unit_disk(){
    while(true){
        vec p(
            2.0f * random_double() -1.0f ,
            2.0f * random_double() -1.0f,
            0.0f
        );
        if(p.squared_length() >= 1.0f) continue;

        return p;
    }
}
class camera{
    public:
        vec origin;
        vec lower_left_corner;
        vec horizontal;
        vec vertical;
        vec u , v , w ;
        float lens_radius ;
        camera(vec look_from , vec look_at , vec vup , float vfov , float aspect , float aperture , float focus_dist){
            lens_radius = aperture / 2.0f;
            // vfov is top-bottom is in degrees
            float theta = vfov * M_PI / 180 ;   
            float half_height =  tan(theta / 2); 
            float half_wide = aspect * half_height;

            // w-new Z axis pointing backwards from lens
            // u - new X -axis pointing right*focus_dist
            // v - new Y -axis pointing up    
            origin = look_from;
            w = unit_vector(look_from - look_at);
            u = unit_vector(cross(vup , w)) ;
            v = cross(w , u) ;

            lower_left_corner = origin - focus_dist*w -focus_dist* half_wide*u - focus_dist*half_height*v ;

            horizontal = 2*half_wide*u*focus_dist;
            vertical = 2*half_height*v*focus_dist;

        }
        ray get_ray(float s , float t) const {
            vec rd = random_in_unit_disk() * lens_radius;
            vec offset =  u * rd.x() + v * rd.y() ;
            return ray(origin + offset, lower_left_corner + s*horizontal + t*vertical - origin - offset);
        }
};
#endif