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
	krender.SetCallbackFunctions();
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
	OCVStuff.setupAruco();

	cv::Mat irCamPams = cv::Mat::eye(3, 3, CV_32F);
	cv::Mat irCamDist = cv::Mat(5, 1, CV_32F);

	irCamPams.at<float>(0, 0) = kcamera.fx();
	irCamPams.at<float>(1, 1) = kcamera.fy();
	irCamPams.at<float>(0, 2) = 512.0f - kcamera.ppx();
	irCamPams.at<float>(1, 2) = 424.0f - kcamera.ppy();

	//irCamDist.at<float>(0, 0) = kcamera.k1();
	//irCamDist.at<float>(1, 0) = kcamera.k2();
	//irCamDist.at<float>(2, 0) = kcamera.p1(); // k3 not used in aruco
	//irCamDist.at<float>(3, 0) = kcamera.p2();

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

		krender.setCameraParams(glm::vec4(kcamera.fx() + 10.0f, kcamera.fx() - 10.0f, kcamera.ppx(), kcamera.ppy()+ 10.0f)); // FIX ME

		if (kcamera.ready())
		{
			kcamera.frames(colorArray, depthArray, infraredArray, NULL, colorDepthMap);

			newColor = cv::Mat(1080, 1920, CV_8UC4, colorArray);








			OCVStuff.detectMarkersColor(newColor);

			//OCVStuff.getDepthToColorMatrix();



			//cv::imshow("bd", cv::Mat(1082, 1920, CV_32FC1, bigDepthArray) / 1000.0f);

			////// This checks if the mapping is copying out correctly
			//cv::Mat newColor = cv::Mat(1080, 1920, CV_32FC1, colorArray);
			//cv::Mat depCol = cv::Mat(424, 512, CV_32FC1);
			//int ind = 0;
			//for (int i = 0; i < 424; i++)
			//{
			//	for (int j = 0; j < 512; j++)
			//	{
			//		int y = colorDepthMap[ind] / 1920;
			//		int x = colorDepthMap[ind] % 1920;
			//		//int index = colorDepthMap[ind];
			//		ind++;
			//		//if (index > 0)
			//		//{
			//			depCol.at<float>(i, j) = newColor.at<float>(y, x);
			//			//std::cout << index << " ";

			//		//}
			//	}
			//}
			//cv::imshow("regi", cv::Mat(424, 512, CV_8UC4, depCol.data));


			//// This checks if the mapping is copying out correctly
			//cv::Mat depCol = cv::Mat(424, 512, CV_32FC1);
			//int ind = 0;
			//for (int i = 0; i < 424; i++)
			//{
			//	for (int j = 0; j < 512; j++)
			//	{
			//		//int u = colorDepthMap[ind] / 1080;
			//		//int v = colorDepthMap[ind] % 1080;
			//		int index = colorDepthMap[ind];
			//		ind++;
			//		//if (index > 0)
			//		//{
			//			depCol.at<float>(i, j) = colorArray[index];
			//			//std::cout << index << " ";

			//		//}
			//	}
			//}
			//cv::imshow("regi", cv::Mat(424, 512, CV_8UC4, depCol.data));


			//for (int i = 0; i < 512 * 424; i += 100)
			//{
			//	std::cout << colorDepthMap[i] << ", ";
			//}

			/*midDepth1 = *(depthArray + ((512 * 256) + 212));
			midDepth2 = *(depthArray + ((512 * 256) + 212));
			midDepth3 = *(depthArray + ((512 * 128) + 212));
			krender.updateGraphPoints(midDepth1, midDepth2, midDepth3);*/

			//std::cout << midDepth1 << std::endl;

			//cv::Mat newColor = cv::Mat(1080, 1920, CV_8UC4, colorArray);
			//cv::imshow("col", newColor);
			//cv::waitKey(1);

			//krender.drawTriangle();
			krender.setColorDepthMapping(colorDepthMap);
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
			if (krender.showImgui())
			{
				//ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
				//ImGui::ShowTestWindow(&show_test_window);
				ImGui::SetNextWindowSize(ImVec2((display_w / 4) - (2 * krender.guiPadding().first), (display_h / 2) - (1.5 * krender.guiPadding().second)), ImGuiSetCond_Always);
				ImGuiWindowFlags window_flags = 0;
				window_flags |= ImGuiWindowFlags_NoTitleBar;
				window_flags |= ImGuiWindowFlags_ShowBorders;
				window_flags |= ImGuiWindowFlags_NoResize;
				window_flags |= ImGuiWindowFlags_NoMove;
				window_flags |= ImGuiWindowFlags_NoCollapse;

				ImGui::Begin("Slider Graph", &show_slider_graph, window_flags);
				ImGui::PushItemWidth(-krender.guiPadding().first);
				ImGui::SetWindowPos(ImVec2(display_w - (display_w / 4) - (2 * krender.guiPadding().first) - krender.guiPadding().first, ((krender.guiPadding().second * 0.5f) + (0))   ));
				ImGui::Text("Help menu - press 'H' to hide");
				if (ImGui::Button("Select color points")) select_color_points_mode ^= 1; ImGui::SameLine();
				if (ImGui::Button("Reset")) OCVStuff.resetColorPoints();

				if (ImGui::Button("Select depth points")) select_depth_points_mode ^= 1; ImGui::SameLine();
				if (ImGui::Button("Reset")) OCVStuff.resetDepthPoints();

				ImGui::End();

			}



			
			if (newFrame)
			{
				krender.renderLiveVideoWindow(depthArray);
				krender.renderColorWindow(colorArray);
				krender.setColorDepthMapping(colorDepthMap);
				krender.drawPoints();
			}
			else 
			{
				krender.renderPrevious();
				krender.renderPreviousColor();
				krender.setColorDepthMapping(colorDepthMap);
				krender.drawPoints();


			}

			if (select_color_points_mode && newFrame == true)
			{
				//krender.getMouseClickPositionsColor();
				//krender.drawMouseClickPositionsColor();

				cv::Mat newColor = cv::Mat(1080, 1920, CV_32FC1, colorArray);
				cv::Mat depCol = cv::Mat(424, 512, CV_32FC1);
				int ind = 0;
				for (int i = 0; i < 424; i++)
				{
					for (int j = 0; j < 512; j++)
					{
						int y = colorDepthMap[ind] / 1920;
						int x = colorDepthMap[ind] % 1920;
						//int index = colorDepthMap[ind];
						ind++;
						//if (index > 0)
						//{
						depCol.at<float>(i, j) = newColor.at<float>(y, x);
						//std::cout << index << " ";

						//}
					}
				}




				cv::Mat outImg = cv::Mat(424, 512, CV_8UC4, depCol.data);
				cv::circle(outImg, cv::Point2f(200.0f, 100.0f), 10, cv::Scalar(255.0f, 0.0f, 0.0f, 255.0f), 5);
				cv::circle(outImg, cv::Point2f(272.0f, 120.0f), 10, cv::Scalar(255.0f, 0.0f, 0.0f, 255.0f), 5);
				cv::circle(outImg, cv::Point2f(386.0f, 140.0f), 10, cv::Scalar(255.0f, 0.0f, 0.0f, 255.0f), 5);

				cv::imshow("regi", outImg);

				depthPoints.resize(4);
				colorPoints.resize(4);

				//depthPoints[0] = cv::Point2f(120.0f, 80.0f);
				//depthPoints[0] = cv::Point2f(120.0f, 80.0f);

			}

			if (select_depth_points_mode  && newFrame == true)
			{
				std::vector<float> depthPointsFromBuffer = krender.getDepthPoints();
				int numPoints = depthPointsFromBuffer.size() / 4;
				if (numPoints > 0)
				{


					//krender.getMouseClickPositionsDepth();






					depthPoints.resize(numPoints);
					colorPoints.resize(numPoints);
					cv::Mat tDepth = cv::Mat(424, 512, CV_32SC1, colorDepthMap);


					int ind = 0;
					for (int j = 0; j < numPoints; j++)
					{
						depthPoints[j].x = depthPointsFromBuffer[ind] / 1000.0f;
						depthPoints[j].y = depthPointsFromBuffer[ind + 1] / 1000.0f;
						depthPoints[j].z = depthPointsFromBuffer[ind + 2] / 1000.0f;

						int index = depthPointsFromBuffer[ind + 3];

						int xPixDepth = index % 512;
						int yPixDepth = index / 512;
						cv::circle(tDepth, cv::Point2f(xPixDepth, yPixDepth), 10, cv::Scalar(0.0f, 0.0f, 0.0f, 255.0f), 5);


						int colIndex = colorDepthMap[index];

						int xColor = colIndex % 1920;
						int yColor = colIndex / 1920;

						colorPoints[j] = cv::Point2f(xColor, yColor);

						std::cout << "xCol: " << xColor << " yCol: " << yColor <<std::endl;
						std::cout << "xDep: " << xPixDepth << " yDep: " << yPixDepth << std::endl;



						ind += 4;

					}

					//std::cout << "dep: " << depthPoints << std::endl;
					//std::cout << "col: " << colorPoints << std::endl;

					if (numPoints >= 4)
					{
						cv::Mat registration;
						registration = OCVStuff.getDepthToColorMatrix(depthPoints, colorPoints);



						std::cout << "reg: " << registration << std::endl;
					}



					for (int k = 0; k < numPoints; k++)
					{
						cv::circle(newColor, colorPoints[k], 10, cv::Scalar(255.0f, 0.0f, 0.0f, 255.0f), 5);

					}
					cv::Mat pDown;
					cv::pyrDown(newColor, pDown);

					cv::imshow("with circles", pDown);
					cv::imshow("irr", 0.1 * cv::Mat(424, 512, CV_32SC1, colorDepthMap));

					

					//krender.labelDepthPointsOnColorImage(depthArray, colorDepthMap);
					//krender.getMouseClickPositionsDepth();
					//krender.drawMouseClickPositionsDepth();
				}
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