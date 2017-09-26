#define GLUT_NO_LIB_PRAGMA
//##### OpenGL ######
#include <GL/glew.h>
#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/transform.hpp>

//#include "shader.hpp"


#include "opencv2/core/utility.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"




#include <iostream>
#include <fstream>

#include <vector>
#include <list>
#include <numeric>

#include "glutils.h"
#include "glslprogram.h"

#define _USE_MATH_DEFINES
#include <math.h>

class kRender
{
	public:
		kRender()
			: m_window()
			, m_show_imgui(true)
			, m_screen_height(1080)
			, m_screen_width(1920)
			, m_depth_height(424)
			, m_depth_width(512)
			, m_color_height(1080)
			, m_color_width(1920)
			, m_big_depth_height(1082)
			, m_big_depth_width(1920)
			, m_VAO()
			, m_VBO_Color()
			, m_VBO_Depth()
			, m_EBO()
			, m_gui_padding(std::make_pair<int,int>(50, 50))
			, m_render_scale_height(1.0f)
			, m_render_scale_width(1.0f)
			//, m_graph_points_x()
			//, m_graph_points_y()
			//, m_graph_points_z()
			//, m_graph_points_long_x()
			//, m_graph_points_long_y()
			//, m_graph_points_long_z()
			//, m_graph_vector_x()
		{}
		~kRender();

		GLFWwindow * window()
		{
			return m_window;
		}
		
		void SetCallbackFunctions();

		bool showImgui()
		{
			return m_show_imgui;
		}

		float renderScaleHeight()
		{
			return m_render_scale_height;
		}
		void renderScaleHeight(float scale)
		{
			m_render_scale_height = scale;
		}
		float renderScaleWidth()
		{
			return m_render_scale_width;
		}
		void renderScaleWidth(float scale)
		{
			m_render_scale_width = scale;
		}

		//std::vector<float> graphPointsX()
		//{
		//	return m_graph_vector_x;
		//}


		std::pair<int, int> guiPadding()
		{
			return std::make_pair<int, int> (m_gui_padding.first * m_render_scale_width, m_gui_padding.second * m_render_scale_height);
		}

		std::vector<float> getDepthPoints()
		{
			return m_depthPointsFromBuffer;
		}

		void getDepthPoints3D();

		void getMouseClickPositionsDepth();

		void anchorMW(std::pair<int, int> anchor)
		{
			m_anchorMW = std::make_pair<int, int> ((float)anchor.first * m_render_scale_width, (float)anchor.second * m_render_scale_height);
		}
		void anchorSG(std::pair<int, int> anchor)
		{
			m_anchorSG = anchor;
		}
		void anchorAS(std::pair<int, int> anchor)
		{
			m_anchorAS = anchor;
		}
		GLFWwindow * loadGLFWWindow();

		void compileAndLinkShader();
		void requestShaderInfo();
		void setLocations();
		void setVertPositions();
		void setTextures();

		//void setWindowPositions();
		//void setWindowPositions(std::pair<int, int> anchorMW, std::pair<int, int> anchorAS, std::pair<int, int> anchorSG);
		void setWindowLayout();
		void setupComputeFBO();

		// The correcter way 
		void setRenderingOptions(bool showDepthFlag, bool showBigDepthFlag,  bool showInfraFlag, bool showColorFlag, bool showLightFlag, bool showPointFlag, bool showFlowFlag, bool showEdgesFlag);
		void setBuffersForRendering(float * depthArray, float * bigDepthArray, float * colorArray, float * infraArray, unsigned char * flowPtr);
		void setDepthImageRenderPosition();
		void setColorImageRenderPosition(float vertFov);
		void setInfraImageRenderPosition();
		void setFlowImageRenderPosition();
		void setPointCloudRenderPosition(float modelZ);
		void setLightModelRenderPosition();
		void setViewMatrix(float xRot, float yRot, float zRot, float xTran, float yTran, float zTran);
		void setProjectionMatrix();

		void setColorDepthMapping(int* colorDepthMap);
		void renderLiveVideoWindow();



		void renderFlow(unsigned char* flowPtr);
		void drawPoints();
		void drawLightModel();
		void setComputeWindowPosition();

		void setCameraParams(glm::vec4 camPams, glm::vec4 camPamsColor)
		{
			m_cameraParams = camPams;
			m_cameraParams_color = camPamsColor;
		}

		std::vector<std::pair<int, int>> getDepthPoints2D()
		{
			return m_depthPixelPoints2D;
		}
		void labelDepthPointsOnColorImage(float* depthArray, int* colorDepthMap);
		void setRegistrationMatrix(glm::mat4 reg);
		void resetRegistrationMatrix();
		void setExportPly(bool opt)
		{
			m_export_ply = opt;
		}
		//void setGraphPoints(int size, float valueX, float valueY, float valueZ);
		//void updateGraphPoints(float valueX, float valueY, float valueZ);

		void setRVec(cv::Mat rvec)
		{
			rotation_vector = rvec;
		}
		void setTVec(cv::Mat tvec)
		{
			translation_vector = tvec;
		}
		void setIrBrightness(float irL, float irH)
		{
			m_ir_low = irL;
			m_ir_high = irH;
		}

		void setFov(float fov)
		{
			m_vertFov = fov;
		}

		void setCheckerBoardPointsColor(std::vector<cv::Point2f> pointsColor);
		void setCheckerBoardPointsInfra(std::vector<cv::Point2f> pointsInfra);

		// compute shader time
		void filterDepth(bool useBigDepth = false);
		void computeDepthToVertex(bool useBigDepth = false);
		void computeVertexToNormal(bool useBigDepth = false);
		void computeEdges();
		void computeBlur(bool useBigDepth = false);
		void renderPointCloud(bool useBigDepth = false);

		void cleanUp();

private:

	GLSLProgram renderProg;
	GLSLProgram computeProg;
	GLSLProgram filterProg;
	GLSLProgram edgeProg;
	GLSLProgram currentDepthProg;
	GLSLProgram v2nProg;

	GLFWwindow * m_window;
	bool m_show_imgui;

	GLuint m_VAO, m_EBO;
	GLuint m_VBO_Color, m_VBO_Depth;
	std::vector<float> m_vertices;
	//GLfloat* m_vertices; 
	std::vector<unsigned int> m_indices;

	std::vector<float> m_color_vert;
	std::vector<float> m_depth_vert;

	GLuint m_VAO_BD;
	GLuint m_EBO_BD;
	GLuint m_buf_Pointcloud_big_depth;
	GLuint m_buf_Pointcloud_RGB_big_depth;
	GLuint m_buf_Pointcloud_RGB_big_depth_prev;
	GLuint m_buf_Pointcloud_RGB_big_depth_curr;
	GLuint m_buf_Pointcloud_big_depth_prev;
	GLuint m_buf_Pointcloud_big_depth_curr;
	GLuint m_buf_color_big_depth_map;
	GLuint m_VAO_Pointcloud;
	GLuint m_buf_Pointcloud;
	GLuint m_buf_color_depth_map;
	std::vector<float> m_verticesBigDepthPointcloud;

	//std::vector<float> m_verticesBigDepthPointcloudRGB;
	std::vector<unsigned int> m_indexBigDepthPointCloud;

	//std::vector<float> m_verticesBigDepthPointcloudRGB_curr;
	//std::vector<float> m_verticesBigDepthPointcloudRGB_prev;

	std::vector<float> m_verticesBigDepthPointcloud_prev;
	std::vector<float> m_verticesBigDepthPointcloud_curr;


	std::vector<float> m_colorBigDepthMapping;
	std::vector<float> m_verticesPointcloud;
	std::vector<float> m_colorDepthMapping;

	GLuint m_VAO_checkerboard;
	GLuint m_VBO_checkerboard;
	std::vector<float> m_infra_checkerboard_verts;
	std::vector<float> m_color_checkerboard_verts;


	GLuint m_programID;
	GLuint m_ProjectionID;
	GLuint m_MvpID;
	GLuint m_ModelID;
	GLuint m_ViewProjectionID;

	GLuint m_ambientID;
	GLuint m_lightID;

	GLuint m_irLowID;
	GLuint m_irHighID;

	GLuint m_DepthSubroutineID;
	GLuint m_BigDepthSubroutineID;
	GLuint m_InfraSubroutineID;
	GLuint m_ColorSubroutineID;
	GLuint m_EdgesSubroutineID;
	GLuint m_FlowSubroutineID;
	GLuint m_VertexSubroutineID;
	GLuint m_PointsSubroutineID;
	GLuint m_CalibrationSubroutineID;
	GLuint m_colorTypeSubroutineID;

	GLuint m_fboComputeHandle;
	GLuint m_vertex3DSubroutineID;
	GLuint m_depth2DSubroutineID;
	GLuint m_color2DSubroutineID;
	GLuint m_position2DSubroutineID;
	GLuint m_depthDataTypeSubroutineID;
	GLuint m_invkID;
	GLuint m_camPamsID;


	//textures
	GLuint m_textureDepth;
	GLuint m_textureFilteredDepth;
	GLuint m_textureBigDepth;
	GLuint m_textureFilteredBigDepth;
	GLuint m_textureInfra;
	GLuint m_textureColor;
	GLuint m_textureColorPrevious;
	GLuint m_textureFlow;
	GLuint m_textureDepthPrevious;
	GLuint m_textureEdges;

	GLuint m_textureVertex;
	GLuint m_textureNormal;
	GLuint m_textureBigVertex;
	GLuint m_textureBigVertex_prev;
	GLuint m_textureBigVertex_curr;
	GLuint m_textureBigNormal;

	int m_screen_height;
	int m_screen_width;
	int m_depth_height;
	int m_depth_width;
	int m_color_height;
	int m_color_width;
	int m_big_depth_height;
	int m_big_depth_width;

	float m_render_scale_height;
	float m_render_scale_width;

	std::pair<int, int> m_anchorMW;
	std::pair<int, int> m_anchorAS;
	std::pair<int, int> m_anchorSG;

	std::pair<int, int> m_gui_padding;

	glm::mat4 ColorView = glm::translate(glm::mat4(1.0f), glm::vec3(-0.f, -0.f, -0.0f));

	glm::vec4 m_cameraParams;
	glm::vec4 m_cameraParams_color;

	// k.x = fx, k.y = fy, k.z = cx, k.y = cy, skew = 1
	glm::mat4 getInverseCameraMatrix(const glm::vec4 & k) {
		glm::mat4 invK;
		invK[0] = glm::vec4(1.0f / k.x, 0, -k.z / k.x, 0);
		invK[1] = glm::vec4(0, 1.0f / k.y, -k.w / k.y, 0);
		invK[2] = glm::vec4(0, 0, 1, 0);
		invK[3] = glm::vec4(0, 0, 0, 1);
		//std::cout << invK[0][0] << " " << invK[0][1] << " " << invK[0][2] << " " << invK[0][3] << " " << std::endl;
		//std::cout << invK[1][0] << " " << invK[1][1] << " " << invK[1][2] << " " << invK[1][3] << " " << std::endl;
		//std::cout << invK[2][0] << " " << invK[2][1] << " " << invK[2][2] << " " << invK[2][3] << " " << std::endl;
		//std::cout << invK[3][0] << " " << invK[3][1] << " " << invK[3][2] << " " << invK[3][3] << " " << std::endl;

		return invK;
	}

	inline int divup(int a, int b) { return (a % b != 0) ? (a / b + 1) : (a / b); }

	void writePLYFloat(std::vector<float> PC, std::vector<float> NC, const char* FileName);




	bool m_export_ply = false;

	float m_mouse_pos_x;
	float m_mouse_pos_y;
	glm::mat4 m_registration_matrix = glm::mat4(1.0f);

	// this static wrapped clas was taken from BIC comment on https://stackoverflow.com/questions/7676971/pointing-to-a-function-that-is-a-class-member-glfw-setkeycallback
	void MousePositionCallback(GLFWwindow* window, double positionX, double positionY);
	void KeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

	class GLFWCallbackWrapper
	{
	public:
		GLFWCallbackWrapper() = delete;
		GLFWCallbackWrapper(const GLFWCallbackWrapper&) = delete;
		GLFWCallbackWrapper(GLFWCallbackWrapper&&) = delete;
		~GLFWCallbackWrapper() = delete;

		static void MousePositionCallback(GLFWwindow* window, double positionX, double positionY);
		static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
		static void KeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void SetApplication(kRender *application);
	private:
		static kRender* s_application;
	};

	std::vector<float> m_depthPointsFromBuffer;
	std::vector<std::pair<int, int>> m_depthPixelPoints2D;

	cv::Mat rotation_vector; // Rotation in axis-angle form
	cv::Mat translation_vector;


	float m_ir_low = 0.0f;
	float m_ir_high = 65536.0f;
	float m_vertFov = 35.0f;
	//std::vector<cv::Point2f> m_detected_points_color;
	//std::vector<cv::Point2f> m_detected_points_infra;

	glm::mat4 m_model_depth = glm::mat4(1.0);
	glm::mat4 m_model_color = glm::mat4(1.0);
	glm::mat4 m_model_infra = glm::mat4(1.0);
	glm::mat4 m_model_flow = glm::mat4(1.0f);
	glm::mat4 m_model_pointcloud = glm::mat4(1.0f);
	glm::mat4 m_model_lightmodel = glm::mat4(1.0f);
	glm::mat4 m_view = glm::mat4(1.0f);
	glm::mat4 m_projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 3000.0f); // some default matrix

	bool m_showDepthFlag = false;
	bool m_showInfraFlag = false;
	bool m_showColorFlag = false;
	bool m_showBigDepthFlag = false;
	bool m_showLightFlag = false;
	bool m_showPointFlag = false;
	bool m_showFlowFlag = false;
	bool m_showEdgesFlag = false;
};