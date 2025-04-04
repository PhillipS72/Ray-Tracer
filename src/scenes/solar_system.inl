#define GLM_FORCE_RADIANS
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "GlossyMaterial.h"
#include "Sphere.h"
#include "Square.h"
#include "Tetrahedron.h"
#include "RayTracer.h"

using namespace glm;

Scene* solar_system() {
    // Define Materials
    std::shared_ptr<GlossyMaterial> sun_material = std::make_shared<GlossyMaterial>(vec3(1.0f, 0.8f, 0.2f), vec3(0.0f), 0.0);
    sun_material->convert_to_light(vec3(1.0f, 0.75f, 0.2f), vec3(200.0f)); // Sunlight

    std::shared_ptr<GlossyMaterial> planet_material1 = std::make_shared<GlossyMaterial>(vec3(0.0f, 0.5f, 1.0f), vec3(0.2f), 0.3f);
    std::shared_ptr<GlossyMaterial> planet_material2 = std::make_shared<GlossyMaterial>(vec3(0.2f, 1.0f, 0.2f), vec3(0.1f), 0.5f);
    std::shared_ptr<GlossyMaterial> moon_material = std::make_shared<GlossyMaterial>(vec3(0.7f), vec3(0.05f), 0.8f);
    std::shared_ptr<GlossyMaterial> ring_material = std::make_shared<GlossyMaterial>(vec3(0.7f, 0.7f, 0.7f), vec3(0.05f), 0.5f);

    // Root Node (Universe)
    std::unique_ptr<Node> root_node = std::make_unique<Node>();

    // Sun
    std::unique_ptr<Node> sun = std::make_unique<Node>();
    sun->model = std::make_unique<Sphere>(1.0f, vec3(0.0f, 0.0f, 0.0f), sun_material);
    root_node->childnodes.push_back(std::move(sun));
    root_node->childtransforms.push_back(translate(vec3(0.0f, 0.0f, -25.0f)) * scale(vec3(3.0f)));

    // Planet 1
    std::unique_ptr<Node> planet1 = std::make_unique<Node>();
    planet1->model = std::make_unique<Sphere>(0.7f, vec3(0.0f, 0.0f, 0.0f), planet_material1);
    root_node->childnodes.push_back(std::move(planet1));
    root_node->childtransforms.push_back(translate(vec3(6.0f, 0.0f, -27.0f)));

    // Planet 2 (With a Ring)
    std::unique_ptr<Node> planet2 = std::make_unique<Node>();
    planet2->model = std::make_unique<Sphere>(1.2f, vec3(0.0f, 0.0f, 0.0f), planet_material2);
    root_node->childnodes.push_back(std::move(planet2));
    root_node->childtransforms.push_back(translate(vec3(-6.0f, 0.0f, -23.0f)) * scale(vec3(1.25f)));

    // Rings around the planet
    std::unique_ptr<Node> planet_ring = std::make_unique<Node>();
    planet_ring->model = std::make_unique<Sphere>(1.5f, vec3(0.0f, 0.0f, 0.0f), ring_material); // Larger but thin sphere
    root_node->childnodes.push_back(std::move(planet_ring));
    root_node->childtransforms.push_back(
        translate(vec3(5.65f, 0.0f, -25.0f)) *  // Position at the planet
        rotate(radians(45.0f), vec3(0.0f, 0.0f, 1.0f)) *  // Tilt towards sun
        scale(vec3(2.0f, 0.01f, 2.0f)) // Make it a thin disk
    );

    //Rocket Materials
    std::shared_ptr<GlossyMaterial> rocket_body_material = std::make_shared<GlossyMaterial>(vec3(0.8f, 0.1f, 0.1f), vec3(0.2f), 0.5f); // Red body
    std::shared_ptr<GlossyMaterial> rocket_nose_material = std::make_shared<GlossyMaterial>(vec3(0.8f, 0.8f, 0.8f), vec3(0.1f), 0.6f); // White nose cone
    std::shared_ptr<GlossyMaterial> fin_material = std::make_shared<GlossyMaterial>(vec3(0.1f, 0.1f, 0.8f), vec3(0.1f), 0.4f); // Blue fins
    std::shared_ptr<GlossyMaterial> fire_material_1 = std::make_shared<GlossyMaterial>(vec3(1.0f, 0.9f, 0.3f), vec3(0.5f), 0.2f); // Bright yellow
    fire_material_1 = std::make_shared<GlossyMaterial>(vec3(1.0f, 0.9f, 0.3f), vec3(0.0f), 0.0f); // No gloss
    std::shared_ptr<GlossyMaterial> fire_material_2 = std::make_shared<GlossyMaterial>(vec3(1.0f, 0.5f, 0.1f), vec3(0.3f), 0.2f); // Orange
    fire_material_2 = std::make_shared<GlossyMaterial>(vec3(1.0f, 0.5f, 0.1f), vec3(0.0f), 0.0f);
    std::shared_ptr<GlossyMaterial> fire_material_3 = std::make_shared<GlossyMaterial>(vec3(0.8f, 0.2f, 0.05f), vec3(0.2f), 0.1f); // Deep red
    fire_material_3 = std::make_shared<GlossyMaterial>(vec3(0.8f, 0.2f, 0.05f), vec3(0.0f), 0.0f);
    std::shared_ptr<GlossyMaterial> window_material = std::make_shared<GlossyMaterial>(vec3(0.3f, 0.6f, 1.0f), vec3(0.3f), 0.8f);
    window_material->convert_to_light(vec3(0.3f, 0.6f, 1.0f), vec3(5.0f)); // Emits blue light


    // Root Node (Rocket Ship)
    std::unique_ptr<Node> rocket = std::make_unique<Node>();

    // Rocket Body
    std::unique_ptr<Node> rocket_body = std::make_unique<Node>();
    rocket_body->model = std::make_unique<Sphere>(0.5f, vec3(0.0f, 0.0f, 0.0f), planet_material1);
    root_node->childnodes.push_back(std::move(rocket_body));
    root_node->childtransforms.push_back(
        translate(vec3(0.0f, -1.5f, -15.0f)) *
        scale(vec3(0.5f, 2.0f, 0.5f))         
    );

    // Rocket Nose Cone
    std::unique_ptr<Node> rocket_nose = std::make_unique<Node>();
    rocket_nose->model = std::make_unique<Tetrahedron>(0.5f, rocket_nose_material);
    root_node->childnodes.push_back(std::move(rocket_nose));
    root_node->childtransforms.push_back(
        translate(vec3(-0.02f, 0.62f, -15.0f)) *  
        scale(vec3(0.75f, 1.0f, 1.0f)) *        
        rotate(radians(-130.0f), vec3(0.0f, 0.0f, 1.0f))
    );

    // Rocket Left Fin
    std::unique_ptr<Node> left_fin = std::make_unique<Node>();
    left_fin->model = std::make_unique<Tetrahedron>(0.3f, fin_material);
    root_node->childnodes.push_back(std::move(left_fin));
    root_node->childtransforms.push_back(
        translate(vec3(-0.1f, -0.55f, 0.0f)) * 
        rotate(radians(90.0f), vec3(0.0f, 0.0f, 1.0f)) *
        scale(vec3(1.0f, 0.4f, 1.5f))
    );

    // Rocket Right Fin
    std::unique_ptr<Node> right_fin = std::make_unique<Node>();
    right_fin->model = std::make_unique<Tetrahedron>(0.3f, fin_material);
    root_node->childnodes.push_back(std::move(right_fin));
    root_node->childtransforms.push_back(
        translate(vec3(0.1f, -0.55f, 0.0f)) * 
        rotate(radians(90.0f), vec3(0.0f, 0.0f, 1.0f)) *
        scale(vec3(1.0f, -0.4f, 1.5f))
    );

    // Add Rocket to Scene
    root_node->childnodes.push_back(std::move(rocket));
    root_node->childtransforms.push_back(translate(vec3(0.0f, 0.0f, 0.0f)));

    // Fire Effect at Bottom of Rocket
    std::unique_ptr<Node> fire_1 = std::make_unique<Node>();
    fire_1->model = std::make_unique<Sphere>(0.2f, vec3(0.0f, -0.8f, 0.0f), fire_material_1);
    root_node->childnodes.push_back(std::move(fire_1));
    root_node->childtransforms.push_back(translate(vec3(0.0f, -4.0f, -15.0f)) * scale(vec3(0.75f, 0.75f, 0.75f)));
    std::unique_ptr<Node> fire_2 = std::make_unique<Node>();
    fire_2->model = std::make_unique<Sphere>(0.15f, vec3(0.0f, -1.0f, 0.0f), fire_material_2);
    root_node->childnodes.push_back(std::move(fire_2));
    root_node->childtransforms.push_back(translate(vec3(0.0f, -5.0f, -15.0f)) * scale(vec3(1.0f, 1.0f, 1.0f)));
    std::unique_ptr<Node> fire_3 = std::make_unique<Node>();
    fire_3->model = std::make_unique<Sphere>(0.1f, vec3(0.0f, -1.2f, 0.0f), fire_material_3);
    root_node->childnodes.push_back(std::move(fire_3));
    root_node->childtransforms.push_back(translate(vec3(0.0f, -6.5f, -15.0f)) * scale(vec3(1.5f, 1.5f, 1.5f)));

    // Rocket Window
    std::unique_ptr<Node> rocket_window = std::make_unique<Node>();
    rocket_window->model = std::make_unique<Sphere>(0.15f, vec3(0.0f, 0.0f, 0.0f), window_material);
    root_node->childnodes.push_back(std::move(rocket_window));
    root_node->childtransforms.push_back(
        translate(vec3(0.0f, -1.0f, -14.0f)) *
        scale(vec3(0.25f, 0.25f, 0.1f))
    );

    // Moon
    std::unique_ptr<Node> moon = std::make_unique<Node>();
    moon->model = std::make_unique<Sphere>(1.0f, vec3(8.0f, -4.0f, -30.0f), moon_material);
    root_node->childnodes.push_back(std::move(moon));

    // Initialize scene
    std::unique_ptr<Scene> scene = std::make_unique<Scene>(std::move(root_node));
    scene->is_space_scene = true; // Set background to black
    return scene.release();
}
