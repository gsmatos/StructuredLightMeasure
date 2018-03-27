#ifndef FRINGESMEASURE_H
#define FRINGESMEASURE_H

#include <opencv2/core.hpp>
#include <Acquisition.h>
#include <SinFringes.h>
#include <GreyCode.h>

using namespace cv;

struct PhaseModulation
{
    MatrixXd Phase;
    MatrixXd Modulation;
};

struct StereoPhaseModulation
{
    PhaseModulation RightCam;
    PhaseModulation LeftCam;
    string Name = "";
    string Path = "";
};

Mat unwrapPhase (const Mat& grayCode, const Mat& fringes);

//Create, project, acquire and unwrap the fringe maps.
StereoPhaseModulation FringesAquisition2Cams(int RightCam, int LeftCam, int ResX, int ResY, int Period, int Steps, bool SaveImages, bool ShowImages);
PhaseModulation FringesAquisition1Cam(int ResX, int ResY, int Period, int Steps, bool SaveImages,
                                      bool ShowImages, bool Vertical, VideoCapture &cam, bool Adjust, string PhaseName = "Capture");

#endif // FRINGESMEASURE_H
