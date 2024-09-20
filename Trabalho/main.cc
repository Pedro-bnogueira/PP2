#include "rtweekend.h"

#include "bvh.h"
#include "camera.h"
#include "constant_medium.h"
#include "hittable.h"
#include "hittable_list.h"
#include "material.h"
#include "quad.h"
#include "sphere.h"
#include "texture.h"

void my_scene(int image_width, int samples_per_pixel, int max_depth) {
    
    hittable_list boxes1;

    auto ground = make_shared<lambertian>(color(0.48, 0.83, 0.53));

    int boxes_per_side = 20;
    for (int i = 0; i < boxes_per_side; i++) {
        for (int j = 0; j < boxes_per_side; j++) {
            auto w = 100.0;
            auto x0 = -1000.0 + i*w;
            auto z0 = -1000.0 + j*w;
            auto y0 = 0.0, y2 = 555.0;
            auto x1 = x0 + w;
            auto y1 = random_double(1,101), y3 = 555.0 - random_double(1,101);
            auto z1 = z0 + w;

            boxes1.add(box(point3(x0,y0,z0), point3(x1,y1,z1), ground));
            boxes1.add(box(point3(x0,y2,z0), point3(x1,y3,z1), ground));
        }
    }
    
    hittable_list world;

    world.add(make_shared<bvh_node>(boxes1));

    // Esfera de Luz
    auto light = make_shared<diffuse_light>(color(7, 7, 7));

    world.add(make_shared<sphere>(point3(-600,70,400), 50, light));
    world.add(make_shared<sphere>(point3(110,110,115), 70, light));
    world.add(make_shared<sphere>(point3(600,80,150), 40, light));

    world.add(make_shared<sphere>(point3(-600,400,400), 40, light));
    world.add(make_shared<sphere>(point3(110,500,300), 70, light));
    world.add(make_shared<sphere>(point3(600,500,150), 40, light));

    // Pequenas esferas de luz intercaladas com esferas com olhos
    world.add(make_shared<sphere>(point3(-500,200,1000), 20, light));
    world.add(make_shared<sphere>(point3(-300,400,800), 20, light));
    world.add(make_shared<sphere>(point3(-100,200,1000), 20, light));
    world.add(make_shared<sphere>(point3(100,400,800), 20, light));
    world.add(make_shared<sphere>(point3(300,200,1000), 20, light));
    world.add(make_shared<sphere>(point3(500,400,800), 20, light));
    world.add(make_shared<sphere>(point3(700,200,800), 20, light));
    world.add(make_shared<sphere>(point3(900,400,800), 20, light));

    // Esfera com olhos
    auto eye_texture = make_shared<image_texture>("eye.png");
    auto eye_surface = make_shared<lambertian>(eye_texture);

    world.add(make_shared<sphere>(point3(-500,400,1000), 70, eye_surface));
    world.add(make_shared<sphere>(point3(-300,200,800), 70, eye_surface));
    world.add(make_shared<sphere>(point3(-100,400,1000), 70, eye_surface));
    world.add(make_shared<sphere>(point3(100,200,800), 70, eye_surface));
    world.add(make_shared<sphere>(point3(300,400,1000), 70, eye_surface));
    world.add(make_shared<sphere>(point3(500,200,800), 70, eye_surface));
    world.add(make_shared<sphere>(point3(700,400,800), 70, eye_surface));
    world.add(make_shared<sphere>(point3(900,200,800), 70, eye_surface));

    // Esfera de Metal
    world.add(make_shared<sphere>(point3(-100, 100, 200), 60, make_shared<dielectric>(1.5)));
    world.add(make_shared<sphere>(
        point3(0, 0, 0), 40, make_shared<metal>(color(0.8, 0.8, 0.9), 1.0)
    ));

    // Esfera com Motion Blur
    auto center1 = point3(-400,300,250);
    auto center2 = center1 + vec3(30,0,0);
    auto sphere_material = make_shared<lambertian>(color(0.7, 0.3, 0.1));
    world.add(make_shared<sphere>(center1, center2, 50, eye_surface));

    // Esfera de vidro
    auto boundary = make_shared<sphere>(point3(360,150,145), 70, make_shared<dielectric>(1.5));
    world.add(boundary);
    world.add(make_shared<constant_medium>(boundary, 0.2, color(0.2, 0.4, 0.9)));
    boundary = make_shared<sphere>(point3(0,0,0), 5000, make_shared<dielectric>(1.5));
    world.add(make_shared<constant_medium>(boundary, .0001, color(1,1,1)));

    // Quadrado com circulos
    hittable_list boxes2;
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    auto crimson = make_shared<lambertian>(color(0.9, 0.1, 0.3));
    int ns = 1000;

    for (int j = 0; j < ns; j++) {
        boxes2.add(make_shared<sphere>(point3::random(0,165), 10, crimson));
    }

    world.add(make_shared<translate>(
        make_shared<rotate_y>(
            make_shared<bvh_node>(boxes2), 15),
            vec3(-500,270,395)
        )
    );

    camera cam;

    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = image_width;
    cam.samples_per_pixel = samples_per_pixel;
    cam.max_depth         = max_depth;
    cam.background        = color(0,0,0);

    cam.vfov     = 40;
    cam.lookfrom = point3(-850, 278, 650);
    cam.lookat   = point3(2000, 200, 0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0;

    cam.render(world);
}

void my_scene2(int image_width, int samples_per_pixel, int max_depth) {
    hittable_list boxes1;

    auto ground = make_shared<lambertian>(color(0.48, 0.83, 0.53));

    int boxes_per_side = 20;
    for (int i = 0; i < boxes_per_side; i++) {
        for (int j = 0; j < boxes_per_side; j++) {
            auto w = 100.0;
            auto x0 = -1000.0 + i*w;
            auto z0 = -1000.0 + j*w;
            auto y0 = 0.0, y2 = 555.0;
            auto x1 = x0 + w;
            auto y1 = random_double(1,101), y3 = 555.0 - random_double(1,101);
            auto z1 = z0 + w;

            boxes1.add(box(point3(x0,y0,z0), point3(x1,y1,z1), ground));
            boxes1.add(box(point3(x0,y2,z0), point3(x1,y3,z1), ground));
        }
    }

    hittable_list world;

    world.add(make_shared<bvh_node>(boxes1));

    // Esfera de Luz
    auto light = make_shared<diffuse_light>(color(7, 7, 7));

    world.add(make_shared<sphere>(point3(-600,70,400), 50, light));
    world.add(make_shared<sphere>(point3(110,110,115), 70, light));
    world.add(make_shared<sphere>(point3(550,120,550), 20, light));
    world.add(make_shared<sphere>(point3(600,80,150), 40, light));

    world.add(make_shared<sphere>(point3(-600,400,400), 40, light));
    world.add(make_shared<sphere>(point3(110,500,300), 70, light));
    world.add(make_shared<sphere>(point3(550,400,800), 20, light));
    world.add(make_shared<sphere>(point3(600,500,150), 40, light));
    
    // Esfera com olhos
    auto eye_texture = make_shared<image_texture>("eye.png");
    auto eye_surface = make_shared<lambertian>(eye_texture);

    world.add(make_shared<sphere>(point3(-500,400,1000), 70, eye_surface));
    world.add(make_shared<sphere>(point3(-300,200,800), 70, eye_surface));
    world.add(make_shared<sphere>(point3(-100,400,1000), 70, eye_surface));
    world.add(make_shared<sphere>(point3(100,200,800), 70, eye_surface));
    world.add(make_shared<sphere>(point3(300,400,1000), 70, eye_surface));
    world.add(make_shared<sphere>(point3(500,200,800), 70, eye_surface));
    world.add(make_shared<sphere>(point3(700,400,800), 70, eye_surface));
    world.add(make_shared<sphere>(point3(900,200,800), 70, eye_surface));

    // Esfera de Metal
    world.add(make_shared<sphere>(point3(-100, 100, 200), 60, make_shared<dielectric>(1.5)));
    world.add(make_shared<sphere>(
        point3(0, 0, 0), 40, make_shared<metal>(color(0.8, 0.8, 0.9), 1.0)
    ));

    // Esfera doida
    auto center1 = point3(-400,300,250);
    auto center2 = center1 + vec3(30,0,0);
    auto sphere_material = make_shared<lambertian>(color(0.7, 0.3, 0.1));
    world.add(make_shared<sphere>(center1, center2, 50, eye_surface));

    // Esfera de vidro
    auto boundary = make_shared<sphere>(point3(360,150,145), 70, make_shared<dielectric>(1.5));
    world.add(boundary);
    world.add(make_shared<constant_medium>(boundary, 0.2, color(0.2, 0.4, 0.9)));
    boundary = make_shared<sphere>(point3(0,0,0), 5000, make_shared<dielectric>(1.5));
    world.add(make_shared<constant_medium>(boundary, .0001, color(1,1,1)));

    // Quadrado com circulos
    hittable_list boxes2;
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    auto crimson = make_shared<lambertian>(color(0.9, 0.1, 0.3));
    int ns = 1000;

    for (int j = 0; j < ns; j++) {
        boxes2.add(make_shared<sphere>(point3::random(0,165), 10, crimson));
    }

    world.add(make_shared<translate>(
        make_shared<rotate_y>(
            make_shared<bvh_node>(boxes2), 15),
            vec3(-500,270,395)
        )
    );

    camera cam;

    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = image_width;
    cam.samples_per_pixel = samples_per_pixel;
    cam.max_depth         = max_depth;
    cam.background        = color(0,0,0);

    cam.vfov     = 40;
    cam.lookfrom = point3(478, 278, -600);
    cam.lookat   = point3(278, 278, 0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0;

    cam.render(world);
}

void my_scene3(int image_width, int samples_per_pixel, int max_depth) {
    hittable_list boxes1;

    auto ground = make_shared<lambertian>(color(0.48, 0.83, 0.53));

    int boxes_per_side = 20;
    for (int i = 0; i < boxes_per_side; i++) {
        for (int j = 0; j < boxes_per_side; j++) {
            auto w = 100.0;
            auto x0 = -1000.0 + i*w;
            auto z0 = -1000.0 + j*w;
            auto y0 = 0.0, y2 = 555.0;
            auto x1 = x0 + w;
            auto y1 = random_double(1,101), y3 = 555.0 - random_double(1,101);
            auto z1 = z0 + w;

            boxes1.add(box(point3(x0,y0,z0), point3(x1,y1,z1), ground));
            boxes1.add(box(point3(x0,y2,z0), point3(x1,y3,z1), ground));
        }
    }
    
    hittable_list world;

    world.add(make_shared<bvh_node>(boxes1));

    // Esfera de Luz
    auto light = make_shared<diffuse_light>(color(7, 7, 7));

    world.add(make_shared<sphere>(point3(-600,70,400), 50, light));
    world.add(make_shared<sphere>(point3(110,110,115), 70, light));
    world.add(make_shared<sphere>(point3(600,80,150), 40, light));

    world.add(make_shared<sphere>(point3(-600,400,400), 40, light));
    world.add(make_shared<sphere>(point3(110,500,300), 70, light));
    world.add(make_shared<sphere>(point3(600,500,150), 40, light));

    // Pequenas esferas de luz intercaladas com esferas com olhos
    world.add(make_shared<sphere>(point3(-500,200,1000), 20, light));
    world.add(make_shared<sphere>(point3(-300,400,800), 20, light));
    world.add(make_shared<sphere>(point3(-100,200,1000), 20, light));
    world.add(make_shared<sphere>(point3(100,400,800), 20, light));
    world.add(make_shared<sphere>(point3(300,200,1000), 20, light));
    world.add(make_shared<sphere>(point3(500,400,800), 20, light));
    world.add(make_shared<sphere>(point3(700,200,800), 20, light));
    world.add(make_shared<sphere>(point3(900,400,800), 20, light));

    // Esfera com olhos
    auto eye_texture = make_shared<image_texture>("eye.png");
    auto eye_surface = make_shared<lambertian>(eye_texture);

    world.add(make_shared<sphere>(point3(-500,400,1000), 70, eye_surface));
    world.add(make_shared<sphere>(point3(-300,200,800), 70, eye_surface));
    world.add(make_shared<sphere>(point3(-100,400,1000), 70, eye_surface));
    world.add(make_shared<sphere>(point3(100,200,800), 70, eye_surface));
    world.add(make_shared<sphere>(point3(300,400,1000), 70, eye_surface));
    world.add(make_shared<sphere>(point3(500,200,800), 70, eye_surface));
    world.add(make_shared<sphere>(point3(700,400,800), 70, eye_surface));
    world.add(make_shared<sphere>(point3(900,200,800), 70, eye_surface));

    // Esfera de Metal
    world.add(make_shared<sphere>(point3(-100, 100, 200), 60, make_shared<dielectric>(1.5)));
    world.add(make_shared<sphere>(
        point3(0, 0, 0), 40, make_shared<metal>(color(0.8, 0.8, 0.9), 1.0)
    ));

    // Esfera com Motion Blur
    auto center1 = point3(-400,300,250);
    auto center2 = center1 + vec3(30,0,0);
    auto sphere_material = make_shared<lambertian>(color(0.7, 0.3, 0.1));
    world.add(make_shared<sphere>(center1, center2, 50, eye_surface));

    // Esfera de vidro
    auto boundary = make_shared<sphere>(point3(360,150,145), 70, make_shared<dielectric>(1.5));
    world.add(boundary);
    world.add(make_shared<constant_medium>(boundary, 0.2, color(0.2, 0.4, 0.9)));
    boundary = make_shared<sphere>(point3(0,0,0), 5000, make_shared<dielectric>(1.5));
    world.add(make_shared<constant_medium>(boundary, .0001, color(1,1,1)));

    // Quadrado com circulos
    hittable_list boxes2;
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    auto crimson = make_shared<lambertian>(color(0.9, 0.1, 0.3));
    int ns = 1000;

    for (int j = 0; j < ns; j++) {
        boxes2.add(make_shared<sphere>(point3::random(0,165), 10, crimson));
    }

    world.add(make_shared<translate>(
        make_shared<rotate_y>(
            make_shared<bvh_node>(boxes2), 15),
            vec3(-500,270,395)
        )
    );

    camera cam;

    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = image_width;
    cam.samples_per_pixel = samples_per_pixel;
    cam.max_depth         = max_depth;
    cam.background        = color(0,0,0);

    cam.vfov     = 40;
    cam.lookfrom = point3(850, 278, 650);
    cam.lookat   = point3(-2000, 200, 0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0;

    cam.render(world);
}

void my_scene4(int image_width, int samples_per_pixel, int max_depth) {
    hittable_list boxes1;

    auto ground = make_shared<lambertian>(color(0.48, 0.83, 0.53));

    int boxes_per_side = 20;
    for (int i = 0; i < boxes_per_side; i++) {
        for (int j = 0; j < boxes_per_side; j++) {
            auto w = 100.0;
            auto x0 = -1000.0 + i*w;
            auto z0 = -1000.0 + j*w;
            auto y0 = 0.0, y2 = 555.0;
            auto x1 = x0 + w;
            auto y1 = random_double(1,101), y3 = 555.0 - random_double(1,101);
            auto z1 = z0 + w;

            boxes1.add(box(point3(x0,y0,z0), point3(x1,y1,z1), ground));
            boxes1.add(box(point3(x0,y2,z0), point3(x1,y3,z1), ground));
        }
    }

    hittable_list world;

    world.add(make_shared<bvh_node>(boxes1));

    // Esfera de Luz
    auto light = make_shared<diffuse_light>(color(7, 7, 7));

    world.add(make_shared<sphere>(point3(-600,70,400), 50, light));
    world.add(make_shared<sphere>(point3(110,110,115), 70, light));
    world.add(make_shared<sphere>(point3(550,120,550), 20, light));
    world.add(make_shared<sphere>(point3(600,80,150), 40, light));

    world.add(make_shared<sphere>(point3(-600,400,400), 40, light));
    world.add(make_shared<sphere>(point3(110,500,300), 70, light));
    world.add(make_shared<sphere>(point3(550,400,800), 20, light));
    world.add(make_shared<sphere>(point3(600,500,150), 40, light));
    
    // Esfera com olhos
    auto eye_texture = make_shared<image_texture>("eye.png");
    auto eye_surface = make_shared<lambertian>(eye_texture);

    world.add(make_shared<sphere>(point3(-500,400,1000), 70, eye_surface));
    world.add(make_shared<sphere>(point3(-300,200,800), 70, eye_surface));
    world.add(make_shared<sphere>(point3(-100,400,1000), 70, eye_surface));
    world.add(make_shared<sphere>(point3(100,200,800), 70, eye_surface));
    world.add(make_shared<sphere>(point3(300,400,1000), 70, eye_surface));
    world.add(make_shared<sphere>(point3(500,200,800), 70, eye_surface));
    world.add(make_shared<sphere>(point3(700,400,800), 70, eye_surface));
    world.add(make_shared<sphere>(point3(900,200,800), 70, eye_surface));

    // Esfera de Metal
    world.add(make_shared<sphere>(point3(-100, 100, 200), 60, make_shared<dielectric>(1.5)));
    world.add(make_shared<sphere>(
        point3(0, 0, 0), 40, make_shared<metal>(color(0.8, 0.8, 0.9), 1.0)
    ));

    // Esfera doida
    auto center1 = point3(-400,300,250);
    auto center2 = center1 + vec3(30,0,0);
    auto sphere_material = make_shared<lambertian>(color(0.7, 0.3, 0.1));
    world.add(make_shared<sphere>(center1, center2, 50, eye_surface));

    // Esfera de vidro
    auto boundary = make_shared<sphere>(point3(360,150,145), 70, make_shared<dielectric>(1.5));
    world.add(boundary);
    world.add(make_shared<constant_medium>(boundary, 0.2, color(0.2, 0.4, 0.9)));
    boundary = make_shared<sphere>(point3(0,0,0), 5000, make_shared<dielectric>(1.5));
    world.add(make_shared<constant_medium>(boundary, .0001, color(1,1,1)));

    // Quadrado com circulos
    hittable_list boxes2;
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    auto crimson = make_shared<lambertian>(color(0.9, 0.1, 0.3));
    int ns = 1000;

    for (int j = 0; j < ns; j++) {
        boxes2.add(make_shared<sphere>(point3::random(0,165), 10, crimson));
    }

    world.add(make_shared<translate>(
        make_shared<rotate_y>(
            make_shared<bvh_node>(boxes2), 15),
            vec3(-500,270,395)
        )
    );

    camera cam;

    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = image_width;
    cam.samples_per_pixel = samples_per_pixel;
    cam.max_depth         = max_depth;
    cam.background        = color(0,0,0);

    cam.vfov     = 40;
    cam.lookfrom = point3(-478, 278, -600);
    cam.lookat   = point3(-278, 278, 0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0;

    cam.render(world);
}

int main() {
    switch (4) {
        case 1: my_scene(800, 1000,  10);    break;
        case 2: my_scene2(800, 1000, 10);    break;
        case 3: my_scene3(800, 1000, 10);    break; 
        case 4: my_scene4(800, 1000, 10);    break;
    }
}