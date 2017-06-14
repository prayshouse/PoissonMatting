#include <opencv.hpp>
#include <string>
#include <vector>

using namespace std;
using namespace cv;

int main()
{
	Mat src = imread("troll.png");
	Mat trimap = imread("trollTrimap");
	Mat test = Mat::zeros(50, 50, CV_8UC4);
	//createAlphaMat(test);
	string str = "./";
	for (int i = 0; i < test.rows; i++)
	{
		for (int j = 0; j < test.cols; j++)
		{
			/*test.at<Vec4b>(i, j)[3] = 0;
			test.at<Vec4b>(i, j)[2] = 120;
			test.at<Vec4b>(i, j)[1] = 255;
			test.at<Vec4b>(i, j)[0] = 255;*/
		}
	}

	//imshow("A", src);
	vector<int> compression_params;
	compression_params.push_back(IMWRITE_PNG_COMPRESSION);
	compression_params.push_back(9);
	imwrite("empty.png", test, compression_params);
	imshow("empty", test);
	waitKey(0);
	return 0;
}