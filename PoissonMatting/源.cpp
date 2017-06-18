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

void laplaceOp(const Mat src, OutputArray output)
{
	Mat gaussian;
	GaussianBlur(src, gaussian, Size(3, 3), 0, 0, BORDER_DEFAULT);
	imshow("gauseion", gaussian);
	Mat dst;
	int delta = 0, ddepth = CV_16S, scale = 1, kernel_size = 3;
	Laplacian(gaussian, dst, ddepth, kernel_size, scale, delta, BORDER_DEFAULT);
	convertScaleAbs(dst, output);
}

int main()
{
	Mat src = imread("troll.png");
	imshow("src", src);
	Mat trimap = imread("trollTrimap.bmp", CV_LOAD_IMAGE_GRAYSCALE);

	Mat grey = Mat::zeros(src.rows, src.cols, CV_8U);
	cvtColor(src, grey, CV_RGB2GRAY);
	//Mat grey = imread("troll.png", CV_LOAD_IMAGE_GRAYSCALE);

	// 区分前景后景标志位
	// 标志位存储在foreFlag, unknowFlag, backFlag里面
	// 图像存储在foreGrey, backGrey里面
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
	imshow("grey", grey);

	// 计算unknown区域中，Fp和Bp的值，取最近的点
	// 计算的值存储在foreInpaint和backInpaint里面
	Mat foreInpaint = Mat::zeros(src.rows, src.cols, CV_8U);
	Mat backInpaint = Mat::zeros(src.rows, src.cols, CV_8U);
	inpaint(foreGrey, unknowFlag + backFlag, foreInpaint, 3, INPAINT_TELEA);
	inpaint(backGrey, unknowFlag + foreFlag, backInpaint, 3, INPAINT_TELEA);
	foreInpaint = foreInpaint & ~backFlag;
	backInpaint = backInpaint & ~foreFlag;

	// 计算F-B，并用Gaussian filter过滤
	// 计算的值存储在gaussianDifferent里面
	Mat different = Mat::zeros(src.rows, src.cols, CV_16S);
	Mat gaussianDifferent = Mat(src.rows, src.cols, CV_16S);
	Mat showDifferent;
	
	for (int i = 0; i < src.rows; i++)
		for (int j = 0; j < src.cols; j++)
			different.at<short>(i, j) = (int)foreInpaint.at<uchar>(i, j) - (int)backInpaint.at<uchar>(i, j);
	GaussianBlur(different, gaussianDifferent, Size(3, 3), 0, 0, BORDER_DEFAULT);
	convertScaleAbs(gaussianDifferent, showDifferent);
	imshow("showdiff", gaussianDifferent);

	//Mat xGradient = Mat::zeros(src.rows, src.cols, CV_8U);
	//Mat yGradient = Mat::zeros(src.rows, src.cols, CV_8U);
	//Mat xxGradient = Mat::zeros(src.rows, src.cols, CV_8U);
	//Mat xyGradient = Mat::zeros(src.rows, src.cols, CV_8U);
	//Mat yxGradient = Mat::zeros(src.rows, src.cols, CV_8U);
	//Mat yyGradient = Mat::zeros(src.rows, src.cols, CV_8U);
	//gradient(different, xGradient, yGradient);
	//gradient(different / xGradient, xxGradient, xyGradient);
	//gradient(different / yGradient, yxGradient, yyGradient);
	//Mat b = Mat::zeros(src.rows, src.cols, CV_8U);
	//b = xxGradient + yyGradient;
	/*Mat gaussianDifferent;
	Mat gaussianSrc;
	GaussianBlur(different, gaussianDifferent, Size(3, 3), 0, 0, BORDER_DEFAULT);
	GaussianBlur(grey, gaussianSrc, Size(3, 3), 0, 0, BORDER_DEFAULT);
	imshow("gdiff", gaussianDifferent);
	imshow("gsrc", gaussianSrc);
	Mat terminatLaplace;
	Mat terminatLaplaceS;
	Laplacian(gaussianDifferent, terminatLaplace, CV_16S, 3, 1, 0, BORDER_DEFAULT);
	Laplacian(gaussianSrc, terminatLaplaceS, CV_16S, 3, 1, 0, BORDER_DEFAULT);
	Mat laplace = Mat::zeros(src.rows, src.cols, CV_8U);
	Mat laplaceS = Mat::zeros(src.rows, src.cols, CV_8U);
	convertScaleAbs(laplace, terminatLaplace);
	convertScaleAbs(laplaceS, terminatLaplaceS);*/

	// 计算△I
	// 计算结果存储在laplace里面
	Mat laplace = Mat::zeros(src.rows, src.cols, CV_8U);
	laplaceOp(grey, laplace);
	imshow("laplace", laplace);


	// 计算 gradient-x, gradient-y
	// 梯度结果存储在xGrad, yGrad中，绝对值结果存储在xAbsGrad, yAbsGrad中
	Mat gaussianSrc;
	Mat xGrad, yGrad;
	Mat xAbsGrad = Mat(src.rows, src.cols, CV_16S);
	Mat yAbsGrad = Mat(src.rows, src.cols, CV_16S);
	GaussianBlur(grey, gaussianSrc, Size(3, 3), 0, 0, BORDER_DEFAULT);
	Sobel(gaussianSrc, xGrad, -1, 1, 0, 3, 1, 0, BORDER_DEFAULT);
	Sobel(gaussianSrc, yGrad, -1, 0, 1, 3, 1, 0, BORDER_DEFAULT);
	convertScaleAbs(xGrad, xAbsGrad);
	convertScaleAbs(yGrad, yAbsGrad);
	imshow("absgrad-x", xAbsGrad);

	// 计算div(▽I / (F - B))
	// 散度结果存储在div之中
	Mat xAbsGradQuotient = Mat(src.rows, src.cols, CV_16S);
	Mat yAbsGradQuotient = Mat(src.rows, src.cols, CV_16S);
	Mat xxGrad, yyGrad, xxAbsGrad, yyAbsGrad;
	Mat div;
	// divide(xAbsGrad, gaussianDifferent, xAbsGradQuotient, 1, -1);
	// divide(yAbsGrad, gaussianDifferent, yAbsGradQuotient, 1, -1);

	Sobel(xAbsGradQuotient, xxGrad, -1, 1, 0, 3, 1, 0, BORDER_DEFAULT);
	Sobel(yAbsGradQuotient, yyGrad, -1, 0, 1, 3, 1, 0, BORDER_DEFAULT);
	convertScaleAbs(xxGrad, xxAbsGrad);
	convertScaleAbs(yyGrad, yyAbsGrad);
	div = xxAbsGrad + yyAbsGrad;
	imshow("div", div);


	// 计算α
	Mat alphaNew = Mat::zeros(src.rows, src.cols, CV_8U);
	Mat alphaOld = Mat::zeros(src.rows, src.cols, CV_8U);
	alphaNew = foreFlag + 0.5 * unknowFlag;
	
	imshow("alphaNew", alphaNew);

	double threshold = 0.1;
	double minus;
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
					alphaNew.at<uchar>(i, j) = 0.25 * (alphaNew.at<uchar>(i - 1, j) + alphaNew.at<uchar>(i, j - 1)
						+ alphaOld.at<uchar>(i, j + 1) + alphaOld.at<uchar>(i + 1, j) - laplace.at<uchar>(i, j));
				}
			}
		}
		n++;
		minus = sum(alphaNew)[0] - sum(alphaOld)[0];
		if (minus < threshold) break;
	}
	cout << n << endl;

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
	//imwrite("output.png", output, compression_params);
	imshow("output", output);
	waitKey(0);
	return 0;
}