#ifndef CAMERA_H
#define CAMERA_H
#include "ray.h"

class camera{
    public:
        vec origin;
        vec lower_left_corner;
        vec horizontal;
        vec vertical;
        camera(vec look_from , vec look_at , vec vup , float vfov , float aspect){
            vec u , v , w ;  
            // vfov is top-bottom is in degrees
            float theta = vfov * M_PI / 180 ;   
            float half_height =  tan(theta / 2); 
            float half_wide = aspect * half_height;

            // w-new Z axis pointing backwards from lens
            // u - new X -axis pointing right
            // v - new Y -axis pointing up
            origin = look_from;
            w = unit_vector(look_from - look_at);
            u = unit_vector(cross(vup , w)) ;
            v = cross(w , u) ;

            lower_left_corner = origin - w - half_wide*u - half_height*v ;

            horizontal = 2*half_wide*u;
            vertical = 2*half_height*v;

        }
        ray get_ray(float u , float v) const {
            return ray(origin, lower_left_corner + u*horizontal + v*vertical - origin);
        }
};
#endif