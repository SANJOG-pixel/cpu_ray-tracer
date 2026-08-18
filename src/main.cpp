#include<iostream>
#include "ray.h"
#include "sphere.h"
#include "camera.h"
#include "hitablelist.h"
#include "material.h"
#include <cfloat>   
#include<cstdlib>
using namespace std;

// vec color(const ray& r , hitable* world ,int depth){
//     hit_record rec;
//     if(world->hit(r,0.001,FLT_MAX,rec)){
//         // THE LAMBERTIAN BOUNCE: Hit Point (P) + Normal (N) + Random Point (S)
//         vec target = rec.P + rec.N + random_int_unitSphere();
//         // base case : stop bouncing if we get trapped
//         if(depth <= 0){
//             return vec(0.0,0.0,0.0);
//         }
//         // COLOR ATTENUATION: The surface absorbs 50% of the light on every bounce
//         return 0.5 * color(ray(rec.P , target - rec.P),world,depth-1);
//     }
//     // If it missed everything, paint the background sky
//     vec unit_direction = unit_vector(r.direction());
//     float t = 0.5 * (unit_direction.y() + 1);
//     return (1-t)*vec(1.0, 1.0, 1.0) + t* vec(0.5, 0.7, 1.0) ;
// }

hitable* random_scene(){
    int n = 500;
    hitable** list = new hitable*[n];
    
    list[0] =new sphere(vec(0,-1000,0),1000,new lambertian(vec(0.15, 0.15, 0.18)));

    int i =1;
    for(int a =-11;a < 11 ;a++){ // control X- axis (left -right)
        for(int b = -11 ; b <11 ; b++){   // control Z-axis(forward- backward)
            // this create 22 by 22 grid -> so it runs 484 times
            float choose_mat = random_double();
            vec centre(a + 0.9*random_double(), 0.2 , b + 0.9*random_double());    

            if((centre - vec(4,0.2,0)).length() > 0.9 &&
                (centre - vec(0,0.2,0)).length() > 0.9 && 
                (centre - vec(-4,0.2,0)).length() > 0.9){
                    if(choose_mat < 0.50){
                        float r = (0.4 +  0.6*random_double()) * random_double();
                        float g = (0.3 +  0.5*random_double()) * random_double();
                        float b = (0.2 +  0.4*random_double()) * random_double();

                        list[i++] = new sphere(centre , 0.2 , new lambertian(vec(r,g,b)));

                    }else if(choose_mat < 0.75){
                        
                        list[i++] = new sphere(centre ,0.2 ,
                            new metal(vec(0.5 *(1+ random_double()),
                                        0.5 *(1+ random_double()),
                                        0.5* (1+ random_double())) , 
                                    0.5 * random_double() * 0.5));    // fuzz - keeps fuzz btw 0.05 and 0.3 so from perfect mirror to slight blurr
                    }else{
                        
                        list[i++] = new sphere(centre ,0.2 , new dielectric(1.5));
                    }
                }
        }
    }
    // list[i++] =new sphere(vec(0,1,0),1.0, new dielectric(1.5));
    list[i++] = new glowing_metal(vec(15.0, 12.5, 3.8),vec(0.95, 0.90, 0.70),0.08) ;                    
    list[i++] = new sphere(vec(-4,1,0),1.0, new lambertian(vec(0.6, 0.3, 0.1)));
    list[i++] = new sphere(vec(4,1,0),1.0 , new metal(vec(0.7,0.6,0.5),0.0));
     
    return new hitable_list(list,i);
}
vec color(const ray& r, hitable* world ,int depth){
    hit_record rec;
    if(world->hit(r, 0.001, FLT_MAX , rec)){
        ray scatterd;
        vec attenuation;
        vec emitted = rec.mat_type->emitted();

        if(depth > 0 && rec.mat_type->scatter(r, rec , attenuation , scatterd)){
            // COLOUR ATTENUATION: surface absorbs diff % of the light on every bounce so multiplying it by the attenuation
            return emitted + attenuation * color(scatterd , world , depth-1);
        }else{
            // base case : stop bouncing if we get trapped
            return emitted;
        }
    }
    // If it missed everything paint the background sky
    vec unit_direction = unit_vector(r.direction());
    float t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0-t)*vec(0.08, 0.08, 0.10) + t*vec(0.35, 0.29, 0.20);
}   
int main(){
    int nx = 400;
    int ny = 200 ;
    int ns = 50;
    cout << "P3\n" << nx <<" "<< ny<< "\n255\n";

    // vec look_from(13,3,3);
    // vec look_at(0,0,0);
    // float vfov = 20.0;
    vec look_from(8, 4.5, 12);     
    vec look_at(0, 0.2, 0);        
    float vfov = 20.0;            
    float aspect = float(nx) / float(ny) ;
    camera cam(look_from , look_at , vec(0,1,0) , vfov , aspect);
    hitable* world = random_scene();

    for(int  j = ny - 1 ; j>=0 ;j--){   // control rows --> start from top of the image and go to bottom row
        cerr << "\rScanlines remaining: " << j << ' ' << flush;
        for(int i = 0; i <= nx -1 ; i++){   // control cols --> start from left and go right
            vec col(0.0,0.0,0.0);
            for(int s = 0 ; s < ns ;s++){
                float u = float(i + random_double()) / float(nx);
                float v = float (j + random_double()) / float(ny);
                ray r = cam.get_ray(u,v);
                col = col + color(r,world,100);
            }
            col = col / float(ns); // store the avg of 100 rays for a 1 pixel


            // Gamma Correction : monitor does square of the brightness(which is in btw 0 and 1) before displaying;so it becomes very dark
            // so we sqrt the intial brightness and store it:so when monitors square the sqrt it cancels the effect
            col = vec(sqrt(col[0]),sqrt(col[1]),sqrt(col[2]));      
            
            // Clamp btw [0,1]
            float r = col[0] > 1.0f ? 1.0f : col[0];
            float g = col[1] > 1.0f ? 1.0f : col[1]; 
            float b = col[2] > 1.0f ? 1.0f : col[2];    

            
            int ir = int(255.99 * r);
            int ig = int(255.99 * g);
            int ib = int(255.99 * b);
            cout << ir << " " << ig << " " << ib << "\n";
        }
    }
    cerr << "\nDone.\n";
    return 0;
}
