#include "kick/material/shader_enums.h"

using namespace std;

namespace kick {
    string to_string(AttributeDescriptor attDesc) {
        std::string res = "AttributeDescriptor {";
        res += " Index: " + std::to_string(attDesc.index);
        res += ", Size: " + std::to_string(attDesc.size);
        res += ", Type: " + std::to_string(attDesc.type);
        res += ", Name: " + attDesc.name;
        res += ", Semantic: " + to_string(attDesc.semantic);
        res += " }";
        return res;
    }

    string to_string(UniformDescriptor uniformDesc) {
        std::string res = "UniformDescriptor {";
        res += " Index: " + std::to_string(uniformDesc.index);
        res += ", Size: " + std::to_string(uniformDesc.size);
        res += ", Type: " + std::to_string(uniformDesc.type);
        res += ", Name: " + uniformDesc.name;
        res += " }";
        return res;
    }
}