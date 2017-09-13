#include "kinect.h"


#include "opencv2/core/utility.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error %d: %s\n", error, description);
}

void kRenderInit()
{
	krender.compileAndLinkShader();
	// Set locations
	krender.setLocations();
	krender.setVertPositions();
	krender.setTextures();
	//krender.setGraphPoints(2000, 500.0f, 500.0f, 500.0f);
	krender.anchorMW(std::make_pair<int, int>(1920 - 512 - krender.guiPadding().first, krender.guiPadding().second));
}




int main(int, char**)
{
	int display_w, display_h;
	// load openGL window
	window = krender.loadGLFWWindow();
	glfwGetFramebufferSize(window, &display_w, &display_h);
	// Setup ImGui binding
	ImGui_ImplGlfwGL3_Init(window, true);
	ImVec4 clear_color = ImColor(114, 144, 154);

	kRenderInit();

	//gfusion.queryWorkgroupSizes();

	//gfusion.createTexture();
	//gfusion.initScene();




	const int samples = 50;
	float time[samples];
	int index = 0;



	kcamera.start();

	while (!kcamera.ready())
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}

	bool newFrame = false;
	bool show_slider_graph = true;

	float midDepth1 = 0.0f;
	float midDepth2 = 0.0f;
	float midDepth3 = 0.0f;

	// Main loop
	while (!glfwWindowShouldClose(window))
	{
		glfwGetFramebufferSize(window, &display_w, &display_h);
		krender.renderScaleHeight((float)display_h / 1080.0f);
		krender.renderScaleWidth((float)display_w / 1920.0f);


		krender.anchorMW(std::make_pair<int, int>(50, 1080 - 424 - 50 ));

		//// Rendering
		glViewport(0, 0, display_w, display_h);
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);

		//gfusion.update(float(glfwGetTime()));

		//krender.requestShaderInfo();

		krender.setCameraParams(glm::vec4(kcamera.fx(), kcamera.fx(), kcamera.ppx(), kcamera.ppy()));

		if (kcamera.ready())
		{
			kcamera.frames(colorArray, depthArray, infraredArray, NULL);

			/*midDepth1 = *(depthArray + ((512 * 256) + 212));
			midDepth2 = *(depthArray + ((512 * 256) + 212));
			midDepth3 = *(depthArray + ((512 * 128) + 212));
			krender.updateGraphPoints(midDepth1, midDepth2, midDepth3);*/

			//std::cout << midDepth1 << std::endl;

			//cv::Mat newColor = cv::Mat(1080, 1920, CV_8UC4, colorArray);
			//cv::imshow("col", newColor);
			//cv::waitKey(1);

			//krender.drawTriangle();

			cv::Mat temp = cv::Mat(424, 512, CV_32FC1, infraredArray);
			cv::Mat grey1;
			temp.convertTo(grey1, CV_8UC1, 255.0 / 100000.0);
			OCVStuff.setCurrentFrame(grey1);
			OCVStuff.doOpticalFlow();
			//OCVStuff.displayFlow();
			flow = OCVStuff.getFlow();
			OCVStuff.swapFrames();
			newFrame = true; // make this a return bool on the kcamera.frames()


		}
		else
		{
			newFrame = false;
		}

		bool show_test_window = true;


			glfwPollEvents();
			ImGui_ImplGlfwGL3_NewFrame();

			// 1. Show a simple window
			// Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"
//			{
//				
//				ImGui::SetNextWindowSize(ImVec2(display_w - (2 * krender.guiPadding().first) , (display_h / 2) - (1.5 * krender.guiPadding().second)), ImGuiSetCond_Always);
//				ImGuiWindowFlags window_flags = 0;
//				window_flags |= ImGuiWindowFlags_NoTitleBar;
//				window_flags |= ImGuiWindowFlags_ShowBorders;
//				window_flags |= ImGuiWindowFlags_NoResize;
//				window_flags |= ImGuiWindowFlags_NoMove;
//				window_flags |= ImGuiWindowFlags_NoCollapse;
//
//				ImGui::Begin("Slider Graph", &show_slider_graph, window_flags);
//				ImGui::PushItemWidth(-krender.guiPadding().first);
//				ImGui::SetWindowPos(ImVec2(krender.guiPadding().first, ((krender.guiPadding().second * 0.5f) + (display_h / 2))   ));
//
//				static bool showX = true;
//				static bool showY = false;
//				static bool showZ = false;
//
//				//static float f = 0.0f;
//				//ImGui::Text("Hello, world!");
//				//ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
//				//ImGui::ColorEdit3("clear color", (float*)&clear_color);
//				
//				// Checkbox
//				ImGui::Checkbox("X", &showX); ImGui::SameLine();
//				ImGui::Checkbox("Y", &showY); ImGui::SameLine();
//				ImGui::Checkbox("Z", &showZ); //ImGui::SameLine();
//
//				//if (ImGui::Button("Another Window")) show_another_window ^= 1;
//				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
//
////#define IM_ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))
////
////				static float values[150] = { 0 };
////				static int values_offset = 0;
////				if (1)
////				{
////					static float refresh_time = ImGui::GetTime(); // Create dummy data at fixed 60 hz rate for the demo
////					for (; ImGui::GetTime() > refresh_time + 1.0f / 30.0f; refresh_time += 1.0f / 30.0f)
////					{
////						static float phase = 0.0f;
////						values[values_offset] = midDepth1;
////						values_offset = (values_offset + 1) % IM_ARRAYSIZE(values);
////						phase += 0.10f*values_offset;
////					}
////				}
////				ImGuiStyle& style = ImGui::GetStyle();
////				style.Colors[ImGuiCol_PlotLines] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
////				static float scanTimeGui = 0.0f;
////
////				//std::vector<float> temp = krender.graphPointsX();
////
////				if(showX) ImGui::PlotLines("X", &temp[0], 2000, 0, "avg 0.0", 0.0f, 700.0f, ImVec2(0, 160), sizeof(float));
////				style.Colors[ImGuiCol_PlotLines] = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
////
////				if(showY) ImGui::PlotLines("Y", values, IM_ARRAYSIZE(values), values_offset, "avg 0.0", 200.0f, 700.0f, ImVec2(0, 160));
////
////				style.Colors[ImGuiCol_PlotLines] = ImVec4(0.0f, 0.0f, 1.0f, 1.0f);
////				if(showZ) ImGui::PlotLines("Z", values, IM_ARRAYSIZE(values), values_offset, "avg 0.0", 200.0f, 700.0f, ImVec2(0, 160));
////
////				ImGui::SliderFloat("", &scanTimeGui, 0.0f, 3600.0f);
////				scanTimeGui++;
////
////				ImGui::End();
//			}


			//// 2. Show another simple window, this time using an explicit Begin/End pair
			//if (show_another_window)
			//bool show_another_window = true;
			//{
			//	ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiSetCond_FirstUseEver);
			//	ImGui::Begin("Another Window", &show_another_window);
			//	ImGui::Text("Hello");
			//	ImGui::End();
			//}
			//{



			//}
			////// 3. Show the ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
			//if (show_test_window)
			{
				ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
				ImGui::ShowTestWindow(&show_test_window);
			}


			
			if (newFrame)
			{
				krender.renderLiveVideoWindow(depthArray);
				krender.renderColorWindow(colorArray);
				krender.drawPoints();
			}
			else 
			{
				krender.renderPrevious();
				krender.renderPreviousColor();
				krender.drawPoints();


			}

			krender.drawLightModel();
			//krender.renderFlow(flow.ptr());


			krender.setComputeWindowPosition();
			//gfusion.render();
			ImGui::Render();
			glfwSwapBuffers(window);
		}

	

	// Cleanup
	ImGui_ImplGlfwGL3_Shutdown();
	kcamera.stop();

	krender.cleanUp();

	return 0;
}