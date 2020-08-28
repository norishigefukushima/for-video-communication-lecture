#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include "getopt.h"

#pragma comment(lib, "opencv_world320.lib")

using namespace std;
using namespace cv;

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

Scalar getMSSIM(const Mat& i1, const Mat& i2)
{
	const double C1 = 6.5025, C2 = 58.5225;
	/***************************** INITS **********************************/
	int d = CV_32F;

	Mat I1, I2;
	i1.convertTo(I1, d);           // cannot calculate on one byte large values
	i2.convertTo(I2, d);

	Mat I2_2 = I2.mul(I2);        // I2^2
	Mat I1_2 = I1.mul(I1);        // I1^2
	Mat I1_I2 = I1.mul(I2);        // I1 * I2

								   /*************************** END INITS **********************************/

	Mat mu1, mu2;   // PRELIMINARY COMPUTING
	GaussianBlur(I1, mu1, Size(11, 11), 1.5);
	GaussianBlur(I2, mu2, Size(11, 11), 1.5);

	Mat mu1_2 = mu1.mul(mu1);
	Mat mu2_2 = mu2.mul(mu2);
	Mat mu1_mu2 = mu1.mul(mu2);

	Mat sigma1_2, sigma2_2, sigma12;

	GaussianBlur(I1_2, sigma1_2, Size(11, 11), 1.5);
	sigma1_2 -= mu1_2;

	GaussianBlur(I2_2, sigma2_2, Size(11, 11), 1.5);
	sigma2_2 -= mu2_2;

	GaussianBlur(I1_I2, sigma12, Size(11, 11), 1.5);
	sigma12 -= mu1_mu2;

	///////////////////////////////// FORMULA ////////////////////////////////
	Mat t1, t2, t3;

	t1 = 2 * mu1_mu2 + C1;
	t2 = 2 * sigma12 + C2;
	t3 = t1.mul(t2);              // t3 = ((2*mu1_mu2 + C1).*(2*sigma12 + C2))

	t1 = mu1_2 + mu2_2 + C1;
	t2 = sigma1_2 + sigma2_2 + C2;
	t1 = t1.mul(t2);               // t1 =((mu1_2 + mu2_2 + C1).*(sigma1_2 + sigma2_2 + C2))

	Mat ssim_map;
	divide(t3, t1, ssim_map);      // ssim_map =  t3./t1;

	Scalar mssim = mean(ssim_map); // mssim = average of ssim map
	return mssim;
}

int main(int argc, char* argv[])
{
	if (argc < 3)
	{
		cout << "error: too few arguments" << endl;
		//printUsage(argc);
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

	double avg = 0.0;
	Scalar vAVG;

	switch (mode)
	{
	case 0:
	{
		vAVG = getMSSIM(yuv1, yuv2);
		if (printOutputInfo)
		{
			cout << "Y :" << endl << vAVG[0] << endl;
		}
		else
		{
			cout << vAVG[0] << endl;
		}
		break;
	}
	case 1:
	{
		vAVG = getMSSIM(yuv1, yuv2);
		for (int i = 0; i < 3; i++)
			avg += vAVG[i];
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
		vAVG = getMSSIM(src1, src2);
		for (int i = 0; i < 3; i++)
			avg += vAVG[i];
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
		vAVG = getMSSIM(yuv1, yuv2);
		if (printOutputInfo)
		{
			cout << "Y : " << endl << vAVG[0] << endl;
			cout << "U : " << endl << vAVG[1] << endl;
			cout << "V : " << endl << vAVG[2] << endl;
		}
		else
		{
			cout << vAVG[0] << endl;
			cout << vAVG[1] << endl;
			cout << vAVG[2] << endl;
		}
		break;
	}
	case 4:
	{
		vAVG = getMSSIM(src1, src2);
		if (printOutputInfo)
		{
			cout << "R : " << endl << vAVG[2] << endl;
			cout << "G : " << endl << vAVG[1] << endl;
			cout << "B : " << endl << vAVG[0] << endl;
		}
		else
		{
			cout << vAVG[2] << endl;
			cout << vAVG[1] << endl;
			cout << vAVG[0] << endl;
		}
		break;
	}
	default:
		cout << "error: invalid mode" << endl;
		//printUsage(argv);
		return 5;
	}

}