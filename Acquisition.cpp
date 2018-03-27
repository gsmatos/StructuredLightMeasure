#include <Acquisition.h>

using namespace std;
using namespace cv;
using namespace Eigen;

Mat PaintSaturated(const Mat& Image, int DownThreshold, int UpThreshold)
{
    Mat Painted = Image.clone();
    Vec3b Red {0,0,255};
    Vec3b Blue {255,0,0};

    for (int i = 0; i < Painted.rows; ++i)
    {
        for (int j = 0; j < Painted.cols; ++j)
        {
            if(Painted.at<Vec3b>(i,j)[0] < DownThreshold)
            {
                Painted.at<Vec3b>(i,j) = Blue;
            }
            if(Painted.at<Vec3b>(i,j)[1] > UpThreshold)
            {
                Painted.at<Vec3b>(i,j) = Red;
            }
        }
    }
    return Painted;
}

void TestCameraParameters()
{
    VideoCapture cam(0);
    cam.set( CAP_PROP_SETTINGS, 1 );
    namedWindow("after");

    while(true)
    {
        Mat image;
        cam >> image;
        imshow("after", image);
//        cam.set(CAP_PROP_GAMMA, 4.15e+006 );
        int option = waitKey(30);
        if(option == 27 ) break;
        if (option == 32)
        {
            cout <<"Gain = "<< cam.get(CAP_PROP_GAIN) <<endl;
            cout <<"expo = "<<  cam.get(CAP_PROP_EXPOSURE) <<endl;
            cout <<"bright = "<<  cam.get(CAP_PROP_BRIGHTNESS )<<endl;
            cout <<"contr = "<<  cam.get(CAP_PROP_CONTRAST )<<endl;
            cout <<"sat = "<<   cam.get(CAP_PROP_SATURATION )<<endl;
        }
    }
}

Mat ProcessImageToFindCorners(Mat& capture)
{
    cvtColor(capture, capture, CV_RGB2GRAY);
    Mat temp;
    GaussianBlur(capture ,temp, Size(0,0), 105) ;
    addWeighted(capture, 1.8, temp, -0.8,0,capture);
    return capture;
}

vector<vector<Mat>> ProjectCaptureCams(const vector<Mat>& Images, int NumOfCams, bool adjust)
{
    namedWindow("wind", WINDOW_NORMAL);
    moveWindow("wind", 2000,0);
    waitKey(100);
    cvSetWindowProperty("wind", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);

    if (adjust)
    {
        namedWindow("wind");
        for (int i = 0; i< NumOfCams; ++i)
        {
            VideoCapture cam(i);
            cam.set( CAP_PROP_SETTINGS, 1 );
            imshow("wind", Images[4]);

            while(1)
            {
                Mat current;
                cam>>current;
                cvtColor( current, current, COLOR_RGB2GRAY );
                namedWindow("Frame");
                imshow("Frame", current);
                if (waitKey(100) == 32) break;
            }
        }

    }

    //    namedWindow("Captured");
    vector<vector<Mat>> AllCapImages;
    int i = 0;
    while( i < (int) Images.size() )
    {
        imshow("wind", Images[i]);
        waitKey(10);
        vector<Mat> CapImages;
        for(int j = 0; j < NumOfCams; ++j)
        {
            VideoCapture cam(j);
            Mat frame;
            cam.grab();
            cam.retrieve(frame);
            cam.grab();
            cam.retrieve(frame);
            Mat tmp;
            cvtColor( frame, tmp, COLOR_RGB2GRAY );
            //            imshow("Captured", tmp);
            //            waitKey ();
            CapImages.push_back(tmp);
        }
        AllCapImages.push_back(CapImages);
        i++;
    }

    destroyWindow("wind");

    vector<vector<Mat>> AllCapOrg;

    for (int i = 0; i < NumOfCams; ++i)
    {
        vector<Mat> Temp;
        for (int j = 0; j < AllCapImages.size (); ++j)
        {

            Temp.push_back (AllCapImages[j][i]);
        }
        AllCapOrg.push_back (Temp);
    }

    return AllCapOrg;
}

vector<Mat> ProjectCapture1Cam(const vector <Mat>& Images, VideoCapture& cam, bool adjust )
{
    namedWindow("wind", WINDOW_NORMAL);
    moveWindow("wind", 2000,0);
    waitKey(300);
    cvSetWindowProperty("wind", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
    if (adjust)
    {
        cam.set( CAP_PROP_SETTINGS, 1 );
        imshow("wind", Images[1]);
        while(1)
        {
            Mat current;
            cam>>current;
            cvtColor( current, current, COLOR_RGB2GRAY );
            namedWindow("Frame");
            imshow("Frame", current);
            if (waitKey(100) == 32) break;
        }
    }
    int i = 0;
    vector<Mat> CapImages;
    while( i < (int) Images.size() )
    {
        Mat frameR, tmpR;
        imshow("wind", Images[i]);
        waitKey(400);
        //Workaroud: capture twice to grant new frame
        cam.grab();
        cam.retrieve(frameR);
        cam.grab();
        cam.retrieve(frameR);
        cvtColor( frameR, tmpR, COLOR_RGB2GRAY );
        CapImages.push_back(tmpR);
        ++i;
    }

    destroyWindow("wind");

    return CapImages;
}

void UndoAutoCamAdjusts(VideoCapture& cam)
{
    cam.set(CAP_PROP_GAIN, 0);
    cam.set(CAP_PROP_BRIGHTNESS, 128);
//    cam.set(CAP_PROP_CONTRAST, 128);
    cam.set(CAP_PROP_SATURATION, 128);
    cam.set(CAP_PROP_FOCUS, 0 );
}


vector<vector<Mat>> ProjectCapture2Cams(const vector <Mat>& Images, bool adjust, int RightCam, int LeftCam)
{
    VideoCapture camR(RightCam);
    VideoCapture camL(LeftCam);
    namedWindow("wind", WINDOW_NORMAL);
    moveWindow("wind", 2000,0);
    waitKey(300);
    cvSetWindowProperty("wind", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
    if (adjust)
    {
        for (int i = 0; i< 2; ++i)
        {
            VideoCapture cam(i);
            cam.set( CAP_PROP_SETTINGS, 1 );
            imshow("wind", Images[1]);

            while(1)
            {
                Mat current;
                cam>>current;
                cvtColor( current, current, COLOR_RGB2GRAY );
                namedWindow("Frame");
                imshow("Frame", current);
                if (waitKey(100) == 32) break;
            }
        }

    }

    vector<vector<Mat>> AllCapImages;
    int i = 0;

    while( i < (int) Images.size() )
    {
        vector<Mat> CapImages;
        Mat frameR, tmpR, frameL, tmpL;
        imshow("wind", Images[i]);
        waitKey(400);
        camR.grab();
        camR.retrieve(frameR);
        camR.grab();
        camR.retrieve(frameR);
        cvtColor( frameR, tmpR, COLOR_RGB2GRAY );
        CapImages.push_back(tmpR);

        camL.grab();
        camL.retrieve(frameL);
        camL.grab();
        camL.retrieve(frameL);
        cvtColor( frameL, tmpL, COLOR_RGB2GRAY );
        CapImages.push_back(tmpL);


        AllCapImages.push_back(CapImages);
        i++;
    }

    destroyWindow("wind");

    vector<vector<Mat>> AllCapOrg;

    for (int i = 0; i < 2; ++i)
    {
        vector<Mat> Temp;
        for (int j = 0; j < AllCapImages.size (); ++j)
        {

            Temp.push_back (AllCapImages[j][i]);
        }
        AllCapOrg.push_back (Temp);
    }

    return AllCapOrg;
}

void CalibrationCapture(string& BasePath, bool ToGray, bool adjust)
{
    VideoCapture cam(0);
    cam.set(CV_CAP_PROP_FRAME_WIDTH, 1920);
    cam.set(CV_CAP_PROP_FRAME_HEIGHT, 1080);
    if (adjust)
    {
        cam.set( CAP_PROP_SETTINGS, 1 );
        while(1)
        {
            Mat current;
            cam>>current;
            cvtColor( current, current, COLOR_RGB2GRAY );
            namedWindow("Frame");
            imshow("Frame", current);
            if (waitKey(50) == 32) break;
        }
    }
    vector<int> compression_params;
    compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
    compression_params.push_back(0);
    int i = 1;
    while(1)
    {
        Mat current;
        cam>>current;
        if (ToGray == true) cvtColor( current, current, COLOR_RGB2GRAY );
        namedWindow("Frame");
        imshow("Frame", current);
        int key = waitKey(20);
        if(key == 32)
        {
            imwrite(BasePath+to_string(i)+".png", current, compression_params);
            cout << BasePath+to_string(i)+".png" << endl;
            i++;
        }
        if(key == 27) break;
    }
}

void CalibrationCapture2Cams(string& BasePath, bool adjust)
{
    VideoCapture camR(0);
    VideoCapture camL(0);
    camR.set(CV_CAP_PROP_FRAME_WIDTH, 1920);
    camR.set(CV_CAP_PROP_FRAME_HEIGHT, 1080);
    camL.set(CV_CAP_PROP_FRAME_WIDTH, 1920);
    camL.set(CV_CAP_PROP_FRAME_HEIGHT, 1080);
    if (adjust)
    {
        namedWindow("Frame");
        camR.set( CAP_PROP_SETTINGS, 1 );
        while(1)
        {
            Mat current;
            camR>>current;
            cvtColor( current, current, COLOR_RGB2GRAY );

            imshow("Frame", current);
            if (waitKey(40) == 32) break;
        }

        camL.set( CAP_PROP_SETTINGS, 1 );
        while(1)
        {
            Mat current;
            camL>>current;
            cvtColor( current, current, COLOR_RGB2GRAY );
            imshow("Frame", current);
            if (waitKey(40) == 32) break;
        }
    }

    vector<int> compression_params;
    compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
    compression_params.push_back(0);

    int i = 1;
    namedWindow("FrameR");
    namedWindow("FrameL");
    moveWindow("FrameR", 800,200);
    moveWindow("FrameL", 100,200);
    while(1)
    {
        Mat currentR, currentL;
        camR >> currentR;
        camL >> currentL;
        cvtColor( currentR, currentR, COLOR_RGB2GRAY );
        cvtColor( currentL, currentL, COLOR_RGB2GRAY );

        imshow("FrameR", currentR);
        imshow("FrameL", currentL);

        int key = waitKey(20);

        if(key == 32)
        {
            imwrite("Right"+BasePath+to_string(i)+".png", currentR, compression_params);
            imwrite("Left"+BasePath+to_string(i)+".png", currentL, compression_params);
            cout << BasePath+to_string(i)+".png" << endl;
            i++;
        }

        if(key == 27) break;
    }
}

vector<Point3f> BuildObjPoints (Size NumCorners, int SquareSizeX, int SquareSizeY)
{
    vector<Point3f> ObjPoints;
    for( int i = 0; i < NumCorners.height; i++ )
    {
        for( int j = 0; j < NumCorners.width; j++ )
        {
            ObjPoints.push_back(cv::Point3f(i*SquareSizeX,j*SquareSizeY,0));
        }
    }
    return ObjPoints;
}

Mat BuildRtMat (Mat Rvec, Mat Tvec)
{
    Mat Rt;
    Mat last = (Mat_<double>(1,4) << 0, 0, 0, 1);
    hconcat (Rvec, Tvec, Rt);
    vconcat (Rt, last, Rt);
    return Rt;
}

void CalcRt2Cams (StereoCamParameters& Params, int SquareSizeX, int SquareSizeY, bool adjust)
{
    if (adjust)
    {
        for (int i = 0; i< 2; ++i)
        {
            VideoCapture cam(i);
            cam.set( CAP_PROP_SETTINGS, 1 );
            while(1)
            {
                Mat current;
                cam>>current;
                cvtColor( current, current, COLOR_RGB2GRAY );
                namedWindow("Frame");
                imshow("Frame", current);
                if (waitKey(100) == 32) break;
            }
        }
        destroyWindow ("Frame");
    }

    vector<Point2f> cornersR, cornersL;
    Size CBSize{14,9};

    VideoCapture camR(0);
    VideoCapture camL(1);

    Mat currentR, currentL;
    namedWindow("FrameR");
    moveWindow ("FrameR", 50,20);
    namedWindow("FrameL");
    moveWindow("FrameL", 700, 20);
    while(1)
    {
        camR >> currentR;
        camL >> currentL;
        cvtColor( currentR, currentR, COLOR_RGB2GRAY );
        cvtColor( currentL, currentL, COLOR_RGB2GRAY );
        imshow("FrameR", currentR);
        imshow("FrameL", currentL);
        if(waitKey(50) == 32)
        {
            bool validR = findChessboardCorners (currentR, CBSize, cornersR);

            bool validL = findChessboardCorners (currentL, CBSize, cornersL);

            if (validR && validL)
            {
                cornerSubPix (currentR, cornersR,  Size(5, 5), Size(-1, -1),
                              TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));
                cornerSubPix (currentL, cornersL,  Size(5, 5), Size(-1, -1),
                              TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));
                drawChessboardCorners (currentR,CBSize,cornersR,validR);
                drawChessboardCorners (currentL,CBSize,cornersL,validL);
                imshow("FrameR", currentR);
                imshow("FrameL", currentL);
                waitKey ();
                imwrite ("RightImage.png", currentR);
                imwrite ("LeftImage.png", currentL);
                qWarning() << "Acqsition done!";
                break;
            }else qWarning() << "Invalid acquisition! Try again";
        }
    }

    vector<Point3f> objpts = BuildObjPoints (CBSize, SquareSizeX, SquareSizeY);
    Mat RvecR, TvecR, RvecL, TvecL;
    StereoCamParametersCV ParamsCV (Params);

    solvePnP (objpts,cornersR,ParamsCV.CamR.KK,ParamsCV.CamR.kc,RvecR,TvecR);
    solvePnP (objpts,cornersL,ParamsCV.CamL.KK,ParamsCV.CamL.kc,RvecL,TvecL);
    Rodrigues (RvecR, RvecR);
    Rodrigues (RvecL, RvecL);
    Mat RtR = BuildRtMat (RvecR,TvecR);
    Mat RtL = BuildRtMat (RvecL,TvecL);

    FileStorage save("CalibParams.xml", FileStorage::WRITE);
    save << "KKR" << ParamsCV.CamR.KK;
    save << "kcR" << ParamsCV.CamR.kc;
    save << "KKL" << ParamsCV.CamL.KK;
    save << "kcL" << ParamsCV.CamL.kc;
    save << "RtR" << RtR;
    save << "RtL" << RtL;
    save.release ();

    cv2eigen(RtL,Params.CamParamL.Rt);
    cv2eigen(RtR,Params.CamParamR.Rt);

    Params.RtLR =  Params.CamParamR.Rt * Params.CamParamL.Rt.inverse();
    cout << Params.RtLR << endl;
}

void WatchCameras()
{
    VideoCapture capR(0);
    VideoCapture capL(1);
    namedWindow("Left");
    namedWindow("Right");
    for(;;)
    {
        Mat frameL, frameR;
        capL >> frameL;
        capR >> frameR;
        imshow("Left", frameL);
        imshow("Right", frameR);
        if ((char)32 == waitKey(20)) break;
    }
}

Matrix4Xd ProjectPoint(const Matrix4d &H, const Matrix4Xd &objtPts)
{
    // Normal projection -> pts = KK*Rt*obj
    Matrix4Xd imgPts = H*objtPts;

    // Normalized image points.Divide each point by Z
    imgPts = imgPts.array().rowwise() / imgPts.row(2).array();

    return imgPts;
}

void CameraParameters::SetK(const Matrix3d &KK3x3)
{
    KK3 = KK3x3;
    KK = Eigen::MatrixXd::Identity(4, 4);
    KK.block(0, 0, 3, 3) = KK3;
}

string StereoCamParameters::ToString () const
{
    string caml = CamParamL.ToString ();
    string camr = CamParamR.ToString ();
    return caml+camr;
}

string CameraParameters::ToString() const
{
    ostringstream s;
    s << "CameraParameters" << endl;
    s << "- KK:\n" << KK3 << endl;
    s << "- Rt:\n" << Rt << endl;
    s << "- kc: " << kc.transpose() << endl;
    s << "- ImageSize: " << ImageSize.transpose() << endl;
    return s.str();
}

StereoCamParameters ReadCamsParams(bool LoadRt)
{
    StereoCamParameters Params;
    Mat KKR, kcR, KKL, kcL;
    FileStorage load("CalibParams.xml", FileStorage::READ);
    load["KKR"] >> KKR;
    load["kcR"] >> kcR;
    load["KKL"] >> KKL;
    load["kcL"] >> kcL;

    cv2eigen(KKR,Params.CamParamR.KK3);
    cv2eigen(kcR,Params.CamParamR.kc);
    cv2eigen(KKL,Params.CamParamR.KK3);
    cv2eigen(kcL,Params.CamParamL.kc);
    Params.CamParamR.SetK (Params.CamParamR.KK3);
    Params.CamParamL.SetK (Params.CamParamR.KK3);

    Mat RtR, RtL;
    if (LoadRt)
    {
        load ["RtR"] >> RtR;
        load ["RtL"] >> RtL;
        cv2eigen(RtR,Params.CamParamR.Rt);
        cv2eigen(RtL,Params.CamParamL.Rt);
        Params.RtLR =  Params.CamParamR.Rt * Params.CamParamL.Rt.inverse();
    }
    load.release ();
    return Params;
}

void UndistorImages(vector<Mat>& Images, const Matrix3d& KK, const VectorXd& kc)
{
    Mat CameraMatrix, DistCoeff;
    eigen2cv (KK, CameraMatrix);
    eigen2cv (kc, DistCoeff);
    for(int i = 0; i < (int)Images.size(); i++)
    {
        undistort(Images[i],Images[i], CameraMatrix, DistCoeff);
    }
}
