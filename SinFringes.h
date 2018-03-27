#ifndef SINFRINGES_H_INCLUDED
#define SINFRINGES_H_INCLUDED

#include <math.h>
#include <iostream>

using namespace std;
using namespace cv;

struct FGParams{
    int imgResX;
    int imgResY;
    int pixelsPerPeriod;
    int steps;
};

struct FGImages{
    FGImages(FGParams P): Params{P} {BuildFringes();}
    FGParams Params;
    void BuildFringes();
    vector<Mat> getImagesH() const {return ImagesH;}
    vector<Mat> getImagesV() const {return ImagesV;}
//private:
    vector<Mat> ImagesH;
    vector<Mat> ImagesV;
};

struct resultsFG
{
    Mat modulation;
    Mat decodedFG;
    Mat decodedFGNorm;
};

resultsFG ProcessFG(const vector<Mat>& capturedFG);

#endif // SINFRINGES_H_INCLUDED
