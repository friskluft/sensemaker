#pragma once

#include <unordered_map>
#include "../roi_cache.h"
#include "image_matrix.h"

#define _USE_MATH_DEFINES	// For M_PI, etc.
#include <cmath>
#include "gabor.h"
#include <omp.h>

/// @brief Extract face feature based on gabor filtering.
class Gabor_features
{
public:
    static bool required(const FeatureSet& fs) { return fs.isEnabled(GABOR); }
    static const int num_features = 7;
    Gabor_features (const ImageMatrix& Im0);
    void get_feature_values (std::vector<double>& fvals);
    static void reduce (size_t start, size_t end, std::vector<int>* ptrLabels, std::unordered_map <int, LR>* ptrLabelData);

private:
    void conv_ddd (double* c, double* a, double* b, int na, int ma, int nb, int mb);
    void conv_dud (double* c, const unsigned int* a, double* b, int na, int ma, int nb, int mb);
    void conv_parallel (double* c, double* a, double* b, int na, int ma, int nb, int mb);
    void conv_parallel_dud (double* c, const unsigned int* a, double* b, int na, int ma, int nb, int mb);

    // Creates a non-normalized Gabor filter
    void Gabor (
        double* Gex,    // buffer of size n*n*2
        double f0, 
        double sig2lam, 
        double gamma, 
        double theta, 
        double fi, 
        int n);

    // Computes Gabor energy 
    void GaborEnergy (
        const ImageMatrix& Im, 
        PixIntens* /* double* */ out, 
        double* auxC, 
        double* Gex, 
        double f0, 
        double sig2lam, 
        double gamma, 
        double theta, 
        int n);

    // Result cache
    std::vector<double> fvals;
};

