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

cv::Mat flow;// = cv::Mat(424, 512, CV_8UC3);
//cv::Mat tFlow;



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

bool showDepthFlag = false;
bool showBigDepthFlag = false;
bool showInfraFlag = false;
bool showColorFlag = false;
bool showLightFlag = false;
bool showPointFlag = false;

bool showFlowFlag = false;
bool showEdgesFlag = false;

float irBrightness = 1.0;
float irLow = 0.0f;
float irHigh = 65536.0f;
float vertFov = 35.0f;



float xRot = 0.0f;
float zRot = 0.0f;
float yRot = 0.0f;
float xTran = 0.0f;
float yTran = 0.0f;
float zTran = 2000.0f;
void resetSliders() 
{
	xRot = 0.0f;
	zRot = 0.0f;
	yRot = 0.0f;
	xTran = 0.0f;
	yTran = 0.0f;
	zTran = 2000.0f;
}

float zModelPC_offset = 0.0f;

cv::Mat infraGrey;

bool calibratingFlag = false;

//////////////////////////////////////////////////
// SAVING IMAGES

