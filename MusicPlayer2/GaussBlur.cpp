#include "stdafx.h"
#include "GaussBlur.h"


CGaussBlur::CGaussBlur()
{
}


CGaussBlur::~CGaussBlur()
{
	if (m_pTempl != NULL)
		free(m_pTempl);
}

void CGaussBlur::SetSigma(double sigma)
{
	int i;
	m_sigma = sigma;
	m_r = (int)(m_sigma * 3 + 0.5);
	if (m_r <= 0) m_r = 1;

	//分配模板
	LPVOID pOldTempl = m_pTempl;
	m_pTempl = (double*)realloc(m_pTempl, sizeof(double) * (m_r + 1));

	//分配失败？
	if (m_pTempl == NULL)
	{
		if (pOldTempl != NULL)
			free(pOldTempl);

		return;
	}

	//计算 p[0] 灰度值为1 的模板面
	double k1 = (double)((-0.5) / (m_sigma * m_sigma));
	for (i = 0; i <= m_r; i++)
		m_pTempl[i] = exp(k1 * i * i);

	//计算模板加权总和
	double sum = m_pTempl[0];
	for (i = 1; i <= m_r; i++)
	{
		sum += (m_pTempl[i] * 2);
	}

	//归一化
	sum = (double)(1.0 / sum); //取倒数
	for (i = 0; i <= m_r; i++)
		m_pTempl[i] *= sum;
}

void CGaussBlur::Reset()
{
	m_r = -1;
	m_sigma = (double)(-1.0);
	if (m_pTempl != NULL)
	{
		free(m_pTempl);
		m_pTempl = NULL;
	}
}

void CGaussBlur::DoGaussBlur(const CImage & image_src, CImage & image_dest)
{
	if (image_src.IsNull())
		return;
	int width = image_src.GetWidth();
	int height = image_src.GetHeight();
	int bpp = image_src.GetBPP();
	if (!image_dest.IsNull())
		image_dest.Destroy();
	image_dest.Create(width, height, bpp);
	Filter(image_src.GetPixelAddress(0, height - 1), image_dest.GetPixelAddress(0, height - 1), width, height, bpp);
}

bool CGaussBlur::Filter(LPCVOID pSrc, LPVOID pDest, int width, int height, int bpp)
{
	if (pSrc == NULL || pDest == NULL)
		return false;

	//只能处理 8, 24， 32 bpp
	if (bpp != 24 && bpp != 8 && bpp != 32)
		return false;

	if (m_r < 0 || m_pTempl == NULL)
		return false;

	int absHeight = (height >= 0) ? height : (-height);
	int stride = (width * bpp + 31) / 32 * 4;
	int pixelSize = bpp / 8;

	//申请缓冲区，存储中间结果
	LPVOID pTemp = malloc(stride * absHeight);
	if (pTemp == NULL)
		return false;

	CGaussBlurThreadParams params;

	params.pSrc = (LPBYTE)pSrc;
	params.pDest = (LPBYTE)pTemp;
	params.width = width;
	params.height = absHeight;
	params.stride = stride;
	params.pixelSize = pixelSize;
	params.r = m_r;
	params.pTempl = m_pTempl;
	params.rowBegin = 0;
	params.rowEnd = absHeight;
	params.bHorz = true;

	if (bpp == 8)
		GaussBlurThreadProc8(&params);
	else
		GaussBlurThreadProc24(&params);


	params.pSrc = (LPBYTE)pTemp;
	params.pDest = (LPBYTE)pDest;
	params.bHorz = false;

	if (bpp == 8)
		GaussBlurThreadProc8(&params);
	else
		GaussBlurThreadProc24(&params);

	free(pTemp);
	return true;
}

DWORD CGaussBlur::GaussBlurThreadProc8(LPVOID lpParameters)
{
	CGaussBlurThreadParams *pInfo = (CGaussBlurThreadParams*)lpParameters;

	double result;
	int row, col, subRow, subCol, MaxVal, x, x1;
	LPINT pSubVal, pRefVal;

	if (pInfo->bHorz)
	{
		//水平方向
		pSubVal = &subCol;
		pRefVal = &col;
		MaxVal = pInfo->width - 1;
	}
	else
	{
		//垂直方向
		pSubVal = &subRow;
		pRefVal = &row;
		MaxVal = pInfo->height - 1;
	}

	LPBYTE pSrcPixel = NULL;
	LPBYTE pDestPixel = NULL;

	for (row = pInfo->rowBegin; row < pInfo->rowEnd; ++row)
	{
		for (col = 0; col < pInfo->width; ++col)
		{
			pDestPixel = pInfo->pDest + pInfo->stride * row + col;

			result = 0;

			subRow = row;
			subCol = col;

			for (x = -pInfo->r; x <= pInfo->r; x++)
			{
				//边界处理
				x1 = (x >= 0) ? x : (-x);
				*pSubVal = *pRefVal + x;
				if (*pSubVal < 0) *pSubVal = 0;
				else if (*pSubVal > MaxVal) *pSubVal = MaxVal;

				pSrcPixel = pInfo->pSrc + pInfo->stride * subRow + subCol;

				result += *pSrcPixel * pInfo->pTempl[x1];
			}
			*pDestPixel = (BYTE)result;
		}
	}
	return 0;
}

DWORD CGaussBlur::GaussBlurThreadProc24(LPVOID lpParameters)
{
	CGaussBlurThreadParams *pInfo = (CGaussBlurThreadParams*)lpParameters;

	double result[3];
	int row, col, subRow, subCol, MaxVal, x, x1;
	LPINT pSubVal, pRefVal;

	if (pInfo->bHorz)
	{
		//水平方向
		pSubVal = &subCol;
		pRefVal = &col;
		MaxVal = pInfo->width - 1;
	}
	else
	{
		//垂直方向
		pSubVal = &subRow;
		pRefVal = &row;
		MaxVal = pInfo->height - 1;
	}

	LPBYTE pSrcPixel = NULL;
	LPBYTE pDestPixel = NULL;

	for (row = pInfo->rowBegin; row < pInfo->rowEnd; ++row)
	{
		for (col = 0; col < pInfo->width; ++col)
		{
			pDestPixel = pInfo->pDest + pInfo->stride * row + pInfo->pixelSize * col;

			result[0] = 0;
			result[1] = 0;
			result[2] = 0;

			subRow = row;
			subCol = col;

			for (x = -pInfo->r; x <= pInfo->r; x++)
			{
				x1 = (x >= 0) ? x : (-x);
				*pSubVal = *pRefVal + x;

				//边界处理：Photoshop 采用的是方法1。
				//方法1：取边缘像素（图像边缘像素向内部扩散！）
				if (*pSubVal < 0) *pSubVal = 0;
				else if (*pSubVal > MaxVal) *pSubVal = MaxVal;

				//方法2：取当前像素（使得越靠近图像边缘的地方越清晰）
				/*
				if(*pSubVal < 0 || *pSubVal > MaxVal)
				*pSubVal = *pRefVal;
				*/

				pSrcPixel = pInfo->pSrc + pInfo->stride * subRow + pInfo->pixelSize * subCol;

				result[0] += pSrcPixel[0] * pInfo->pTempl[x1];
				result[1] += pSrcPixel[1] * pInfo->pTempl[x1];
				result[2] += pSrcPixel[2] * pInfo->pTempl[x1];
			}
			pDestPixel[0] = (BYTE)result[0];
			pDestPixel[1] = (BYTE)result[1];
			pDestPixel[2] = (BYTE)result[2];
		}
	}
	return 0;
}
