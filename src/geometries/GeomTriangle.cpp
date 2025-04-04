#define GLM_FORCE_RADIANS
#define GLM_ENABLE_EXPERIMENTAL
#include "GeomTriangle.h"

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>

#include "Intersection.h"
#include "Ray.h"

GeomTriangle::GeomTriangle(std::vector<glm::vec3> &vertices, std::vector<glm::vec3> &normals) {
    this->vertices[0] = vertices[0];
    this->vertices[1] = vertices[1];
    this->vertices[2] = vertices[2];

    this->normals[0] = normals[0];
    this->normals[1] = normals[1];
    this->normals[2] = normals[2];
}

std::vector<Intersection> GeomTriangle::intersect(Ray &ray) {
    using namespace glm;
    /**
     * NOTE: Ray is already transformed to the Model coordinate space.
     */

    // vector to store the intersections
    std::vector<Intersection> intersections;

   // Triangle vertices
   glm::vec3 A = this->vertices[0];
   glm::vec3 B = this->vertices[1];
   glm::vec3 C = this->vertices[2];

   // Compute edges
   glm::vec3 edge1 = B - A;
   glm::vec3 edge2 = C - A;
   glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));

   // Compute determinant
   glm::vec3 P = glm::cross(ray.dir, edge2);
   float det = glm::dot(edge1, P);

   if (fabs(det) < 1e-8) return intersections;

   float invDet = 1.0f / det;
   glm::vec3 T = ray.p0 - A;

   // Compute u
   float u = glm::dot(T, P) * invDet;
   if (u < 0 || u > 1) return intersections;

   // Compute v
   glm::vec3 Q = glm::cross(T, edge1);
   float v = glm::dot(ray.dir, Q) * invDet;
   if (v < 0 || (u + v) > 1) return intersections;

   float t = glm::dot(edge2, Q) * invDet;

   if (t > 0) {
       glm::vec3 intersection_point = ray.p0 + t * ray.dir;
       glm::vec3 interpolated_normal = glm::normalize((1.0f - u - v) * this->normals[0] + u * this->normals[1] + v * this->normals[2]);

       intersections.push_back({t, intersection_point, interpolated_normal, this, nullptr});
   }


    return intersections;
}