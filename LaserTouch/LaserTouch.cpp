#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>

using namespace cv;

int main()
{
	cv::VideoCapture cap(0);
	if (!cap.isOpened())
	{
		return -1;
	}

	cv::Mat frame;
	while (cap.read(frame))
	{
		cv::imshow("win", frame);
		const int key = cv::waitKey(1);
		if (key == 'q')
		{
			break;
		}
	}
	cv::destroyAllWindows();
	return 0;
}
