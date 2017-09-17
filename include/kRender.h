#define GLUT_NO_LIB_PRAGMA
//##### OpenGL ######
#include <GL/glew.h>
#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

//#include "shader.hpp"

#include <iostream>
#include <fstream>

#include <vector>
#include <list>
#include <numeric>

#include "glutils.h"
#include "glslprogram.h"



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
			, m_VBO()
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

		void setColorDepthMapping(int* colorDepthMap);
		void renderLiveVideoWindow(float* depthArray);
		void renderColorWindow(float* colorArray);
		void copyToPreviousFrame();
		void renderPrevious();
		void renderPreviousColor();

		void renderFlow(unsigned char* flowPtr);
		void drawPoints();
		void drawLightModel();
		void setComputeWindowPosition();

		void setCameraParams(glm::vec4 camPams)
		{
			m_cameraParams = camPams;
		}

		std::vector<std::pair<int, int>> getDepthPoints2D()
		{
			return m_depthPixelPoints2D;
		}
		void labelDepthPointsOnColorImage(float* depthArray, int* colorDepthMap);

		//void setGraphPoints(int size, float valueX, float valueY, float valueZ);
		//void updateGraphPoints(float valueX, float valueY, float valueZ);

		void cleanUp();

private:

	GLSLProgram renderProg;
	GLSLProgram computeProg;
	GLSLProgram v2nProg;

	GLFWwindow * m_window;
	bool m_show_imgui;

	GLuint m_VBO, m_VAO, m_EBO;
	std::vector<float> m_vertices;
	//GLfloat* m_vertices; 
	std::vector<unsigned int> m_indices;

	GLuint m_VAO_Pointcloud;
	GLuint m_buf_Pointcloud;
	GLuint m_buf_color_depth_map;
	std::vector<float> m_verticesPointcloud;
	std::vector<float> m_colorDepthMapping;

	GLuint m_programID;
	GLuint m_ProjectionID;
	GLuint m_MvpID;
	GLuint m_ModelID;
	GLuint m_ViewProjectionID;

	GLuint m_ambientID;
	GLuint m_lightID;

	GLuint m_DepthSubroutineID;
	GLuint m_ColorSubroutineID;
	GLuint m_FlowSubroutineID;
	GLuint m_VertexSubroutineID;
	GLuint m_PointsSubroutineID;
	GLuint m_colorTypeSubroutineID;

	GLuint m_fboComputeHandle;
	GLuint m_vertex3DSubroutineID;
	GLuint m_depth2DSubroutineID;
	GLuint m_depthDataTypeSubroutineID;
	GLuint m_invkID;


	//textures
	GLuint m_textureDepth;
	GLuint m_textureInfra;
	GLuint m_textureColor;
	GLuint m_textureFlow;
	GLuint m_textureDepthPrevious;

	GLuint m_textureVertex;
	GLuint m_textureNormal;

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

	glm::mat4 ColorView = glm::translate(glm::mat4(1.0f), glm::vec3(-0.f, -0.f, -5.2f));

	glm::vec4 m_cameraParams;

	// k.x = fx, k.y = fy, k.z = cx, k.y = cy, skew = 1
	glm::mat4 getInverseCameraMatrix(const glm::vec4 & k) {
		glm::mat4 invK;
		invK[0] = glm::vec4(1.0f / k.x, 0, -k.z / k.x, 0);
		invK[1] = glm::vec4(0, 1.0f / k.y, -k.w / k.y, 0);
		invK[2] = glm::vec4(0, 0, 1, 0);
		invK[3] = glm::vec4(0, 0, 0, 1);
		return invK;
	}

	inline int divup(int a, int b) { return (a % b != 0) ? (a / b + 1) : (a / b); }

	void writePLYFloat(std::vector<float> PC, std::vector<float> NC, const char* FileName);






	float m_mouse_pos_x;
	float m_mouse_pos_y;

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





};