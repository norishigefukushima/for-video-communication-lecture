#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include "getopt.h"

#pragma comment(lib, "opencv_world320.lib")

using namespace std;
using namespace cv;

double PSNR64F(InputArray I1_, InputArray I2_)
{

	Mat I1, I2;
	if (I1_.channels() == 1 && I2_.channels() == 1)
	{
		I1_.getMat().convertTo(I1, CV_64F);
		I2_.getMat().convertTo(I2, CV_64F);
	}
	if (I1_.channels() == 3 && I2_.channels() == 3)
	{
		Mat temp;
		cvtColor(I1_, temp, COLOR_BGR2GRAY);
		temp.convertTo(I1, CV_64F);
		cvtColor(I2_, temp, COLOR_BGR2GRAY);
		temp.convertTo(I2, CV_64F);
	}

	Mat s1;
	absdiff(I1, I2, s1);       // |I1 - I2|
	s1 = s1.mul(s1);           // |I1 - I2|^2

	Scalar s = sum(s1);        // sum elements per channel

	double sse = s.val[0] + s.val[1] + s.val[2]; // sum channels

	if (sse <= 1e-10) // for small values return zero
		return 0;
	else
	{
		double mse = sse / (double)(I1.channels() * I1.total());
		double psnr = 10.0 * log10((255.0 * 255.0) / mse);
		return psnr;
	}
}

// 実行ファイルの使用方法を出力
void printUsage(char* argv[])
{
	cout << endl;
	cout << "Usage: " << argv[0] << " [-n] image1 image2 mode" << endl;
	cout << endl;
	cout << "mode 0: only Y" << endl;
	cout << "     1: mean YUV" << endl;
	cout << "     2: mean RGB" << endl;
	cout << "     3: for each YUV" << endl;
	cout << "     4: for each RGB" << endl;
}

int main(int argc, char* argv[])
{
	if (argc < 3)
	{
		cout << "error: too few arguments" << endl;
		//printUsage(argv);
		return 1;
	}

	bool printOutputInfo = true;    // trueの時は Y: U: V: などの情報を出力

	int opt;
	while ((opt = getopt(argc, argv, "n")) != -1)
	{
		switch (opt)
		{
		case 'n':
			printOutputInfo = false;
			break;

		default:    /* '?' */
			//printUsage(argv);
			return 2;
		}
	}

	string str1 = argv[optind];
	string str2 = argv[optind + 1];

	Mat src1 = imread(str1);
	Mat src2 = imread(str2);

	if (src1.empty())
	{
		cout << "error: couldn't read the image 1" << endl;
		//printUsage(argv);
		return 3;
	}
	if (src2.empty())
	{
		cout << "error: couldn't read the image 2" << endl;
		//printUsage(argv);
		return 4;
	}

	int mode;

	if (argc - optind == 2) mode = 0;    // 実行ファイル名とオプション以外に引数が2つしかない場合
	else mode = stoi(argv[optind + 2]);

	Mat yuv1, yuv2;
	cvtColor(src1, yuv1, CV_BGR2YUV);
	cvtColor(src2, yuv2, CV_BGR2YUV);

	vector<Mat> vRGB1(3), vRGB2(3);
	split(src1, vRGB1);
	split(src2, vRGB2);

	vector<Mat> vYUV1(3), vYUV2(3);
	split(yuv1, vYUV1);
	split(yuv2, vYUV2);

	double avg = 0.0;
	double psnr[3] = { 0.0, 0.0, 0.0 };

	switch(mode)
	{
	case 0:
	{
		if (printOutputInfo)
		{
			cout << "Y :" << endl << PSNR64F(vYUV1[0], vYUV2[0]) << endl;
		}
		else
		{
			cout << PSNR64F(vYUV1[0], vYUV2[0]) << endl;
		}
		break;
	}
	case 1:
	{
		for (int i = 0; i < 3; i++)
			avg += PSNR64F(vYUV1[i], vYUV2[i]);
		if (printOutputInfo)
		{
			cout << "mean YUV : " << endl << avg / 3 << endl;
		}
		else
		{
			cout << avg / 3 << endl;
		}
		break;
	}
	case 2:
	{
		for (int i = 0; i < 3; i++)
			avg += PSNR64F(vRGB1[i], vRGB2[i]);
		if (printOutputInfo)
		{
			cout << "mean RGB : " << endl << avg / 3 << endl;
		}
		else
		{
			cout << avg / 3 << endl;
		}
		break;
	}
	case 3:
	{
		for (int i = 0; i < 3; i++)
			psnr[i] = PSNR64F(vYUV1[i], vYUV2[i]);
		if (printOutputInfo)
		{
			cout << "Y : " << endl << psnr[0] << endl;
			cout << "U : " << endl << psnr[1] << endl;
			cout << "V : " << endl << psnr[2] << endl;
		}
		else
		{
			cout << psnr[0] << endl;
			cout << psnr[1] << endl;
			cout << psnr[2] << endl;
		}
		break;
	}
	case 4:
	{
		for (int i = 0; i < 3; i++)
			psnr[i] = PSNR64F(vRGB1[i], vRGB2[i]);
		if (printOutputInfo)
		{
			cout << "R : " << endl << psnr[2] << endl;
			cout << "G : " << endl << psnr[1] << endl;
			cout << "B : " << endl << psnr[0] << endl;
		}
		else
		{
			cout << psnr[2] << endl;
			cout << psnr[1] << endl;
			cout << psnr[0] << endl;
		}
		break;
	}
	default:
		cout << "error: invalid mode" << endl;
		//printUsage(argv);
		return 5;
	}
}