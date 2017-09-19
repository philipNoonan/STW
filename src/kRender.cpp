#include "kRender.h"

kRender::~kRender()
{
}

void kRender::GLFWCallbackWrapper::MousePositionCallback(GLFWwindow* window, double positionX, double positionY)
{
	s_application->MousePositionCallback(window, positionX, positionY);
}

void kRender::GLFWCallbackWrapper::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	s_application->MouseButtonCallback(window, button, action, mods);
}


void kRender::GLFWCallbackWrapper::KeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	s_application->KeyboardCallback(window, key, scancode, action, mods);
}

void kRender::GLFWCallbackWrapper::SetApplication(kRender* application)
{
	GLFWCallbackWrapper::s_application = application;
}

kRender* kRender::GLFWCallbackWrapper::s_application = nullptr;

void kRender::MousePositionCallback(GLFWwindow* window, double positionX, double positionY)
{
	//...
	//std::cout << "mouser" << std::endl;
	m_mouse_pos_x = positionX;
	m_mouse_pos_y = positionY;
}
void kRender::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		// m_depthPixelPoints2D.push_back(std::make_pair(m_mouse_pos_x, m_mouse_pos_y));
		// get correct current offset and scakle for the window
		int depth_pos_x = m_mouse_pos_x / m_render_scale_width;
		int depth_pos_y = m_mouse_pos_y / m_render_scale_height;

		if (depth_pos_x < m_depth_width && depth_pos_y < m_depth_height)
		{
			m_depthPixelPoints2D.push_back(std::make_pair(depth_pos_x, depth_pos_y));
			m_depthPointsFromBuffer.resize(m_depthPixelPoints2D.size() * 4); // for 4 floats per vertex (x,y,z, + padding)


		}
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		if (m_depthPixelPoints2D.size() > 0)
		{
			m_depthPixelPoints2D.pop_back();
			m_depthPointsFromBuffer.resize(m_depthPixelPoints2D.size() * 4); // for 4 floats per vertex (x,y,z, + padding)

		}
		// pop_back entry on vector
	}
	
	if (m_depthPixelPoints2D.size() > 0 && action == GLFW_PRESS)
	{
		std::cout << m_depthPixelPoints2D.size();
		for (auto i : m_depthPixelPoints2D)
		{
			std::cout << " x: " << i.first << " y: " << i.second << std::endl;
		}
	}
	else if (m_depthPixelPoints2D.size() == 0 && action == GLFW_PRESS)
	{
		std::cout << "no entries yet, left click points on depth image" << std::endl;
	}
	//std::cout << "mouse button pressed: " << button << " " << action << " x: " <<  m_mouse_pos_x << " y: " << m_mouse_pos_y << std::endl;

}

void kRender::KeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	//...
	//std::cout << "keyer" << std::endl;
	if (key == GLFW_KEY_H && action == GLFW_PRESS)
		m_show_imgui = !m_show_imgui;
	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
		m_registration_matrix[3][0] -= 5.0f;
	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
		m_registration_matrix[3][0] += 5.0f;
	if (key == GLFW_KEY_PAGE_UP && action == GLFW_PRESS)
		m_registration_matrix[3][1] -= 5.0f;
	if (key == GLFW_KEY_PAGE_DOWN && action == GLFW_PRESS)
		m_registration_matrix[3][1] += 5.0f;
	if (key == GLFW_KEY_UP && action == GLFW_PRESS)
		m_registration_matrix[3][2] -= 5.0f;
	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
		m_registration_matrix[3][2] += 5.0f;

	std::cout << "manual x " << m_registration_matrix[3][0] << " manual y " << m_registration_matrix[3][1] << " manual z " << m_registration_matrix[3][2] << std::endl;
}

void kRender::SetCallbackFunctions()
{
	GLFWCallbackWrapper::SetApplication(this);
	glfwSetCursorPosCallback(m_window, GLFWCallbackWrapper::MousePositionCallback);
	glfwSetKeyCallback(m_window, GLFWCallbackWrapper::KeyboardCallback);
	glfwSetMouseButtonCallback(m_window, GLFWCallbackWrapper::MouseButtonCallback);
}

GLFWwindow * kRender::loadGLFWWindow()
{

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_REFRESH_RATE, 30);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	glEnable(GL_DEPTH_TEST);

	m_window = glfwCreateWindow(m_screen_width, m_screen_height, "SDFFusion", nullptr, nullptr);

	if (m_window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		//return -1;
	}

	glfwMakeContextCurrent(m_window);
	glfwSwapInterval(1); // Enable vsync
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		//return -1;
	}


	return m_window;
}

void kRender::requestShaderInfo()
{
	renderProg.printActiveUniforms();
}

void kRender::getMouseClickPositionsDepth()
{
	// vector of 2d depth pos from 2d pixel location of depth or IR image
}

void kRender::setRegistrationMatrix(glm::mat4 reg)
{
	glm::mat4 regTran = glm::transpose(reg);
	glm::mat4 regInv = glm::inverse(regTran);
	m_registration_matrix = regInv;



}

void kRender::resetRegistrationMatrix()
{
	m_registration_matrix = glm::mat4(1);
	m_depthPixelPoints2D.resize(0);
	m_depthPointsFromBuffer.resize(0);
}


































void kRender::compileAndLinkShader()
{
	try {
		renderProg.compileShader("shaders/vertShader.vs");
		renderProg.compileShader("shaders/fragShader.fs");
		renderProg.link();

		computeProg.compileShader("shaders/compute3D.cs");
		computeProg.link();

		v2nProg.compileShader("shaders/vertexTonormal.cs");
		v2nProg.link();
	}
	catch (GLSLProgramException &e) {
		std::cerr << e.what() << std::endl;
		exit(EXIT_FAILURE);
	}
}

void kRender::setLocations()
{
	m_ProjectionID = glGetUniformLocation(renderProg.getHandle(), "projection");
	m_MvpID = glGetUniformLocation(renderProg.getHandle(), "MVP");
	m_ModelID = glGetUniformLocation(renderProg.getHandle(), "model");
	m_ViewProjectionID = glGetUniformLocation(renderProg.getHandle(), "ViewProjection");

	m_ambientID = glGetUniformLocation(renderProg.getHandle(), "ambient");
	m_lightID = glGetUniformLocation(renderProg.getHandle(), "light");

	m_irBrightnessID = glGetUniformLocation(renderProg.getHandle(), "irBrightness");

	m_DepthSubroutineID = glGetSubroutineIndex(renderProg.getHandle(), GL_FRAGMENT_SHADER, "fromDepth");
	m_InfraSubroutineID = glGetSubroutineIndex(renderProg.getHandle(), GL_FRAGMENT_SHADER, "fromInfra");
	m_ColorSubroutineID = glGetSubroutineIndex(renderProg.getHandle(), GL_FRAGMENT_SHADER, "fromColor");
	m_FlowSubroutineID = glGetSubroutineIndex(renderProg.getHandle(), GL_FRAGMENT_SHADER, "fromFlow");
	m_VertexSubroutineID = glGetSubroutineIndex(renderProg.getHandle(), GL_FRAGMENT_SHADER, "fromVertex");
	m_PointsSubroutineID = glGetSubroutineIndex(renderProg.getHandle(), GL_FRAGMENT_SHADER, "fromPoints");
	m_CalibrationSubroutineID = glGetSubroutineIndex(renderProg.getHandle(), GL_FRAGMENT_SHADER, "fromCalibration");
	m_colorTypeSubroutineID = glGetSubroutineUniformLocation(renderProg.getHandle(), GL_FRAGMENT_SHADER, "getColorSelection");

	m_vertex3DSubroutineID = glGetSubroutineIndex(renderProg.getHandle(), GL_VERTEX_SHADER, "fromVertex3D");
	m_depth2DSubroutineID = glGetSubroutineIndex(renderProg.getHandle(), GL_VERTEX_SHADER, "fromDepth2D");
	m_color2DSubroutineID = glGetSubroutineIndex(renderProg.getHandle(), GL_VERTEX_SHADER, "fromColor2D");


	m_position2DSubroutineID = glGetSubroutineIndex(renderProg.getHandle(), GL_VERTEX_SHADER, "fromPosition2D");
	m_depthDataTypeSubroutineID = glGetSubroutineUniformLocation(renderProg.getHandle(), GL_VERTEX_SHADER, "getPositionSelection");


	m_invkID = glGetUniformLocation(computeProg.getHandle(), "invK");


}

void kRender::setVertPositions()
{
	std::vector<float> vertices = {
		// Positions		// Texture coords
		1.0f, 1.0f, 0.0f, 1.0f, 1.0f, // top right
		1.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom right
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom left
		-1.0f, 1.0f, 0.0f, 0.0f, 1.0f  // Top left
	};

	std::vector<float> verticesColor = {
		// Positions			  // Texture coords
		1920.0f, 1080.0f,	0.0f, 1.0f, 1.0f, // top right
		1920.0f, 0,			0.0f, 1.0f, 0.0f, // bottom right
		0,		 0,			0.0f, 0.0f, 0.0f, // bottom left
		0,		 1080.0f,	0.0f, 0.0f, 1.0f  // Top left
	};
	m_color_vert = verticesColor;

	std::vector<float> verticesDepth = {
		// Positions			  // Texture coords
		512.0f,  424.0f,	0.0f, 1.0f, 1.0f, // top right
		512.0f,	 0,			0.0f, 1.0f, 0.0f, // bottom right
		0,		 0,			0.0f, 0.0f, 0.0f, // bottom left
		0,		 424.0f,	0.0f, 0.0f, 1.0f  // Top left
	};
	m_depth_vert = verticesDepth;

	//std::vector<float> vertices = {
	//	// Positions		// Texture coords
	//	1920.0f, 1080.0f, 0.0f, 1.0f, 1.0f, // top right
	//	1920.0f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom right
	//	0.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom left
	//	0.0f, 1080.0f, 0.0f, 0.0f, 1.0f  // Top left
	//};

	m_vertices = vertices;

	std::vector<unsigned int>  indices = {  // Note that we start from 0!
		0, 1, 3, // First Triangle
		1, 2, 3  // Second Triangle
	};

	m_indices = indices;

	std::vector<float>  verticiesPointcloud;
	verticiesPointcloud.resize(m_depth_height * m_depth_width * 4); // x,y,z, values for the projected 3D pointcloud
	m_verticesPointcloud = verticiesPointcloud;

	std::vector<float>  colorDepthMapping;
	colorDepthMapping.resize(m_depth_height * m_depth_width * 2); // x, y pixel location (normalised to 0 - 1 coords (i.e. divided by 1920 and 1080)) for grabbing the color texture 
	m_colorDepthMapping = colorDepthMapping;

	std::vector<float> checkerBoardPoints;
	checkerBoardPoints.resize(5 * 7 * 2);
	m_infra_checkerboard_verts = checkerBoardPoints;
	m_color_checkerboard_verts = checkerBoardPoints;

}




void kRender::setTextures()
{
	glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO_Color);
	glGenBuffers(1, &m_VBO_Depth);
	glGenBuffers(1, &m_EBO);

	glBindVertexArray(m_VAO);
	//glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	//glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(float), &m_vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO_Color);
	glBufferData(GL_ARRAY_BUFFER, m_color_vert.size() * sizeof(float), &m_color_vert[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), &m_indices[0], GL_STATIC_DRAW);

	// Position attribute for Color
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// TexCoord attribute for Color
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (GLvoid*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// now go for depth
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO_Depth);
	glBufferData(GL_ARRAY_BUFFER, m_depth_vert.size() * sizeof(float), &m_depth_vert[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), &m_indices[0], GL_STATIC_DRAW);
	
	// Position attribute for Depth
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (GLvoid*)0);
	glEnableVertexAttribArray(2);

	// TexCoord attribute for Depth
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (GLvoid*)(3 * sizeof(float)));
	glEnableVertexAttribArray(3);

	// setup and bind VBO to VAO 
	glGenBuffers(1, &m_VBO_checkerboard);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO_checkerboard);
	glBufferData(GL_ARRAY_BUFFER, m_infra_checkerboard_verts.size() * sizeof(float), &m_infra_checkerboard_verts[0], GL_DYNAMIC_DRAW); //we will overwrite this on the fly anyway with both color and infra as needed, so no need to bind the color vector

	// Position attribute
	int bindLoc_CB = 5;
	glVertexAttribPointer(bindLoc_CB, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(bindLoc_CB);


	glBindVertexArray(0);

	// set up VOA for pointcloud
	glGenVertexArrays(1, &m_VAO_Pointcloud);
	glBindVertexArray(m_VAO_Pointcloud);

	// set up for pointcloud SSBO
	glGenBuffers(1, &m_buf_Pointcloud);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_buf_Pointcloud);
	glBufferData(GL_SHADER_STORAGE_BUFFER, m_verticesPointcloud.size() * sizeof(float), &m_verticesPointcloud[0], GL_DYNAMIC_DRAW);

	glGenBuffers(1, &m_buf_color_depth_map);
	glBindBuffer(GL_ARRAY_BUFFER, m_buf_color_depth_map);
	glBufferData(GL_ARRAY_BUFFER, m_colorDepthMapping.size() * sizeof(float), &m_colorDepthMapping[0], GL_DYNAMIC_DRAW);

	// bind buffers to VAO
	int bindingLocation_buf_pointcloud = 4;
	glBindBuffer(GL_ARRAY_BUFFER, m_buf_Pointcloud);
	glVertexAttribPointer(bindingLocation_buf_pointcloud, 4, GL_FLOAT, GL_FALSE, 0, 0); // 4  floats per vertex, x,y,z and 1 for padding? this is annoying...
	glEnableVertexAttribArray(bindingLocation_buf_pointcloud);

	int bindingLocation_buf_color_depth_map = 1;
	glBindBuffer(GL_ARRAY_BUFFER, m_buf_color_depth_map);
	glVertexAttribPointer(bindingLocation_buf_color_depth_map, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(bindingLocation_buf_color_depth_map);

	glBindVertexArray(0);



// Texture Depth Generate
glGenTextures(1, &m_textureDepth);
glActiveTexture(GL_TEXTURE0);
glBindTexture(GL_TEXTURE_2D, m_textureDepth); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// Set texture wrapping to GL_REPEAT
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, m_depth_width, m_depth_height, 0, GL_RED, GL_FLOAT, NULL);
glBindTexture(GL_TEXTURE_2D, 0);

// Texture Infra Generate
glGenTextures(1, &m_textureInfra);
glActiveTexture(GL_TEXTURE1);
glBindTexture(GL_TEXTURE_2D, m_textureInfra); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// Set texture wrapping to GL_REPEAT
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, m_depth_width, m_depth_height, 0, GL_RED, GL_FLOAT, NULL);
glBindTexture(GL_TEXTURE_2D, 0);

// Texture Color Generate
glGenTextures(1, &m_textureColor);
glActiveTexture(GL_TEXTURE2);
glBindTexture(GL_TEXTURE_2D, m_textureColor); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// Set texture wrapping to GL_REPEAT
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_color_width, m_color_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
glBindTexture(GL_TEXTURE_2D, 0);

// Texture Flow Generate
glGenTextures(1, &m_textureFlow);
glActiveTexture(GL_TEXTURE3);
glBindTexture(GL_TEXTURE_2D, m_textureFlow); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// Set texture wrapping to GL_REPEAT
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, m_depth_width, m_depth_height, 0, GL_RG, GL_FLOAT, NULL);
glBindTexture(GL_TEXTURE_2D, 0);

// Texture Vertex3D
glGenTextures(1, &m_textureVertex);
glBindTexture(GL_TEXTURE_2D, m_textureVertex); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object
glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, m_depth_width, m_depth_height);
glBindImageTexture(4, m_textureVertex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

// Texture Normal3D
glGenTextures(1, &m_textureNormal);
glBindTexture(GL_TEXTURE_2D, m_textureNormal); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object
glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, m_depth_width, m_depth_height);
glBindImageTexture(5, m_textureNormal, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);


}

void kRender::setupComputeFBO()
{
	//glGenFramebuffers(1, &m_fboComputeHandle);
	//glBindFramebuffer(GL_FRAMEBUFFER, m_fboComputeHandle);



}


void kRender::setWindowLayout()
{
	m_anchorMW = std::make_pair<int, int>(50, 50);


}

void kRender::setComputeWindowPosition()
{
	glViewport(m_anchorMW.first + m_depth_width * m_render_scale_width, m_anchorMW.second, m_depth_width * m_render_scale_width, m_depth_height * m_render_scale_height);
}

void kRender::setColorDepthMapping(int* colorDepthMap)
{
	// 2d array index is given by
	// p.x = idx / size.x
	// p.y = idx % size.x

	//for m_colorDepthMapping[j + 1] = y color image axis, 1 at top
	// m_colorDepthMapping[j] = x axis, 0 on the left, 

	// MAP ME¬¬¬
	int j = 0;
	for (int i = 0; i < (m_depth_width * m_depth_height); i++, j+=2)
	{
		int yCoord = colorDepthMap[i] / m_color_width;
		int xCoord = colorDepthMap[i] % m_color_width;
		m_colorDepthMapping[j] = ((float)xCoord) / (float)m_color_width;
		m_colorDepthMapping[j + 1] = (1080.0f - (float)yCoord) / (float)m_color_height;

		//if (!(i % 1000))
		//{
		//	std::cout << " X: " << xCoord << ", Y: " << yCoord;
		//}

		//m_colorDepthMapping[j] = 0.5f;
		//m_colorDepthMapping[j + 1] = 1.0f;


		//if (m_colorDepthMapping[j] <= 0 || m_colorDepthMapping[j + 1] <= 0)
		//{
		//	std::cout << m_colorDepthMapping[j] << ", " << m_colorDepthMapping[j + 1] << " ";
		//}

	}



	glBindBuffer(GL_ARRAY_BUFFER, m_buf_color_depth_map);
	glBufferSubData(GL_ARRAY_BUFFER, 0, m_colorDepthMapping.size() * sizeof(float), m_colorDepthMapping.data());
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//// Other way to copy data to buffer, taken from https://learnopengl.com/#!Advanced-OpenGL/Advanced-Data
	//glBindBuffer(GL_ARRAY_BUFFER, m_buf_color_depth_map);
	//void *ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	//memcpy_s(ptr, m_colorDepthMapping.size() * sizeof(float), m_colorDepthMapping.data(), m_colorDepthMapping.size() * sizeof(float));
	//glUnmapBuffer(GL_ARRAY_BUFFER);

}

void kRender::labelDepthPointsOnColorImage(float* depthArray, int* colorDepthMap)
{

}

void kRender::setRenderingOptions(bool showDepthFlag, bool showInfraFlag, bool showColorFlag, bool showLightFlag, bool showPointFlag)
{
	m_showDepthFlag = showDepthFlag;
	m_showInfraFlag = showInfraFlag;
	m_showColorFlag = showColorFlag;
	m_showLightFlag = showLightFlag;
	m_showPointFlag = showPointFlag;
}


// set up the data buffers for rendering
void kRender::setBuffersForRendering(float * depthArray, float * infraArray, float * colorArray)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_textureDepth);
	if (depthArray != NULL)
	{
		glTexSubImage2D(GL_TEXTURE_2D, /*level*/0, /*xoffset*/0, /*yoffset*/0, m_depth_width, m_depth_height, GL_RED, GL_FLOAT, depthArray);
	}

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_textureInfra);
	if (infraArray != NULL)
	{
		glTexSubImage2D(GL_TEXTURE_2D, /*level*/0, /*xoffset*/0, /*yoffset*/0, m_depth_width, m_depth_height, GL_RED, GL_FLOAT, infraArray);
	}

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_textureColor);
	if (colorArray != NULL)
	{
		glTexSubImage2D(GL_TEXTURE_2D, /*level*/0, /*xoffset*/0, /*yoffset*/0, m_color_width, m_color_height, GL_BGRA, GL_UNSIGNED_BYTE, colorArray);
	}





}

void kRender::setDepthImageRenderPosition()
{
	int w, h;
	glfwGetFramebufferSize(m_window, &w, &h);
	float zDist = 1500.0f;
	float halfHeightAtDist = zDist * tan(22.5f * M_PI / 180.0f);
	float halfWidthAtDistance = halfHeightAtDist * (float)w / (float)h; // notsure why this ratio is used here...
	m_model_depth = glm::translate(glm::mat4(1.0f), glm::vec3(-halfWidthAtDistance, -halfHeightAtDist, -zDist));

}

void kRender::setColorImageRenderPosition()
{
	int w, h;
	glfwGetFramebufferSize(m_window, &w, &h);
	float zDist = 4000.0f;
	float halfHeightAtDist = zDist * tan(22.5f * M_PI / 180.0f);
	float halfWidthAtDistance = halfHeightAtDist * (float)w / (float)h; // notsure why this ratio is used here...
	m_model_color = glm::translate(glm::mat4(1.0f), glm::vec3(-m_color_width / 2.0f, -halfHeightAtDist, -zDist));

	//m_model_color = glm::translate(glm::mat4(1.0f), glm::vec3(-m_color_width / 2.0f, 0.0f, -2000.0f));
}

void kRender::setInfraImageRenderPosition()
{
	int w, h;
	glfwGetFramebufferSize(m_window, &w, &h);
	float zDist = 1500.0f;
	float halfHeightAtDist = zDist * tan(22.5f * M_PI / 180.0f);
	float halfWidthAtDistance = halfHeightAtDist * (float)w / (float)h; // notsure why this ratio is used here...
	m_model_infra = glm::translate(glm::mat4(1.0f), glm::vec3(-halfWidthAtDistance, halfHeightAtDist - m_depth_height, -zDist));

}

void kRender::setPointCloudRenderPosition(float modelZ)
{
	int w, h;
	glfwGetFramebufferSize(m_window, &w, &h);
	float zDist = zDist + modelZ;
	float halfHeightAtDist = zDist * tan(22.5f * M_PI / 180.0f);
	float halfWidthAtDistance = halfHeightAtDist * (float)w / (float)h; // notsure why this ratio is used here...
	m_model_pointcloud = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, zDist));

	glm::mat4 flipYZ = glm::mat4(1);
	//flipYZ[0][0] = -1.0f;
	flipYZ[1][1] = -1.0f;
	flipYZ[2][2] = -1.0f;

	m_model_pointcloud = flipYZ * m_model_pointcloud;

}

void kRender::setLightModelRenderPosition()
{
	int w, h;
	glfwGetFramebufferSize(m_window, &w, &h);
	float zDist = 1500.0f;
	float halfHeightAtDist = zDist * tan(22.5f * M_PI / 180.0f);
	float halfWidthAtDistance = halfHeightAtDist * (float)w / (float)h; // notsure why this ratio is used here...
	m_model_lightmodel = glm::translate(glm::mat4(1.0f), glm::vec3(halfWidthAtDistance - m_depth_width, -halfHeightAtDist, -zDist));

}


void kRender::setViewMatrix(float xRot, float yRot, float zRot, float xTran, float yTran, float zTran)
{
	glm::mat4 t0, t1, r0;
	m_view = glm::mat4(1.0f);

	t0 = glm::translate(glm::mat4(1.0), glm::vec3(xTran, yTran, zTran));
	t1 = glm::translate(glm::mat4(1.0), glm::vec3(-xTran, -yTran, -zTran));

	r0 = glm::eulerAngleXYZ(glm::radians(xRot), glm::radians(yRot), glm::radians(zRot));
	

	m_view = t1 * r0 * t0;
	//m_view = glm::translate(m_view, glm::vec3(0.0f, 0.0f, 0.0f));

}

void kRender::setProjectionMatrix()
{
	int w, h;
	glfwGetFramebufferSize(m_window, &w, &h);
	m_projection = glm::perspective(glm::radians(45.0f), (float)w / (float)h, 1.0f, 10000.0f); // scaling the texture to the current window size seems to work
	glViewport(0, 0, w, h);


}


void kRender::renderLiveVideoWindow()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	renderProg.use();
	glBindVertexArray(m_VAO);
	glm::mat4 MVP;

	// FOR DEPTH
	if (m_showDepthFlag)
	{
		MVP = m_projection * m_view * m_model_depth;
		glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &m_depth2DSubroutineID);
		glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &m_DepthSubroutineID);
		glUniformMatrix4fv(m_ProjectionID, 1, GL_FALSE, glm::value_ptr(m_projection));
		glUniformMatrix4fv(m_MvpID, 1, GL_FALSE, glm::value_ptr(MVP));
		// draw depth tile
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}




	// FOR INFRA
	if (m_showInfraFlag)
	{
		MVP = m_projection * m_view * m_model_infra;
		glUniform1f(m_irBrightnessID, m_ir_brightness);
		glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &m_depth2DSubroutineID); // use the depth vertex subroutine
		glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &m_InfraSubroutineID); // use the infra fragment subroutine
		glUniformMatrix4fv(m_ProjectionID, 1, GL_FALSE, glm::value_ptr(m_projection));
		glUniformMatrix4fv(m_MvpID, 1, GL_FALSE, glm::value_ptr(MVP));
		//draw infra tile
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}


	//FOR COLOR
	if (m_showColorFlag)
	{
		MVP = m_projection * m_view * m_model_color;
		glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &m_color2DSubroutineID);
		glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &m_ColorSubroutineID);
		glUniformMatrix4fv(m_ProjectionID, 1, GL_FALSE, glm::value_ptr(m_projection));
		glUniformMatrix4fv(m_MvpID, 1, GL_FALSE, glm::value_ptr(MVP));
		//draw color tile
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}


	// FOR CALIBRATION POINTS
	// INFRA
	if (m_showInfraFlag)
	{
		if (m_infra_checkerboard_verts.size() > 0)
		{
			glm::mat4 model_calib_points = glm::translate(m_model_infra, glm::vec3(0.0f, 0.0f, 1.0f)); // small z shift so that you can see the pixels in front of the infrared tile
			MVP = m_projection * m_view * model_calib_points;
			glUniformMatrix4fv(m_MvpID, 1, GL_FALSE, glm::value_ptr(MVP));
			glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &m_position2DSubroutineID);
			glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &m_CalibrationSubroutineID);
			glBindBuffer(GL_ARRAY_BUFFER, m_VBO_checkerboard);
			glBufferSubData(GL_ARRAY_BUFFER, 0, m_infra_checkerboard_verts.size() * sizeof(float), m_infra_checkerboard_verts.data());
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glPointSize(5.0f);

			glDrawArrays(GL_POINTS, 0, 5 * 7 * 2);
		}
	}

	// COLOR
	if (m_showColorFlag)
	{
		if (m_color_checkerboard_verts.size() > 0)
		{
			glm::mat4 model_calib_points = glm::translate(m_model_color, glm::vec3(0.0f, 0.0f, 10.0f)); // small z shift so that you can see the pixels in front of the infrared tile
			MVP = m_projection * m_view * model_calib_points;
			glUniformMatrix4fv(m_MvpID, 1, GL_FALSE, glm::value_ptr(MVP));
			glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &m_position2DSubroutineID);
			glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &m_CalibrationSubroutineID);
			glBindBuffer(GL_ARRAY_BUFFER, m_VBO_checkerboard);
			glBufferSubData(GL_ARRAY_BUFFER, 0, m_color_checkerboard_verts.size() * sizeof(float), m_color_checkerboard_verts.data());
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glPointSize(5.0f);

			glDrawArrays(GL_POINTS, 0, 5 * 7 * 2);
		}
	}

	// FOR LIGHTMODEL
	if (m_showLightFlag)
	{
		MVP = m_projection * m_view * m_model_lightmodel;
		glm::vec3 light = glm::vec3(1.0f, 1.0f, -1.0f);
		glm::vec3 ambient = glm::vec3(0.1f, 0.1f, 0.1f);
		// Bind Textures using texture units
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, m_textureVertex);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, m_textureNormal);

		glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &m_depth2DSubroutineID);
		glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &m_VertexSubroutineID);

		glUniform3fv(m_lightID, 1, glm::value_ptr(light));
		glUniform3fv(m_ambientID, 1, glm::value_ptr(ambient));
		glUniformMatrix4fv(m_ProjectionID, 1, GL_FALSE, glm::value_ptr(m_projection));
		glUniformMatrix4fv(m_MvpID, 1, GL_FALSE, glm::value_ptr(MVP));


		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}


	glBindVertexArray(0);
}



void kRender::renderFlow(unsigned char* flowPtr)
{
	//glm::mat4 projection = glm::ortho(0.0f, 512.0f, 0.0f, 424.0f);
	//int w, h;
	//glfwGetFramebufferSize(m_window, &w, &h);
	//glViewport(m_anchorMW.first, m_anchorMW.second - m_depth_height, m_depth_width * m_render_scale_width, m_depth_height * m_render_scale_height);

	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//renderProg.use();
	//glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &m_depth2DSubroutineID);

	//glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &m_FlowSubroutineID);

	////// Bind Textures using texture units
	//glActiveTexture(GL_TEXTURE3);
	//glBindTexture(GL_TEXTURE_2D, m_textureFlow);
	////glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1920, 1080, 0, GL_BGRA, GL_UNSIGNED_BYTE, colorArray);
	//glTexSubImage2D(GL_TEXTURE_2D, /*level*/0, /*xoffset*/0, /*yoffset*/0, m_depth_width, m_depth_height, GL_RG, GL_FLOAT, flowPtr);
	////glUniform1i(glGetUniformLocation(m_programID, "_currentTexture"), 0);
	//glUniformMatrix4fv(m_ProjectionID, 1, GL_FALSE, glm::value_ptr(projection));

	//glBindVertexArray(m_VAO);
	//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	//glBindVertexArray(0);



}


void kRender::computeDepthToVertex()
{
	// get inverse camera matrix
	glm::mat4 invK = getInverseCameraMatrix(m_cameraParams);

	// bind image textures 
	glBindImageTexture(1, m_textureDepth, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
	glBindImageTexture(2, m_textureVertex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

	computeProg.use();
	//computeProg.setUniform("invK", invK);
	glUniformMatrix4fv(m_invkID, 1, GL_FALSE, glm::value_ptr(invK));

	int xWidth = divup(m_depth_width, 32);
	int yWidth = divup(m_depth_height, 32);

	glDispatchCompute(xWidth, yWidth, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void kRender::computeVertexToNormal()
{
	glBindImageTexture(0, m_textureVertex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	glBindImageTexture(1, m_textureNormal, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	v2nProg.use();

	int xWidth = divup(m_depth_width, 32);
	int yWidth = divup(m_depth_height, 32);
	glDispatchCompute(xWidth, yWidth, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void kRender::renderPointCloud()
{
	if (m_showPointFlag)
	{
		glm::mat4 MVP = m_projection * m_view * m_model_pointcloud;

		glEnable(GL_POINT_SPRITE);
		glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);








		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// bind the color texture for color sampling
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, m_textureColor);

		/*glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, m_textureNormal);*/


		renderProg.use();

		// set subroutines
		glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &m_vertex3DSubroutineID); // "fromVertex3D"
		glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &m_PointsSubroutineID); // "fromPoints"

																			   //set uniforms
		//glUniform3fv(m_lightID, 1, glm::value_ptr(light));
		//glUniform3fv(m_ambientID, 1, glm::value_ptr(ambient));
		glUniformMatrix4fv(m_ProjectionID, 1, GL_FALSE, glm::value_ptr(m_projection));
		//glUniformMatrix4fv(m_ViewProjectionID, 1, GL_FALSE, glm::value_ptr(VP));
		glUniformMatrix4fv(m_MvpID, 1, GL_FALSE, glm::value_ptr(MVP));

		// draw points
		glPointSize(2.0f);
		glBindVertexArray(m_VAO_Pointcloud);
		glDrawArrays(GL_POINTS, 0, 512 * 424);
		glBindVertexArray(0);
	}
}

void kRender::drawPoints()
{




	////// This works for grabbing the buffer from the compute3D shader and copying to host memory
	std::vector<float> PC;
	PC.resize(512 * 424 * 4);
	std::vector<float> NC;
	NC.resize(512 * 424 * 4);
	glBindBuffer(GL_ARRAY_BUFFER, m_buf_Pointcloud);
	void *ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
	memcpy_s(PC.data(), PC.size() * sizeof(float), ptr, PC.size() * sizeof(float));
	//// int oneDindex = (row * length_of_row) + column; // Indexes
	glUnmapBuffer(GL_ARRAY_BUFFER);
	
	//std::vector<cv::Point3f> dPoints;


	int ind = 0;
	int numBadPixel = 0;
	if (m_depthPixelPoints2D.size() > 0)
	{
		for (auto i : m_depthPixelPoints2D)
		{
			int index1D = ((m_depth_width * i.second) + i.first);
			memcpy_s(&m_depthPointsFromBuffer[ind], 4 * sizeof(float), &PC[index1D * 4], 4 * sizeof(float));
			m_depthPointsFromBuffer[ind + 3] = index1D;
			if (m_depthPointsFromBuffer[ind] == 0)
			{
				//invalid depth point, pop back one of the mouse clicks
				numBadPixel++;
				std::cout << "invalid depth point, pop back one of the mouse clicks " << std::endl;
			}
			else
			{
				//dPoints.push_back(cv::Point3f(PC[index1D * 4], PC[(index1D * 4) + 1],PC[(index1D * 4) + 2]));

				ind += 4;
			}
		}

		for (int j = 0; j < numBadPixel; j++)
		{
			m_depthPixelPoints2D.pop_back();
		}
	}
	//memcpy_s(&m_depthPointsFromBuffer[0], 4 * sizeof(float), &PC[((424 * 100) + 120 ) * 4], 4 * sizeof(float));
	//memcpy_s(&m_depthPointsFromBuffer[4], 4 * sizeof(float), &PC[((424 * 120) + 200 ) * 4], 4 * sizeof(float));
	//memcpy_s(&m_depthPointsFromBuffer[8], 4 * sizeof(float), &PC[((424 * 180 ) + 280) * 4], 4 * sizeof(float));
	//memcpy_s(&m_depthPointsFromBuffer[12], 4 * sizeof(float), &PC[((424 * 140 ) + 340) * 4], 4 * sizeof(float));



	if (m_export_ply)
	{
		writePLYFloat(PC, NC, "./outPC.ply");
		setExportPly(false);
	}

	/// This was to see if the rvec and tvec from solvepnp appear to get the same color coords as when checking from the depth to color mapping table
	//if (m_depthPixelPoints2D.size() > 4)
	//{
	//	cv::Mat imagePointsMat;
	//	cv::Mat colorCamPams = cv::Mat::eye(3, 3, CV_32F);
	//	cv::Mat colorCamDist = cv::Mat(4, 1, CV_32F);
	//	colorCamPams.at<float>(0, 0) = 992.2276f;
	//	colorCamPams.at<float>(1, 1) = 990.6481f;
	//	colorCamPams.at<float>(0, 2) = (1920.0f - 940.4056f);
	//	colorCamPams.at<float>(1, 2) = (1080.0f - 546.1401f);

	//	colorCamDist.at<float>(0, 0) = 0.002308769f;
	//	colorCamDist.at<float>(1, 0) = -0.03766959f;
	//	colorCamDist.at<float>(2, 0) = -0.0063f;
	//	colorCamDist.at<float>(3, 0) = 0.0036f;

	//	cv::projectPoints(dPoints, rotation_vector, translation_vector, colorCamPams, colorCamDist, imagePointsMat);

	//	std::cout << imagePointsMat << std::endl;
	//}



	//



	//// Camera matrix
	//glm::mat4 View = glm::lookAt(
	//	glm::vec3(0, 0, 100), // Camera is at (4,3,3), in World Space
	//	glm::vec3(0, 0, 0), // and looks at the origin
	//	glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
	//);


	glm::mat4 View0 = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	glm::mat4 View =  m_registration_matrix * View0;

	glm::mat4 model = glm::mat4(1.0f);
	// flip axis y and z


	//View = View * flipYZ;
	//model = glm::rotate(model, 180.0f, glm::vec3(0, 1, 0));

	glm::mat4 Projection = glm::perspective(glm::radians(75.0f), 1920.0f / 1080.0f, -1.0f, 100.0f); // 1920.0f/1080.0f    512.0f / 424.0f //GLM USES RADIANSSO CONVERT!!!
	//glm::mat4 Projection = glm::ortho(-800.0f, 800.0f, -800.0f, 800.0f, -1.0f, 2500.0f);
	glm::mat4 MV = View * model;
	glm::mat4 MVP = Projection * View * model;
	glm::mat4 VP = Projection * View;
	int w, h;
	glfwGetFramebufferSize(m_window, &w, &h);










	// convert depth points to vertex 3D





}



void kRender::drawLightModel()
{
	glm::vec3 light = glm::vec3(1.0f, 1.0f, -1.0f);
	glm::vec3 ambient = glm::vec3(0.1f, 0.1f, 0.1f);

	glm::mat4 View = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.0f));
	glm::mat4 model = glm::mat4(1.0f);
	//model = glm::rotate(model, -1.0f, glm::vec3(0, 1, 0));


	//glm::mat4 Projection = glm::perspective(glm::radians(60.0f), 1.0f, 0.1f, 4500.0f);
	glm::mat4 Projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -100.0f, 100.0f);
	glm::mat4 MVP = Projection * View * model;
	int w, h;
	glfwGetFramebufferSize(m_window, &w, &h);
	glViewport(0, h - m_depth_height * m_render_scale_height, m_depth_width * m_render_scale_width, m_depth_height * m_render_scale_height);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	renderProg.use();

	// Bind Textures using texture units
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, m_textureVertex);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, m_textureNormal);

	glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &m_depth2DSubroutineID);
	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &m_VertexSubroutineID);

	glUniform3fv(m_lightID, 1, glm::value_ptr(light));
	glUniform3fv(m_ambientID, 1, glm::value_ptr(ambient));

	glUniformMatrix4fv(m_ProjectionID, 1, GL_FALSE, glm::value_ptr(m_projection));

	glUniformMatrix4fv(m_MvpID, 1, GL_FALSE, glm::value_ptr(MVP));

	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);

}




//
//void kRender::drawTriangle()
//{
//	glBindVertexArray(m_VAO);
//
//
//	static const GLfloat g_vertex_buffer_data[] = {
//		-1.0f, -1.0f, 0.0f,
//		1.0f, -1.0f, 0.0f,
//		0.0f,  1.0f, 0.0f,
//	};
//
//	// The following commands will talk about our 'vertexbuffer' buffer
//	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
//	// Give our vertices to OpenGL.
//	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
//
//	// 1rst attribute buffer : vertices
//	glEnableVertexAttribArray(0);
//	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
//	glVertexAttribPointer(
//		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
//		3,                  // size
//		GL_FLOAT,           // type
//		GL_FALSE,           // normalized?
//		0,                  // stride
//		(void*)0            // array buffer offset
//	);
//	// Draw the triangle !
//	glDrawArrays(GL_TRIANGLES, 0, 3); // Starting from vertex 0; 3 vertices total -> 1 triangle
//	glDisableVertexAttribArray(0);
//
//
//}


//void kRender::setGraphPoints(int size, float valueX, float valueY, float valueZ)
//{
//	m_graph_points_x.clear();
//	m_graph_points_y.clear();
//	m_graph_points_z.clear();
//
//	std::list<float> graphPointsX(size, valueX);
//	std::list<float> graphPointsY(size, valueY);
//	std::list<float> graphPointsZ(size, valueZ);
//
//	m_graph_points_x = graphPointsX;
//	m_graph_points_y = graphPointsY;
//	m_graph_points_z = graphPointsZ;
//
//
//	m_graph_points_long_x.clear();
//	m_graph_points_long_y.clear();
//	m_graph_points_long_z.clear();
//
//	m_graph_points_long_x = graphPointsX;
//	m_graph_points_long_y = graphPointsY;
//	m_graph_points_long_z = graphPointsZ;
//
//}

//void kRender::updateGraphPoints(float valueX, float valueY, float valueZ)
//{
//	static int frameCounter = 0;
//	m_graph_points_x.push_front(valueX);
//	m_graph_points_x.pop_back();
//
//	m_graph_points_y.push_front(valueY);
//	m_graph_points_y.pop_back();
//
//	m_graph_points_z.push_front(valueZ);
//	m_graph_points_z.pop_back();
//
//	if (frameCounter == 30)
//	{
//
//		auto it = m_graph_points_x.begin();
//		std::advance(it, 30);
//		float sumX = std::accumulate(m_graph_points_x.begin(), it, 0.0f);
//
//		it = m_graph_points_y.begin();
//		std::advance(it, 30);
//		float sumY = std::accumulate(m_graph_points_y.begin(), it, 0.0f);
//
//		it = m_graph_points_z.begin();
//		std::advance(it, 30);
//		float sumZ = std::accumulate(m_graph_points_z.begin(), it, 0.0f);
//
//
//		m_graph_points_long_x.push_front(sumX / 30.0f);
//		m_graph_points_long_x.pop_back();
//
//		m_graph_points_long_y.push_front(sumY / 30.0f);
//		m_graph_points_long_y.pop_back();
//
//		m_graph_points_long_z.push_front(sumZ / 30.0f);
//		m_graph_points_long_z.pop_back();
//	}
//
//	// factor this out
//	std::vector<float> graphX(m_graph_points_x.begin(), m_graph_points_x.end());
//	std::vector<float> graphY(m_graph_points_y.begin(), m_graph_points_y.end());
//	std::vector<float> graphZ(m_graph_points_z.begin(), m_graph_points_z.end());
//
//	m_graph_vector_x = graphX;
//	m_graph_vector_y = graphY;
//	m_graph_vector_z = graphZ;
//
//
//
//}








void kRender::cleanUp()
{
	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_VBO_Color);
	glDeleteBuffers(1, &m_VBO_Depth);

	glDeleteBuffers(1, &m_EBO);
	glfwTerminate();


}





void kRender::setCheckerBoardPointsColor(std::vector<cv::Point2f> pointsColor)
{
	if (pointsColor.empty())
	{
		m_color_checkerboard_verts.resize(0);
	}
	else
	{
		m_color_checkerboard_verts.resize(7 * 5 * 2);
		int j = 0;
		for (int i = 0; i < pointsColor.size(); i++, j += 2)
		{
			m_color_checkerboard_verts[j] = pointsColor[i].x; // here we scale down and shift to -1.0 -> 1.0
			m_color_checkerboard_verts[j + 1] = (m_color_height - pointsColor[i].y); // y is flipped here.... does this matter? or is it just so that openGL renders the point correctly, since the texture read is 1 - texture.y
		}

	}
}

void kRender::setCheckerBoardPointsInfra(std::vector<cv::Point2f> pointsInfra)
{
	if (pointsInfra.empty())
	{
		m_infra_checkerboard_verts.resize(0);
	}
	else
	{
		m_infra_checkerboard_verts.resize(7 * 5 * 2);
		//m_detected_points_infra = pointsInfra;
		int j = 0;
		for (int i = 0; i < pointsInfra.size(); i++, j += 2)
		{
			m_infra_checkerboard_verts[j] = pointsInfra[i].x; // here we scale down and shift to -1.0 -> 1.0
			m_infra_checkerboard_verts[j + 1] = (m_depth_height - pointsInfra[i].y); // y is flipped here.... does this matter? or is it just so that openGL renders the point correctly, since the texture read is 1 - texture.y
		}

	}
}



void kRender::writePLYFloat(std::vector<float> PC, std::vector<float> NC, const char* FileName)
{
	std::ofstream outFile(FileName);

	if (!outFile)
	{
		//cerr << "Error opening output file: " << FileName << "!" << endl;
		printf("Error opening output file: %s!\n", FileName);
		exit(1);
	}

	////
	// Header
	////
	
	const int pointNum = 512*424;
	const int vertNum = 6;

	outFile << "ply" << std::endl;
	outFile << "format ascii 1.0" << std::endl;
	outFile << "element vertex " << pointNum << std::endl;
	outFile << "property float x" << std::endl;
	outFile << "property float y" << std::endl;
	outFile << "property float z" << std::endl;
	if (vertNum == 6)
	{
		outFile << "property float nx" << std::endl;
		outFile << "property float ny" << std::endl;
		outFile << "property float nz" << std::endl;
	}
	outFile << "end_header" << std::endl;

	////
	// Points
	////

	for (int pi = 0; pi < m_verticesPointcloud.size()-2; pi+=4)
	{

		outFile << PC[pi] << " " << PC[pi+1] << " " << PC[pi+2] << " " << NC[pi] << " " << NC[pi+1] << " " << NC[pi+2] << std::endl;

	}

	outFile.close();

	return;
}