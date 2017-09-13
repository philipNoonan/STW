#include "opencv2/core/utility.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/optflow.hpp"



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
};






