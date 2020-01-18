#include <stdio.h>
#include <stdarg.h>
#include <experimental/filesystem>
#include <regex>
#include <opencv2/opencv.hpp>
#include <opencv2/freetype.hpp>
#include "utils.h"
// #include "CvxText.hpp"

using namespace cv;
using namespace std;
using namespace std::experimental;


void ShowManyImages(string title, int nArgs, ...)
{
    int size;
    int i;
    int m, n;
    int x, y;

    // w - Maximum number of images in a row
    // h - Maximum number of images in a column
    int w, h;

    // scale - How much we have to resize the image
    float scale;
    int max;

    // If the number of arguments is lesser than 0 or greater than 12
    // return without displaying
    if(nArgs <= 0) {
        printf("Number of arguments too small....\n");
        return;
    }
    else if(nArgs > 14) {
        printf("Number of arguments too large, can only handle maximally 12 images at a time ...\n");
        return;
    }
    // Determine the size of the image,
    // and the number of rows/cols
    // from number of arguments
    else if (nArgs == 1) {
        w = h = 1;
        size = 300;
    }
    else if (nArgs == 2) {
        w = 2; h = 1;
        size = 1000;
    }
    else if (nArgs == 3 || nArgs == 4) {
        w = 2; h = 2;
        size = 300;
    }
    else if (nArgs == 5 || nArgs == 6) {
        w = 3; h = 2;
        size = 200;
    }
    else if (nArgs == 7 || nArgs == 8) {
        w = 4; h = 2;
        size = 200;
    }
    else {
        w = 4; h = 3;
        size = 150;
    }

    // Create a new 3 channel image
    Mat DispImage = Mat::zeros(Size(100 + size*w, 60 + size*h), CV_8UC3);

    // Used to get the arguments passed
    va_list args;
    va_start(args, nArgs);

    // Loop for nArgs number of arguments
    for (i = 0, m = 20, n = 20; i < nArgs; i++, m += (20 + size)) {
        // Get the Pointer to the IplImage
        Mat img = va_arg(args, Mat);

        // Check whether it is NULL or not
        // If it is NULL, release the image, and return
        if(img.empty()) {
            printf("Invalid arguments");
            return;
        }

        // Find the width and height of the image
        x = img.cols;
        y = img.rows;

        // Find whether height or width is greater in order to resize the image
        max = (x > y)? x: y;

        // Find the scaling factor to resize the image
        scale = (float) ( (float) max / size );

        // Used to Align the images
        if( i % w == 0 && m!= 20) {
            m = 20;
            n+= 20 + size;
        }

        // Set the image ROI to display the current image
        // Resize the input image and copy the it to the Single Big Image
        Rect ROI(m, n, (int)( x/scale ), (int)( y/scale ));
        Mat temp; resize(img,temp, Size(ROI.width, ROI.height));
        temp.copyTo(DispImage(ROI));
    }

    // Create a new window, and show the Single Big Image
    namedWindow( title, 1 );
    imshow( title, DispImage);
    waitKey();

    // End the number of arguments
    va_end(args);
}

int GetFileMatchRegex(
        const std::string &search_directory,
        const std::string &match_regex_string,
        std::vector<std::string> &found_file_list,
        bool do_recursive_search)
{
    std::regex match_regex(match_regex_string);

    int num_file_found = 0;

    if (do_recursive_search == false)
    {
        for (const filesystem::directory_entry& r_entry :
                filesystem::directory_iterator(search_directory))
        {
            if (std::regex_match(r_entry.path().string().substr(search_directory.size()), match_regex) == true)
            {
                found_file_list.push_back(r_entry.path().string());
                ++num_file_found;
            }
        }
    }
    else if (do_recursive_search == true)
    {
        for (const filesystem::directory_entry& r_entry :
                filesystem::recursive_directory_iterator(search_directory))
        {
            if (std::regex_match(r_entry.path().string().substr(search_directory.size()), match_regex) == true)
            {
                found_file_list.push_back(r_entry.path().string().substr(search_directory.size()));
                ++num_file_found;
            }
        }
    }

    return num_file_found;
}

std::vector<std::string> s_split(const std::string& in, const std::string& delim) 
{
    std::regex re{ delim };
    return std::vector<std::string> 
    {
        std::sregex_token_iterator(in.begin(), in.end(), re, -1),
        std::sregex_token_iterator()
    };
}

void drawRect(cv::Mat& im, int left, int top, int right, int bottom)
{
    cv::rectangle(im, cv::Point(left, top), cv::Point(right, bottom), cv::Scalar(225, 178, 50), 3);
}

void drawText(cv::Mat& im, string word, int left, int top)
{
    //设置绘制文本的相关参数
	// std::string text = "Hello World!";
	int font_face = cv::FONT_HERSHEY_COMPLEX;
	double font_scale = 2;
	int thickness = 2;
	int baseline;
	//获取文本框的长宽
	// cv::Size text_size = cv::getTextSize(text, font_face, font_scale, thickness, &baseline);

    cv::Point origin;
    origin.x = left;
    origin.y = top;
    cv::putText(im, word, origin, font_face, font_scale, cv::Scalar(0, 255, 255), thickness, 8, 0);

}

void PutText(cv::Mat& img, const std::string& text, const cv::Rect& roi, const cv::Scalar& color)
{
	// int fontFace = cv::FONT_HERSHEY_COMPLEX; //cv::FONT_HERSHEY_SCRIPT_SIMPLEX
	// double fontScale = 2;
	int thickness = 2;  //8
    int baseline = 0;
    int lineType = 8;

    CV_Assert(!img.empty() && (img.type() == CV_8UC3 || img.type() == CV_8UC1));
    CV_Assert(roi.area() > 0);
    CV_Assert(!text.empty());

    // Calculates the width and height of a text string
    // cv::Size textSize = cv::getTextSize(text, fontFace, fontScale, thickness, &baseline);
    int fontHeight = 30;
    Ptr<freetype::FreeType2> ft2 = freetype::createFreeType2();
    //下面的字库要自己下载并拷贝到需要的位置
    ft2->loadFontData( "../data/simhei.ttf", 0 );
    Size textSize = ft2->getTextSize(text, fontHeight, thickness, &baseline);

    // Y-coordinate of the baseline relative to the bottom-most text point
    if(thickness > 0){
        baseline += thickness;
    }

    // Render the text over here (fits to the text size)
    cv::Mat textImg(textSize.height + baseline, textSize.width, img.type());

    if (color == cv::Scalar::all(0)) textImg = cv::Scalar::all(255);
    else textImg = cv::Scalar::all(0);

    // Estimating the resolution of bounding image
    cv::Point textOrg((textImg.cols - textSize.width) / 2,
                      (textImg.rows + textSize.height - baseline) / 2);

    // cv::putText(textImg, text, textOrg, fontFace, fontScale, color, thickness);
    ft2->putText(textImg, text, textOrg, fontHeight, color, thickness, lineType, true);

    std::cout<< "processing: " << text <<std::endl;
    // imshow("box", textImg);
    // waitKey(0);

    // Resizing according to the ROI
    cv::resize(textImg, textImg, roi.size());
    std::cout<< "resize: " << std::endl;

    cv::Mat textImgMask = textImg;
    if (textImgMask.type() == CV_8UC3)
        cv::cvtColor(textImgMask, textImgMask, cv::COLOR_BGR2GRAY);

    // Creating the mask
    cv::equalizeHist(textImgMask, textImgMask);
    std::cout<< "mask: " << std::endl;

    if (color == cv::Scalar::all(0)) cv::threshold(textImgMask, textImgMask, 1, 255, cv::THRESH_BINARY_INV);
    else cv::threshold(textImgMask, textImgMask, 254, 255, cv::THRESH_BINARY);

    std::cout<< "th: " << std::endl;
    // Put into the original image
    cv::Mat destRoi = img(roi);
    textImg.copyTo(destRoi, textImgMask);
}

/*本方法能移除 \n \t 等*/
string trim_all_remove_if(string &originStr)
{
    std::string str = originStr;

    // std::cout << "origin str:      |" << str << "|" << std::endl;
    // trim all spaces: remove_if
    auto itor = remove_if(str.begin(), str.end(), ::isspace);
    // std::cout << "after remove_if: |" << str << "|" << std::endl;
    str.erase(itor, str.end());
    // std::cout << "after era:       |" << str << "|" << std::endl;

    return str;
}
