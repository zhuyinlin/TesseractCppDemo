#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <opencv2/opencv.hpp>
#include <tesseract/renderer.h>
#include <iostream>
#include <string>
// #include <typeinfo>
// #include <vector>
// #include <utility>
// #include <fstream>
#include <experimental/filesystem>
#include <regex>
#include "utils.h"

using namespace cv;
using namespace std;
// using namespace std::experimental;


static void PreloadRenderers(
    tesseract::TessBaseAPI* api,
    tesseract::PointerVector<tesseract::TessResultRenderer>* renderers,
    tesseract::PageSegMode pagesegmode, const char* outputbase) {
  if (pagesegmode == tesseract::PSM_OSD_ONLY) {
#ifndef DISABLED_LEGACY_ENGINE
    renderers->push_back(new tesseract::TessOsdRenderer(outputbase));
#endif  // ndef DISABLED_LEGACY_ENGINE
  } else {
    bool error = false;
    bool b;
    api->GetBoolVariable("tessedit_create_hocr", &b);
    if (b) {
      bool font_info;
      api->GetBoolVariable("hocr_font_info", &font_info);
      auto* renderer =
          new tesseract::TessHOcrRenderer(outputbase, font_info);
      if (renderer->happy()) {
        renderers->push_back(renderer);
      } else {
        delete renderer;
        // tprintf("Error, could not create hOCR output file: %s\n",
        //         strerror(errno));
        error = true;
      }
    }

    api->GetBoolVariable("tessedit_create_alto", &b);
    if (b) {
      auto* renderer =
              new tesseract::TessAltoRenderer(outputbase);
      if (renderer->happy()) {
        renderers->push_back(renderer);
      } else {
        delete renderer;
        // tprintf("Error, could not create ALTO output file: %s\n",
        //         strerror(errno));
        error = true;
      }
    }

    api->GetBoolVariable("tessedit_create_tsv", &b);
    if (b) {
      bool font_info;
      api->GetBoolVariable("hocr_font_info", &font_info);
      auto* renderer =
          new tesseract::TessTsvRenderer(outputbase, font_info);
      if (renderer->happy()) {
        renderers->push_back(renderer);
      } else {
        delete renderer;
        // tprintf("Error, could not create TSV output file: %s\n",
        //         strerror(errno));
        error = true;
      }
    }

    api->GetBoolVariable("tessedit_create_pdf", &b);
    if (b) {
      #ifdef WIN32
        if (_setmode(_fileno(stdout), _O_BINARY) == -1)
          // tprintf("ERROR: cin to binary: %s", strerror(errno));
      #endif  // WIN32
      bool textonly;
      api->GetBoolVariable("textonly_pdf", &textonly);
      auto* renderer =
        new tesseract::TessPDFRenderer(outputbase, api->GetDatapath(),
                                       textonly);
      if (renderer->happy()) {
        renderers->push_back(renderer);
      } else {
        delete renderer;
        // tprintf("Error, could not create PDF output file: %s\n",
        //         strerror(errno));
        error = true;
      }
    }

    api->GetBoolVariable("tessedit_write_unlv", &b);
    if (b) {
      api->SetVariable("unlv_tilde_crunching", "true");
      auto* renderer =
        new tesseract::TessUnlvRenderer(outputbase);
      if (renderer->happy()) {
        renderers->push_back(renderer);
      } else {
        delete renderer;
        // tprintf("Error, could not create UNLV output file: %s\n",
        //         strerror(errno));
        error = true;
      }
    }

    api->GetBoolVariable("tessedit_create_lstmbox", &b);
    if (b) {
      auto* renderer =
        new tesseract::TessLSTMBoxRenderer(outputbase);
      if (renderer->happy()) {
        renderers->push_back(renderer);
      } else {
        delete renderer;
        // tprintf("Error, could not create LSTM BOX output file: %s\n",
        //         strerror(errno));
        error = true;
      }
    }

    api->GetBoolVariable("tessedit_create_boxfile", &b);
    if (b) {
      auto* renderer =
        new tesseract::TessBoxTextRenderer(outputbase);
      if (renderer->happy()) {
        renderers->push_back(renderer);
      } else {
        delete renderer;
        // tprintf("Error, could not create BOX output file: %s\n",
        //         strerror(errno));
        error = true;
      }
    }

    api->GetBoolVariable("tessedit_create_wordstrbox", &b);
    if (b) {
      auto* renderer =
        new tesseract::TessWordStrBoxRenderer(outputbase);
      if (renderer->happy()) {
        renderers->push_back(renderer);
      } else {
        delete renderer;
        // tprintf("Error, could not create WordStr BOX output file: %s\n",
        //         strerror(errno));
        error = true;
      }
    }

    api->GetBoolVariable("tessedit_create_txt", &b);
    if (b || (!error && renderers->empty())) {
      // Create text output if no other output was requested
      // even if text output was not explicitly requested unless
      // there was an error.
      auto* renderer =
        new tesseract::TessTextRenderer(outputbase);
      if (renderer->happy()) {
        renderers->push_back(renderer);
      } else {
        delete renderer;
        // tprintf("Error, could not create TXT output file: %s\n",
        //         strerror(errno));
      }
    }
  }

  if (!renderers->empty()) {
    // Since the PointerVector auto-deletes, null-out the renderers that are
    // added to the root, and leave the root in the vector.
    for (int r = 1; r < renderers->size(); ++r) {
      (*renderers)[0]->insert((*renderers)[r]);
      (*renderers)[r] = nullptr;
    }
  }
}

void basicSample(const char* imagePath, const char* outFileName)
{
    char *outText;
 
    tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
    // Initialize tesseract-ocr with English, without specifying tessdata path
    if (api->Init(NULL, "chi_sim")) {
        fprintf(stderr, "Could not initialize tesseract.\n");
        exit(1);
    }
 
    // Open input image with leptonica library
    Pix *image = pixRead(imagePath); //"../data/phototest.png"
    api->SetImage(image);

    // Get OCR result
    outText = api->GetUTF8Text();
    ofstream OutFile(outFileName);
    OutFile << outText;
    OutFile.close();
    // printf("OCR output:\n%s", outText);
 
    // Destroy used object and release memory
    api->End();
    delete api;
    // delete [] outText;
    pixDestroy(&image);
}

void boxSample(const char* imagePath)
{
    tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
    // Initialize tesseract-ocr with English, without specifying tessdata path
    if (api->Init(NULL, "chi_sim")) {
        fprintf(stderr, "Could not initialize tesseract.\n");
        exit(1);
    }
 
    // Open input image with leptonica library
    Pix *image = pixRead(imagePath);
    api->SetImage(image);

    // Get Box
    Boxa* boxes = api->GetComponentImages(tesseract::RIL_TEXTLINE, true, NULL, NULL);
    printf("Found %d textline image components.\n", boxes->n);
    for (int i = 0; i < boxes->n; i++) {
        BOX* box = boxaGetBox(boxes, i, L_CLONE);
        api->SetRectangle(box->x, box->y, box->w, box->h);
        char* ocrResult = api->GetUTF8Text();
        int conf = api->MeanTextConf();
        fprintf(stdout, "Box[%d]: x=%d, y=%d, w=%d, h=%d, confidence: %d, text: %s",
                i, box->x, box->y, box->w, box->h, conf, ocrResult);
        boxDestroy(&box);
    }

    // Destroy used object and release memory
    api->End();
    delete api;
    // delete [] outText;
    pixDestroy(&image);
}

void lineSample(const char* imagePath)
{
    tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
    // Initialize tesseract-ocr with English, without specifying tessdata path
    if (api->Init(NULL, "chi_sim")) {
        fprintf(stderr, "Could not initialize tesseract.\n");
        exit(1);
    }
 
    // Open input image with leptonica library
    Pix *image = pixRead(imagePath);
    api->SetImage(image);

    cv::Mat originIm = cv::imread(imagePath, 1); // show image
    cv::Mat ocrIm = Mat::zeros(Size(originIm.cols, originIm.rows), CV_8UC3);

    // api->SetPageSegMode(tesseract::PSM_AUTO);
    std::cout << "psm: " << api->GetPageSegMode() << std::endl;
    api->Recognize(0);
    tesseract::ResultIterator* ri = api->GetIterator();
    // RIL_BLOCK,     // Block of text/image/separator line.
    // RIL_PARA,      // Paragraph within a block.
    // RIL_TEXTLINE,  // Line within a paragraph.
    // RIL_WORD,      // Word within a textline.
    // RIL_SYMBOL     // Symbol/character within a word.
    tesseract::PageIteratorLevel level = tesseract::RIL_TEXTLINE;
    if (ri != 0) {
        do {
            const char* word = ri->GetUTF8Text(level);
            float conf = ri->Confidence(level);
            int x1, y1, x2, y2;
            ri->BoundingBox(level, &x1, &y1, &x2, &y2);
            // printf("word: '%s';  \tconf: %.2f; BoundingBox: %d,%d,%d,%d;\n",
            //         word, conf, x1, y1, x2, y2);
            // drawRect(ocrIm, x1, y1, x2, y2);
            string tmp(word);
            if (trim_all_remove_if(tmp).empty() ) continue;
            // drawText(ocrIm, tmp, x1, y1);
            cv::Rect roi(x1, y1, x2-x1, y2-y1);
            cv::Scalar color(255, 255, 255);
            PutText(ocrIm, tmp, roi, color);
            delete[] word;
        } while (ri->Next(level));

        ShowManyImages("Image", 2, originIm, ocrIm);

        // cv::imshow("BoundingBox", im);
        // cv::waitKey(0);
    }

    // Destroy used object and release memory
    api->End();
    delete api;
    // delete [] outText;
    pixDestroy(&image);
}

void wordSample(const char* imagePath)
{
    tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
    // Initialize tesseract-ocr with English, without specifying tessdata path
    if (api->Init(NULL, "chi_sim")) {
        fprintf(stderr, "Could not initialize tesseract.\n");
        exit(1);
    }
 
    // Open input image with leptonica library
    Pix *image = pixRead(imagePath);
    api->SetImage(image);

    api->Recognize(0);
    tesseract::ResultIterator* ri = api->GetIterator();
    tesseract::PageIteratorLevel level = tesseract::RIL_WORD;
    if (ri != 0) {
        do {
            const char* word = ri->GetUTF8Text(level);
            float conf = ri->Confidence(level);
            int x1, y1, x2, y2;
            ri->BoundingBox(level, &x1, &y1, &x2, &y2);
            printf("word: '%s';  \tconf: %.2f; BoundingBox: %d,%d,%d,%d;\n",
                    word, conf, x1, y1, x2, y2);
            delete[] word;
        } while (ri->Next(level));
    }

    // Destroy used object and release memory
    api->End();
    delete api;
    // delete [] outText;
    pixDestroy(&image);
}

void pageAna(const char* imagePath)
{
    // tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
    tesseract::TessBaseAPI tessApi;
    tessApi.InitForAnalysePage();
 
    //加入的调试信息
    // tessApi.SetVariable("tessedit_dump_pageseg_images", "true");    //show no lines and no image picture
    // tessApi.SetVariable("textord_show_blobs", "true");  //show blobs result
    // tessApi.SetVariable("textord_show_boxes", "true");  //show blobs' bounding boxes
    // tessApi.SetVariable("textord_tabfind_show_blocks", "true"); //show candidate tab-stops and tab vectors
    // tessApi.SetVariable("textord_tabfind_show_reject_blobs", "true");   //show rejected blobs
    // tessApi.SetVariable("textord_tabfind_show_initial_partitions", "true"); //show initial partitions
    // tessApi.SetVariable("textord_tabfind_show_partitions", "1");    //show final partitions
    // tessApi.SetVariable("textord_tabfind_show_initialtabs", "true");    //show initial tab-stops
    // tessApi.SetVariable("textord_tabfind_show_finaltabs", "true");  //show final tab vectors
    // tessApi.SetVariable("textord_tabfind_show_images", "true"); //show image blobs

    //PSM_AUTO_ONLY
    // tessApi.SetPageSegMode(tesseract::PSM_AUTO_ONLY);
    Pix *image = pixRead(imagePath);
    tessApi.SetImage(image);

    tesseract::PageIterator *iter = tessApi.AnalyseLayout();

    /*const char* outputbase = "./output"; //nullptr;
    tessApi.SetOutputName(outputbase);
    tesseract::PageSegMode pagesegmode = tesseract::PSM_AUTO;

    Set in_training_mode to true when using one of these configs:
    ambigs.train, box.train, box.train.stderr, linebox, rebox, lstm.train.
    In this mode no other OCR result files are written.
    bool b = false;
    bool in_training_mode =
        (tessApi.GetBoolVariable("tessedit_ambigs_training", &b) && b) ||
        (tessApi.GetBoolVariable("tessedit_resegment_from_boxes", &b) && b) ||
        (tessApi.GetBoolVariable("tessedit_make_boxes_from_boxes", &b) && b) ||
        (tessApi.GetBoolVariable("tessedit_train_line_recognizer", &b) && b);

    // Avoid memory leak caused by auto variable when exit() is called.
    static tesseract::PointerVector<tesseract::TessResultRenderer> renderers;

    if (in_training_mode) {
        fprintf(stdout, "============ in training mode:\n");
        renderers.push_back(nullptr);
    } else if (outputbase != nullptr) {
        fprintf(stdout, "============ not in training mode:\n");
        PreloadRenderers(&tessApi, &renderers, pagesegmode, outputbase);
    }

    if (!renderers.empty()) {
        fprintf(stdout, "============ debug:\n");
        //加入的调试信息
        tessApi.SetVariable("tessedit_dump_pageseg_images", "true");    //show no lines and no image picture
        tessApi.SetVariable("textord_show_blobs", "true");  //show blobs result
        tessApi.SetVariable("textord_show_boxes", "true");  //show blobs' bounding boxes
        tessApi.SetVariable("textord_tabfind_show_blocks", "true"); //show candidate tab-stops and tab vectors
        tessApi.SetVariable("textord_tabfind_show_reject_blobs", "true");   //show rejected blobs
        tessApi.SetVariable("textord_tabfind_show_initial_partitions", "true"); //show initial partitions
        tessApi.SetVariable("textord_tabfind_show_partitions", "1");    //show final partitions
        tessApi.SetVariable("textord_tabfind_show_initialtabs", "true");    //show initial tab-stops
        tessApi.SetVariable("textord_tabfind_show_finaltabs", "true");  //show final tab vectors
        tessApi.SetVariable("textord_tabfind_show_images", "true"); //show image blobs

        bool succeed = tessApi.ProcessPages(imagePath, nullptr, 0, renderers[0]);
        if (!succeed) {
            fprintf(stderr, "Error during processing.\n");
        }
    }*/

    cv::Mat im = cv::imread(imagePath, 1);
    // Instead of RIL_WORD you can use any other PageSegMode
    //RIL_WORD, RIL_TEXTLINE
    while (iter->Next(tesseract::RIL_WORD)) {
        int left, top, right, bottom;

        iter->BoundingBox(
                tesseract::RIL_WORD,
                &left, &top, &right, &bottom
                );
        drawRect(im, left, top, right, bottom);
    }
    cv::imshow("BoundingBox", im);
    cv::waitKey(0);

    // Destroy used object and release memory
    tessApi.End();
    delete &tessApi;
    // delete [] outText;
    pixDestroy(&image);
}

void batchSample(std::string imgPath, std::string outPath) 
{
    // cv::String pattern = "/home/xshine4/Downloads/tmp/123/*.png";
    // vector<cv::String> fn;
    // glob(pattern, fn, false);

    // vector<Mat> images;
    // size_t count = fn.size(); //number of png files in images folder
    // for (size_t i = 0; i < count; i++)
    // {
    //     // images.push_back(imread(fn[i]));
    //     // imshow("img", imread(fn[i]));
    //     // waitKey(1000);
    // }
    // return images;
    //
    // std::string imgPath = "/home/xshine4/Downloads/tmp/123/";
    // std::string outPath = "/home/xshine4/Downloads/tmp/output/";

    std::vector<std::string> imgFullNameList;
    int imgNum = GetFileMatchRegex(imgPath,".*png$", imgFullNameList, true);
    std::cout << "total image num: " << imgNum << std::endl;

    std::string imgName;
    for(int num =0; num < imgNum; num++)
    {
        std::string fullImgPath = imgPath + imgFullNameList.at(num);
        imgName = s_split(imgFullNameList.at(num), ".png").at(0);
        std::cout << "processing: " << fullImgPath << std::endl;

        std::string outFileName = outPath + imgName + ".txt";

        std::cout << "output: " << outFileName << std::endl;
        // basicSample(fullImgPath.c_str(), outFileName.c_str());
    }
}

// void ShowManyImages(string title, int nArgs, ...)
// {
//     int size;
//     int i;
//     int m, n;
//     int x, y;
//
//     // w - Maximum number of images in a row
//     // h - Maximum number of images in a column
//     int w, h;
//
//     // scale - How much we have to resize the image
//     float scale;
//     int max;
//
//     // If the number of arguments is lesser than 0 or greater than 12
//     // return without displaying
//     if(nArgs <= 0) {
//         printf("Number of arguments too small....\n");
//         return;
//     }
//     else if(nArgs > 14) {
//         printf("Number of arguments too large, can only handle maximally 12 images at a time ...\n");
//         return;
//     }
//     // Determine the size of the image,
//     // and the number of rows/cols
//     // from number of arguments
//     else if (nArgs == 1) {
//         w = h = 1;
//         size = 300;
//     }
//     else if (nArgs == 2) {
//         w = 2; h = 1;
//         size = 800;
//     }
//     else if (nArgs == 3 || nArgs == 4) {
//         w = 2; h = 2;
//         size = 300;
//     }
//     else if (nArgs == 5 || nArgs == 6) {
//         w = 3; h = 2;
//         size = 200;
//     }
//     else if (nArgs == 7 || nArgs == 8) {
//         w = 4; h = 2;
//         size = 200;
//     }
//     else {
//         w = 4; h = 3;
//         size = 150;
//     }
//
//     // Create a new 3 channel image
//     Mat DispImage = Mat::zeros(Size(100 + size*w, 60 + size*h), CV_8UC3);
//
//     // Used to get the arguments passed
//     va_list args;
//     va_start(args, nArgs);
//
//     // Loop for nArgs number of arguments
//     for (i = 0, m = 20, n = 20; i < nArgs; i++, m += (20 + size)) {
//         // Get the Pointer to the IplImage
//         Mat img = va_arg(args, Mat);
//
//         // Check whether it is NULL or not
//         // If it is NULL, release the image, and return
//         if(img.empty()) {
//             printf("Invalid arguments");
//             return;
//         }
//
//         // Find the width and height of the image
//         x = img.cols;
//         y = img.rows;
//
//         // Find whether height or width is greater in order to resize the image
//         max = (x > y)? x: y;
//
//         // Find the scaling factor to resize the image
//         scale = (float) ( (float) max / size );
//
//         // Used to Align the images
//         if( i % w == 0 && m!= 20) {
//             m = 20;
//             n+= 20 + size;
//         }
//
//         // Set the image ROI to display the current image
//         // Resize the input image and copy the it to the Single Big Image
//         Rect ROI(m, n, (int)( x/scale ), (int)( y/scale ));
//         Mat temp; resize(img,temp, Size(ROI.width, ROI.height));
//         temp.copyTo(DispImage(ROI));
//     }
//
//     // Create a new window, and show the Single Big Image
//     namedWindow( title, 1 );
//     imshow( title, DispImage);
//     waitKey();
//
//     // End the number of arguments
//     va_end(args);
// }

int main()
{
    // basic > command line > box, why?
        
    // basicSample("../data/phototest.png", "../output/test1.txt");
    // boxSample("../data/phototest.png");
    lineSample("../data/phototest.png");
    // wordSample("../data/phototest.png");

    // pageAna("../data/phototest.png");

    // batchSample("/home/xshine4/Downloads/tmp/123/",
    //             "/home/xshine4/Downloads/tmp/output/");

    // Mat img(150,600,CV_8UC3,Scalar(255,255,255));//初始化图像
	// putTextZH(img, "力拔山兮气盖世", Point(30, 30), Scalar(0, 0, 0), 80, "华文行楷");
	// // imwrite("1.png", img);
	// imshow("test", img);
	// waitKey(0);

    return 0;
}
