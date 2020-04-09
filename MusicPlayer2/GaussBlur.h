//实现高斯模糊的类，算法来自：http://www.cnblogs.com/hoodlum1980/p/4528486.html
#pragma once

class CGaussBlurThreadParams
{
public:
	int r;
	double* pTempl;
	LPBYTE pSrc;	//Src  位图的位图数据起点 （对所有线程，pSrc，pDest 是相同的）
	LPBYTE pDest;	//Dest 位图的位图数据起点

	int width;		//图像宽度
	int height;		//图像高度（已经被取绝对值）

	//处理的行范围：[rowBegin, rowEnd) ，不包括 rowEnd （每个线程不同）
	int rowBegin;
	int rowEnd;
	int stride;		//扫描行宽度（bytes）
	int pixelSize;	//像素大小 =bpp/8;

	bool bHorz;		//true-水平模糊，false-纵向模糊
};


class CGaussBlur
{
public:
	CGaussBlur();
	~CGaussBlur();

	void SetSigma(double sigma);		//设置高斯模糊半径
	void Reset();
	void DoGaussBlur(const CImage& image_src, CImage& image_dest);		//对image_src执行高斯模糊，结果保存在image_dest里

protected:
	int m_r;		//完整模板正方形的边长为 (2 * r + 1)
	double m_sigma;		//高斯半径（方差的平方根）
	double* m_pTempl;	//模板T[r+1];

	bool Filter(LPCVOID pSrc, LPVOID pDest, int width, int height, int bpp);

	DWORD WINAPI GaussBlurThreadProc8(LPVOID lpParameters);
	DWORD WINAPI GaussBlurThreadProc24(LPVOID lpParameters);
};

