#ifndef ACQUISITION_H
#define ACQUISITION_H

#include <math.h>
#include <iostream>
#include <opencv2/core.hpp>

#include <Eigen/Eigen>
#include <opencv2/core/eigen.hpp>

#include <vector>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <opencv2/calib3d/calib3d.hpp>

#include <QDebug>

using namespace std;
using namespace cv;
using namespace Eigen;

Mat PaintSaturated(const Mat &Image, int DownThreshold, int UpThreshold);

//Camera parameters limits tester
void TestCameraParameters ();

Mat ProcessImageToFindCorners(Mat &capture);

//CALL THIS FUNCTION ALWAYS AFTER EXPOSURE IS CHANGED AND BEFORE CAPTURE
void UndoAutoCamAdjusts(VideoCapture& cam);

vector<vector<Mat>> ProjectCaptureCams(const vector <Mat>& Images, int NumOfCams, bool adjust=false);
vector<Mat> ProjectCapture1Cam(const vector <Mat>& Images, VideoCapture& cam, bool adjust=false);
vector<vector<Mat>> ProjectCapture2Cams(const vector <Mat>& Images, bool adjust=false, int RightCam = 0, int LeftCam= 1);

void CalibrationCapture(string& BasePath, bool ToGray, bool adjust=true);
void CalibrationCapture2Cams(string& BasePath, bool adjust=true);

vector<Point3f> BuildObjPoints (Size NumCorners, int SquareSizeX, int SquareSizeY);

Matrix4Xd ProjectPoint(const Matrix4d &H, const Matrix4Xd &objtPts);



struct CameraParameters
{
    // 3x3 KK, use SetK to set it
    Matrix3d KK3;
    // 4x4 KK, use SetK to set it
    Matrix4d KK;
    Matrix4d Rt;
    VectorXd kc;
    Vector2d ImageSize;

    Matrix4Xd Project(const Matrix4Xd& objtPts) const {return ProjectPoint(GetH(), objtPts);}
    Matrix4d GetH() const {return KK*Rt;}
    void SetK(const Matrix3d& KK3x3);
    string ToString() const;
};

struct StereoCamParameters
{
    CameraParameters CamParamL;
    CameraParameters CamParamR;
    Matrix4d RtLR;
    StereoCamParameters()
    {
        CamParamL = CameraParameters();
        CamParamR = CameraParameters();
    }
    string ToString() const;
};

struct CameraParametersCV
{
    Mat KK, kc, Rt;
    CameraParametersCV(CameraParameters Params) {
        eigen2cv(Params.KK3,KK); eigen2cv(Params.kc,kc); eigen2cv(Params.Rt,Rt);
    }
    CameraParametersCV();
};

struct StereoCamParametersCV
{
    CameraParametersCV CamR, CamL;
    StereoCamParametersCV(StereoCamParameters Params)
        :
          CamR  (CameraParametersCV(Params.CamParamR)),
          CamL  (CameraParametersCV(Params.CamParamL)){}
};

Mat BuildRtMat (Mat Rvec, Mat Tvec);
void CalcRt2Cams (StereoCamParameters& Params, int SquareSizeX, int SquareSizeY, bool adjust);
void WatchCameras();
StereoCamParameters ReadCamsParams(bool LoadRt);
void UndistorImages(vector<Mat>& Images, const Matrix3d& KK, const VectorXd& kc);

#endif // ACQUISITION_H
