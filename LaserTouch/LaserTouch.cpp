#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "TouchInput.h"
#include <string>

using namespace cv;
using namespace std;

int main()
{

	// Read property file
	FileStorage fs("Property.json", FileStorage::READ);
	if (!fs.isOpened())
	{
		cerr << "Failed to open property file" << endl;
		return -1;
	}
	int width, height, FPS, aperture, contrast, threshold, minArea, maxArea;
	float inertia;
	fs["width"] >> width;
	fs["height"] >> height;
	fs["FPS"] >> FPS;
	fs["aperture"] >> aperture;
	fs["contrast"] >> contrast;
	fs["threshold"] >> threshold;
	fs["minArea"] >> minArea;
	fs["maxArea"] >> maxArea;
	fs["inertia"] >> inertia;

	FileNode fn = fs["calibration"];
	Point2f src_pt[4];
	FileNodeIterator it = fn.begin(), it_end = fn.end();
	for (int id = 0; it != it_end; ++it)
	{
		FileNode item = *it;
		item >> src_pt[id];
		++id;
	}
	Point2f dst_pt[] = {
		Point2f(384, 216),
		Point2f(1536, 216),
		Point2f(1536, 864),
		Point2f(384, 864) };
	Mat h_matrix = getPerspectiveTransform(src_pt, dst_pt);

	// Init camera
	VideoCapture camera(0);
	if (!camera.isOpened())
	{
		printf("Cannot open the USB camera\n");
		return -1;
	}
	camera.set(CAP_PROP_FOURCC, 'MJPG');
	camera.set(CAP_PROP_FRAME_WIDTH, width);
	camera.set(CAP_PROP_FRAME_HEIGHT, height);
	camera.set(CAP_PROP_FPS, FPS);
	camera.set(CAP_PROP_APERTURE, aperture);
	camera.set(CAP_PROP_CONTRAST, contrast);

	printf("Resolution: %.fx%.f  FPS: %.f\n", 
		camera.get(CAP_PROP_FRAME_WIDTH), 
		camera.get(CAP_PROP_FRAME_HEIGHT), 
		camera.get(CAP_PROP_FPS));
	Mat frame;
	Mat gray;
	vector<Mat> channels;


	// Init blob detector
	SimpleBlobDetector::Params params;

	params.minThreshold = threshold;
	params.maxThreshold = 255;
	params.thresholdStep = 30;						// Run threshold only one time

	params.filterByArea = true;
	params.minArea = minArea;
	params.maxArea = maxArea;

	params.filterByInertia = true;
	params.minInertiaRatio = inertia;

	params.filterByCircularity = false;
	//params.minCircularity = 0.1f;

	params.filterByConvexity = false;
	//params.minConvexity = 0.87f;
	
	params.filterByColor = true;
	params.blobColor = 255;

	vector<KeyPoint> detectPoints;
	int detectCount;
	Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create(params);
	

	// Init touch input
	vector<TouchInput*> touchPoints;
	int touchCount;
	bool touched[MAX_COUNT] = { 0 };
	// assume a maximum of 10 contacts and turn touch feedback off
	InitializeTouchInjection(MAX_COUNT, TOUCH_FEEDBACK_NONE);


	printf("Press Esc or Q to stop\n");
	

	// Init timer
	//TickMeter tm;
	//tm.reset();
	//tm.start();
	//int frameCount = 0;
	while (camera.read(frame))
	{
		//cvtColor(frame, gray, COLOR_RGB2GRAY);
		cv::split(frame, channels);												// Faster than RGB2GRAY when using monochrome camera
		detector->detect(channels.at(0), detectPoints);

		//std::vector<KeyPoint>::const_iterator it = keypoints.begin(), end = keypoints.end();
		//for (; it != end & ; ++it)
		int i = 0;
		//printf("%d %d\n", (int)touchPoints.size(), (int)detectPoints.size());
		touchCount = touchPoints.size();
		detectCount = detectPoints.size();

		for (i = 0; i < detectCount && i < MAX_COUNT; ++i)
		{
			// Keystone correction
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
				//if (0 <= display_pt[0].x && display_pt[0].x <= 1920 && 0 <= display_pt[0].y && display_pt[0].y <= 1080) {
					printf("init %d\n", i);
					touchPoints.push_back(new TouchInput(i, (int)display_pt[0].x, (int)display_pt[0].y, touchCount + 1));
					touched[i] = true;
				//}
			}
		}

		//for (int i = 0; i < MAX_COUNT; ++i) printf("%d ", touched[i]); 
		
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
		//int count = 0;
		//vector<KeyPoint>::const_iterator it = detectPoints.begin(), end = detectPoints.end();
		//while (it != end)
		//{
		//	putText(frame, to_string(count) + " " + to_string(it->pt.x) + " " + to_string(it->pt.y), it->pt, FONT_HERSHEY_PLAIN, 0.8, Scalar(255, 0, 255));
		//	++count;
		//	++it;
		//}

		cv::imshow("Camera", frame);

		const int key = waitKey(1);
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
	destroyAllWindows();
	return 0;
}