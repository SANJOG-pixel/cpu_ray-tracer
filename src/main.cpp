    #include<iostream>
    #include<vector>
    #include<algorithm>
    #include "ray.h"
    #include "sphere.h"
    #include "camera.h"
    #include "hitablelist.h"
    #include "material.h"
    #include <cfloat>   
    #include<cstdlib>
    using namespace std;

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
        list[i++] = new sphere(vec(0, 1, 0), 1.0, new glowing_metal(vec(15.0, 12.5, 3.8),vec(0.95, 0.90, 0.70),0.08));          
        list[i++] = new sphere(vec(-4,1,0),1.0, new noise_lambertian(vec(0.65,0.32,0.10) , 6.0f , 0.5f));
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

    vector<vec> extract_bright_pixels(const vector<vec>& framebuffer , int nx , int ny ,float threshold){
        vector<vec> bright(framebuffer.size() , vec(0.0f,0.0f,0.0f));
        for(int y = 0 ; y < ny ; y++){
            for(int x = 0 ; x < nx ; x++){
                int idx = nx * y + x;

                float max_channels= max(framebuffer[idx][0] , max(framebuffer[idx][1] , framebuffer[idx][2]));

                if(max_channels > threshold) bright[idx] = framebuffer[idx];
                else{
                    bright[idx] = vec(0.0f,0.0f,0.0f);
                }
            }
        }
        return bright;
    }

    vector<vec> horizontal_blur(const vector<vec>& image , int nx , int ny){
        vector<vec> result(image.size() , vec(0.0f,0.0f,0.0f));
        const float kernel[9] = {
            1.0f / 56.0f ,
            4.0f / 56.0f ,
            7.0f / 56.0f , 
            10.0f / 56.0f ,
            12.0f / 56.0f ,
            10.0f / 56.0f ,
            7.0f / 56.0f ,
            4.0f / 56.0f ,
            1.0f / 56.0f ,
        };
        for(int y = 0; y < ny ; y++){
            for(int x = 0 ; x < nx ; x++){
                vec sum(0.0f,0.0f,0.0f);

                for(int  k = -3 ; k <= 3 ; k++){
                    int new_x = x + k;
                
                    if(new_x < 0) new_x = 0;
                    if(new_x >= nx) new_x = nx -1;

                    sum += image[y * nx + new_x] * kernel[k+3];
                }
                result[y * nx + x] = sum;
            }
        }
        return result;
    }
    vector<vec> vertical_blur(const vector<vec>& image , int nx , int ny){
        vector<vec> result(image.size() , vec(0.0f,0.0f,0.0f));
        const float kernel[9] = {
            1.0f / 56.0f ,
            4.0f / 56.0f ,
            7.0f / 56.0f , 
            10.0f / 56.0f ,
            12.0f / 56.0f ,
            10.0f / 56.0f ,
            7.0f / 56.0f ,
            4.0f / 56.0f ,
            1.0f / 56.0f ,
        };
        for(int y = 0; y < ny ; y++){
            for(int x = 0 ; x < nx ; x++){
                vec sum(0.0f,0.0f,0.0f);

                for(int  k = -3 ; k <= 3 ; k++){
                    int new_y = y + k;
                
                    if(new_y < 0) new_y = 0;
                    if(new_y >= ny) new_y = ny -1;

                    sum += image[new_y * nx + x] * kernel[k+3];
                }
                result[y * nx + x] = sum;
            }
        }
        return result;
    }
    int main(){
        const int nx = 2500;
        const int ny = 1600 ;
        const int ns = 150;

        vec look_from(8, 4.5, 12);     
        vec look_at(0, 0.2, 0);        
        float vfov = 20.0;            

        const float aspect = float(nx) / float(ny) ;
        // Depth of field
        const float focus_dist = 15.0f ;
        const float aperture  = 0.18f;

        camera cam(look_from , look_at , vec(0,1,0) , vfov , aspect, aperture , focus_dist);
        hitable* world = random_scene();
        vector<vec> framebuffer(nx*ny , vec(0.0f,0.0f,0.0f));

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
                framebuffer[j * nx + i] = col ;
            }
        }
        cerr << "\nRendering complete.\n";

        const float bloom_threshold = 0.5f;
        const float bloom_strength = 2.5f ;

        // extract the bright pixels
        vector<vec> bright = extract_bright_pixels(framebuffer , nx , ny, bloom_threshold);
        vector<vec> blurred_h1 = horizontal_blur(bright, nx, ny);
        vector<vec> blurred_v1 = vertical_blur(blurred_h1, nx, ny);
        vector<vec> blurred_h2 = horizontal_blur(blurred_v1, nx, ny);  // 2nd pass
        vector<vec> blurred_v2 = vertical_blur(blurred_h2, nx, ny);    // 2nd pass

        // add bloom back to HDR image
        for(int  i = 0 ; i < nx*ny ; i++){
            framebuffer[i] = framebuffer[i] + bloom_strength*blurred_v2[i];
        }
        cerr << "Bloom applied.\n";

        cout << "P3\n" << nx << " " << ny << "\n255\n";

        for(int j = ny-1 ; j >=0 ; j--){
            for(int  i = 0 ; i < nx ; i++){
                vec col = framebuffer[j * nx + i];
                // Reinhard tone mapping formula: f(x) = x / (1 + x)
                col = vec(
                    col[0] / (1.0f + col[0]),
                    col[1] / (1.0f + col[1]),
                    col[2] / (1.0f + col[2])
                );
                // Gamma Correction : monitor does square of the brightness(which is in btw 0 and 1) before displaying;so it becomes very dark
                // so we sqrt the intial brightness and store it:so when monitors square the sqrt it cancels the effect
                col = vec(sqrt(col[0]),sqrt(col[1]),sqrt(col[2]));      
                
                // Clamp btw [0,1]
                auto clamp = [] (float x){
                    if(x < 0) return 0.0f;
                    if(x > 1) return 1.0f;
                    return x;
                };

                float r = clamp(col[0]);
                float g = clamp(col[1]); 
                float b = clamp(col[2]);    
                
                int ir = int(255.99 * r);
                int ig = int(255.99 * g);
                int ib = int(255.99 * b);
                cout << ir << " " << ig << " " << ib << "\n";
            }
        }
        cerr << "\nDone.\n";
        return 0;
    }
