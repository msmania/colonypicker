//
// colonypicker.cpp
//

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include "opencv2\imgproc\imgproc_c.h" // cvcanny
#include "opencv2/objdetect/objdetect.hpp" // CvHaarClassifierCascade

#pragma comment(lib, "opencv_core249.lib")
#pragma comment(lib, "opencv_highgui249.lib")

#pragma comment(lib, "opencv_imgproc249.lib") // cvcanny
#pragma comment(lib, "opencv_objdetect249.lib") // CvHaarClassifierCascade

const char LENA_IMAGE[] = "E:\\VSDev\\Projects\\colonypicker\\res\\lena.jpg";
const char COLONY_IMAGE[] = "E:\\VSDev\\Projects\\colonypicker\\res\\colony.jpg";
const char OFFICE_IMAGE[] = "E:\\VSDev\\Projects\\colonypicker\\res\\office.jpg";
const char CASCADE_DEFINITION[] = "E:\\VSDev\\Projects\\colonypicker\\res\\haarcascade_frontalface_default.xml";

// http://gihyo.jp/dev/feature/01/opencv/0002?page=2
void Test01() {
    IplImage *Image = NULL;

    Image = cvLoadImage(LENA_IMAGE, CV_LOAD_IMAGE_COLOR);
    if ( Image ) {
        cvNamedWindow("Lena", CV_WINDOW_AUTOSIZE);
        cvShowImage("Lena", Image);
        cvWaitKey(0);
        cvDestroyWindow("Lena");
        cvReleaseImage(&Image);
    }
}

// http://gihyo.jp/dev/feature/01/opencv/0002?page=3
void Test02() {
    IplImage *InputImage = NULL;
    IplImage *OutputImage = NULL;

    InputImage = cvLoadImage(LENA_IMAGE, CV_LOAD_IMAGE_GRAYSCALE);

    if ( InputImage ) {
        OutputImage = cvCreateImage(cvGetSize(InputImage), IPL_DEPTH_8U, 1);
        cvCanny(InputImage, OutputImage, 64, 128); // Canny edge detector

        cvNamedWindow("Lena", CV_WINDOW_AUTOSIZE);
        cvShowImage("Lena", OutputImage);
        cvWaitKey(0);
        cvDestroyWindow("Lena");
        
        cvReleaseImage(&InputImage);
        cvReleaseImage(&OutputImage);
    }
}

// http://gihyo.jp/dev/feature/01/opencv/0002?page=4
void Test03() {
    IplImage *InputImage = NULL;
    IplImage *OutputImage = NULL;
    CvMat *Rotation = NULL;

    InputImage = cvLoadImage(LENA_IMAGE, CV_LOAD_IMAGE_COLOR);
    if ( InputImage ) {    
        OutputImage = cvCreateImage(cvGetSize(InputImage), IPL_DEPTH_8U, 3);

        Rotation = cvCreateMat(2, 3, CV_32FC1);
        cv2DRotationMatrix(
            cvPoint2D32f(InputImage->height/2, InputImage->width/2),
            30.0, 1, Rotation);

        cvWarpAffine(InputImage, OutputImage, Rotation);

        cvNamedWindow("Lena", CV_WINDOW_AUTOSIZE);
        cvShowImage("Lena", OutputImage);
        cvWaitKey(0);
        cvDestroyWindow("Lena");
        
        cvReleaseImage(&InputImage);
        cvReleaseImage(&OutputImage);
    }
}

// http://gihyo.jp/dev/feature/01/opencv/0003?page=3
void Test04() {
    IplImage *Image  = NULL;
    CvHaarClassifierCascade *Cascade = NULL;
    CvMemStorage *Storage = NULL;
    CvSeq *Faces = NULL;

    Image = cvLoadImage(OFFICE_IMAGE, 1);
    Cascade = (CvHaarClassifierCascade*)cvLoad(CASCADE_DEFINITION);
    Storage = cvCreateMemStorage(0);

    Faces = cvHaarDetectObjects(Image, Cascade, Storage);

    for ( int i=0 ; i<Faces->total ; ++i ) {
        CvRect FaceRect = *(CvRect*)cvGetSeqElem(Faces, i);
        cvRectangle(Image,
            cvPoint(FaceRect.x, FaceRect.y), 
            cvPoint(FaceRect.x + FaceRect.width, FaceRect.y + FaceRect.height),
            CV_RGB(255, 0, 0), 3);
    }

    cvReleaseMemStorage(&Storage);
    
    cvNamedWindow("Face detection", CV_WINDOW_AUTOSIZE);
    cvShowImage("Face detection", Image);
    cvWaitKey(0);

    cvReleaseHaarClassifierCascade(&Cascade);
    cvDestroyWindow("Face detection");
    cvReleaseImage(&Image);
}
