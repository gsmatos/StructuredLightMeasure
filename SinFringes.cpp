#include <math.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <SinFringes.h>

using namespace std;
using namespace cv;


void FGImages::BuildFringes()
{
    for (int i  = 0; i< Params.steps; ++i)
    {
        ImagesV.push_back(Mat(Params.imgResX,Params.imgResX, CV_8U));
        ImagesH.push_back(Mat(Params.imgResX,Params.imgResX, CV_8U));
    }

    float currentStep;
    float omega;
    float currentPhase;
    float currentCol;
    for (int i  = 0; i< Params.steps; ++i)
    {
        for (int j = 0; j<Params.imgResX; ++j)
        {
            currentStep = 2*CV_PI/Params.steps*i;
            omega = 2*CV_PI/Params.pixelsPerPeriod;

            currentPhase = (omega*j)+(omega/2)+currentStep;
            currentCol = (sin(currentPhase)+1)/2*255;

            ImagesV[i].col(j) = currentCol;
        }
    }

    for (int i = 0; i < Params.steps; ++i)
    {
        transpose(ImagesV[i],ImagesH[i]);
        ImagesV[i] =  ImagesV[i](Rect(0,0,Params.imgResX, Params.imgResY));
        ImagesH[i] =  ImagesH[i](Rect(0,0,Params.imgResX, Params.imgResY));
    }
}

resultsFG ProcessFG(const vector<Mat>& capturedFG)
{
    Mat sinMat = Mat::zeros(capturedFG[0].rows, capturedFG[0].cols, CV_32F);
    Mat cosMat = Mat::zeros(capturedFG[0].rows, capturedFG[0].cols, CV_32F);
    Mat atanMat = Mat::zeros(capturedFG[0].rows, capturedFG[0].cols, CV_32F);

    Mat convertedFG;

    for (int i = 0; i < capturedFG.size(); ++i)
    {
        capturedFG[i].convertTo(convertedFG, CV_32F);
        sinMat += convertedFG*sin(2*CV_PI*i/capturedFG.size());
        cosMat += (-(convertedFG*cos(2*CV_PI*i/capturedFG.size())));
    }

    //    Mat sinOverCos = sinMat/cosMat;

    for (int i = 0; i < cosMat.rows; ++i)
    {
        for (int j = 0; j < cosMat.cols; ++j)
        {
            atanMat.at<float>(i,j) = atan2(sinMat.at<float>(i,j), cosMat.at<float>(i,j));
        }
    }

    Mat modulation;
    sqrt(sinMat.mul(sinMat) + cosMat.mul(cosMat), modulation);

    Mat decodedFG = atanMat+(CV_PI/2);
    Mat decodedFGnorm = (atanMat*128/CV_PI)+128;

    resultsFG results;
    results.decodedFG = decodedFG;
    results.decodedFGNorm = decodedFGnorm;
    results.modulation = modulation;
    return results;
}



