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

		void renderLiveVideoWindow(float* depthArray);
		void renderColorWindow(float* colorArray);
		void copyToPreviousFrame();
		void renderPrevious();
		void renderPreviousColor();

		void renderFlow(unsigned char* flowPtr);
		void drawPoints();

		void setComputeWindowPosition();

		void setCameraParams(glm::vec4 camPams)
		{
			m_cameraParams = camPams;
		}


		//void setGraphPoints(int size, float valueX, float valueY, float valueZ);
		//void updateGraphPoints(float valueX, float valueY, float valueZ);

		void cleanUp();

private:

	GLSLProgram renderProg;
	GLSLProgram computeProg;
	GLSLProgram v2nProg;

	GLFWwindow * m_window;

	GLuint m_VBO, m_VAO, m_EBO;
	std::vector<float> m_vertices;
	//GLfloat* m_vertices; 
	std::vector<unsigned int> m_indices;



	GLuint m_programID;
	GLuint m_ProjectionID;
	GLuint m_MvpID;
	GLuint m_ModelID;

	GLuint m_ambientID;
	GLuint m_lightID;

	GLuint m_DepthSubroutineID;
	GLuint m_ColorSubroutineID;
	GLuint m_FlowSubroutineID;
	GLuint m_VertexSubroutineID;
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



	// GRAPH STUFF
	//std::list<float> m_graph_points_x;
	//std::list<float> m_graph_points_y;
	//std::list<float> m_graph_points_z;

	//std::list<float> m_graph_points_long_x;
	//std::list<float> m_graph_points_long_y;
	//std::list<float> m_graph_points_long_z;

	//std::vector<float> m_graph_vector_x;
	//std::vector<float> m_graph_vector_y;
	//std::vector<float> m_graph_vector_z;

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



};