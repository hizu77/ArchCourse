#pragma once
#include "PointGen.h"
#include "Options.h"
#include <vector>

float GetBoundingBoxVolume(const float* range) {
    float result = (range[1] - range[0]) * (range[3] - range[2]) * (range[5] - range[4]);
    return result;
}

Result NoOmp(size_t iterations) {
    std::random_device rd;
    Point points(rd());
    size_t hits = 0;

    double start_time = omp_get_wtime();
    for (size_t i = 0; i < iterations; ++i) {
        hits += points.isHit();
    }
    double end_time = omp_get_wtime();

    Result result;
    result.threads_count = 0;
    result.time = (end_time - start_time) * 1000; // ms
    result.volume = GetBoundingBoxVolume(get_axis_range()) * (float)hits / (float)iterations;

    return result;
}

Result Omp(size_t iterations, uint8_t threads_count) {
    size_t random_value = std::random_device()();
    size_t hits = 0;

    omp_set_num_threads(threads_count);
    double start_time = omp_get_wtime();
    #pragma omp parallel shared(hits, iterations)
    {
        Point local_point(random_value + omp_get_thread_num());
        size_t local_hits = 0;
        #pragma omp for schedule(static)
        for (size_t i = 0; i < iterations; ++i) {
            if (local_point.isHit()) {
                ++local_hits;
            }
        }
        #pragma omp atomic
        hits += local_hits;
    }
    double end_time = omp_get_wtime();

    Result result;
    result.threads_count = threads_count;
    result.time = (end_time - start_time) * 1000; // ms
    result.volume = GetBoundingBoxVolume(get_axis_range()) * (float)hits / (float)iterations;

    return result;
}

Result Calculate(const Options& options) {
    if (!options.threads) {
        return NoOmp(options.iterations);
    } else {
        return Omp(options.iterations, options.threads.value());
    }
}


