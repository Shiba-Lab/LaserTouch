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
	//cv::VideoCapture camera(0 + CAP_DSHOW);
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
	camera.set(CAP_PROP_CONTRAST, 20);
	//camera.set(CAP_PROP_SETTINGS, 0);  // Use DirectShow property page to set exprosure 

	printf("%f %f %f\n", camera.get(CAP_PROP_FRAME_WIDTH), camera.get(CAP_PROP_FRAME_HEIGHT), camera.get(CAP_PROP_FPS));
	printf("%f %f\n", camera.get(CAP_PROP_EXPOSURE), camera.get(CAP_PROP_GAIN));

	cv::Mat frame;
	cv::Mat gray;
	cv::Mat thold;
	vector<Mat> channels;

	SimpleBlobDetector::Params params;

	params.minThreshold = 127;
	params.maxThreshold = 255;
	params.thresholdStep = 16;

	params.filterByArea = true;
	params.minArea = 10;
	params.maxArea = 10000;

	params.filterByCircularity = false;
	params.minCircularity = 0.1;

	params.filterByConvexity = false;
	params.minConvexity = 0.87;

	params.filterByInertia = true;
	params.minInertiaRatio = 0.1;

	//params.filterByColor = true;
	//params.blobColor = 255;

	vector<KeyPoint> detectPoints;
	int detectCount;
	Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create(params);
	
	Point2f src_pt[] = {
		Point2f(  253, 452 ),
		Point2f( 1021, 445 ),
		Point2f(  916, 656 ),
		Point2f(  347, 655 )};
	Point2f dst_pt[] = {
		Point2f(  384, 216 ),
		Point2f( 1536, 216 ),
		Point2f( 1536, 864 ),
		Point2f(  384, 864 )};
	Mat h_matrix = getPerspectiveTransform(src_pt, dst_pt);

	vector<TouchInput*> touchPoints;
	int touchCount;
	bool touched[MAX_COUNT] = { 0 };
	// assume a maximum of 10 contacts and turn touch feedback off
	InitializeTouchInjection(MAX_COUNT, TOUCH_FEEDBACK_NONE);

	//TickMeter tm;
	//tm.reset();
	//tm.start();
	//int frameCount = 0;
	while (camera.read(frame))
	{
		//cv::cvtColor(frame, gray, COLOR_RGB2GRAY);
		cv::split(frame, channels);
		//cv::threshold(channels.at(0), thold, 0, 255, THRESH_BINARY_INV);
		cv::bitwise_not(channels.at(0), thold);
		detector->detect(thold, detectPoints);

		//std::vector<KeyPoint>::const_iterator it = keypoints.begin(), end = keypoints.end();
		//for (; it != end & ; ++it)
		int i = 0;
		//printf("%d %d\n", (int)touchPoints.size(), (int)detectPoints.size());
		touchCount = touchPoints.size();
		detectCount = detectPoints.size();

		for (i = 0; i < detectCount && i < MAX_COUNT; ++i)
		{
			//射影変換
			vector<Point2f> camera_pt(1);
			camera_pt[0] = detectPoints[i].pt;
			vector<Point2f> display_pt(1);
			perspectiveTransform(camera_pt, display_pt, h_matrix);

			printf("%f %f %f %f\n", detectPoints[i].pt.x, detectPoints[i].pt.y, display_pt[0].x, display_pt[0].y);
			
			if (i < touchCount)
			{
				//printf("update\n");
				touchPoints[i]->UpdateInject(i, (int)display_pt[0].x, (int)display_pt[0].y, touchCount);  // ?
				//touched[i] = true;
			}
			else
			{
<<<<<<< HEAD
				if (0 <= display_pt[0].x && display_pt[0].x < 1920 && 0 <= display_pt[0].y && display_pt[0].y < 1080) {
=======
				if (0 <= display_pt[0].x && display_pt[0].x <= 1920 && 0 <= display_pt[0].y && display_pt[0].y <= 1080) {
>>>>>>> parent of bfec286... Update LaserTouch.cpp
					printf("init %d\n", i);
					touchPoints.push_back(new TouchInput(i, (int)display_pt[0].x, (int)display_pt[0].y, touchCount + 1));
					touched[i] = true;
				}
			}
		}

		for (int i = 0; i < MAX_COUNT; ++i) printf("%d ", touched[i]); printf("\n");
		
		for (int j = touchCount - 1; j >= i; --j)
		{
			if (touched[j])
			{
				printf("Delete %d\n", j);
				touched[j] = false;
				touchPoints[j]->UpInject((int)touchPoints.size());
				delete touchPoints[j];
				touchPoints.erase(touchPoints.begin() + j);
			}
		}

		
		vector<POINTER_TOUCH_INFO> contacts;
		for (i = 0; i < (int)touchPoints.size(); ++i)
		{
			contacts.push_back(touchPoints[i]->contact);
		}
		//printf("count %d %d\n", (int)contacts.size(), (int)touchPoints.size());
		
		InjectTouchInput(static_cast<UINT32>(contacts.size()), &contacts[0]);


		drawKeypoints(frame, detectPoints, frame, Scalar(0, 0, 255), DrawMatchesFlags::DEFAULT);
		

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