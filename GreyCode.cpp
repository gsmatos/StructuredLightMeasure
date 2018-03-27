#include <math.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <GreyCode.h>

using namespace std;
using namespace cv;

void GCImages::init(){
    float halfPeriod = Param.pixelsPerPeriod / 2;
    float numPeriods = Param.imgResX/halfPeriod;
    numProjections = ceil(log(numPeriods)/log(2));
    projectionSize = halfPeriod*pow(2,numProjections);
    for (int i = 0; i<numProjections; ++i)
    {
        greyCodeImages.push_back(Mat::zeros(projectionSize,projectionSize,CV_8U));
        greyCodeImagesH.push_back(Mat::zeros(projectionSize,projectionSize,CV_8U));
        greyCodeImagesV.push_back(Mat::zeros(projectionSize,projectionSize,CV_8U));
    }

}

//Insert a Mat toInsert in the row insertInRow. Raises Original.rols. CHANGES ORIGINAL.
void insertRow(Mat& Original, const Mat& toInsert, int insertInRow)
{
    if(insertInRow>=Original.rows)
    {
        if(insertInRow==Original.rows) //If last line, just concatenate
        {
            vconcat(Original, toInsert, Original);
            return;
        }else{ //Out of range insertion
            cout<<"ERROR: not enough rows in Original";
            return;
        }
    }
    Mat top = Original(Rect(0,0,Original.cols,insertInRow ));
    Mat bottom = Original(Rect(0,insertInRow,Original.cols,Original.rows-insertInRow ));
    vconcat(top,toInsert,top);
    vconcat(top,bottom,Original);
}

Mat builderSup(const Mat& greyCodeBuilder, int halfImageDivisor){
    Mat M1 = greyCodeBuilder(Rect(0,0,greyCodeBuilder.cols,halfImageDivisor));
    Mat M2 = greyCodeBuilder(Rect(0,halfImageDivisor,greyCodeBuilder.cols,halfImageDivisor));
    insertRow(M2, M1, halfImageDivisor);
    return M2;
}

void insertBlackWhite(vector<Mat>& M)
{
    Mat White = Mat(M[0].rows, M[0].rows, CV_8U, 255);
    Mat Black = Mat::zeros(M[0].rows, M[0].rows, CV_8U);

    reverse(M.begin(), M.end());

    M.push_back(White);
    M.push_back(Black);

    reverse(M.begin(), M.end());
}

void GCImages::buildImagesGC()
{
    Mat greyCodeBuilder =  greyCodeImages[0];

    //    Start by selecting the portion of the image to be edited
    int imageDivisor = projectionSize;

    int halfImageDivisor;
    Mat greyCodeBuilderSup;

    for (int i=0; i<numProjections; ++i)
    {

        if (i==0)
        {
            halfImageDivisor = imageDivisor/2;

            for (int j = halfImageDivisor; j< projectionSize; ++j)
                greyCodeBuilder.row(j) = 255;
            greyCodeImages[0] = greyCodeBuilder;
            imageDivisor = halfImageDivisor;
        }else
        {
            halfImageDivisor = imageDivisor/2;
            greyCodeBuilder = greyCodeBuilder(Rect(0,halfImageDivisor,greyCodeBuilder.cols,imageDivisor));
            greyCodeBuilderSup = builderSup(greyCodeBuilder,halfImageDivisor);

            for(int k = 0;k < pow(2,i); ++k )
            {
                if (k%2==0)
                {
                    greyCodeBuilder.copyTo(greyCodeImages[i](Rect(0,k*imageDivisor,greyCodeBuilder.cols, greyCodeBuilder.rows)));
                }else
                {
                    greyCodeBuilderSup.copyTo(greyCodeImages[i](Rect(0,k*imageDivisor,greyCodeBuilderSup.cols, greyCodeBuilderSup.rows)));
                }
            }
            imageDivisor=halfImageDivisor;
        }
    }

    insertBlackWhite(greyCodeImages);
    insertBlackWhite(greyCodeImagesH);
    insertBlackWhite(greyCodeImagesV);

    for (int i = 0; i<greyCodeImages.size(); ++i )
    {
        greyCodeImagesH[i] = greyCodeImages[i](Rect(0,0,Param.imgResX, Param.imgResY));
    }

    vector<Mat> transposed = greyCodeImagesV;

    for (int i = 0; i<greyCodeImages.size(); ++i )
    {
        transpose(greyCodeImages[i],transposed[i]);
        greyCodeImagesV[i] = transposed[i](Rect(0,0,Param.imgResX, Param.imgResY));
    }

}

Mat DecodeGC(const vector<Mat>& capturedGC)
{
    Mat preGreyCode = PreGreyCode(capturedGC);
    int maxBits = capturedGC.size()-2;
    Mat decodedGC = preGreyCode;

    int currentPixel;

    int currentBit = 1;

    bool adder = false;

    for (int i = 0; i < preGreyCode.cols; ++i)
    {
        for (int j = 0; j < preGreyCode.rows; ++j)
        {
            currentPixel = (int)preGreyCode.at<uchar>(j,i,0);
            for (int k = 0; k < maxBits; ++k)
            {
                for(int l = 0; l < (maxBits-k-1); ++l)
                {
                    currentBit <<= 1;
                }
                if (adder) decodedGC.at<uchar>(j,i,0)^= currentBit;
                if ((currentPixel & currentBit) > 0) adder = !adder;
                currentBit = 1;
            }
            adder = false;
        }
    }
    return decodedGC;
}

Mat PreGreyCode(const vector<Mat>& capturedGC)
{
    Mat divisor = (capturedGC[0]+capturedGC[1])/2;
    Mat preDecodedGC=capturedGC[0];

    int currentGC = 1;

    for (int i = 2; i < capturedGC.size(); ++i) //Start with 2 to remove the only black and only white images
    {
        for (int j= 0; j < capturedGC.size()-i-1; ++j)
        {
            currentGC <<= 1;
        }
        for (int k = 0; k<preDecodedGC.rows; ++k)
        {
            for (int l = 0; l<preDecodedGC.cols; ++l)
            {
                if (capturedGC[i].at<uchar>(k,l,0)>divisor.at<uchar>(k,l,0))
                {
                    preDecodedGC.at<uchar>(k,l,0) |=  currentGC;
                }
            }
        }
        currentGC = 1; //currentGC is calculated again every iteration
    }
    return preDecodedGC;
}



