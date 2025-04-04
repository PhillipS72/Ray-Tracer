#include "GeomSphere.h"

#include <iostream>
#include <utility>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

#include "Intersection.h"
#include "Ray.h"

std::vector<Intersection> GeomSphere::intersect(Ray &ray) {
    /**
     * NOTE: Ray is already transformed to the Model coordinate space.
     */
    using namespace glm;

    // vector to store the intersections
    std::vector<Intersection> intersections;

    vec3 sphere_center = vec3(0.0f);
    float radius = 1.0f;

    vec3 origin = ray.p0;
    vec3 direction = normalize(ray.dir);

    // Compute quadratic coefficients
    vec3 oc = origin - sphere_center;
    float A = dot(direction, direction);
    float B = 2.0f * dot(direction, oc);
    float C = dot(oc, oc) - (radius * radius);

    // Compute discriminant
    float discriminant = B * B - 4.0f * A * C;
    if (discriminant < 0) {
        return intersections;
    }

    float sqrtD = sqrt(discriminant);
    float t1 = (-B - sqrtD) / (2.0f * A);
    float t2 = (-B + sqrtD) / (2.0f * A);

    if (t1 > 0) {
        vec3 intersection_point = origin + t1 * direction;
        vec3 normal = normalize(intersection_point - sphere_center);
        intersections.push_back({t1, intersection_point, normal, this, nullptr});
    }
    if (t2 > 0) {
        vec3 intersection_point = origin + t2 * direction;
        vec3 normal = normalize(intersection_point - sphere_center);
        intersections.push_back({t2, intersection_point, normal, this, nullptr});
    }

    return intersections;
};