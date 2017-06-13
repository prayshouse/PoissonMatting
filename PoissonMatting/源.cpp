#include <opencv.hpp>

using namespace std;
using namespace cv;

int main()
{
	Mat src = imread("A.png");
	imshow("A", src);

	waitKey(0);
	return 0;
}