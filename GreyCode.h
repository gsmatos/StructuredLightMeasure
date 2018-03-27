#ifndef GREYCODE_H_INCLUDED
#define GREYCODE_H_INCLUDED

#include <iostream>
#include <opencv2/core.hpp>

using namespace std;
using namespace cv;

struct GCParams{
    int imgResX;
    int imgResY;
    int pixelsPerPeriod;
};

struct GCImages{
    GCImages(GCParams P): Param{P} {init(); buildImagesGC();}
    void init();
    void buildImagesGC();
    vector<Mat> getImagesH() const {return greyCodeImagesH;}
    vector<Mat> getImagesV() const {return greyCodeImagesV;}
//    private:
        int numProjections;
        vector<Mat> greyCodeImages;
        vector<Mat> greyCodeImagesH;
        vector<Mat> greyCodeImagesV;
        float projectionSize;
        GCParams Param;
};

//struct captureProcessGC{
//    vector<Mat> capturedGCR;
////    vector<Mat> capturedGCL;
//    Mat decodedGCR;
//    Mat decodedGCL;
//    void DecodeAll();
//};

Mat DecodeGC(const vector<Mat>& capturedGC);
void InsertRow(Mat& Original, const Mat& toInsert, int insertInRow);
Mat BuilderSup(const Mat& greyCodeBuilder, int halfImageDivisor);
void InsertBlackWhite(vector<Mat>& M);
Mat PreGreyCode(const vector<Mat>& capturedGC);

#endif // GREYCODE_H_INCLUDED
