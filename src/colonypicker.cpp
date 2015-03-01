//
// HoughCIircles
//

#include <stdio.h>
#include <fstream>

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#define LOGERROR(msg, ...) wprintf(msg, __VA_ARGS__)
#define LOGINFO LOGERROR

#ifdef _DEBUG
#pragma comment(lib, "opencv_core249d.lib")
#pragma comment(lib, "opencv_highgui249d.lib")
#pragma comment(lib, "opencv_imgproc249d.lib")
#else
#pragma comment(lib, "opencv_core249.lib")
#pragma comment(lib, "opencv_highgui249.lib")
#pragma comment(lib, "opencv_imgproc249.lib")
#endif

using namespace cv;

bool ColonyPicking(const char *ImageFile, const char *OutputImage, std::wofstream &Statistics) {
    Mat Image = imread(ImageFile, 0);
    if ( Image.empty()) {
        LOGERROR(L"Failed to load image: `%S`\n", ImageFile);
        return false;
    }
    
    medianBlur(Image, Image, 5);
    
    Mat ConvertedImage;
    cvtColor(Image, ConvertedImage, COLOR_GRAY2BGR);

    vector<Vec3f> Circles;
    HoughCircles(Image, Circles, CV_HOUGH_GRADIENT,

        // dp - Inverse ratio of the accumulator resolution to the image resolution.
        //For example, if dp=1 , the accumulator has the same resolution as the input image.
        // If dp=2 , the accumulator has half as big width and height.
        1,

        // minDist - Minimum distance between the centers of the detected circles.
        // If the parameter is too small, multiple neighbor circles may be falsely detected in addition to a true one.
        // If it is too large, some circles may be missed.
        12,

        // param1 - First method-specific parameter.
        // In case of CV_HOUGH_GRADIENT , it is the higher threshold of the two passed to the Canny()edge detector (the lower one is twice smaller).
        100,

        // param2 - Second method-specific parameter.
        // In case of CV_HOUGH_GRADIENT , it is the accumulator threshold for the circle centers at the detection stage.
        // The smaller it is, the more false circles may be detected. Circles, corresponding to the larger accumulator values, will be returned first.
        16,

        1, 30 // min/max radius
        );

    for( size_t i = 0; i < Circles.size(); ++i ) {
        Vec3i c = Circles[i];
        circle( ConvertedImage, Point(c[0], c[1]), c[2], Scalar(0,0,255), 3, CV_AA);
        circle( ConvertedImage, Point(c[0], c[1]), 2, Scalar(0,255,0), 3, CV_AA);
        
        if ( Statistics.is_open() )
            Statistics << ImageFile << ", " << i << ", " << c[0] << ", " << c[1] << ", " << c[2] << std::endl;
    }

    LOGINFO(L"; %S -> %d colonies\n", ImageFile, Circles.size());

    if ( OutputImage )
        imwrite(OutputImage, ConvertedImage);
    
    return true;
}
