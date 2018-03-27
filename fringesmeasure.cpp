#include <FringesMeasure.h>

Mat unwrapPhase (const Mat& grayCode, const Mat& fringes)
{
    Mat unwrappedPhase = Mat::zeros(grayCode.rows, grayCode.cols, CV_32F);

    for (int i  = 0; i < grayCode.rows; ++i)
    {
        for (int j = 0; j < grayCode.cols; ++j)
        {

            if ( ((-CV_PI/2)<fringes.at<float>(i,j)) && (fringes.at<float>(i,j)< (CV_PI/2)) )
            {
                unwrappedPhase.at<float>(i,j) = (floor(grayCode.at<uchar>(i,j)/2)*2*CV_PI) + (CV_PI+ fringes.at<float>(i,j));
            }else
            {
                if ((CV_PI/2) > fringes.at<float>(i,j))
                {
                    unwrappedPhase.at<float>(i,j) = (floor((grayCode.at<uchar>(i,j)+1)/2)*CV_PI*2) + (CV_PI+fringes.at<float>(i,j));
                }else
                {
                    unwrappedPhase.at<float>(i,j) = ((floor((grayCode.at<uchar>(i,j)+1)/2) - 1) * CV_PI * 2) + (fringes.at<float>(i,j)+ CV_PI);
                }
            }
        }
    }
    return unwrappedPhase;
}

StereoPhaseModulation FringesAquisition2Cams(int RightCam, int LeftCam, int ResX, int ResY, int Period, int Steps, bool SaveImages, bool ShowImages)
{
    GCParams greyCodeParam {ResX, ResY, Period};
    GCImages ImagesGC(greyCodeParam);

    vector<vector<Mat>> GreyCode;
    GreyCode = ProjectCapture2Cams(ImagesGC.getImagesV(), RightCam, LeftCam, false);

    Mat decodedR = DecodeGC(GreyCode[0]);
    Mat decodedL = DecodeGC(GreyCode[0]);

    FGParams FringeParams{ResX, ResY, Period, Steps};
    FGImages ImagesFG(FringeParams);

    vector<vector<Mat>> Fringes;
    Fringes = ProjectCapture2Cams(ImagesFG.ImagesV, RightCam, LeftCam, false);

    resultsFG decodedFgR;
    resultsFG decodedFgL;
    decodedFgR = ProcessFG(Fringes[0]);
    decodedFgL = ProcessFG(Fringes[1]);

    Mat unwrapR, unwrapL;
    unwrapR = unwrapPhase(decodedR, decodedFgR.decodedFG);
    unwrapL = unwrapPhase(decodedL, decodedFgL.decodedFG);

    if (ShowImages)
    {
        Mat convertedR, convertedL;
        double mini, maxm;

        minMaxLoc(unwrapR, &mini, &maxm);
        unwrapR.convertTo(convertedR,CV_8U,255.0/(maxm-mini),-255.0*mini/(maxm-mini));

        minMaxLoc(unwrapL, &mini, &maxm);
        unwrapL.convertTo(convertedL,CV_8U,255.0/(maxm-mini),-255.0*mini/(maxm-mini));

        namedWindow ("UnwrapR");
        imshow("UnwrapR", convertedR);

        namedWindow ("UnwrapL");
        imshow("UnwrapL", convertedL);


        namedWindow ("Mod");
        imshow("Mod", decodedFgR.modulation);
        waitKey ();
    }

    if (SaveImages)
    {
        vector<int> compression_params;
        compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
        compression_params.push_back(0);

        imwrite("PhaseR.png", unwrapR, compression_params);
        imwrite("ModR.png", decodedFgR.modulation, compression_params);
        imwrite("PhaseL.png", unwrapL, compression_params);
        imwrite("ModL.png", decodedFgL.modulation, compression_params);
    }

    StereoPhaseModulation Phase;
    cv2eigen(decodedFgR.modulation,Phase.RightCam.Modulation );
    cv2eigen(decodedFgL.modulation,Phase.LeftCam.Modulation );
    cv2eigen(unwrapR,Phase.RightCam.Phase);
    cv2eigen(unwrapL,Phase.LeftCam.Phase);

    return Phase;
}


PhaseModulation FringesAquisition1Cam(int ResX, int ResY, int Period, int Steps,
                                      bool SaveImages, bool ShowImages, bool Vertical, VideoCapture& cam, bool Adjust, string PhaseName)
{
    GCParams greyCodeParam {ResX, ResY, Period};
    GCImages ImagesGC(greyCodeParam);

    vector<Mat> GreyCode;

    if (Vertical == true) GreyCode = ProjectCapture1Cam(ImagesGC.getImagesV(), cam, Adjust);
    else GreyCode = ProjectCapture1Cam(ImagesGC.getImagesH(), cam, Adjust);

    Mat decodedGC = DecodeGC(GreyCode);

    FGParams FringeParams{ResX, ResY, Period, Steps};
    FGImages ImagesFG(FringeParams);

    vector<Mat> Fringes;
    if (Vertical == true) Fringes = ProjectCapture1Cam(ImagesFG.ImagesV,cam, Adjust);
    else Fringes = ProjectCapture1Cam(ImagesFG.ImagesH,cam, Adjust);

    resultsFG decodedFg;
    decodedFg = ProcessFG(Fringes);

    Mat unwrap;
    unwrap = unwrapPhase(decodedGC, decodedFg.decodedFG);

    if (ShowImages)
    {
        Mat converted;
        double mini, maxm;

        minMaxLoc(unwrap, &mini, &maxm);
        unwrap.convertTo(converted,CV_8U,255.0/(maxm-mini),-255.0*mini/(maxm-mini));


        namedWindow ("UnwrapR");
        imshow("UnwrapR", converted);
        namedWindow ("GreyCode");
        imshow("GreyCode", decodedGC);
        namedWindow ("ModR");
        imshow("ModR", decodedFg.modulation);
        waitKey ();
        destroyWindow("UnwrapR");
        destroyWindow("ModR");
    }

    if (SaveImages)
    {
        FileStorage Phase("PhaseMaps.xml", FileStorage::APPEND);
        Phase << PhaseName + "Phase" << unwrap;
        Phase << PhaseName + "Mod" << decodedFg.modulation;
        Phase.release();
    }

    PhaseModulation Phase;
    cv2eigen(decodedFg.modulation,Phase.Modulation );
    cv2eigen(unwrap,Phase.Phase);

    return Phase;
}
