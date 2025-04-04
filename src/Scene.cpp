#include <iostream>
#include <stack>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/noise.hpp>
#include <glm/gtc/random.hpp>
#include <random>

#include "ModelBase.h"
#include "Ray.h"
#include "Scene.h"

Scene::Scene(std::unique_ptr<Node> root_node) {
    /**
     * rootNode is a root node of transformation tree defining how geometries as
     * coverted to the desired scene. We need to process these tree to convert
     * Intial geometries into world view coordinates.
     */

    // Stacks for DFS
    // Each element in stack represents a node and cumulative trasformation matrix defined
    // by all nodes above current node in the tree
    std::stack<std::pair<Node *, glm::mat4>>
        node_stack;

    // start with root node
    node_stack.push(std::make_pair(root_node.get(), glm::mat4(1.0f)));

    while (!node_stack.empty()) {
        // get top node
        Node *curr_node = node_stack.top().first;
        glm::mat4 curr_mat = node_stack.top().second;
        node_stack.pop();

        // check if current node is leaf node
        if (curr_node->model.get() != nullptr) {
            // add to light sources if emmission is non zero
            if (curr_node->model->is_light_source()) {
                light_sources.push_back(curr_node->model.get());
            }

            // update transformation matrix of model
            curr_node->model->transformation_matrix = glm::mat4(curr_mat);
            curr_node->model->inverse_transform_matrix = glm::inverse(glm::mat4(curr_mat));
            models.push_back(std::move(curr_node->model));

            continue;
        }

        // iterate child nodes and update stack
        for (unsigned int idx = 0; idx < curr_node->childnodes.size(); idx++) {
            // calculate cummulative transformation matrix
            glm::mat4 cumm_mat = curr_mat * curr_node->childtransforms[idx];

            // update stack
            node_stack.push(std::make_pair(curr_node->childnodes[idx].get(), cumm_mat));
        }
    }
}

void Scene::intersect(Ray &ray) const {
    using namespace glm;

    ray.intersections.clear();

    // Check intersection with all models
    for (unsigned int idx = 0; idx < models.size(); idx++) {
        models[idx]->intersect(ray);
    }

    // Find the closest intersection
    Intersection intersection;
    intersection.t = std::numeric_limits<float>::max();
    for (unsigned int idx = 0; idx < ray.intersections.size(); idx++) {
        if (ray.intersections[idx].t < intersection.t)
            intersection = ray.intersections[idx];
    }

    // No intersection -> Return sky color
    if (ray.intersections.empty()) {
        ray.color = ray.W_wip * this->get_sky_color(ray);
        ray.isWip = false;
        return;
    }

    // Normal shading mode
    if (this->shading_mode == ShadingMode::NORMAL) {
        ray.color = RGB_to_Linear(0.4f * intersection.normal + 0.6f);
        ray.isWip = false;
        return;
    }

    // If we hit a light source
    if (intersection.model->is_light_source()) {
        ray.color = ray.W_wip * intersection.model->material->emission;
        ray.isWip = false;
        return;
    }

    // Russian Roulette Termination
    static const float lambda = 0.8f; // Probability of survival
    float survival_probability = pow((1.0f - lambda), ray.n_bounces - 1) * lambda;
    
    if (glm::linearRand(0.0f, 1.0f) > lambda) {
        ray.isWip = false;
        return;
    }

    ray.W_wip /= survival_probability;

    vec3 last_bounce_color = intersection.model->material->color_of_last_bounce(ray, intersection, *this);
    ray.color += ray.W_wip * last_bounce_color;

    ray = intersection.model->material->sample_ray_and_update_radiance(ray, intersection);
    ray.n_bounces++;
}

glm::vec3 Scene::get_sky_color(Ray &ray) const {
    /**
     * This function maps a ray pointing towards sky in given direction
     * to a vertical gradient between two colors
     */

    using namespace glm;

    if (this->is_space_scene) {
        vec3 space_color_top = vec3(0.01f, 0.01f, 0.05f);
        vec3 space_color_bottom = vec3(0.0f, 0.0f, 0.0f);

        float alpha = 0.5f * (ray.dir.y + 1.0f);
        vec3 background_color = mix(space_color_bottom, space_color_top, alpha);

        // Procedural Stars
        float star_density = 0.03f;
        float noise_value = glm::perlin(vec3(ray.dir * 100.0f));

        noise_value = 0.5f * (noise_value + 1.0f);

        // Make sure stars appear frequently enough
        if (noise_value > (1.0f - star_density)) {
            float brightness = mix(1.0f, 2.5f, noise_value);
            return mix(background_color, vec3(brightness), 0.95f);
        }

        return background_color;
    }

    // colors for gradient
    vec3 start_color = RGB_to_Linear(vec3(0.227f, 0.392f, 1.0f));
    vec3 end_color = RGB_to_Linear(vec3(0.9f));

    // linear interpolate based on y coordinate
    float alpha = 0.5f * (ray.dir.y + 1.0f);
    return (1 - alpha) * start_color + alpha * end_color;
}