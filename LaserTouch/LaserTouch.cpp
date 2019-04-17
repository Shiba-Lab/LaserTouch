#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


using namespace cv;

int main()
{
	cv::VideoCapture cap(0);
	if (!cap.isOpened())
	{
		printf("Cannot open the USB camera\n");
		return -1;
	}
	printf("Press Esc or Q to stop\n");
	cap.set(CAP_PROP_FOURCC, 'MJPG');
	cap.set(CAP_PROP_FRAME_WIDTH, 1280);
	cap.set(CAP_PROP_FRAME_HEIGHT, 720);
	cap.set(CAP_PROP_FPS, 120.0);
	cap.set(CAP_PROP_EXPOSURE, -11);
	cap.set(CAP_PROP_CONTRAST, 20);
	cap.set(CAP_PROP_GAIN, 0.0);

	printf("%f %f %f\n", cap.get(CAP_PROP_FRAME_WIDTH), cap.get(CAP_PROP_FRAME_HEIGHT), cap.get(CAP_PROP_FPS));
	printf("%f %f\n", cap.get(CAP_PROP_EXPOSURE), cap.get(CAP_PROP_GAIN));

	cv::Mat frame;
	cv::Mat thold;

//	cap.read(frame);
//	imwrite("test.jpg", frame);
	while (cap.read(frame))
	{
		cv::threshold(frame, thold, 128, 256, THRESH_BINARY);
		cv::imshow("Camera", frame);
		cv::imshow("Threshold", thold);

		const int key = cv::waitKey(1);
		if (key == 'q' || key == 27)
		{
			printf("Stopped by user\n");
			break;
		}
	}
	cv::destroyAllWindows();
	return 0;
}