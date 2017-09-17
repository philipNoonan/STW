#include <imgui.h>
#include "imgui_impl_glfw_gl3.h"

#include <stdio.h>
#include <iostream>
#define GLUT_NO_LIB_PRAGMA
//##### OpenGL ######
#include <GL/glew.h>
#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

#include "kRender.h"
#include "interface.h"
#include "openCVStuff.h"



GLFWwindow *window;

kRender krender;

Freenect2Camera kcamera;

openCVStuff OCVStuff;

cv::Mat flow;



/////////////////////////
// KINECT STUFF

const int screenWidth = 1920;
const int screenHeight = 1080;

const int colorWidth = 1920;
const int colorHeight = 1080;

const int depthWidth = 512;
const int depthHeight = 424;

float *mainColor[colorWidth * colorHeight];

float colorArray[colorWidth * colorHeight];
float previousColorArray[depthWidth * depthHeight];
float bigDepthArray[colorWidth * (colorHeight + 2)]; // 1082 is not a typo
													 //float color[512 * 424];
float depthArray[depthWidth * depthHeight];
float infraredArray[depthWidth * depthHeight];
int colorDepthMap[depthWidth * depthHeight];

// depth color points picking
bool select_color_points_mode = false;
bool select_depth_points_mode = false;

std::vector<cv::Point3f> depthPoints;
std::vector<cv::Point2f> colorPoints;
cv::Mat newColor;
