#include <opencv.hpp>
#include <iostream>
#include <string>
#include <vector>

#define BLACK 0
#define WHITE 255

using namespace std;
using namespace cv;

int main()
{
	Mat src = imread("troll.png");
	Mat grey = Mat::zeros(src.rows, src.cols, CV_8U);
	Mat foreGrey = Mat::zeros(src.rows, src.cols, CV_8U);
	Mat backGrey = Mat::zeros(src.rows, src.cols, CV_8U);
	Mat foreInpaint = Mat::zeros(src.rows, src.cols, CV_8U);
	Mat backInpaint = Mat::zeros(src.rows, src.cols, CV_8U);
	Mat trimap = imread("trollTrimap.bmp", CV_LOAD_IMAGE_GRAYSCALE);
	Mat test = Mat::zeros(50, 50, CV_8UC4);
	Mat foreFlag = Mat::zeros(src.rows, src.cols, CV_8U);
	Mat backFlag = Mat::zeros(src.rows, src.cols, CV_8U);
	Mat unknowFlag = Mat::zeros(src.rows, src.cols, CV_8U);

	cvtColor(src, grey, CV_BGR2GRAY);

	// 区分前景后景标志位
	for (int i = 0; i < src.rows; i++)
	{
		for (int j = 0; j < src.cols; j++)
		{
			if (trimap.at<uchar>(i, j) == WHITE)
			{
				foreFlag.at<uchar>(i, j) = WHITE;
				foreGrey.at<uchar>(i, j) = grey.at<uchar>(i, j);
			}
			else if (trimap.at<uchar>(i, j) == BLACK)
			{
				backFlag.at<uchar>(i, j) = WHITE;
				backGrey.at<uchar>(i, j) = grey.at<uchar>(i, j);
			}
			else
			{
				unknowFlag.at<uchar>(i, j) = WHITE;
			}
		}
	}

	inpaint(foreGrey, unknowFlag + backFlag, foreInpaint, 3, INPAINT_TELEA);
	foreInpaint = foreInpaint & ~backFlag;
	inpaint(backGrey, unknowFlag + foreFlag, backInpaint, 3, INPAINT_TELEA);
	backInpaint = backInpaint & ~foreFlag;
	imshow("foreinpaint", foreInpaint);
	imshow("backinpaint", backInpaint);
	
	
	

	//imshow("A", src);
	vector<int> compression_params;
	compression_params.push_back(IMWRITE_PNG_COMPRESSION);
	compression_params.push_back(9);
	//imwrite("empty.png", test, compression_params);
	//imshow("empty", test);
	waitKey(0);
	return 0;
}