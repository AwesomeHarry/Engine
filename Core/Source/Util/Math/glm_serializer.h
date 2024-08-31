#pragma once

#include <glm/glm.hpp>
#include <json.hpp>

// GLM type serialization helpers
namespace glm_serialization {
    template<glm::length_t L, typename T, glm::qualifier Q>
    void to_json(nlohmann::json& j, const glm::vec<L, T, Q>& v) {
        for (glm::length_t i = 0; i < L; ++i) {
            j.push_back(v[i]);
        }
    }

    template<glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
    void to_json(nlohmann::json& j, const glm::mat<C, R, T, Q>& m) {
        for (glm::length_t c = 0; c < C; ++c) {
            for (glm::length_t r = 0; r < R; ++r) {
                j.push_back(m[c][r]);
            }
        }
    }

    template<glm::length_t L, typename T, glm::qualifier Q>
    void from_json(const nlohmann::json& j, glm::vec<L, T, Q>& v) {
        for (glm::length_t i = 0; i < L; ++i) {
            v[i] = j[i].get<T>();
        }
    }

    template<glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
    void from_json(const nlohmann::json& j, glm::mat<C, R, T, Q>& m) {
        glm::length_t index = 0;
        for (glm::length_t c = 0; c < C; ++c) {
            for (glm::length_t r = 0; r < R; ++r) {
                m[c][r] = j[index++].get<T>();
            }
        }
    }
}