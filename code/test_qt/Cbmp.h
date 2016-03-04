//Cbmp.h	����Cbmp�����ؽӿ�

#ifndef _CBMP_H_
#define _CBMP_H_

#include <string>
#include <complex>

#define  _USE_MATH_DEFINES
#include <math.h>
//�����Ӧ��ϵ   [height][width]

//��������
typedef unsigned __int32	PIXEL;
typedef unsigned __int32	BYTES;
typedef unsigned __int32	BITS;
typedef unsigned char		COLOR;

//�Ҷȵ�ɫ��
typedef struct tagRGBQUAD {
	COLOR   RGBBlue;
	COLOR   RGBGreen;
	COLOR   RGBRed;
	COLOR   RGBReserved;
} RGBQUAD;

//�˲�������
#define LOW_PASS		0x01
#define MID_MEAN		0x02
#define HIGH_PASS		0x03
#define DIFF			0x04

#define PREWITT			0x01
#define SOBEL			0x02
#define LAPLACE			0x03

#define BACKGROUND		0x01
#define BG_OUTLINE		0x02
#define _BG_OUTLINE		0x03
#define BG_O_BINARY		0x04

//Ƶ���˲���
#define ILPF			0x01
#define BLPF			0x02
#define GLPF			0x03
#define TLPF			0x04

#define IHPF			0x05
#define BHPF			0x06
#define GHPF			0x07
#define THPF			0x08

const char weight[3][3] = 
{	1, 2, 1,
	2, 4, 2,
	1, 2, 1
};
const char highPass[3][3] =
{	-1, -1, -1,
	-1, +8, -1,
	-1, -1, -1
};
const char prewitt1[3][3] = 
{	-1, -1, -1,
	 0,  0,  0,
	 1,  1,  1
};
const char prewitt2[3][3] = 
{	-1, 0, 1,
	-1, 0, 1,
	-1, 0, 1
};
const char sobel1[3][3] = 
{	-1, -2, -1,
	 0,  0,  0,
	 1,  2,  1
};
const char sobel2[3][3] = 
{	-1, 0, 1,
	-2, 0, 2,
	-1, 0, 1
};
const char laplace[3][3] =
{	 0, -1, 0,
	-1, 4, -1,
	 0, -1, 0
};
//���뺯��
inline double D(double u0, double v0, double u, double v){return sqrt((u0-u)*(u0-u)+(v0-v)*(v0-v));}

//ͼ��洢����
class CbmpArray
{
public:
	//�������
	PIXEL	Height;
	PIXEL	Width;
	COLOR	**Array;	//����Ϊ[0][0]����Ϊ[height-1][width-1];

public:
	CbmpArray():Width(0), Height(0), Array(NULL){};
	~CbmpArray();
	void Init();
	void GenerateArray(PIXEL height, PIXEL width);
};
class CComplexArray
{
public:
	//����ͼ��������
	PIXEL	Height;
	PIXEL	Width;
	std::complex<double>	**Array;	//[height-1][width-1];

// 	//Ƶ����չ���صļ�¼
// 	PIXEL PreHeight;
// 	PIXEL PreWidth;

public:
	CComplexArray():Width(0), Height(0), Array(NULL){};
	~CComplexArray();
	void Init();
	void GenerateArray(PIXEL height, PIXEL width);
};

//bmp��������
class Cbmp
{
public:
	std::string ImagePath;
	RGBQUAD Color[256];
	COLOR *bmpHead;//bmpͷ����

	//��bmp���������Ҫ����Ϣ,�����,��д���ĳ�private
	BYTES	FileSize;
	BYTES	OffsetBytes;
	PIXEL	Width;
	PIXEL	Height;
	BITS	PixelBitCount;
	BYTES	ImageSize;
	bool	IsGray;
	//��ɫ+�ҶȾ���
	CbmpArray *Array_Red, *Array_Green, *Array_Blue, *Array_Gray;

	CComplexArray *ArrayOfFFT;//��һ�������ұ任�Ľ������ʼΪNULL
	double FFTMin,FFTMax;
	CComplexArray *ArrayOfDCT;//��һ�������ұ任�Ľ������ʼΪNULL

public:
	Cbmp();
	~Cbmp();


	bool Open(std::string filepath);
	bool SaveAsGray(std::string save_path, COLOR *newhead, PIXEL width, PIXEL height, const CbmpArray *aray_gray);
	bool SaveAsColor(std::string save_path, COLOR *newhead, PIXEL width, PIXEL height, CbmpArray *array_blue, CbmpArray *array_green, CbmpArray *array_red);

	//����ӵ�ͼ�������
	/***  ����任  ***/
	CbmpArray* Mirror(const CbmpArray * inputarray, bool action=false);
	CbmpArray* Offset(const CbmpArray * inputarray, int x_0, int y_0, bool mode=false, COLOR fill=0);
	CbmpArray* Crop(const CbmpArray * inputarray, int x1, int y1, int x2, int y2, COLOR fill=0);
	CbmpArray* Scale(const CbmpArray * inputarray, double nscale_x, double nscale_y, PIXEL nheight=0, PIXEL nwidth=0, bool resampling=true);
	CbmpArray* Rot(const CbmpArray * inputarray, PIXEL x_0=0, PIXEL y_0=0, double degree=M_PI_2, bool resampling=0, COLOR fill=0);
	CbmpArray* Filter(const CbmpArray * inputarray, char mode, char diff_mode=0, char diff_reserve=0, COLOR threshold=0, COLOR background=0, COLOR outline=255);
	CbmpArray* Histogram(const CbmpArray * inputarray, std::string output_path);
	void histogram(const CbmpArray * inputarray, int * hist);
	void histequalization(const CbmpArray *inputarray, CbmpArray *outputarray);
	
	/***  Ƶ��任  ***/
	CbmpArray* SwitchComplexToCbmpArray(const CComplexArray * inputarray, bool mode=0,  COLOR thresholdmin=0, COLOR thresholdmax=255);
	CComplexArray* FFT1(const CComplexArray * inputarray, PIXEL K, int W=(-1));
	CComplexArray* FFT2(const CComplexArray * inputarray);
	CComplexArray* IFFT1(const CComplexArray * inputarray, PIXEL K);
	CComplexArray* IFFT2(const CComplexArray * inputarray);
	CbmpArray* FFT(const CbmpArray * inputarray);
	CComplexArray* FFT_Filter(const CComplexArray * inputarray, double D0=0, double D1=0, int n=1, int filter=1);
	CComplexArray* DCT1(const CComplexArray * inputarray, PIXEL KWidth);
	CComplexArray* DCT2(const CComplexArray * inputarray);
	CbmpArray* DCT(const CbmpArray * inputarray);
	
private:

};

#endif // !_CBMP_H_