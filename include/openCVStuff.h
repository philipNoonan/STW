#include "opencv2/core/utility.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/optflow.hpp"
#include "opencv2/aruco.hpp"

#include "aruco/aruco.h"
#include "aruco/highlyreliablemarkers.h"


class openCVStuff
{
public:
	openCVStuff()
		: m_algorithm(createOptFlow_DIS(cv::optflow::DISOpticalFlow::PRESET_MEDIUM))
	{}
	~openCVStuff() {};

	void doOpticalFlow() {
		if (m_grey0.empty())
		{
			m_grey1.copyTo(m_grey0);
		}

		m_algorithm->calc(m_grey0, m_grey1, m_flow);

		


	}
	void detectMarkersInfra(cv::Mat inputImage)
	{
		cv::Mat greyColor;

		cv::cvtColor(inputImage, greyColor, CV_RGBA2RGB);

		m_infraCamPams.at<float>(0, 0) = 364.7546f;
		m_infraCamPams.at<float>(1, 1) = 365.5064f;
		m_infraCamPams.at<float>(0, 2) = 254.0044f;
		m_infraCamPams.at<float>(1, 2) = 200.9755f;

		m_infraCamDist.at<float>(0, 0) = 0.0900f;
		m_infraCamDist.at<float>(1, 0) = -0.2460f;
		m_infraCamDist.at<float>(2, 0) = 0.0018f;
		m_infraCamDist.at<float>(3, 0) = 0.0017f;


		cv::Mat inputImageFlip;
		cv::flip(greyColor, inputImageFlip, 1);
		std::vector<int> markerIds;
		std::vector<std::vector<cv::Point2f>> markerCorners, rejectedCandidates;

		camParamsInfra.CameraMatrix = colorCamPams;
		camParamsInfra.Distorsion = colorCamDist;
		camParamsInfra.CamSize = cv::Size(512, 424); // change this if you use full resolution, or just code it properly you idito
		markerDetector.detect(inputImageFlip, detectedMarkers, camParamsInfra, markerSize);




		cv::Mat outputImage = inputImageFlip;
		for (vector<aruco::Marker>::iterator it = detectedMarkers.begin(); it != detectedMarkers.end(); it++)
		{
			aruco::Marker marker = *it;
			aruco::CvDrawingUtils::draw3dAxis(outputImage, marker, camParamsInfra);

		}
		//cv::aruco::drawDetectedMarkers(outputImage, markerCorners, markerIds);
		cv::imshow("oC", outputImage);
	}
	void detectMarkersColor(cv::Mat inputImage)
	{
		cv::Mat greyColor;

		cv::cvtColor(inputImage, greyColor, CV_RGBA2RGB);

		colorCamPams.at<float>(0, 0) = 992.2276f;
		colorCamPams.at<float>(1, 1) = 990.6481f;
		colorCamPams.at<float>(0, 2) = (1920.0f - 940.4056f);
		colorCamPams.at<float>(1, 2) = (1080.0f - 546.1401f);

		colorCamDist.at<float>(0, 0) = 0.002308769f;
		colorCamDist.at<float>(1, 0) = -0.03766959f;
		colorCamDist.at<float>(2, 0) = -0.0063f;
		colorCamDist.at<float>(3, 0) = 0.0036f;

		cv::Mat inputImageFlip;
		cv::flip(greyColor, inputImageFlip, 1);
		std::vector<int> markerIds;
		std::vector<std::vector<cv::Point2f>> markerCorners, rejectedCandidates;

		camParamsColor.CameraMatrix = colorCamPams;
		camParamsColor.Distorsion = colorCamDist;
		camParamsColor.CamSize = cv::Size(1920, 1080); // change this if you use full resolution, or just code it properly you idito
		markerDetectorColor.detect(inputImageFlip, detectedMarkersColor, camParamsColor, markerSize);




		cv::Mat outputImage = inputImageFlip;
		for (vector<aruco::Marker>::iterator it = detectedMarkersColor.begin(); it != detectedMarkersColor.end(); it++)
		{
			aruco::Marker marker = *it;
			aruco::CvDrawingUtils::draw3dAxis(outputImage, marker, camParamsColor);

		}
		//cv::aruco::drawDetectedMarkers(outputImage, markerCorners, markerIds);
		cv::imshow("oI", outputImage);
	}



	void displayFlow()
	{
		cv::split(m_flow, m_flow_uv);
		cv::multiply(m_flow_uv[1], -1, m_flow_uv[1]);
		cv::cartToPolar(m_flow_uv[0], m_flow_uv[1], m_mag, m_ang, true);

		cv::threshold(m_mag, m_magThresh, 0, 255, CV_THRESH_TOZERO);
		cv::normalize(m_magThresh, m_magThresh, 0, 1, cv::NORM_MINMAX);

		m_hsv_split[0] = m_ang;
		m_hsv_split[1] = m_magThresh;
		m_hsv_split[2] = cv::Mat::ones(m_ang.size(), m_ang.type());

		cv::merge(m_hsv_split, 3, m_hsv);
		cv::cvtColor(m_hsv, m_rgb, cv::COLOR_HSV2BGR);
		//cv::imshow("g0", m_grey0);
		//cv::imshow("g1", m_grey1);
		cv::imshow("diff", (m_grey0 - m_grey1) * 10);
		cv::imshow("flow1", m_rgb);
	}


	void setCurrentFrame(cv::Mat grey1)
	{
		m_grey1 = grey1;
	}
	void setPreviousFrame(cv::Mat grey0)
	{
		m_grey0 = grey0;
	}
	void swapFrames()
	{
		cv::swap(m_grey0, m_grey1);
	}
	cv::Mat getFlow()
	{
		return m_flow;
	}
	cv::Mat getFlowAng()
	{
		return m_ang;
	}
	cv::Mat getFlowMag()
	{
		return m_mag;
	}
	bool setupAruco()
	{
		// README SINCE YOU FORGOT AGAIN, FOOL. just set the aruco130d.dll rather than the releasse one?


		cv::FileStorage fs(dictFile, cv::FileStorage::READ);
		int nmarkers, markersize;
		fs["nmarkers"] >> nmarkers;                     // 
		fs["markersize"] >> markersize;                 // 
		fs.release();

		if (!markerDict.fromFile(dictFile))
		{
			cerr << "cannot open dictionary file " << dictFile << endl;
			return false;
		}
		if (markerDict.empty())
		{
			cerr << "marker dictionary file is empty" << endl;
			return false;
		}

		aruco::HighlyReliableMarkers::loadDictionary(markerDict);
		cout << "Dictionary Loaded" << endl;

		//for Color
		markerDetectorColor.setMakerDetectorFunction(aruco::HighlyReliableMarkers::detect);
		markerDetectorColor.setThresholdParams(25, 7);
		//markerDetector.setThresholdMethod(aruco::MarkerDetector::ADPT_THRES);
		markerDetectorColor.setCornerRefinementMethod(aruco::MarkerDetector::LINES);
		markerDetectorColor.setWarpSize((markerDict[0].n() + 2) * 8);
		markerDetectorColor.setMinMaxSize(0.005f, 0.5f);

		//for IR
		markerDetector.setMakerDetectorFunction(aruco::HighlyReliableMarkers::detect);
		markerDetector.setThresholdParams(25, 7);
		//markerDetector.setThresholdMethod(aruco::MarkerDetector::ADPT_THRES);
		markerDetector.setCornerRefinementMethod(aruco::MarkerDetector::LINES);
		markerDetector.setWarpSize((markerDict[0].n() + 2) * 8);
		markerDetector.setMinMaxSize(0.005f, 0.5f);
		//markerDetector.setDesiredSpeed(3);
		//cout << "detector configured" << endl;



	}
	
	void setInfraCamPams(cv::Mat irCP) 
	{
		m_infraCamPams = irCP;
	}

	void setInfraCamDist(cv::Mat irCD)
	{
		m_infraCamDist = irCD;
	}

private:

	cv::Ptr<cv::DenseOpticalFlow> m_algorithm;
	cv::Mat m_colorImage;
	//cv::Mat bigFlow;
	cv::Mat m_grey0;
	cv::Mat m_grey1;

	cv::Mat m_flow;
	cv::Mat m_flow_uv[2];
	cv::Mat m_mag, m_magThresh, m_ang;
	cv::Mat m_hsv_split[3], m_hsv;
	cv::Mat m_rgb;

	std::string dictFile = "resources/dodec.yml";
	aruco::Dictionary markerDict;
	aruco::MarkerDetector markerDetector, markerDetectorColor;
	vector<aruco::Marker> detectedMarkers;
	vector<aruco::Marker> detectedMarkersColor;
	aruco::CameraParameters camParamsColor, camParamsInfra;
	float markerSize = 0.0358f;
	cv::Mat colorCamPams = cv::Mat::eye(3, 3, CV_32F);
	cv::Mat colorCamDist = cv::Mat(5, 1, CV_32F);

	cv::Mat m_infraCamPams = cv::Mat::eye(3, 3, CV_32F);
	cv::Mat m_infraCamDist = cv::Mat(5, 1, CV_32F);


};






