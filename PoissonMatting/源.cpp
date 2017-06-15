#include <opencv.hpp>
#include <iostream>
#include <string>
#include <vector>

#define BLACK 0
#define WHITE 255

using namespace std;
using namespace cv;

void gradient(Mat src, Mat x, Mat y)
{
	for (int i = 0; i < src.rows; i++)
	{
		for (int j = 0; j < src.cols; j++)
		{
			if (i == 0) y.at<uchar>(i, j) = src.at<uchar>(i + 1, j) - src.at<uchar>(i, j);
			else if (i == src.rows - 1) y.at<uchar>(i, j) = src.at<uchar>(i, j) - src.at<uchar>(i - 1, j);
			else y.at<uchar>(i, j) = (float)(src.at<uchar>(i + 1, j) - src.at<uchar>(i - 1, j)) / 2;

			if (j == 0) x.at<uchar>(i, j) = src.at<uchar>(i, j + 1) - src.at<uchar>(i, j);
			else if (j == src.cols - 1) x.at<uchar>(i, j) = src.at<uchar>(i, j) - src.at<uchar>(i, j - 1);
			else x.at<uchar>(i, j) = (float)(src.at<uchar>(i, j + 1) - src.at<uchar>(i, j - 1)) / 2;
		}
	}
}

int main()
{
	Mat src = imread("troll.png");
	Mat trimap = imread("trollTrimap.bmp", CV_LOAD_IMAGE_GRAYSCALE);

	Mat grey = Mat::zeros(src.rows, src.cols, CV_8U);
	cvtColor(src, grey, CV_BGR2GRAY);

	// 区分前景后景标志位
	Mat foreFlag = Mat::zeros(src.rows, src.cols, CV_8U);
	Mat backFlag = Mat::zeros(src.rows, src.cols, CV_8U);
	Mat foreGrey = Mat::zeros(src.rows, src.cols, CV_8U);
	Mat backGrey = Mat::zeros(src.rows, src.cols, CV_8U);
	Mat unknowFlag = Mat::zeros(src.rows, src.cols, CV_8U);
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

	Mat foreInpaint = Mat::zeros(src.rows, src.cols, CV_8U);
	Mat backInpaint = Mat::zeros(src.rows, src.cols, CV_8U);
	inpaint(foreGrey, unknowFlag + backFlag, foreInpaint, 3, INPAINT_TELEA);
	foreInpaint = foreInpaint & ~backFlag;
	inpaint(backGrey, unknowFlag + foreFlag, backInpaint, 3, INPAINT_TELEA);
	backInpaint = backInpaint & ~foreFlag;
	imshow("foreinpaint", foreInpaint);
	imshow("backinpaint", backInpaint);

	Mat different = Mat::zeros(src.rows, src.cols, CV_8U);
	for (int i = 0; i < src.rows; i++)
		for (int j = 0; j < src.cols; j++)
			different.at<uchar>(i, j) = foreInpaint.at<uchar>(i, j) - backInpaint.at<uchar>(i, j);
	imshow("diff", different);
	
	Mat xGradient = Mat::zeros(src.rows, src.cols, CV_8U);
	Mat yGradient = Mat::zeros(src.rows, src.cols, CV_8U);
	Mat xxGradient = Mat::zeros(src.rows, src.cols, CV_8U);
	Mat xyGradient = Mat::zeros(src.rows, src.cols, CV_8U);
	Mat yxGradient = Mat::zeros(src.rows, src.cols, CV_8U);
	Mat yyGradient = Mat::zeros(src.rows, src.cols, CV_8U);

	gradient(different, xGradient, yGradient);
	gradient(different / xGradient, xxGradient, xyGradient);
	gradient(different / yGradient, yxGradient, yyGradient);

	Mat b = Mat::zeros(src.rows, src.cols, CV_8U);

	b = xxGradient + yyGradient;

	Mat alphaNew = Mat::zeros(src.rows, src.cols, CV_8U);
	Mat alphaOld = Mat::zeros(src.rows, src.cols, CV_8U);
	alphaNew = foreFlag + 0.5 * unknowFlag;
	

	double threshold = 0.1;
	int n = 1;

	while (n < 50)
	{
		alphaOld = alphaNew.clone();
		for (int i = 1; i < src.rows - 1; i++)
		{
			for (int j = 1; j < src.cols - 1; j++)
			{
				if (unknowFlag.at<uchar>(i, j) != 0)
				{
					alphaNew.at<uchar>(i, j) = 1.0 / 4 * (alphaNew.at<uchar>(i - 1, j) + alphaNew.at<uchar>(i, j - 1)
						+ alphaOld.at<uchar>(i, j + 1) + alphaOld.at<uchar>(i + 1, j) - b.at<uchar>(i, j));
				}
			}
		}
		n++;
	}

	Mat output = Mat::zeros(src.rows, src.cols, CV_8UC4);
	
	for (int i = 0; i < src.rows; i++)
	{
		for (int j = 0; j < src.cols; j++)
		{
			output.at<Vec4b>(i, j)[0] = src.at<Vec3b>(i, j)[0];
			output.at<Vec4b>(i, j)[1] = src.at<Vec3b>(i, j)[1];
			output.at<Vec4b>(i, j)[2] = src.at<Vec3b>(i, j)[2];
			output.at<Vec4b>(i, j)[3] = alphaNew.at<uchar>(i, j);
		}
	}

	//imshow("A", src);
	vector<int> compression_params;
	compression_params.push_back(IMWRITE_PNG_COMPRESSION);
	compression_params.push_back(9);
	imwrite("output.png", output, compression_params);
	imshow("output", output);
	waitKey(0);
	return 0;
}