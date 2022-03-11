#include "image_moments.h"

ImageMomentsFeature::ImageMomentsFeature() : FeatureMethod("ImageMomentsFeature")
{
    provide_features({
        SPAT_MOMENT_00,
        SPAT_MOMENT_01,
        SPAT_MOMENT_02,
        SPAT_MOMENT_03,
        SPAT_MOMENT_10,
        SPAT_MOMENT_11,
        SPAT_MOMENT_12,
        SPAT_MOMENT_20,
        SPAT_MOMENT_21,
        SPAT_MOMENT_30,
        NORM_SPAT_MOMENT_00, 
        NORM_SPAT_MOMENT_01, 
        NORM_SPAT_MOMENT_02, 
        NORM_SPAT_MOMENT_03, 
        NORM_SPAT_MOMENT_10, 
        NORM_SPAT_MOMENT_20, 
        NORM_SPAT_MOMENT_30, 
        CENTRAL_MOMENT_02,
        CENTRAL_MOMENT_03,
        CENTRAL_MOMENT_11,
        CENTRAL_MOMENT_12,
        CENTRAL_MOMENT_20,
        CENTRAL_MOMENT_21,
        CENTRAL_MOMENT_30,
        NORM_CENTRAL_MOMENT_02,
        NORM_CENTRAL_MOMENT_03,
        NORM_CENTRAL_MOMENT_11,
        NORM_CENTRAL_MOMENT_12,
        NORM_CENTRAL_MOMENT_20,
        NORM_CENTRAL_MOMENT_21,
        NORM_CENTRAL_MOMENT_30,
        WEIGHTED_CENTRAL_MOMENT_02, 
        WEIGHTED_CENTRAL_MOMENT_03, 
        WEIGHTED_CENTRAL_MOMENT_11, 
        WEIGHTED_CENTRAL_MOMENT_12, 
        WEIGHTED_CENTRAL_MOMENT_20, 
        WEIGHTED_CENTRAL_MOMENT_21, 
        WEIGHTED_CENTRAL_MOMENT_30, 
        WEIGHTED_SPAT_MOMENT_00, 
        WEIGHTED_SPAT_MOMENT_01, 
        WEIGHTED_SPAT_MOMENT_02, 
        WEIGHTED_SPAT_MOMENT_03, 
        WEIGHTED_SPAT_MOMENT_10, 
        WEIGHTED_SPAT_MOMENT_11, 
        WEIGHTED_SPAT_MOMENT_12, 
        WEIGHTED_SPAT_MOMENT_20, 
        WEIGHTED_SPAT_MOMENT_21, 
        WEIGHTED_SPAT_MOMENT_30, 
        HU_M1,
        HU_M2,
        HU_M3,
        HU_M4,
        HU_M5,
        HU_M6,
        HU_M7,
        WEIGHTED_HU_M1,
        WEIGHTED_HU_M2,
        WEIGHTED_HU_M3,
        WEIGHTED_HU_M4,
        WEIGHTED_HU_M5,
        WEIGHTED_HU_M6,
        WEIGHTED_HU_M7 });

    add_dependencies({PERIMETER});
}

void ImageMomentsFeature::calculate (LR& r)
{
    const ImageMatrix& im = r.aux_image_matrix;

    const pixData& I = im.ReadablePixels();
    calcOrigins(I);
    calcSpatialMoments(I);
    calcCentralMoments(I);
    calcNormCentralMoments(I);
    calcNormSpatialMoments(I);
    calcHuInvariants(I);

    ImageMatrix weighted_im(r.raw_pixels, r.aabb);
    weighted_im.apply_distance_to_contour_weights(r.raw_pixels, r.contour);

    const pixData& W = weighted_im.ReadablePixels();
    calcOrigins (W);
    calcWeightedSpatialMoments (W);
    calcWeightedCentralMoments(W);
    calcWeightedHuInvariants(W);
}

void ImageMomentsFeature::osized_add_online_pixel (size_t x, size_t y, uint32_t intensity) {} // Not supporting online for image moments

void ImageMomentsFeature::save_value(std::vector<std::vector<double>>& fvals)
{
    fvals[SPAT_MOMENT_00][0] = m00;
    fvals[SPAT_MOMENT_01][0] = m21;
    fvals[SPAT_MOMENT_02][0] = m02;
    fvals[SPAT_MOMENT_03][0] = m03;
    fvals[SPAT_MOMENT_10][0] = m10;
    fvals[SPAT_MOMENT_11][0] = m11;
    fvals[SPAT_MOMENT_12][0] = m12;
    fvals[SPAT_MOMENT_20][0] = m20;
    fvals[SPAT_MOMENT_21][0] = m21;
    fvals[SPAT_MOMENT_30][0] = m30;

    fvals[WEIGHTED_SPAT_MOMENT_00][0] = wm00;
    fvals[WEIGHTED_SPAT_MOMENT_01][0] = wm01;
    fvals[WEIGHTED_SPAT_MOMENT_02][0] = wm02;
    fvals[WEIGHTED_SPAT_MOMENT_03][0] = wm03;
    fvals[WEIGHTED_SPAT_MOMENT_10][0] = wm10;
    fvals[WEIGHTED_SPAT_MOMENT_11][0] = wm11;
    fvals[WEIGHTED_SPAT_MOMENT_12][0] = wm12;
    fvals[WEIGHTED_SPAT_MOMENT_20][0] = wm20;
    fvals[WEIGHTED_SPAT_MOMENT_21][0] = wm21;
    fvals[WEIGHTED_SPAT_MOMENT_30][0] = wm30;

    fvals[CENTRAL_MOMENT_02][0] = mu02;
    fvals[CENTRAL_MOMENT_03][0] = mu03;
    fvals[CENTRAL_MOMENT_11][0] = mu11;
    fvals[CENTRAL_MOMENT_12][0] = mu12;
    fvals[CENTRAL_MOMENT_20][0] = mu20;
    fvals[CENTRAL_MOMENT_21][0] = mu21;
    fvals[CENTRAL_MOMENT_30][0] = mu30;

    fvals[WEIGHTED_CENTRAL_MOMENT_02][0] = wmu02;
    fvals[WEIGHTED_CENTRAL_MOMENT_03][0] = wmu03;
    fvals[WEIGHTED_CENTRAL_MOMENT_11][0] = wmu11;
    fvals[WEIGHTED_CENTRAL_MOMENT_12][0] = wmu12;
    fvals[WEIGHTED_CENTRAL_MOMENT_20][0] = wmu20;
    fvals[WEIGHTED_CENTRAL_MOMENT_21][0] = wmu21;
    fvals[WEIGHTED_CENTRAL_MOMENT_30][0] = wmu30;

    fvals[NORM_CENTRAL_MOMENT_02][0] = nu02;
    fvals[NORM_CENTRAL_MOMENT_03][0] = nu03;
    fvals[NORM_CENTRAL_MOMENT_11][0] = nu11;
    fvals[NORM_CENTRAL_MOMENT_12][0] = nu12;
    fvals[NORM_CENTRAL_MOMENT_20][0] = nu20;
    fvals[NORM_CENTRAL_MOMENT_21][0] = nu21;
    fvals[NORM_CENTRAL_MOMENT_30][0] = nu30;

    fvals[NORM_SPAT_MOMENT_00][0] = w00;
    fvals[NORM_SPAT_MOMENT_01][0] = w01;
    fvals[NORM_SPAT_MOMENT_02][0] = w02;
    fvals[NORM_SPAT_MOMENT_03][0] = w03;
    fvals[NORM_SPAT_MOMENT_10][0] = w10;
    fvals[NORM_SPAT_MOMENT_20][0] = w20;
    fvals[NORM_SPAT_MOMENT_30][0] = w30;

    fvals[HU_M1][0] = hm1;
    fvals[HU_M2][0] = hm2;
    fvals[HU_M3][0] = hm3;
    fvals[HU_M4][0] = hm4;
    fvals[HU_M5][0] = hm5;
    fvals[HU_M6][0] = hm6;
    fvals[HU_M7][0] = hm7;

    fvals[WEIGHTED_HU_M1][0] = whm1;
    fvals[WEIGHTED_HU_M2][0] = whm2;
    fvals[WEIGHTED_HU_M3][0] = whm3;
    fvals[WEIGHTED_HU_M4][0] = whm4;
    fvals[WEIGHTED_HU_M5][0] = whm5;
    fvals[WEIGHTED_HU_M6][0] = whm6;
    fvals[WEIGHTED_HU_M7][0] = whm7;
}

double ImageMomentsFeature::Moment (const pixData& D, int p, int q)
{
    // calc (p+q)th moment of object
    double sum = 0;
    for (int x = 0; x < D.width(); x++)
    {
        for (int y = 0; y < D.height(); y++)
        {
            sum += D.yx(y,x) * pow(x, p) * pow(y, q);
        }
    }
    return sum;
}

void ImageMomentsFeature::calcOrigins(const pixData& D)
{
    // calc orgins
    originOfX = Moment (D, 1, 0) / Moment (D, 0, 0);
    originOfY = Moment (D, 0, 1) / Moment (D, 0, 0);
}

double ImageMomentsFeature::CentralMom(const pixData& D, int p, int q)
{
    // calculate central moment
    double sum = 0;
    for (int x = 0; x < D.width(); x++)
    {
        for (int y = 0; y < D.height(); y++)
        {
            sum += D.yx(y,x) * pow((double(x) - originOfX), p) * pow((double(y) - originOfY), q);
        }
    }
    return sum;
}

// https://handwiki.org/wiki/Standardized_moment
double ImageMomentsFeature::NormSpatMom(const pixData& D, int p, int q)
{
    double stddev = CentralMom(D, 2, 2);
    int w = std::max(q, p);
    double normCoef = pow(stddev, w);
    double retval = CentralMom(D, p, q) / normCoef;
    return retval;
}

double ImageMomentsFeature::NormCentralMom(const pixData& D, int p, int q)
{
    double temp = ((double(p) + double(q)) / 2.0) + 1.0;
    double retval = CentralMom(D, p, q) / pow(Moment(D, 0, 0), temp);
    return retval;
}

std::tuple<double, double, double, double, double, double, double> ImageMomentsFeature::calcHuInvariants_imp (const pixData& D)
{
    // calculate 7 invariant moments
    double h1 = NormCentralMom(D, 2, 0) + NormCentralMom(D, 0, 2);
    double h2 = pow((NormCentralMom(D, 2, 0) - NormCentralMom(D, 0, 2)), 2) + 4 * (pow(NormCentralMom(D, 1, 1), 2));
    double h3 = pow((NormCentralMom(D, 3, 0) - 3 * NormCentralMom(D, 1, 2)), 2) +
        pow((3 * NormCentralMom(D, 2, 1) - NormCentralMom(D, 0, 3)), 2);
    double h4 = pow((NormCentralMom(D, 3, 0) + NormCentralMom(D, 1, 2)), 2) +
        pow((NormCentralMom(D, 2, 1) + NormCentralMom(D, 0, 3)), 2);
    double h5 = (NormCentralMom(D, 3, 0) - 3 * NormCentralMom(D, 1, 2)) *
        (NormCentralMom(D, 3, 0) + NormCentralMom(D, 1, 2)) *
        (pow(NormCentralMom(D, 3, 0) + NormCentralMom(D, 1, 2), 2) - 3 * pow(NormCentralMom(D, 2, 1) + NormCentralMom(D, 0, 3), 2)) +
        (3 * NormCentralMom(D, 2, 1) - NormCentralMom(D, 0, 3)) * (NormCentralMom(D, 2, 1) + NormCentralMom(D, 0, 3)) *
        (pow(3 * (NormCentralMom(D, 3, 0) + NormCentralMom(D, 1, 2)), 2) - pow(NormCentralMom(D, 2, 1) + NormCentralMom(D, 0, 3), 2));
    double h6 = (NormCentralMom(D, 2, 0) - NormCentralMom(D, 0, 2)) * (pow(NormCentralMom(D, 3, 0) + NormCentralMom(D, 1, 2), 2) -
        pow(NormCentralMom(D, 2, 1) + NormCentralMom(D, 0, 3), 2)) + (4 * NormCentralMom(D, 1, 1) * (NormCentralMom(D, 3, 0) + NormCentralMom(D, 1, 2)) *
            NormCentralMom(D, 2, 1) + NormCentralMom(D, 0, 3));
    double h7 = (3 * NormCentralMom(D, 2, 1) - NormCentralMom(D, 0, 3)) * (NormCentralMom(D, 3, 0) + NormCentralMom(D, 1, 2)) * (pow(NormCentralMom(D, 3, 0) + NormCentralMom(D, 1, 2), 2) -
        3 * pow(NormCentralMom(D, 2, 1) + NormCentralMom(D, 0, 3), 2)) - (NormCentralMom(D, 3, 0) - 3 * NormCentralMom(D, 1, 2)) * (NormCentralMom(D, 2, 1) + NormCentralMom(D, 0, 3)) *
        (3 * pow(NormCentralMom(D, 3, 0) + NormCentralMom(D, 1, 2), 2) - pow(NormCentralMom(D, 2, 1) + NormCentralMom(D, 0, 3), 2));
    return {h1, h2, h3, h4, h5, h6, h7};
}

void ImageMomentsFeature::calcHuInvariants (const pixData& D)
{
    std::tie(hm1, hm2, hm3, hm4, hm5, hm6, hm7) = calcHuInvariants_imp(D);
}

void ImageMomentsFeature::calcWeightedHuInvariants (const pixData& D)
{
    std::tie(whm1, whm2, whm3, whm4, whm5, whm6, whm7) = calcHuInvariants_imp(D);
}

void ImageMomentsFeature::calcSpatialMoments (const pixData& D)
{
    m00 = Moment (D, 0, 0);
    m01 = Moment (D, 0, 1);
    m02 = Moment (D, 0, 2);
    m03 = Moment (D, 0, 3);
    m10 = Moment (D, 1, 0);
    m11 = Moment (D, 1, 1);
    m12 = Moment (D, 1, 2);
    m20 = Moment (D, 2, 0);
    m21 = Moment (D, 2, 1);
    m30 = Moment (D, 3, 0);
}

void ImageMomentsFeature::calcWeightedSpatialMoments (const pixData& D)
{
    wm00 = Moment (D, 0, 0);
    wm01 = Moment (D, 0, 1);
    wm02 = Moment (D, 0, 2);
    wm03 = Moment (D, 0, 3);
    wm10 = Moment (D, 1, 0);
    wm11 = Moment (D, 1, 1);
    wm12 = Moment (D, 1, 2);
    wm20 = Moment (D, 2, 0);
    wm21 = Moment (D, 2, 1);
    wm30 = Moment (D, 3, 0);
}

void ImageMomentsFeature::calcCentralMoments(const pixData& D)
{
    mu02 = CentralMom(D, 0, 2);
    mu03 = CentralMom(D, 0, 3);
    mu11 = CentralMom(D, 1, 1);
    mu12 = CentralMom(D, 1, 2);
    mu20 = CentralMom(D, 2, 0);
    mu21 = CentralMom(D, 2, 1);
    mu30 = CentralMom(D, 3, 0);
}

void ImageMomentsFeature::calcWeightedCentralMoments(const pixData& D)
{
    wmu02 = CentralMom(D, 0, 2);
    wmu03 = CentralMom(D, 0, 3);
    wmu11 = CentralMom(D, 1, 1);
    wmu12 = CentralMom(D, 1, 2);
    wmu20 = CentralMom(D, 2, 0);
    wmu21 = CentralMom(D, 2, 1);
    wmu30 = CentralMom(D, 3, 0);
}

void ImageMomentsFeature::calcNormCentralMoments (const pixData& D)
{
    nu02 = NormCentralMom(D, 0, 2);
    nu03 = NormCentralMom(D, 0, 3);
    nu11 = NormCentralMom(D, 1, 1);
    nu12 = NormCentralMom(D, 1, 2);
    nu20 = NormCentralMom(D, 2, 0);
    nu21 = NormCentralMom(D, 2, 1);
    nu30 = NormCentralMom(D, 3, 0);
}

void ImageMomentsFeature::calcNormSpatialMoments (const pixData& D)
{
    w00 = NormSpatMom (D, 0, 0);
    w01 = NormSpatMom (D, 0, 1);
    w02 = NormSpatMom (D, 0, 2);
    w03 = NormSpatMom (D, 0, 3);
    w10 = NormSpatMom (D, 1, 0);
    w20 = NormSpatMom (D, 2, 0);
    w30 = NormSpatMom (D, 3, 0);
}

void ImageMomentsFeature::parallel_process_1_batch (size_t start, size_t end, std::vector<int>* ptrLabels, std::unordered_map <int, LR>* ptrLabelData)
{
    for (auto i = start; i < end; i++)
    {
        int lab = (*ptrLabels)[i];
        LR& r = (*ptrLabelData)[lab];

        if (r.has_bad_data())
            continue;

        ImageMomentsFeature imf;
        imf.calculate(r);
        imf.save_value(r.fvals);
    }
}

