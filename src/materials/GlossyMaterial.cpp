#define GLM_ENABLE_EXPERIMENTAL
#include "GlossyMaterial.h"

#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>

using namespace glm;

Ray GlossyMaterial::sample_ray_and_update_radiance(Ray &ray, Intersection &intersection) {
    /**
     * Calculate the next ray after intersection with the model.
     * This will be used for recursive ray tracing.
     */

    // Decide if diffuse or specular reflection
    float random = linearRand(0.0f, 1.0f);
    vec3 normal = intersection.normal;
    vec3 point = intersection.point;

    // Diffuse reflection
    if (random > shininess) {
        // Step 1: Sample ray direction
        /**
         * TODO: Task 6.1
         * Implement cosine-weighted hemisphere sampling
         */
        // cosin sample next ray
        float s = linearRand(0.0f, 1.0f);
        float t = linearRand(0.0f, 1.0f);

        // TODO: Update u, v based on Equation (8) in handout
        float u = 2.0f * M_PI * s;
        float v = sqrt(1.0 - t);

        vec3 d_local = vec3(
            v * cos(u),
            sqrt(t),
            v * sin(u)
        );

        // Step 2: Transform local d to world coordinates
        vec3 tangent, bitangent;
        if (fabs(normal.x) > fabs(normal.z)) {
            tangent = normalize(cross(normal, vec3(0.0f, 0.0f, 1.0f)));
        }
        else {
            tangent = normalize(cross(normal, vec3(1.0f, 0.0f, 0.0f)));
        }
        bitangent = normalize(cross(normal, tangent));

        vec3 new_dir = normalize(
            d_local.x * tangent +
            d_local.y * normal +
            d_local.z * bitangent
        );

        // Step 3: Update W_wip (radiance weight)
        float lambertian_factor = max(dot(new_dir, normal), 0.0f);
        vec3 W_diffuse = diffuse * lambertian_factor;

        ray.W_wip *= W_diffuse;

        // Step 4: Update ray position and direction
        ray.p0 = point + 0.001f * normal;  // Offset to avoid self-intersections
        ray.dir = new_dir;
        ray.is_diffuse_bounce = true;
        ray.n_bounces++;

        return ray;
    }

    // Specular Reflection

    vec3 reflection_dir = reflect(ray.dir, normal);
    vec3 W_specular = specular;

    ray.W_wip *= W_specular;
    ray.p0 = point + 0.001f * normal;
    ray.dir = reflection_dir;
    ray.is_diffuse_bounce = false;
    ray.n_bounces++;

    return ray;
}

glm::vec3 GlossyMaterial::get_direct_lighting(Intersection &intersection, Scene const &scene) {
    vec3 cummulative_direct_light = vec3(0.0f);

    for (unsigned int idx = 0; idx < scene.light_sources.size(); idx++) {
        // Avoid self-intersection with light source
        if (scene.light_sources[idx] == intersection.model) continue;

        // Get light source position
        vec3 light_pos = scene.light_sources[idx]->get_surface_point();

        // Compute light direction (l_ell)
        vec3 light_dir = normalize(light_pos - intersection.point);

        Ray shadow_ray;
        shadow_ray.p0 = intersection.point + 0.01f * intersection.normal;
        shadow_ray.dir = light_dir;

        bool is_in_shadow = false;
        for (unsigned int idx2 = 0; idx2 < scene.models.size(); idx2++) {
            scene.models[idx2]->intersect(shadow_ray);
        }

        // Find closest intersection along shadow ray
        Intersection closest_intersection;
        closest_intersection.t = std::numeric_limits<float>::max();
        for (auto &inter : shadow_ray.intersections) {
            if (inter.t < closest_intersection.t) {
                closest_intersection = inter;
            }
        }

        if (closest_intersection.model != scene.light_sources[idx]) {
            is_in_shadow = true;
        }

        // Compute Lambertian lighting
        if (!is_in_shadow) {
            vec3 light_emission = scene.light_sources[idx]->material->emission;
            float attenuation_factor = scene.light_sources[idx]->material->get_light_attenuation_factor(closest_intersection.t);
            float lambertian = max(dot(intersection.normal, light_dir), 0.0f);
            vec3 direct_light = light_emission * lambertian / attenuation_factor;
            
            cummulative_direct_light += direct_light;
        }
    }

    // Apply C_diffuse once, after summation
    return this->diffuse * cummulative_direct_light;

}

vec3 GlossyMaterial::color_of_last_bounce(Ray &ray, Intersection &intersection, Scene const &scene) {
    using namespace glm;
    /**
     * Color after last bounce will be `W_wip * last_bounce_color`
     * We shade last bounce for this Glossy material using direct diffuse lighting
     */

    vec3 direct_diff_light = this->get_direct_lighting(intersection, scene);

    return ray.W_wip * diffuse * (1 - shininess) * direct_diff_light;
}