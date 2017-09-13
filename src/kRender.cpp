#include "kRender.h"

kRender::~kRender()
{
}

GLFWwindow * kRender::loadGLFWWindow()
{

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_REFRESH_RATE, 30);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

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

	m_DepthSubroutineID = glGetSubroutineIndex(renderProg.getHandle(), GL_FRAGMENT_SHADER, "fromDepth");
	m_ColorSubroutineID = glGetSubroutineIndex(renderProg.getHandle(), GL_FRAGMENT_SHADER, "fromColor");
	m_FlowSubroutineID = glGetSubroutineIndex(renderProg.getHandle(), GL_FRAGMENT_SHADER, "fromFlow");
	m_VertexSubroutineID = glGetSubroutineIndex(renderProg.getHandle(), GL_FRAGMENT_SHADER, "fromVertex");
	m_PointsSubroutineID = glGetSubroutineIndex(renderProg.getHandle(), GL_FRAGMENT_SHADER, "fromPoints");
	m_colorTypeSubroutineID = glGetSubroutineUniformLocation(renderProg.getHandle(), GL_FRAGMENT_SHADER, "getColorSelection");

	m_vertex3DSubroutineID = glGetSubroutineIndex(renderProg.getHandle(), GL_VERTEX_SHADER, "fromVertex3D");
	m_depth2DSubroutineID = glGetSubroutineIndex(renderProg.getHandle(), GL_VERTEX_SHADER, "fromDepth2D");
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

	//std::vector<float> verticesDepth = {
	//	// Positions		// Texture coords
	//	1920.0f / 2.0f, 1080.0f / 2.0f, 0.0f, 1.0f, 1.0f, // top right
	//	1920.0f / 2.0f, -1080.0f / 2.0f, 0.0f, 1.0f, 0.0f, // bottom right
	//	-1920.0f / 2.0f, -1080.0f / 2.0f, 0.0f, 0.0f, 0.0f, // bottom left
	//	-1920.0f / 2.0f, 1080.0f / 2.0f, 0.0f, 0.0f, 1.0f  // Top left
	//};

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
	verticiesPointcloud.resize(m_depth_height * m_depth_width * 3);

	m_verticesPointcloud = verticiesPointcloud;
}




void kRender::setTextures()
{
	glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_EBO);

	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(float), &m_vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), &m_indices[0], GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// TexCoord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (GLvoid*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	// set up VOA for pointcloud
	glGenVertexArrays(1, &m_VAO_Pointcloud);
	glBindVertexArray(m_VAO_Pointcloud);

	// set up for pointcloud SSBO
	glGenBuffers(1, &m_buf_Pointcloud);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_buf_Pointcloud);
	glBufferData(GL_SHADER_STORAGE_BUFFER, m_verticesPointcloud.size() * sizeof(float), &m_verticesPointcloud[0], GL_DYNAMIC_DRAW);

	// bind buffer to VAO
	glBindBuffer(GL_ARRAY_BUFFER, m_buf_Pointcloud);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); // 3 floats per vertex, no stride, no padding
	glEnableVertexAttribArray(0);
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


void kRender::renderLiveVideoWindow(float* depthArray)
{
	//glm::mat4 projection = glm::ortho(0.0f, 512.0f, 0.0f, 424.0f);
	// Camera matrix
	glm::mat4 View = glm::translate(glm::mat4(1.0f), glm::vec3(-0.3f, -0.f, -3.1f));

	glm::mat4 model = glm::mat4(1.0f);

	glm::mat4 Projection = glm::perspective(glm::radians(60.0f), 1.0f, 0.1f, 100.0f);
	//glm::mat4 Projection = glm::ortho(0.0f, 1920.0f, 0.0f, 1080.0f, -1.0f, 1000.0f);

	glm::mat4 MVP = Projection * View * model;

	int w, h;
	glfwGetFramebufferSize(m_window, &w, &h);
	glViewport(m_anchorMW.first, m_anchorMW.second, m_depth_width * m_render_scale_width, m_depth_height * m_render_scale_height);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	renderProg.use();
	glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &m_depth2DSubroutineID);

	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &m_DepthSubroutineID);
	//glUseProgram(m_programID);

	//// Bind Textures using texture units
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_textureDepth);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1920, 1080, 0, GL_BGRA, GL_UNSIGNED_BYTE, colorArray);
	glTexSubImage2D(GL_TEXTURE_2D, /*level*/0, /*xoffset*/0, /*yoffset*/0, 512, 424, GL_RED, GL_FLOAT, depthArray);
	//glUniform1i(glGetUniformLocation(m_programID, "_currentTexture"), 0);
	glUniformMatrix4fv(m_ProjectionID, 1, GL_FALSE, glm::value_ptr(Projection));
	glUniformMatrix4fv(m_MvpID, 1, GL_FALSE, glm::value_ptr(MVP));

	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);



}

void kRender::renderColorWindow(float* colorArray)
{
	// Camera matrix
	glm::mat4 View = glm::translate(glm::mat4(1.0f), glm::vec3(-0.f, -0.f, -3.0f));

	glm::mat4 model = glm::mat4(1.0f);

	glm::mat4 Projection = glm::perspective(glm::radians(60.0f), 1.0f, 0.1f, 100.0f);
	//glm::mat4 Projection = glm::ortho(0.0f, 1920.0f, 0.0f, 1080.0f, -1.0f, 1000.0f);
	
	glm::mat4 MVP = Projection * View * model;

	int w, h;
	glfwGetFramebufferSize(m_window, &w, &h);
	glViewport(0, 0, m_color_width * m_render_scale_width, m_color_height * m_render_scale_height);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	renderProg.use();
	glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &m_depth2DSubroutineID);

	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &m_ColorSubroutineID);

	//glUseProgram(m_programID);

	//// Bind Textures using texture units
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_textureColor);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1920, 1080, 0, GL_BGRA, GL_UNSIGNED_BYTE, colorArray);
	glTexSubImage2D(GL_TEXTURE_2D, /*level*/0, /*xoffset*/0, /*yoffset*/0, m_color_width, m_color_height, GL_BGRA, GL_UNSIGNED_BYTE, colorArray);
	//glUniform1i(glGetUniformLocation(m_programID, "_currentTexture"), 0);
	glUniformMatrix4fv(m_ProjectionID, 1, GL_FALSE, glm::value_ptr(Projection));
	glUniformMatrix4fv(m_MvpID, 1, GL_FALSE, glm::value_ptr(MVP));

	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}

void kRender::renderFlow(unsigned char* flowPtr)
{
	glm::mat4 projection = glm::ortho(0.0f, 512.0f, 0.0f, 424.0f);
	int w, h;
	glfwGetFramebufferSize(m_window, &w, &h);
	glViewport(m_anchorMW.first, m_anchorMW.second - m_depth_height, m_depth_width * m_render_scale_width, m_depth_height * m_render_scale_height);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	renderProg.use();
	glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &m_depth2DSubroutineID);

	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &m_FlowSubroutineID);

	//// Bind Textures using texture units
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, m_textureFlow);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1920, 1080, 0, GL_BGRA, GL_UNSIGNED_BYTE, colorArray);
	glTexSubImage2D(GL_TEXTURE_2D, /*level*/0, /*xoffset*/0, /*yoffset*/0, m_depth_width, m_depth_height, GL_RG, GL_FLOAT, flowPtr);
	//glUniform1i(glGetUniformLocation(m_programID, "_currentTexture"), 0);
	glUniformMatrix4fv(m_ProjectionID, 1, GL_FALSE, glm::value_ptr(projection));

	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);



}

void kRender::copyToPreviousFrame()
{

	//glCopyImageSubData(m_textureDepth, GL_TEXTURE_2D, 0, 0, 0, 0,
	//	m_textureDepthPrevious, GL_TEXTURE_2D, 0, 0, 0, 0,
	//	m_screen_width, m_screen_height, 1);

	// OR

	//GLuint tempTexture = m_textureDepth;
	//m_textureDepth = m_textureDepthPrevious;
	//m_textureDepthPrevious = tempTexture;

}

void kRender::renderPrevious()
{
	//glm::mat4 projection = glm::ortho(0.0f, 512.0f, 0.0f, 424.0f);
	// Camera matrix
	glm::mat4 View = glm::translate(glm::mat4(1.0f), glm::vec3(-0.3f, -0.f, -3.1f));

	glm::mat4 model = glm::mat4(1.0f);

	glm::mat4 Projection = glm::perspective(glm::radians(60.0f), 1.0f, 0.1f, 100.0f);
	//glm::mat4 Projection = glm::ortho(0.0f, 1920.0f, 0.0f, 1080.0f, -1.0f, 1000.0f);

	glm::mat4 MVP = Projection * View * model;
	int w, h;
	glfwGetFramebufferSize(m_window, &w, &h);
	glViewport(m_anchorMW.first, m_anchorMW.second, m_depth_width * m_render_scale_width, m_depth_height * m_render_scale_height);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	renderProg.use();
	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &m_DepthSubroutineID);
	glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &m_depth2DSubroutineID);

	glUniformMatrix4fv(m_ProjectionID, 1, GL_FALSE, glm::value_ptr(Projection));

	glUniformMatrix4fv(m_MvpID, 1, GL_FALSE, glm::value_ptr(MVP));
	//glUseProgram(m_programID);

	//// Bind Textures using texture units
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, m_textureDepthPrevious);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1920, 1080, 0, GL_BGRA, GL_UNSIGNED_BYTE, colorArray);
	//glTexSubImage2D(GL_TEXTURE_2D, /*level*/0, /*xoffset*/0, /*yoffset*/0, 512, 424, GL_RED, GL_FLOAT, depthArray);
	//glUniform1i(glGetUniformLocation(m_programID, "_currentTexture"), 0);
	//glUniformMatrix4fv(m_ProjectionID, 1, GL_FALSE, glm::value_ptr(projection));

	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);



}

void kRender::renderPreviousColor()
{
	// Camera matrix
	//glm::mat4 View = glm::lookAt(
	//	glm::vec3(0, 0, 20), // Camera is at (4,3,3), in World Space
	//	glm::vec3(0, 0, 0), // and looks at the origin
	//	glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
	//);

	glm::mat4 model = glm::mat4(1.0f);

	glm::mat4 View2 = glm::translate(glm::mat4(1.0f), glm::vec3(-0.f, -0.f, -3.0f));

	glm::mat4 Projection = glm::perspective(glm::radians(60.0f), 1.0f, 0.1f, 100.0f);
	//glm::mat4 Projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -100.0f, 100.0f);

	//glm::mat4 projection = glm::ortho(0.0f, 1920.0f, 0.0f, 1080.0f);
	glm::mat4 MVP = Projection * View2 * model;

	glm::mat4 projection = glm::ortho(0.0f, 1920.0f, 0.0f, 1080.0f);
	int w, h;
	glfwGetFramebufferSize(m_window, &w, &h);
	glViewport(0, 0, m_color_width * m_render_scale_width, m_color_height * m_render_scale_height);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	renderProg.use();
	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &m_ColorSubroutineID);
	glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &m_depth2DSubroutineID);

	glUniformMatrix4fv(m_ProjectionID, 1, GL_FALSE, glm::value_ptr(Projection));

	glUniformMatrix4fv(m_MvpID, 1, GL_FALSE, glm::value_ptr(MVP));
	//glUseProgram(m_programID);

	//// Bind Textures using texture units
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, m_textureDepthPrevious);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1920, 1080, 0, GL_BGRA, GL_UNSIGNED_BYTE, colorArray);
	//glTexSubImage2D(GL_TEXTURE_2D, /*level*/0, /*xoffset*/0, /*yoffset*/0, 512, 424, GL_RED, GL_FLOAT, depthArray);
	//glUniform1i(glGetUniformLocation(m_programID, "_currentTexture"), 0);
	//glUniformMatrix4fv(m_ProjectionID, 1, GL_FALSE, glm::value_ptr(projection));

	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);



}

void kRender::drawPoints()
{

	glm::vec3 light = glm::vec3(1.0f, 1.0f, -1.0f);
	glm::vec3 ambient = glm::vec3(0.1f, 0.1f, 0.1f);

	// get inverse camera matrix
	glm::mat4 invK = getInverseCameraMatrix(m_cameraParams);

	// bind image textures 
	glBindImageTexture(1, m_textureDepth, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
	glBindImageTexture(2, m_textureVertex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

	computeProg.use();
	computeProg.setUniform("invK", invK);

	int xWidth = divup(m_depth_width, 32);
	int yWidth = divup(m_depth_height, 32);

	glDispatchCompute(xWidth, yWidth, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	glBindImageTexture(0, m_textureVertex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	glBindImageTexture(1, m_textureNormal, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	v2nProg.use();
	glDispatchCompute(xWidth, yWidth, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);






	//// Camera matrix
	//glm::mat4 View = glm::lookAt(
	//	glm::vec3(0, 0, 100), // Camera is at (4,3,3), in World Space
	//	glm::vec3(0, 0, 0), // and looks at the origin
	//	glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
	//);


	glm::mat4 View = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 Projection = glm::perspective(glm::radians(60.f), 1.0f, 0.1f, 1500.0f);
	//glm::mat4 Projection = glm::ortho(-1000.0f, 1000.0f, -1000.0f, 1000.0f, -1.0f, 1000.0f);
	glm::mat4 MVP = Projection * View * model;
	glm::mat4 VP = Projection * View;
	int w, h;
	glfwGetFramebufferSize(m_window, &w, &h);
	glViewport(0,0, m_depth_width * m_render_scale_width, m_depth_height * m_render_scale_height);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	renderProg.use();

	glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &m_vertex3DSubroutineID);

	glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &m_PointsSubroutineID);

	glUniform3fv(m_lightID, 1, glm::value_ptr(light));
	glUniform3fv(m_ambientID, 1, glm::value_ptr(ambient));

	glUniformMatrix4fv(m_ProjectionID, 1, GL_FALSE, glm::value_ptr(Projection));
	glUniformMatrix4fv(m_ViewProjectionID, 1, GL_FALSE, glm::value_ptr(VP));

	glUniformMatrix4fv(m_MvpID, 1, GL_FALSE, glm::value_ptr(MVP));
	
	glPointSize(2.0f);
	glBindVertexArray(m_VAO_Pointcloud);
	glDrawArrays(GL_POINTS, 0, 512*424*3);
	glBindVertexArray(0);







	// convert depth points to vertex 3D





}

void kRender::drawLightModel()
{
	glm::vec3 light = glm::vec3(1.0f, 1.0f, -1.0f);
	glm::vec3 ambient = glm::vec3(0.1f, 0.1f, 0.1f);

	glm::mat4 View = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.0f));
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 Projection = glm::perspective(glm::radians(60.0f), 1.0f, 0.1f, 100.0f);
	//glm::mat4 Projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -100.0f, 100.0f);
	glm::mat4 MVP = Projection * View * model;
	int w, h;
	glfwGetFramebufferSize(m_window, &w, &h);
	glViewport((w / 2) - ((m_depth_width * m_render_scale_width) / 2), (h / 2) - ((m_depth_height * m_render_scale_height) / 2), m_depth_width * m_render_scale_width, m_depth_height * m_render_scale_height);

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

	glUniformMatrix4fv(m_ProjectionID, 1, GL_FALSE, glm::value_ptr(Projection));

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
	glDeleteBuffers(1, &m_VBO);
	glDeleteBuffers(1, &m_EBO);
	glfwTerminate();


}
