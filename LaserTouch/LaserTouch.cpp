#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"
#include "TouchInput.h"
#include <string>


using namespace cv;
using namespace std;

int main()
{
	cv::VideoCapture camera(0);
	if (!camera.isOpened())
	{
		printf("Cannot open the USB camera\n");
		return -1;
	}
	printf("Press Esc or Q to stop\n");
	camera.set(CAP_PROP_FOURCC, 'MJPG');
	camera.set(CAP_PROP_FRAME_WIDTH, 1280);
	camera.set(CAP_PROP_FRAME_HEIGHT, 720);
	camera.set(CAP_PROP_FPS, 120.0);
	camera.set(CAP_PROP_EXPOSURE, -11);
	camera.set(CAP_PROP_CONTRAST, 20);
	camera.set(CAP_PROP_GAIN, 0.0);

	printf("%f %f %f\n", camera.get(CAP_PROP_FRAME_WIDTH), camera.get(CAP_PROP_FRAME_HEIGHT), camera.get(CAP_PROP_FPS));
	printf("%f %f\n", camera.get(CAP_PROP_EXPOSURE), camera.get(CAP_PROP_GAIN));

	cv::Mat frame;
	cv::Mat gray;
	cv::Mat thold;

	SimpleBlobDetector::Params params;

	params.minThreshold = 200;
	params.maxThreshold = 211;

	params.filterByArea = true;
	params.minArea = 100;

	params.filterByCircularity = false;
	params.minCircularity = 0.1;

	params.filterByConvexity = false;
	params.minConvexity = 0.87;

	params.filterByInertia = false;
	params.minInertiaRatio = 0.01;

	vector<KeyPoint> keypoints;
	vector<KeyPoint> perviousKeypoints;
	Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create(params);
	bool touched = false;

	vector<TouchInput*> touchs;

	//TickMeter tm;
	//tm.reset();
	//tm.start();
	//int frameCount = 0;
	while (camera.read(frame))
	{
		cv::cvtColor(frame, gray, COLOR_RGB2GRAY);
		cv::threshold(gray, thold, 64, 255, THRESH_BINARY_INV);
		detector->detect(thold, keypoints);

		int count = 0;
		std::vector<KeyPoint>::const_iterator it = keypoints.begin(), end = keypoints.end();
		//for (; it != end; ++it)
		printf("%d ", (int)touchs.size());
		if (it != end)
		{
			//cv::putText(frame, std::to_string(count), it->pt, FONT_HERSHEY_TRIPLEX, 0.6, Scalar(0, 255, 0));
			//++count;
			printf("touch\n");
			if (!touched)
			{
				touched = true;
				touchs.push_back(new TouchInput(0, it->pt.x, it->pt.y));
			}
			else
			{
				touchs[0]->UpdateInput(0, it->pt.x, it->pt.y);
			}
		}
		else
		{
			printf("untouch\n");
			if (touched)
			{
				touched = false;
				delete touchs[0];
				touchs.erase(touchs.begin() + 0);
			}
			
		}

		drawKeypoints(frame, keypoints, frame, Scalar(0, 0, 255), DrawMatchesFlags::DEFAULT);
		

		cv::imshow("Camera", frame);
		//cv::imshow("Threshold", thold);

		const int key = cv::waitKey(1);
		if (key == 'q' || key == 27)
		{
			printf("Stopped by user\n");
			break;
		}
		//if (frameCount > 500) break;
		//++frameCount;
	}
	//tm.stop();
	//cout << frameCount / tm.getTimeSec() << " FPS" << endl;
	cv::destroyAllWindows();
	return 0;
}