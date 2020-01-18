#include <opencv2/opencv.hpp>
#include <string>


/*Function///////////////////////////////////////////////////////////////

Name:       ShowManyImages

Purpose:

This is a function illustrating how to display more than one
image in a single window using Intel OpenCV

Parameters:

string title: Title of the window to be displayed
int    nArgs: Number of images to be displayed
Mat    img1: First Mat, which contains the first image
...
Mat    imgN: First Mat, which contains the Nth image

Language:   C++

The method used is to set the ROIs of a Single Big image and then resizing
and copying the input images on to the Single Big Image.

This function does not stretch the image...
It resizes the image without modifying the width/height ratio..

This function can be called like this:

ShowManyImages("Images", 5, img2, img2, img3, img4, img5);

This function can display upto 12 images in a single window.
It does not check whether the arguments are of type Mat or not.
The maximum window size is 700 by 660 pixels.
Does not display anything if the number of arguments is less than
one or greater than 12.

Idea was from [[BettySanchi]] of OpenCV Yahoo! Groups.

If you have trouble compiling and/or executing
this code, I would like to hear about it.

You could try posting on the OpenCV Yahoo! Groups
[url]http://groups.yahoo.com/group/OpenCV/messages/ [/url]


Parameswaran,
Chennai, India.

cegparamesh[at]gmail[dot]com

...
///////////////////////////////////////////////////////////////////////*/
void ShowManyImages(std::string title, int nArgs, ...);

/**
 * @ search_directory 文件匹配路径
 * @ match_regex_string 匹配正则表达式
 * @ found_file_list 搜索到的文件名称列表
 *
 * @return 返回搜寻到文件的个数
 *
 * 用到std17 中的filesystem.h
 */
int GetFileMatchRegex(
        const std::string &search_directory,
        const std::string &match_regex_string,
        std::vector<std::string> &found_file_list,
        bool do_recursive_search);

/**
 * @in 待切分的字符串
 * @delim 切分符号
 * 
 * @return 所有切分的部分
 */
std::vector<std::string> s_split(const std::string& in, const std::string& delim) ;

void drawRect(cv::Mat& im, int left, int top, int right, int bottom);

void drawText(cv::Mat& im, std::string word, int left, int top);

void PutText(cv::Mat& img, const std::string& text, const cv::Rect& roi, const cv::Scalar& color);

std::string trim_all_remove_if(std::string &originStr);
