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
	int camera_W, camera_H, display_W, display_H, FPS, aperture, contrast, threshold, minArea, maxArea;
	float inertia;
	fs["camera_W"] >> camera_W;
	fs["camera_H"] >> camera_H;
	fs["display_W"] >> display_W;
	fs["display_H"] >> display_H;
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
camera.set(CAP_PROP_FRAME_WIDTH, camera_W);
camera.set(CAP_PROP_FRAME_HEIGHT, camera_H);
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


	// Perspective correction
	detectCount = detectPoints.size();
	vector<Point2f> camera_pt(detectCount);
	vector<Point2f> display_pt(detectCount);
	for (int i = 0; i < detectCount; ++i) camera_pt[i] = detectPoints[i].pt;
	perspectiveTransform(camera_pt, display_pt, h_matrix);


	// Remove out of range detect points
	for (int i = 0; i < detectCount; )
	{
		if (display_pt[i].x < 0 ||
				display_pt[i].x > display_W ||
				display_pt[i].y < 0 ||
				display_pt[i].y > display_H )
		{
			display_pt.erase(display_pt.begin() + i);
			detectCount -= 1;
			continue;
		}
		++i;
	}


	// Track by bounding box


		for (i = 0; i < detectCount && i < MAX_COUNT; ++i)
		{

			if (i < touchCount)
			{
				// Update touch
				touchPoints[i]->UpdateInject(i, (int)display_pt[0].x, (int)display_pt[0].y, touchCount);  // ?
			}
			else
			{
				// New touch
				//if (0 <= display_pt[0].x && display_pt[0].x <= 1920 && 0 <= display_pt[0].y && display_pt[0].y <= 1080) {
					printf("init %d\n", i);
					touchPoints.push_back(new TouchInput(i, (int)display_pt[0].x, (int)display_pt[0].y, touchCount + 1));
					touched[i] = true;
				//}
			}
		}

		
		for (int j = touchCount - 1; j >= i; --j)
		{
			// Delete touch
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

		InjectTouchInput(static_cast<UINT32>(contacts.size()), &contacts[0]);


		drawKeypoints(frame, detectPoints, frame, Scalar(0, 0, 255), DrawMatchesFlags::DEFAULT);
		

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