set(PROJECT_NAME ocr_cpp_test)
project(${PROJECT_NAME})  
cmake_minimum_required(VERSION 2.8)  
set( CMAKE_EXPORT_COMPILE_COMMANDS ON )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++17 -W")  
  
set(OpenCV_DIR "~/Documents/opencv/opencv-3.4.1/release")
find_package(OpenCV REQUIRED)
# message("Found ${OpenCV_LIBS}")
include_directories(${OpenCV_INCLUDE_DIRS})

# set(CMAKE_PREFIX_PATH "/usr/local")
# set(Leptonica_DIR "./leptonica/build")
# find_package(Leptonica ${MINIMUM_LEPTONICA_VERSION} REQUIRED)
# message("Found ${Leptonica_LIBRARIES}")
# include_directories(${Leptonica_INCLUDE_DIRS})

# set(Tesseract_DIR "~/Documents/tesseract-ocr/build")
# find_package(Tesseract ${MINIMUM_TESSERACT_VERSION} REQUIRED)
# message("Found ${Tesseract_LIBRARIES}")
# include_directories(${Tesseract_INCLUDE_DIRS})
# include_directories(
#     /home/xshine4/Documents/tesseract-ocr/tesseract/include
#     /home/xshine4/Documents/tesseract-ocr/tesseract/build/include
# )
  
aux_source_directory(./src DIR_SRCS)  
  
# link_directories(/home/xshine4/Documents/tesseract-ocr/tesseract/build)
  
add_executable(${PROJECT_NAME} ${DIR_SRCS})  
target_link_libraries(${PROJECT_NAME}
    tesseract
    lept
    stdc++fs
    ${OpenCV_LIBS}
    # ${Leptonica_LIBRARIES}
    # ${Tesseract_LIBRARIES}
)

SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")
