//
// Created by Morten Nobel-Jørgensen on 08/09/14.
//

#include "bounds3.h"

namespace kick{

Bounds3::Bounds3() {
}

Bounds3::Bounds3(glm::vec3 const &min, glm::vec3 const &max) : min(min), max(max) {
        }

void Bounds3::expand(glm::vec3 p) {
    min = glm::min(min, p);
    max = glm::max(max, p);
}

void Bounds3::expand(Bounds3 b) {
    min = glm::min(min, b.min);
    max = glm::max(max, b.max);
}

void Bounds3::reset() {
    min = glm::vec3{ std::numeric_limits<float>::max() };
    max = glm::vec3{ std::numeric_limits<float>::lowest() };
}

Bounds3 Bounds3::lerp(float f, Bounds3 b) {
    return Bounds3 {glm::mix(min, b.min, f), glm::mix(max, b.max, f)};

}

bool Bounds3::contains(glm::vec3 point) {
    return min.x <= point.x && min.y <= point.y && min.z <= point.z &&
            max.x >= point.x && max.y >= point.y && max.z >= point.z
            ;
}

    glm::vec3 Bounds3::dimension() { return max-min; }

    glm::vec3 Bounds3::center() { return (max+min)*0.5f; }

    glm::vec3 Bounds3::diagonal() { return (max-min); }

	Bounds3 Bounds3::transform(glm::mat4 trans){
		glm::vec3 values[2] = {min, max};
		Bounds3 res;
		for (int x = 0; x < 2; x++){
			for (int y = 0; y < 2; y++){
				for (int z = 0; z < 2; z++){
					glm::vec4 t = trans * glm::vec4(values[x].x, values[y].y, values[z].z, 1.0);
					res.expand((glm::vec3)t);
				}
			}
		}
		return res;

	}

    std::vector<glm::vec3> Bounds3::toLines() {
        std::vector<glm::vec3> res;
        res.push_back(max);
        res.push_back(glm::vec3(max.x,max.y,min.z));
        res.push_back(max);
        res.push_back(glm::vec3(max.x,min.y,max.z));
        res.push_back(max);
        res.push_back(glm::vec3(min.x,max.y,max.z));

        res.push_back(min);
        res.push_back(glm::vec3(min.x,min.y,max.z));
        res.push_back(min);
        res.push_back(glm::vec3(min.x,max.y,min.z));
        res.push_back(min);
        res.push_back(glm::vec3(max.x,min.y,min.z));

        res.push_back(glm::vec3(max.x,max.y,min.z));
        res.push_back(glm::vec3(max.x,min.y,min.z));

        res.push_back(glm::vec3(max.x,max.y,min.z));
        res.push_back(glm::vec3(min.x,max.y,min.z));

        res.push_back(glm::vec3(min.x,max.y,min.z));
        res.push_back(glm::vec3(min.x,max.y,max.z));

        res.push_back(glm::vec3(min.x,max.y,max.z));
        res.push_back(glm::vec3(min.x,min.y,max.z));

        res.push_back(glm::vec3(min.x,min.y,max.z));
        res.push_back(glm::vec3(max.x,min.y,max.z));

        res.push_back(glm::vec3(max.x,min.y,max.z));
        res.push_back(glm::vec3(max.x,min.y,min.z));
        return res;
    }
}