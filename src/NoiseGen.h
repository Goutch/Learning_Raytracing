#pragma once

#include "vector"
#include "FastNoise.h"

enum class NoiseType {
    simplex,
    fractal,
    perlin,
};

class NoiseGen {
    FastNoise noise;
    std::vector<NoiseType> types;
    std::vector<float> weights;
    std::vector<float> scales;
    int seed;
    float offSetX;
    float offSetY;
public:
    NoiseGen(int seed) {
        this->seed = seed;
        noise=FastNoise(seed);
    }
    void addLayer(NoiseType type, float weight, float scale) {
        types.emplace_back(type);
        weights.emplace_back(weight);
        scales.emplace_back(scale);
    }

    float get(float x, float y) {
        float n_value = 0.0f;
        for (int i = 0; i < types.size(); ++i) {
            switch (types[i]) {
                case NoiseType::simplex:
                    n_value += ((noise.GetSimplex(x * scales[i], y * scales[i]) + 1.0f) / 2.0f) * weights[i];
                    break;
                case NoiseType::perlin:
                    n_value += ((noise.GetPerlin(x * scales[i], y * scales[i]) + 1.0f) / 2.0f) * weights[i];
                    break;
                case NoiseType::fractal:
                    n_value += ((noise.GetSimplexFractal(x * scales[i], y * scales[i]) + 1.0f) / 2.0f) * weights[i];
                    break;
            }
        }
        return n_value;
    }

    float get(float x, float y, float z) {
        float n_value = 0.0f;
        for (int i = 0; i < types.size(); ++i) {
            switch (types[i]) {
                case NoiseType::simplex:
                    n_value += ((noise.GetSimplex(x * scales[i], y * scales[i], z * scales[i]) + 1.0f) / 2.0f) * weights[i];
                    break;
                case NoiseType::perlin:
                    n_value += ((noise.GetPerlin(x * scales[i], y * scales[i], z * scales[i]) + 1.0f) / 2.0f) * weights[i];
                    break;
                case NoiseType::fractal:
                    n_value += ((noise.GetSimplexFractal(x * scales[i], y * scales[i], z * scales[i]) + 1.0f) / 2.0f) * weights[i];
                    break;
            }
        }
        return n_value;
    }


};