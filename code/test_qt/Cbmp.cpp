////////////////////////////////////////////////////////////////////////////////
// Cbmp��ĺ������岿��
// Modifications:
// 1. �����޸�	- 2014-10-10
// 
// 
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Cbmp.h"
#include <string>
#include <fstream>


#define max(a,b)    (((a) > (b)) ? (a) : (b))
#define min(a,b)    (((a) < (b)) ? (a) : (b))
// PIXEL max(PIXEL a, PIXEL b){return(((a) > (b)) ? (a) : (b));}
// PIXEL min(PIXEL a, PIXEL b){return(((a) < (b)) ? (a) : (b));}

Cbmp::Cbmp():FileSize(0), OffsetBytes(0), Width(0), Height(0), PixelBitCount(0), ImageSize(0), IsGray(false)
{
	for(int i=0;i!=256;++i)               //��ʼ��8λ�Ҷȵ�ɫ��
	{
		Color[i].RGBBlue=i;
		Color[i].RGBGreen=i;
		Color[i].RGBRed=i;
		Color[i].RGBReserved=0;
	};
	//�ļ�ͷ
	bmpHead = new COLOR[54];
	//�������
	Array_Red = new CbmpArray;
	Array_Green = new CbmpArray;
	Array_Blue	= new CbmpArray;
	Array_Gray	= new CbmpArray;
	ArrayOfFFT	= NULL;
	ArrayOfDCT	= NULL;
}

Cbmp::~Cbmp()
{
	//����new ����
	delete bmpHead;
	delete Array_Red;
	delete Array_Green;
	delete Array_Blue;
	delete Array_Gray;
	delete ArrayOfDCT;
	delete ArrayOfFFT;

}

////////////////////////////////////////////////////////////////////////////////
//        NAME: Cbmp::Open
// DESCRIPTION: ����24λ��8λ�Ҷ�BMPͼ��
//   ARGUMENTS: std::string filepath - ͼ��·��
// USES GLOBAL: none
// MODIFIES GL: Head, FileSize, OffsetBytes, Width, Height, PixelBitCount ,ImageSize,
//				Array_Blue, Array_Green, Array_Red, Array_Gray
//     RETURNS: bool - ������־
//      AUTHOR: Hxsong
// AUTHOR/DATE: -
//							CH 2014-10-10
////////////////////////////////////////////////////////////////////////////////
bool Cbmp::Open(std::string filepath)
{
	std::fstream fp(filepath.c_str(), std::ios::in|std::ios::out|std::ios::binary);
	if(!fp){
		return 1;
	}
	//ȡ�ļ�ͷ
	fp.seekg(0, fp.beg);
	fp.read(reinterpret_cast<char *>(bmpHead), 54);

	//��ȡ�ļ�ͷ��Ϣ
	FileSize = bmpHead[2]|bmpHead[3]<<8|bmpHead[4]<<16|bmpHead[5]<<24;
	OffsetBytes = bmpHead[10]|bmpHead[11]<<8|bmpHead[12]<<16|bmpHead[13]<<24;
	Width = bmpHead[18]|bmpHead[19]<<8|bmpHead[20]<<16|bmpHead[21]<<24;
	Height = bmpHead[22]|bmpHead[23]<<8|bmpHead[24]<<16|bmpHead[25]<<24;
	PixelBitCount = bmpHead[28]|bmpHead[29]<<8;
	ImageSize = bmpHead[34]|bmpHead[35]<<8|bmpHead[36]<<16|bmpHead[37]<<24;
	
	//��ȡ��ɫ���� /�ҶȾ���

	//**********		24λλͼ		*************//
	if(bmpHead[28]==24){
		IsGray = false;
		COLOR temp[3];
		PIXEL temp_width = Width*3;
		PIXEL fix_width_3b =(temp_width&0X03) ? ((temp_width&0XFFFFFFFC)+4) : (temp_width&0XFFFFFFFC);//������ÿ���ֽ���

		//���ɾ���
		Array_Blue->GenerateArray(Height, Width);
		Array_Green->GenerateArray(Height, Width);
		Array_Red->GenerateArray(Height, Width);
		Array_Gray->GenerateArray(Height, Width);

		fp.seekg(OffsetBytes, fp.beg);
		for(PIXEL i=0;i!=Height;++i){
			for (PIXEL j=0;j!=Width;++j){
				fp.read(reinterpret_cast<char*>(temp), 3);

// 				Array_Blue->Array[Height-1-i][j] = temp[0];
// 				Array_Green->Array[Height-1-i][j] = temp[1];
// 				Array_Red->Array[Height-1-i][j] = temp[2];
// 				Array_Gray->Array[Height-1-i][j] = (temp[2]*19595 + temp[1]*38469 + temp[0]*7472) >> 16;
				Array_Blue->Array[i][j] = temp[0];
				Array_Green->Array[i][j] = temp[1];
				Array_Red->Array[i][j] = temp[2];
				Array_Gray->Array[i][j] = (temp[2]*19595 + temp[1]*38469 + temp[0]*7472) >> 16;
					//temp[2]*0.299 + temp[1]*0.587 +temp[0]*0.114;//�����Ӿ�����
			}
			fp.read(reinterpret_cast<char*>(temp), fix_width_3b - Width*3);
		}
	}

	//**********		16λλͼ		*************//

	//**********		8λ�Ҷ�ͼ		*************//
	if(bmpHead[28]==8){
		IsGray = true;
		COLOR temp[3];
		PIXEL temp_width = Width;
		PIXEL fix_width_1b =(temp_width&0X03) ? ((temp_width&0XFFFFFFFC)+4) : (temp_width&0XFFFFFFFC);

		//���ɾ���
		Array_Blue->GenerateArray(Height, Width);
		Array_Green->GenerateArray(Height, Width);
		Array_Red->GenerateArray(Height, Width);
		Array_Gray->GenerateArray(Height, Width);
		
		fp.seekg(OffsetBytes, fp.beg);//���ؿ�ʼλ��
		for(PIXEL i=0;i!=Height;++i){
			for (PIXEL j=0;j!=Width;++j){
				fp.read(reinterpret_cast<char*>(temp), 1);
				//Array_Gray->Array[Height-1-i][j] = Array_Blue->Array[Height-1-i][j] = Array_Green->Array[Height-1-i][j] = Array_Red->Array[Height-1-i][j] = temp[0];
				Array_Gray->Array[i][j] = Array_Blue->Array[i][j] = Array_Green->Array[i][j] = Array_Red->Array[i][j] = temp[0];
			}
			fp.read(reinterpret_cast<char*>(temp), fix_width_1b - Width);
		}
	}

	//**********		4λλͼ			*************//

	//**********		2λλͼ			*************//

	fp.close();
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
//        NAME: Cbmp::SaveAsColor
// DESCRIPTION: ����ΪBMP��ɫͼ��
//   ARGUMENTS: std::string save_path - ����·��
//				PIXEL width - �����ͼ����
//				PIXEL height - �����ͼ��߶�
//				COLOR **&array_b - ͼ���blue�������� height*width
//				COLOR **&array_g - ͼ���green�������� height*width
//				COLOR **&array_r - ͼ���red�������� height*width
// USES GLOBAL: bmpHead 
// MODIFIES GL: none
//     RETURNS: bool - ������־
//      AUTHOR: Hxsong
// AUTHOR/DATE: -
//							CH 2014-10-10
////////////////////////////////////////////////////////////////////////////////
bool Cbmp::SaveAsColor(std::string save_path, COLOR *newhead, PIXEL width, PIXEL height, CbmpArray *array_blue, CbmpArray *array_green, CbmpArray *array_red)
{
	char NewHead[54];
	for(int i=0;i!=54;++i){
		NewHead[i] = newhead[i];
	}
	std::fstream fp;
	fp.open(save_path.c_str(), std::ios::out|std::ios::binary);
	if(!fp){
		return 1;
	}
	//width
	NewHead[18] = 0XFF&(width);
	NewHead[19] = 0XFF&(width>>8);
	NewHead[20] = 0XFF&(width>>16);
	NewHead[21] = 0XFF&(width>>24);
	//height
	NewHead[22] = 0XFF&(height);
	NewHead[23] = 0XFF&(height>>8);
	NewHead[24] = 0XFF&(height>>16);
	NewHead[25] = 0XFF&(height>>24);
	//��ͼ���С����
	PIXEL temp_width = width*3;
	PIXEL fix_width_3b =(temp_width&0X03) ? ((temp_width&0XFFFFFFFC)+4) : (temp_width&0XFFFFFFFC);

	BYTES newImageSize = fix_width_3b * height;
	NewHead[34] = 0XFF&(newImageSize);
	NewHead[35] = 0XFF&(newImageSize>>8);
	NewHead[36] = 0XFF&(newImageSize>>16);
	NewHead[37] = 0XFF&(newImageSize>>24);

	BYTES newSize = 54 + newImageSize;
	NewHead[2] = 0XFF&newSize;
	NewHead[3] = 0XFF&(newSize>>8);
	NewHead[4] = 0XFF&(newSize>>16);
	NewHead[5] = 0XFF&(newSize>>24);
	//��ɫλ��
	NewHead[28] = 24;

	fp.write(reinterpret_cast<char*>(NewHead), 54);

	char temp[3];
	for(PIXEL i=0;i!=height;++i){
		for (PIXEL j=0;j!=width;++j){
// 			temp[0] = array_blue->Array[height-1-i][j];
// 			temp[1] = array_green->Array[height-1-i][j];
// 			temp[2] = array_red->Array[height-1-i][j];
			temp[0] = array_blue->Array[i][j];
			temp[1] = array_green->Array[i][j];
			temp[2] = array_red->Array[i][j];
			fp.write(temp,3);
		}
		fp.write(temp, fix_width_3b - temp_width);	//��λ
	}

	fp.close();

	return 0;
}

////////////////////////////////////////////////////////////////////////////////
//        NAME: Cbmp::SaveAsGray
// DESCRIPTION: ����Ϊ8ΪBMP�Ҷ�ͼ��
//   ARGUMENTS: std::string save_path - ����·��
//				PIXEL width - �����ͼ����
//				PIXEL height - �����ͼ��߶�
//				COLOR **&array_grayscale - ͼ��ĻҶȾ��� height*width
// USES GLOBAL: bmpHead 
// MODIFIES GL: none
//     RETURNS: bool - ������־
//      AUTHOR: Hxsong
// AUTHOR/DATE: -
//							CH 2014-10-10
////////////////////////////////////////////////////////////////////////////////
bool Cbmp::SaveAsGray(std::string save_path, COLOR *newhead, PIXEL width, PIXEL height, const CbmpArray *array_gray)//�Ҷȴ洢
{
	COLOR NewHead[54];
	for(int i=0;i!=54;++i){
		NewHead[i] = newhead[i];
	}
	std::fstream fp;
	fp.open(save_path.c_str(), std::ios::out|std::ios::binary);
	if(!fp){
		return 1;
	}
	//width
	NewHead[18] = 0XFF&(width);
	NewHead[19] = 0XFF&(width>>8);
	NewHead[20] = 0XFF&(width>>16);
	NewHead[21] = 0XFF&(width>>24);
	//height
	NewHead[22] = 0XFF&(height);
	NewHead[23] = 0XFF&(height>>8);
	NewHead[24] = 0XFF&(height>>16);
	NewHead[25] = 0XFF&(height>>24);

	PIXEL fix_width =(width&0X03) ? ((width&0XFFFFFFFC)+4) : (width&0XFFFFFFFC);
	BYTES newImageSize = fix_width * height;		//ʵ����Ҫ�����ش洢�ռ�
	NewHead[34] = 0XFF&(newImageSize);
	NewHead[35] = 0XFF&(newImageSize>>8);
	NewHead[36] = 0XFF&(newImageSize>>16);
	NewHead[37] = 0XFF&(newImageSize>>24);

	BYTES newSize = 54 + 1024 + newImageSize;
	NewHead[2] = 0XFF&newSize;
	NewHead[3] = 0XFF&(newSize>>8);
	NewHead[4] = 0XFF&(newSize>>16);
	NewHead[5] = 0XFF&(newSize>>24);
	//��ɫλ��
	NewHead[28] = 8;

	fp.write(reinterpret_cast<char*>(NewHead), 54);

	//�����ɫ��
	fp.write(reinterpret_cast<char*>(Color), 1024);

	char temp[3];
	for(PIXEL i=0;i!=height;++i){
		for (PIXEL j=0;j!=width;++j){
			//temp[0] = array_gray->Array[height-1-i][j];
			temp[0] = array_gray->Array[i][j];
			fp.write(temp, 1);
		}
		fp.write(temp,fix_width - width);
	}
	fp.close();
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
//        NAME: Cbmp::Mirror
// DESCRIPTION: ����ͼ��
//   ARGUMENTS: const CbmpArray * inputarray - ��Ҫ����任�ľ���
//				bool action - ����ʽ��0-���Ҿ���, 1-���¾���
// USES GLOBAL: none 
// MODIFIES GL: none
//     RETURNS: CbmpArray* - ���� new CbmpArray ָ�룬ָ������֮��ľ���
//      AUTHOR: Hxsong
// AUTHOR/DATE: -
//							CH 2014-10-22
////////////////////////////////////////////////////////////////////////////////
CbmpArray* Cbmp::Mirror(const CbmpArray * inputarray, bool action/* =false */)
{
	CbmpArray *ret = new CbmpArray;
	ret->GenerateArray(inputarray->Height, inputarray->Width);
	if(!action){
		for(PIXEL i=0;i!=ret->Height;++i){
			for (PIXEL j=0;j!=ret->Width;++j){
				ret->Array[i][j] = inputarray->Array[i][inputarray->Width - j -1];
			}
		}
	}
	else{
		for(PIXEL i=0;i!=ret->Height;++i){
			for (PIXEL j=0;j!=ret->Width;++j){
				ret->Array[i][j] = inputarray->Array[inputarray->Height - i -1][j];
			}
		}
	}
	return ret;
}

////////////////////////////////////////////////////////////////////////////////
//        NAME: Cbmp::Offset
// DESCRIPTION: ͼ��ƽ�ƣ���С���䣬�հ׳����
//   ARGUMENTS: const CbmpArray * inputarray - ��Ҫƽ�Ʊ任�ľ���
//				PIXEL x	- ƽ�Ƶ�x���꣬��Ϊ������Ϊ����
//				PIXEL y - ƽ�Ƶ�y���꣬��Ϊ���£���Ϊ����
//				bool mode	- ƽ��ģʽ�� 0-��С���䣬 1-Ŀ���С�ı䣬����ԭͼ��С
//				COLOR fill - �հ׵����Ҷ�ֵ
// USES GLOBAL: none 
// MODIFIES GL: none
//     RETURNS: CbmpArray* - ���� new CbmpArray ָ�룬ָ��ƽ��֮��ľ�����ʹ���߸�������
//      AUTHOR: Hxsong
// AUTHOR/DATE: -
//							CH 2014-10-22
////////////////////////////////////////////////////////////////////////////////
CbmpArray* Cbmp::Offset(const CbmpArray * inputarray, int x_0, int y_0, bool mode/* =false */, COLOR fill/* =0 */)
{
	CbmpArray *ret	= new CbmpArray;
	PIXEL	nHeight, nWidth;
	if(mode){
		nWidth	= inputarray->Width + abs(x_0);
		nHeight	= inputarray->Height + abs(y_0);
	}
	else{
		nWidth	= inputarray->Width;
		nHeight	= inputarray->Height;
	}
	ret->GenerateArray(nHeight, nWidth);
	for(PIXEL i=0;i!=ret->Height;++i){
		for (PIXEL j=0;j!=ret->Width;++j){
			PIXEL x_1,y_1;
			if(y_0<=0&&mode){y_1 = i;}else{y_1 = i - y_0;}
			if(x_0<=0&&mode){x_1 = j;}else{x_1 = j - x_0;}
			
			if((y_1)<0 || (x_1)<0 || (y_1)>(inputarray->Height - 1) || (x_1)>(inputarray->Width-1)){
				ret->Array[i][j] = fill;
			}
			else{
				ret->Array[i][j] = inputarray->Array[y_1][x_1];
			}
		}
	}
	return ret;
}

////////////////////////////////////////////////////////////////////////////////
//        NAME: Cbmp::Crop
// DESCRIPTION: ͼ����У��հ״����
//   ARGUMENTS: const CbmpArray * inputarray - ���б任�ľ���
//				int x1 - ���½ǵ�x����
//				int x2 - ���Ͻǵ�x����
//				int y1 - ���½�y����
//				int y2 - ���Ͻ�y����
//				COLOR fill - �հ׵����Ҷ�ֵ
// USES GLOBAL: none 
// MODIFIES GL: none
//     RETURNS: CbmpArray* - ���� new CbmpArray ָ�룬ָ�����֮��ľ�����ʹ���߸�������
//      AUTHOR: Hxsong
// AUTHOR/DATE: -
//							CH 2014-10-22
////////////////////////////////////////////////////////////////////////////////
CbmpArray* Cbmp::Crop(const CbmpArray * inputarray, int x1, int y1, int x2, int y2, COLOR fill/* =0 */)
{
	if(x2-x1<=0 || y2-y1<=0){
		return NULL;
	}
	CbmpArray *ret = new CbmpArray;
	ret->GenerateArray(y2-y1+1, x2-x1+1);
	for(PIXEL i=0;i!=ret->Height;++i){
		for (PIXEL j=0;j!=ret->Width;++j){
			if((i+y1)<0 || (j+x1)<0 || (i+y1)>(inputarray->Height - 1) || (j+x1)>(inputarray->Width - 1)){
				ret->Array[i][j] = fill;
			}
			else{
				ret->Array[i][j] = inputarray->Array[i+y1][j+x1];
			}
		}
	}
	return ret;
}

////////////////////////////////////////////////////////////////////////////////
//        NAME: Cbmp::Scale
// DESCRIPTION: ͼ�����ţ����ò�ֵ
//   ARGUMENTS: const CbmpArray * inputarray - �������
//				double nscale - ���ű���
//				PIXEL nheight=0, PIXEL nwidth=0 - ����Ϊ0ʱ���˵�ֱ�����ɱ���
//				bool resampling - ��ֵ��ʽ�� 0-����ڲ�ֵ   1-˫���Բ�ֵ
// USES GLOBAL: none 
// MODIFIES GL: none
//     RETURNS: CbmpArray* - ���� new CbmpArray ָ�룬ָ�����֮��ľ�����ʹ���߸�������
//      AUTHOR: Hxsong
// AUTHOR/DATE: -
//							CH 2014-10-22
////////////////////////////////////////////////////////////////////////////////
CbmpArray* Cbmp::Scale(const CbmpArray * inputarray, double nscale_x, double nscale_y, PIXEL nheight/* =0 */, PIXEL nwidth/* =0 */, bool resampling/* =false */)
{

	COLOR **const &f	= inputarray->Array;
	double scale_x, scale_y;
	//�Ա�������
	PIXEL newWidth, newHeight;
	//���ȼ���ֱ�ӳ���
	if(nheight!=0&&nwidth!=0){
		newWidth	= nwidth;
		newHeight	= nheight;
		scale_x = static_cast<double>(newWidth)/inputarray->Width;
		scale_y = static_cast<double>(newHeight)/inputarray->Height;
	}
	else{
		if(nscale_x<=0 || nscale_y<=0){
			return NULL;
		}
		scale_x = nscale_x;
		scale_y = nscale_y;
		newWidth	= static_cast<PIXEL>(floor(inputarray->Width*scale_x));	//ͼ����Сȡ��
		newHeight	= static_cast<PIXEL>(floor(inputarray->Height*scale_y));
	}

	//ֱ�ӻ���
	CbmpArray *ret = new CbmpArray;
	ret->GenerateArray(newHeight, newWidth);
	PIXEL x(0), y(0);
	double u(0), v(0);//(x+u,y+v)��f[y+v][x+u]
	for(PIXEL i=0;i!=ret->Height;++i){
		for(PIXEL j=0;j!=ret->Width;++j){
			//����ڲ�ֵ,�������뵽�����
			if(!resampling){
				//(x,y)
				x = static_cast<PIXEL>(ceil((j)/scale_x - 0.5));
				y = static_cast<PIXEL>(ceil((i)/scale_y - 0.5));
				if((y)>inputarray->Height-1){ y=y-1;}
				if((x)>inputarray->Width-1){ x=x-1;}
				ret->Array[i][j] = f[y][x];
			}
			//˫���Բ�ֵ
			else{
				x = static_cast<PIXEL>(floor(j/scale_x));
				y = static_cast<PIXEL>(floor(i/scale_y));
				//��Ե��������ڲ�ֵ����
				if(i==0 || j==0 || i==ret->Height-1 || j==ret->Width-1){
					//(x,y)
					x = static_cast<PIXEL>(ceil((j)/scale_x - 0.5));
					y = static_cast<PIXEL>(ceil((i)/scale_y - 0.5));
					if((y)>inputarray->Height-1){ y=y-1;}
					if((x)>inputarray->Width-1){ x=x-1;}
					ret->Array[i][j] = f[y][x];
				}
				else{
					if((y+1)>inputarray->Height-1){ y=y-1;}
					if((x+1)>inputarray->Width-1){ x=x-1;}
					//(x+u,y+v)		
					u = j/scale_x - x;
					v = i/scale_y - y;
					ret->Array[i][j] = static_cast<COLOR>(f[y][x]*(1-u)*(1-v) + f[y][x+1]*(u)*(1-v) + f[y][x+1]*(1-u)*(v) + f[y+1][x+1]*(u)*(v));
				}
			}
		}
	}
	return ret;
}

////////////////////////////////////////////////////////////////////////////////
//        NAME: Cbmp::Rot
// DESCRIPTION: ͼ����ת���հ״���ֵ
//   ARGUMENTS: const CbmpArray * inputarray - �������
//				PIXEL x_0, PIXEL y_0 - ��ת����
//				double beta - ��ת�Ƕ�
//				bool resampling - ��ֵ��ʽ�� 0-����ڲ�ֵ   1-˫���Բ�ֵ
// USES GLOBAL: none 
// MODIFIES GL: none
//     RETURNS: CbmpArray* - ���� new CbmpArray ָ�룬ָ�����֮��ľ�����ʹ���߸�������
//      AUTHOR: Hxsong
// AUTHOR/DATE: -
//							CH 2014-10-22
////////////////////////////////////////////////////////////////////////////////
CbmpArray* Cbmp::Rot(const CbmpArray * inputarray, PIXEL x_0/* =0 */, PIXEL y_0/* =0 */, double degree/* =M_PI_2 */, bool resampling/* =0 */, COLOR fill/* =0 */)
{
	if(x_0<0 || x_0>inputarray->Width-1 || y_0<0 || y_0>inputarray->Height){
		return NULL;
	}
	COLOR **const &f	= inputarray->Array;//�򻯼������

	double beta = ((degree/M_PI)-floor(degree/M_PI))*M_PI;//Լ��beta
	//4����ȷ���¾����С
	__int32 p1x(0),p1y(0),p2x,p2y,p3x,p3y,p4x,p4y;
	p2x = static_cast<__int32>(floor(cos(beta)*inputarray->Width - sin(beta)*0));
	p2y = static_cast<__int32>(floor(sin(beta)*inputarray->Width + cos(beta)*0));
	p3x = static_cast<__int32>(floor(cos(beta)*inputarray->Width - sin(beta)*inputarray->Height));
	p3y = static_cast<__int32>(floor(sin(beta)*inputarray->Width + cos(beta)*inputarray->Height));
	p4x = static_cast<__int32>(floor(cos(beta)*0 - sin(beta)*inputarray->Height));
	p4y = static_cast<__int32>(floor(sin(beta)*0 + cos(beta)*inputarray->Height));


	PIXEL newWidth	= max(p1x,max(p2x,max(p3x,p4x))) - min(p1x,min(p2x,min(p3x,p4x)));
	PIXEL newHeight	= max(p1y,max(p2y,max(p3y,p4y))) - min(p1y,min(p2y,min(p3y,p4y)));

	CbmpArray *ret = new CbmpArray;
	ret->GenerateArray(newHeight, newWidth);
	PIXEL x(0), y(0);
	double u(0), v(0);//(x+u,y+v)��f[y+v][x+u]
	double xOrigin,yOrigin;
	__int32 iOrigin,jOrigin;
	for (PIXEL i=0;i!=ret->Height;++i){
		for (PIXEL j=0;j!=ret->Width;++j){
			//����ӳ�䣨j��i����ԭ���꣨xOrigin,yOrigin������ֵ����
			jOrigin	= j + p4x + 1;
			iOrigin	= i;
			xOrigin	= jOrigin*cos(beta)/(cos(beta)*cos(beta) + sin(beta)*sin(beta)) +\
				      iOrigin*sin(beta)/(cos(beta)*cos(beta) + sin(beta)*sin(beta));
			yOrigin	= (-1)*jOrigin*sin(beta)/(cos(beta)*cos(beta) + sin(beta)*sin(beta)) +\
					   iOrigin*cos(beta)/(cos(beta)*cos(beta) + sin(beta)*sin(beta));
			//ӳ�䵽ԭͼ��ĵ����
			if(xOrigin<0 || yOrigin<0 || xOrigin>inputarray->Width-1 || yOrigin>inputarray->Height-1){
				ret->Array[i][j] = fill;
			}
			//ӳ�䵽ԭͼ�Ľ��в�ֵ����
			else{
				if(resampling){//˫�����ڲ�
					x	= static_cast<PIXEL>(floor(xOrigin));
					u	= xOrigin - x;
					y	= static_cast<PIXEL>(floor(yOrigin));
					v	= yOrigin - y;
					if((y+1)>inputarray->Height-1){ y=y-1;}
					if((x+1)>inputarray->Width-1){ x=x-1;}
					ret->Array[i][j] = static_cast<COLOR>(f[y][x]*(1-u)*(1-v) + f[y][x+1]*(u)*(1-v) + f[y][x+1]*(1-u)*(v) + f[y+1][x+1]*(u)*(v));
				}
				else{//����ڲ�ֵ
					x = static_cast<PIXEL>(ceil(xOrigin - 0.5));
					y = static_cast<PIXEL>(ceil(yOrigin - 0.5));
					if((y)>inputarray->Height-1){ y=y-1;}
					if((x)>inputarray->Width-1){ x=x-1;}
					ret->Array[i][j] = f[y][x];
				}
			}
		}
	}
	//ƽ����ת����
	CbmpArray *retTemp = Offset(ret, x_0, y_0, true, fill);
	delete ret;
	ret = retTemp;

	return ret;
}

////////////////////////////////////////////////////////////////////////////////
//        NAME: Cbmp::Filter
// DESCRIPTION: �����˲�
//   ARGUMENTS: const CbmpArray * inputarray - �������
//				char mode - �˲�ģʽ����ͨ����ͨ...
//				char diff_mode - ΢���˲�ʹ�ã�ȷ������
//				char diff_reserve - ΢���˲�ʹ�ã�ȷ��������ʽ
//				COLOR threshold - ΢����ֵ
//				COLOR background - �����Ҷ�		����ȡ�Ჿ�ֲ���
//				COLOR outline - �����Ҷ�
// USES GLOBAL: none 
// MODIFIES GL: none
//     RETURNS: CbmpArray* - ���� new CbmpArray ָ��
//      AUTHOR: Hxsong
// AUTHOR/DATE: -
//							CH 2014-10-24
////////////////////////////////////////////////////////////////////////////////
CbmpArray* Cbmp::Filter(const CbmpArray * inputarray, char mode, char diff_mode/* =0 */, char diff_reserve/* =0 */, COLOR threshold/* =0 */, COLOR background/* =0 */, COLOR outline/* =255 */)
{
	COLOR **const &f = inputarray->Array;
	CbmpArray *ret = new CbmpArray;
	ret->GenerateArray(inputarray->Height,inputarray->Width);

	/* ��ͨ�˲� */
	if (mode == LOW_PASS){
		for(PIXEL i=0;i!=ret->Height;++i){
			for(PIXEL j=0;j!=ret->Width;++j){
			//��Ե����:ԭֵ
				if(i==0 || i==ret->Height-1 || j==0 || j==ret->Width-1){
					ret->Array[i][j] = f[i][j];
				}
				else{
					ret->Array[i][j] =	(f[i-1][j-1]*weight[0][0]	+ f[i-1][j]*weight[0][1]	+ f[i-1][j+1]*weight[0][2] +\
										f[i][j-1]*weight[1][0]		+ f[i][j]*weight[1][1]		+ f[i][j+1]*weight[1][2] +\
										f[i+1][j-1]*weight[2][0]	+ f[i+1][j]*weight[2][1]	+ f[i+1][j+1]*weight[2][2])>>4;
				}
			}
		}
	}

	/* ��ֵ�˲� */
	if (mode == MID_MEAN){
		COLOR temp[9];
		COLOR exc(0);
		for(PIXEL i=0;i!=ret->Height;++i){
			for(PIXEL j=0;j!=ret->Width;++j){
				//��Ե����:ԭֵ
				if(i==0 || i==ret->Height-1 || j==0 || j==ret->Width-1){
					ret->Array[i][j] = f[i][j];
				}
				else{
					//����3x3
					for(int x=0;x!=3;++x){
						for(int y=0;y!=3;++y){
							temp[3*x + y] = f[i-1+x][j-1+y];
						}
					}
					for(int iSoft=0;iSoft!=9;++iSoft){
						for(int jSort=0;jSort!=9-iSoft;++jSort){
							if(temp[8-iSoft]<temp[jSort]){
								exc = temp[8-iSoft];
								temp[8-iSoft] = temp[jSort];
								temp[jSort] = exc;
							}
						}
					}
					ret->Array[i][j] = temp[4];//��ֵΪtemp[4]
				}
			}
		}
	}

	/* ������ͨ�˲� */
	if (mode == HIGH_PASS){
		for(PIXEL i=0;i!=ret->Height;++i){
			for(PIXEL j=0;j!=ret->Width;++j){
				//��Ե����:ԭֵ
				if(i==0 || i==ret->Height-1 || j==0 || j==ret->Width-1){
					ret->Array[i][j] = f[i][j];
				}
				else{
					double temp  =	(	f[i-1][j-1]*highPass[0][0]	+ f[i-1][j]*highPass[0][1]	+ f[i-1][j+1]*highPass[0][2] +\
										f[i][j-1]*highPass[1][0]	+ f[i][j]*highPass[1][1]	+ f[i][j+1]*highPass[1][2] +\
										f[i+1][j-1]*highPass[2][0]	+ f[i+1][j]*highPass[2][1]	+ f[i+1][j+1]*highPass[2][2]);
					//��������
					temp<=0?ret->Array[i][j]=0 : (temp>255? ret->Array[i][j]=255 : ret->Array[i][j]=static_cast<COLOR>(temp));	
				}
			}
		}
	}

	/* ΢���˲� */
	if (mode == DIFF){
		/* PREWITT */
		if (diff_mode == PREWITT){
			for(PIXEL i=0;i!=ret->Height;++i){
				for(PIXEL j=0;j!=ret->Width;++j){
					//��Ե����:ԭֵ
					if(i==0 || i==ret->Height-1 || j==0 || j==ret->Width-1){
						ret->Array[i][j] = f[i][j];
					}
					else{
						//ˮƽ�����ݶ�
						double temp1  =	(	f[i-1][j-1]*prewitt1[0][0]	+ f[i-1][j]*prewitt1[0][1]	+ f[i-1][j+1]*prewitt1[0][2] +\
											f[i][j-1]*prewitt1[1][0]	+ f[i][j]*prewitt1[1][1]	+ f[i][j+1]*prewitt1[1][2] +\
											f[i+1][j-1]*prewitt1[2][0]	+ f[i+1][j]*prewitt1[2][1]	+ f[i+1][j+1]*prewitt1[2][2]);
						double temp2  =	(	f[i-1][j-1]*prewitt2[0][0]	+ f[i-1][j]*prewitt2[0][1]	+ f[i-1][j+1]*prewitt2[0][2] +\
											f[i][j-1]*prewitt2[1][0]	+ f[i][j]*prewitt2[1][1]	+ f[i][j+1]*prewitt2[1][2] +\
											f[i+1][j-1]*prewitt2[2][0]	+ f[i+1][j]*prewitt2[2][1]	+ f[i+1][j+1]*prewitt2[2][2]);
						double temp	=sqrt(temp1*temp1 + temp2*temp2);
						//��������
						temp<=0?ret->Array[i][j]=0 : (temp>255? ret->Array[i][j]=255 : ret->Array[i][j]=static_cast<COLOR>(temp));
					}
				}
			}
		}
		/* SOBEL */
		if (diff_mode == SOBEL){
			for(PIXEL i=0;i!=ret->Height;++i){
				for(PIXEL j=0;j!=ret->Width;++j){
					//��Ե����:ԭֵ
					if(i==0 || i==ret->Height-1 || j==0 || j==ret->Width-1){
						ret->Array[i][j] = f[i][j];
					}
					else{
						//ˮƽ�����ݶ�
						double temp1  =	(	f[i-1][j-1]*sobel1[0][0]	+ f[i-1][j]*sobel1[0][1]	+ f[i-1][j+1]*sobel1[0][2] +\
							f[i][j-1]*sobel1[1][0]	+ f[i][j]*sobel1[1][1]	+ f[i][j+1]*sobel1[1][2] +\
							f[i+1][j-1]*sobel1[2][0]	+ f[i+1][j]*sobel1[2][1]	+ f[i+1][j+1]*sobel1[2][2]);
						double temp2  =	(	f[i-1][j-1]*sobel2[0][0]	+ f[i-1][j]*sobel2[0][1]	+ f[i-1][j+1]*sobel2[0][2] +\
							f[i][j-1]*sobel2[1][0]	+ f[i][j]*sobel2[1][1]	+ f[i][j+1]*sobel2[1][2] +\
							f[i+1][j-1]*sobel2[2][0]	+ f[i+1][j]*sobel2[2][1]	+ f[i+1][j+1]*sobel2[2][2]);
						double temp	=sqrt(temp1*temp1 + temp2*temp2);
						//��������
						temp<=0?ret->Array[i][j]=0 : (temp>255? ret->Array[i][j]=255 : ret->Array[i][j]=static_cast<COLOR>(temp));
					}
				}
			}
		}
		/* LAPLACE */
		if (diff_mode == LAPLACE){
			for(PIXEL i=0;i!=ret->Height;++i){
				for(PIXEL j=0;j!=ret->Width;++j){
					//��Ե����:ԭֵ
					if(i==0 || i==ret->Height-1 || j==0 || j==ret->Width-1){
						ret->Array[i][j] = f[i][j];
					}
					else{
						double temp  =	(	f[i-1][j-1]*laplace[0][0]	+ f[i-1][j]*laplace[0][1]	+ f[i-1][j+1]*laplace[0][2] +\
							f[i][j-1]*laplace[1][0]	+ f[i][j]*laplace[1][1]	+ f[i][j+1]*laplace[1][2] +\
							f[i+1][j-1]*laplace[2][0]	+ f[i+1][j]*laplace[2][1]	+ f[i+1][j+1]*laplace[2][2]);
						//��������
						temp<=0?ret->Array[i][j]=0 : (temp>255? ret->Array[i][j]=255 : ret->Array[i][j]=static_cast<COLOR>(temp));	
					}
				}
			}
		}
	}
	//
	return ret;
}
void Cbmp::histogram(const CbmpArray *inputarray,int *hist)
{
	for(int i=0;i<256;i++)
	{
		hist[i]=0;
	}
	for(PIXEL i=0;i<inputarray->Height;i++)
		for(PIXEL j=0;j<inputarray->Width;j++)
		{
			int a = inputarray->Array[i][j];
			++hist[a];
		}
}
void Cbmp::histequalization(const CbmpArray *inputarray, CbmpArray *outputarray)
{
	int count[256];

	histogram(inputarray,count);
	// �Ҷ�ӳ���
	unsigned char	bMap[256];
	// ����Ҷ�ӳ���
	for (int i = 0; i < 256; i++)
	{
		// ��ʼΪ0
		int lTemp = 0;		
		for (int j = 0; j <= i ; j++)
		{
			lTemp += count[j];
		}	
		// �����Ӧ���»Ҷ�ֵ
		bMap[i] = (unsigned char) (lTemp * 255 / inputarray->Height / inputarray->Width);
	}

	for(PIXEL i = 0; i < inputarray->Height; i++){
		for(PIXEL j = 0; j < inputarray->Width; j++){
			outputarray->Array[i][j] = bMap[inputarray->Array[i][j]];              // �����µĻҶ�ֵ	
		}
	}
}
CbmpArray* Cbmp::Histogram(const CbmpArray * inputarray, std::string output_path)
{
	CbmpArray *ret = new CbmpArray;
	ret->GenerateArray(inputarray->Height, inputarray->Width);

	histequalization(inputarray, ret);

	//��ֱ��ͼ
	int oldHistogram[256], newHistogram[256];
	histogram(inputarray, oldHistogram);
	histogram(ret, newHistogram);

	CbmpArray *OldHis = new CbmpArray;
	OldHis->GenerateArray(100, 256);
	CbmpArray *NewHis = new CbmpArray;
	NewHis->GenerateArray(100, 256);
	for(PIXEL i=0; i!=100; ++i){
		for(PIXEL j=0; j!=256; ++j){
			OldHis->Array[i][j] = 0;
			NewHis->Array[i][j] = 0;
		}
	}
	int oldMax(0), newMax(0);
	for(PIXEL i=0; i!=256; ++i){
		oldMax = (oldHistogram[i]<oldMax?oldMax:oldHistogram[i]);
		newMax = (newHistogram[i]<newMax?newMax:newHistogram[i]);
	}

	for(PIXEL j=0; j!=255; ++j){
		for(PIXEL i=0; i!=(oldHistogram[j]*100/oldMax); ++i){
			OldHis->Array[i][j] = 255;
		}
	}
	for(PIXEL j=0; j!=255; ++j){
		for(PIXEL i=0; i!=(newHistogram[j]*100/newMax); ++i){
			NewHis->Array[i][j] = 255;
		}
	}
	SaveAsGray(output_path+"OldHistogram.bmp", bmpHead, 256, 100, OldHis);
	SaveAsGray(output_path+"NewHistogram.bmp", bmpHead, 256, 100, NewHis);

	delete OldHis,NewHis;
	return ret;
}

////////////////////////////////////////////////////////////////////////////////
//        NAME: Cbmp::CbmpArray* SwitchComplexToCbmpArray(const CComplexArray * inputarray, COLOR min=0, COLOR max=255);
// DESCRIPTION: ������Ҷ�������ұ任���ɵĸ��������һ��Ϊͼ����ʾ��Χ
//   ARGUMENTS: const CComplexArray * inputarray - ������� COLOR min=0, COLOR max=255 ��һ���ķ�Χ������
// USES GLOBAL: none
// MODIFIES GL: none
//     RETURNS: CbmpArray* - ���� new CbmpArray
//      AUTHOR: Hxsong
// AUTHOR/DATE: -
//							CH 2014-12-09
////////////////////////////////////////////////////////////////////////////////
CbmpArray* Cbmp::SwitchComplexToCbmpArray(const CComplexArray * inputarray, bool mode/* =0 */, COLOR thresholdmin/* =0 */, COLOR thresholdmax/* =255 */)
{
	CbmpArray *ret = new CbmpArray;
	ret->GenerateArray(inputarray->Height, inputarray->Width);

	//ȡ�������һ���� thresholdmin-thresholdmax  --> ret
	double DFT_Min(255),DFT_Max(0); //��ʼ�������Сֵ --> �����֮�������޵�ֵ
	for(PIXEL i=0;i!=inputarray->Height;++i){
		for(PIXEL j=0;j!=inputarray->Width;++j){
			double testd = abs(inputarray->Array[i][j]);
			double testb = log(1+testd);
			DFT_Min = min(DFT_Min, testb);
			DFT_Max = max(DFT_Max, testb);
			ret->Array[i][j] = testb;
		}
	}
	if(mode){
		DFT_Min = FFTMin;
		DFT_Max = FFTMax;
	}
	//��һ��
	double DFT_Scale = (thresholdmax-thresholdmin)/(DFT_Max-DFT_Min);
	for(PIXEL i=0;i!=ret->Height;++i){
		for(PIXEL j=0;j!=ret->Width;++j){
			int fix = (ret->Array[i][j]-DFT_Min)*DFT_Scale + thresholdmin;
			COLOR testc =  static_cast<COLOR>(fix>=255?255: fix<=0?0:fix);
			ret->Array[i][j] = testc;
		}
	}

	return ret;
}


////////////////////////////////////////////////////////////////////////////////
//        NAME: Cbmp::FFT1
// DESCRIPTION: һάFFT �任,����ľ������Ϊ 2��4��8��16��32��64��128��256��512--��2��ڤ
//   ARGUMENTS: const CbmpArray * inputarray - ������� width*1,  PIXEL K - �����Ĳ���, int W=-1 - �����ӵ�������Ϊ���ұ任ʹ��
// USES GLOBAL: none 
// MODIFIES GL: none
//     RETURNS: CbmpArray* - ���� new CbmpArray ָ�룬ָ������ת��֮���Ƶ��ͼ
//      AUTHOR: Hxsong
// AUTHOR/DATE: -
//							CH 2014-12-04
////////////////////////////////////////////////////////////////////////////////
CComplexArray* Cbmp::FFT1(const CComplexArray * inputarray, PIXEL K, int W/* =(-1) */)
{
	if(inputarray->Height==0 || inputarray->Width==0)
		return NULL;
	CComplexArray *ret = new CComplexArray;
	ret->GenerateArray(inputarray->Height, inputarray->Width);
	PIXEL N1 = pow(2, K);
	PIXEL N2(inputarray->Width/2);
	
	//N!=2  �ݹ�ֽ�
	if(N1!=2){
		CComplexArray *Xeven = new CComplexArray; //ż����
		CComplexArray *Xodd = new CComplexArray; //������
		Xeven->GenerateArray(1, N2);
		Xodd->GenerateArray(1, N2);
		CComplexArray *Xeven1; //ż����
		CComplexArray *Xodd1; //������
		//��inputarray�ֽ�Ϊż���к�������
		for(PIXEL i=0; i!=N2; ++i){
			Xeven->Array[0][i] = inputarray->Array[0][i*2];
			Xodd->Array[0][i] = inputarray->Array[0][i*2+1];
		}
		//���صݹ����
		Xeven1 = FFT1(Xeven, K-1);
		Xodd1  = FFT1(Xodd, K-1);
		//д���ݹ鹫ʽ
		for(PIXEL i=0; i!=N2; ++i){
			double W_N_beta(W*2*M_PI*i/N1);		//W
			std::complex<double> W_N_K(cos(W_N_beta), sin(W_N_beta));
			ret->Array[0][i]	= Xeven1->Array[0][i] + W_N_K*Xodd1->Array[0][i];
			ret->Array[0][i+N2] = Xeven1->Array[0][i] - W_N_K*Xodd1->Array[0][i];
		}
		delete Xeven;
		delete Xodd;
		delete Xeven1;
		delete Xodd1;
	}
	//N = 2
	else{
		double W_N_beta(W*2*M_PI*1/N1);
		std::complex<double> W_N_K(cos(W_N_beta), sin(W_N_beta));
		ret->Array[0][0] = inputarray->Array[0][0] + inputarray->Array[0][1];
		ret->Array[0][1] = inputarray->Array[0][0] + inputarray->Array[0][1]*W_N_K;
	}
	return ret;
}

////////////////////////////////////////////////////////////////////////////////
//        NAME: Cbmp::IFFT1
// DESCRIPTION: һάFFT ��任
////////////////////////////////////////////////////////////////////////////////
//std::complex<double> Cbmp::FFT1(const CComplexArray * inputarray, PIXEL K)
CComplexArray* Cbmp::IFFT1(const CComplexArray * inputarray, PIXEL K)
{
	CComplexArray *ret = new CComplexArray;
	ret->GenerateArray(inputarray->Height, inputarray->Width);
	PIXEL N1 = pow(2, K);
	PIXEL N2(inputarray->Width/2);

	//N!=2  �ݹ�ֽ�
	if(N1!=2){
		CComplexArray *Xeven = new CComplexArray; //ż����
		CComplexArray *Xodd = new CComplexArray; //������
		Xeven->GenerateArray(1, N2);
		Xodd->GenerateArray(1, N2);
		CComplexArray *Xeven1; //ż���е����
		CComplexArray *Xodd1 ; //�����е����
		//��inputarray�ֽ�Ϊż���к�������
		for(PIXEL i=0; i!=N2; ++i){
			Xeven->Array[0][i] = inputarray->Array[0][i*2];
			Xodd->Array[0][i] = inputarray->Array[0][i*2+1];
		}
		//���صݹ����
		Xeven1 = IFFT1(Xeven, K-1);
		Xodd1  = IFFT1(Xodd, K-1);
		//д���ݹ鹫ʽ
		for(PIXEL i=0; i!=N2; ++i){
			double W_N_beta(2*M_PI*i/N1);		//W�ķ���
			std::complex<double> W_N_K(cos(W_N_beta), sin(W_N_beta));
			ret->Array[0][i]	= 0.5*(Xeven1->Array[0][i] + W_N_K*Xodd1->Array[0][i]);
			ret->Array[0][i+N2] = 0.5*(Xeven1->Array[0][i] - W_N_K*Xodd1->Array[0][i]);
		}
		delete Xeven;
		delete Xodd;
		delete Xeven1;
		delete Xodd1;
	}
	//N = 2
	else{
		double W_N_beta(2*M_PI*1/N1);
		std::complex<double> W_N_K(cos(W_N_beta), sin(W_N_beta));
		ret->Array[0][0] = 0.5*(inputarray->Array[0][0] + inputarray->Array[0][1]);
		ret->Array[0][1] = 0.5*(inputarray->Array[0][0] + inputarray->Array[0][1]*W_N_K);
	}
	return ret;
}

////////////////////////////////////////////////////////////////////////////////
//        NAME: Cbmp::FFT2
// DESCRIPTION: FFT2 �任
//   ARGUMENTS: const CbmpArray * inputarray - �������
// USES GLOBAL: none 
// MODIFIES GL: none
//     RETURNS: CbmpArray* - ���� new CbmpArray ָ�룬ָ������ת��֮���Ƶ��ͼ
//      AUTHOR: Hxsong
// AUTHOR/DATE: -
//							CH 2014-10-22
////////////////////////////////////////////////////////////////////////////////
CComplexArray* Cbmp::FFT2(const CComplexArray * inputarray)
{
	CComplexArray *ret = new CComplexArray;
	ret->GenerateArray(inputarray->Height, inputarray->Width);

	PIXEL KWidth(0), KHeight(0);
	//ȡ��2�Ĵ���
	while (true)
	{
		int count(0);
		pow(2, KWidth)	< ret->Width	? ++KWidth  : ++count;
		pow(2, KHeight)	< ret->Height	? ++KHeight : ++count;
		if (count==2){break;}
	}

	//f(x,y) --> f(x,v)
	//fft1ȡһ��/��
	CComplexArray *fft1 = new CComplexArray;
	fft1->Init();
	fft1->GenerateArray(1, ret->Width);
	CComplexArray *fft0;
	for(PIXEL x=0;x!=ret->Height;++x){
		for(PIXEL i=0; i!=ret->Width; ++i){
			fft1->Array[0][i] = inputarray->Array[x][i]; //��һ�֣�ȡ�����һ�У������ --> ret
		}
		fft0 = FFT1(fft1, KWidth);
		for(PIXEL i=0; i!=ret->Width; ++i){
			ret->Array[x][i] = fft0->Array[0][i];
		}
		fft0->Init();
	}

	//f(x,v) --> f(u,v)
	fft1->Init();
	fft1->GenerateArray(1, ret->Height);
	for(PIXEL v=0;v!=ret->Width;++v){
		for(PIXEL j=0; j!=ret->Height; ++j){
			fft1->Array[0][j] = ret->Array[j][v];
		}
		fft0 = FFT1(fft1, KWidth);//K=6 --> 64
		for(PIXEL j=0; j!=ret->Height; ++j){
			ret->Array[j][v] = fft0->Array[0][j];
		}
		fft0->Init();
	}
	//�����ѻ������ת�����
	delete fft1, fft0;
	if(ArrayOfFFT!=NULL){
		delete ArrayOfFFT;
	}
	ArrayOfFFT = ret;
	return ret;
}
////////////////////////////////////////////////////////////////////////////////
//        NAME: Cbmp::IFFT2
// DESCRIPTION: IFFT2 ��任
////////////////////////////////////////////////////////////////////////////////
CComplexArray* Cbmp::IFFT2(const CComplexArray * inputarray)
{
	CComplexArray *ret =new CComplexArray;
	ret->GenerateArray(inputarray->Height, inputarray->Width);

	PIXEL KWidth(0), KHeight(0);
	//ȡ��2�Ĵ���
	while (true)
	{
		int count(0);
		pow(2, KWidth)	< ret->Width	? ++KWidth  : ++count;
		pow(2, KHeight)	< ret->Height	? ++KHeight : ++count;
		if (count==2){break;}
	}

	//F(u,v) --> F(u,x)
	//fft1ȡһ��/��
	CComplexArray *fft1 = new CComplexArray;
	fft1->Init();
	fft1->GenerateArray(1, ret->Width);
	CComplexArray *fft0;

	for(PIXEL x=0;x!=ret->Height;++x){
		for(PIXEL i=0; i!=ret->Width; ++i){
			fft1->Array[0][i] = inputarray->Array[x][i]; //����inputarray --> ret
		}
		fft0 = IFFT1(fft1, KWidth);
		for(PIXEL i=0; i!=ret->Width; ++i){
			ret->Array[x][i] = fft0->Array[0][i];
		}
		fft0->Init();
	}

	//f(u,v) --> f(x,v)
	fft1->Init();
	fft1->GenerateArray(1, ret->Height);
	
	for(PIXEL v=0;v!=ret->Width;++v){
		for(PIXEL j=0; j!=ret->Height; ++j){
			fft1->Array[0][j] = ret->Array[j][v];
		}
		fft0 = IFFT1(fft1, KHeight);//K=6 --> 64
		for(PIXEL j=0; j!=ret->Height; ++j){
			ret->Array[j][v] = fft0->Array[0][j];
		}
		fft0->Init();
	}
	//�����ѻ������ת�����
	delete fft1, fft0;

	return ret;
}
////////////////////////////////////////////////////////////////////////////////
//        NAME: Cbmp::FFT
// DESCRIPTION: FFT2 �任
//   ARGUMENTS: const CbmpArray * inputarray - �������
// USES GLOBAL: PIXEL	Height, PIXEL	Width	-	ԭͼ��ĳ�����任��Ҫ 
// MODIFIES GL: none
//     RETURNS: CbmpArray* ret- ����  Ƶ��ͼ
//      AUTHOR: Hxsong
// AUTHOR/DATE: -
//							CH 2014-12-05
////////////////////////////////////////////////////////////////////////////////
CbmpArray* Cbmp::FFT(const CbmpArray * inputarray)
{
	CbmpArray *ret = new CbmpArray;
	ret->GenerateArray(inputarray->Height, inputarray->Width);

	//Ԥ����ԭ������չ��2�Ĵη����������ת��Ϊ�����󣬲��� f(x, y)*(-1)^(x+y)ƽ��
		//�����С2����
	PIXEL KWidth(0), KHeight(0);
	while (true)
	{
		int count(0);
		pow(2, KWidth)	< inputarray->Width	 ? ++KWidth  : ++count;
		pow(2, KHeight)	< inputarray->Height ? ++KHeight : ++count;
		if (count==2){break;}
	}
		//������չ��2��K��,��չ������0
	CbmpArray *reshapeInput = Offset(inputarray, pow(2, KWidth)-inputarray->Width, pow(2, KHeight)-inputarray->Height, 1, 0);
	CComplexArray *ComArray = new CComplexArray;
	ComArray->GenerateArray(reshapeInput->Height, reshapeInput->Width);
	std::string OutputPath("G:\\2014�γ��ļ�\\����ͼ����\\�γ����\\");
	SaveAsGray(OutputPath+"��չ2��.bmp", bmpHead, reshapeInput->Width, reshapeInput->Height, reshapeInput);

		//������תΪ��������[height][width] *(-1)^(i+j)
	for(PIXEL i=0; i!=ComArray->Height; ++i){
		for(PIXEL j=0; j!=ComArray->Width; ++j){
			ComArray->Array[i][j] = std::complex<double>(reshapeInput->Array[i][j]*pow(-1, i+j), 0);
		}
	}
	
	//FFT2�任�õ������� ret --> ArrayOfFFT
	FFT2(ComArray);//
	
	//����һ�θ���Ҷ�任�õ�������ת��Ϊ��ʾ��Ƶ��ͼ ArrayOfFFT --> COLOR
	CbmpArray *ret0;
	//��ʼ�������Сֵ --> �����֮�������޵�ֵ
	//������ʼ��FFT����������Сֵ��Ƶ���˲��ο�
	FFTMin = 255;
	FFTMax = 0;
	for(PIXEL i=0;i!=ArrayOfFFT->Height;++i){
		for(PIXEL j=0;j!=ArrayOfFFT->Width;++j){
			double testd = abs(ArrayOfFFT->Array[i][j]);
			double testb = log(1+testd);
			FFTMin = min(FFTMin, testb);
			FFTMax = max(FFTMax, testb);
		}
	}
	ret0 = SwitchComplexToCbmpArray(ArrayOfFFT, 1);

	//���������ų�ԭͼ���С
	ret = Scale(ret0, 0, 0, ret->Height, ret->Width, 1);
	delete ret0;
	return ret;
}
////////////////////////////////////////////////////////////////////////////////
//        NAME: Cbmp::FFT_Filter
// DESCRIPTION: ͼ��Ƶ�����˲�
//   ARGUMENTS: const CComplexArray * inputarray - �������
// USES GLOBAL: none 
// MODIFIES GL: none
//     RETURNS: CbmpArray* ret- ����  ��任��ĸ�������
//      AUTHOR: Hxsong
// AUTHOR/DATE: -
//							CH 2014-12-05
////////////////////////////////////////////////////////////////////////////////
CComplexArray* Cbmp::FFT_Filter(const CComplexArray * inputarray, double D0/* =0 */, double D1/* =0 */, int n/* =1 */, int filter/* =1 */)
{
	//��������ĸ�����
	CComplexArray * ret = new CComplexArray;
	ret->GenerateArray(inputarray->Height, inputarray->Width);
	for(PIXEL i=0; i!=ret->Height; ++i){
		for(PIXEL j=0; j!=ret->Width; ++j){
			ret->Array[i][j] =  inputarray->Array[i][j];
		}
	}

	//�����˲���ģ�壬H(U, V)
	double u0(ret->Height/2 + 0.5), v0(ret->Width/2 + 0.5);
	double ** H;

	H = new double *[ret->Height];
	for(PIXEL i=0; i!=ret->Height; ++i)
		H[i] = new double [ret->Width];

	if(filter==ILPF){
		for(PIXEL i=0; i!=ret->Height; ++i){
			for(PIXEL j=0; j!=ret->Width; ++j){
				H[i][j] = D(u0, v0, i, j)<=D0?1:0;
			}
		}
	}
	else if(filter==BLPF){
		double temp(0);
		for(PIXEL i=0; i!=ret->Height; ++i){
			for(PIXEL j=0; j!=ret->Width; ++j){
				temp = pow(D(u0, v0, i, j)/D0, 2*n);
				H[i][j] = 1/(1 + temp);
			}
		}
	}
	else if(filter==GLPF){
		double temp(0);
		double Duv;
		for(PIXEL i=0; i!=ret->Height; ++i){
			for(PIXEL j=0; j!=ret->Width; ++j){
				Duv = D(u0, v0, i, j);
				temp = pow(D(u0, v0, i, j)/D0, 2)/(-2);
				H[i][j] = exp(temp);
			}
		}
	}
	else if(filter==TLPF){
		double temp(0);
		double Duv;
		for(PIXEL i=0; i!=ret->Height; ++i){
			for(PIXEL j=0; j!=ret->Width; ++j){
				if(D0>=D1){
					H[i][j] = 1;
					continue;
				}
				Duv = D(u0, v0, i, j);
				H[i][j] = Duv<D0?1:(Duv<D1?(D1-Duv)/(D1-D0):0);	
			}
		}
	}
	//��ͨ����
	else if(filter==IHPF){
		for(PIXEL i=0; i!=ret->Height; ++i){
			for(PIXEL j=0; j!=ret->Width; ++j){
				H[i][j] = D(u0, v0, i, j)>=D0?1:0;
			}
		}
	}
	else if(filter==BHPF){
		double temp(0);
		for(PIXEL i=0; i!=ret->Height; ++i){
			for(PIXEL j=0; j!=ret->Width; ++j){
				temp = pow(D0/D(u0, v0, i, j), 2*n);
				H[i][j] = 1/(1 + temp);
			}
		}
	}
	else if(filter==GHPF){
		double temp(0);
		double Duv;
		for(PIXEL i=0; i!=ret->Height; ++i){
			for(PIXEL j=0; j!=ret->Width; ++j){
				Duv = D(u0, v0, i, j);
				temp = pow(D(u0, v0, i, j)/D0, 2)/(-2);
				H[i][j] = 1-exp(temp);
			}
		}
	}
	else if(filter==THPF){
		double temp(0);
		double Duv;
		for(PIXEL i=0; i!=ret->Height; ++i){
			for(PIXEL j=0; j!=ret->Width; ++j){
				if(D0>=D1){
					H[i][j] = 1;
					continue;
				}
				Duv = D(u0, v0, i, j);
				H[i][j] = Duv<D0?0:(Duv<D1?(Duv-D0)/(D1-D0):1);	
			}
		}
	}

	//ģ�����
	for(PIXEL i=0; i!=ret->Height; ++i){
		for(PIXEL j=0; j!=ret->Width; ++j){
			ret->Array[i][j] = ret->Array[i][j]*std::complex<double>(H[i][j], 0);
		}
	}

	//return
	for(PIXEL i=0; i!=ret->Height; ++i){
		delete []H[i];
	}
	delete []H;
	return ret;
}

////////////////////////////////////////////////////////////////////////////////
//        NAME: Cbmp::DCT1
// DESCRIPTION: DCT һά���ұ任
//   ARGUMENTS: const CbmpArray * inputarray - �������
// USES GLOBAL: PIXEL	Height, PIXEL	Width	-	ԭͼ��ĳ�����任��Ҫ 
// MODIFIES GL: none
//     RETURNS: CbmpArray* ret- ����  ������ʾ��ͼ��
//      AUTHOR: Hxsong
// AUTHOR/DATE: -
//							CH 2014-12-05
////////////////////////////////////////////////////////////////////////////////
CComplexArray* Cbmp::DCT1(const CComplexArray * inputarray, PIXEL KWidth)
{
	if(inputarray->Height==0 || inputarray->Width==0)
		return NULL;
	PIXEL N1 = inputarray->Width;
	CComplexArray * ret = new CComplexArray;
	ret->GenerateArray(1, N1);
//�ǿ����㷨
	double * temp = new double[N1];
	for(PIXEL u=0; u!=N1; ++u){
		temp[u] = 0;
		for(PIXEL x=0; x!=N1; ++x){
			temp[u] += inputarray->Array[0][x].real()*cos((x+0.5)*M_PI*u/N1);
		}
	}
//�����㷨
// 	//�������зֽ���չΪ��ż fe /fo
// 	CComplexArray * fe = new CComplexArray;
// 	fe->GenerateArray(1, N1);
// 	CComplexArray * fo = new CComplexArray;
// 	fo->GenerateArray(1, N1);
// 	for(PIXEL i=0; i!=N1/2; ++i){
// 		fe->Array[0][i] = inputarray->Array[0][2*i];
// 		fo->Array[0][i] = inputarray->Array[0][2*i+1];
// 	}//��չĩβ0
// 	for(PIXEL i=N1/2; i!=N1; ++i){
// 		fe->Array[0][i] = fo->Array[0][i] = 0;
// 	}
// 	CComplexArray * y1 = new CComplexArray;
// 	y1->GenerateArray(1, N1);
// 	CComplexArray * y2 = new CComplexArray;
// 	y2->GenerateArray(1, N1);
// 
// 	//FFT
// 	y1 = FFT1(fe, KWidth, 1);//FFT��Wn��Ϊ��
// 	y2 = FFT1(fo, KWidth, 1);
// 
// 	for(PIXEL u=0; u!=N1; ++u){
// 		double beta1	= M_PI*u/(2*N1);
// 		double beta2	= beta1 + M_PI*u/N1;
// 		std::complex<double> e1(cos(beta1), sin(beta1)), e2(cos(beta2), sin(beta2)), temp;
// 		temp = e1*y1->Array[0][u] + e2*y2->Array[0][u];
// 		ret->Array[0][u] = std::complex<double>(temp.real(), 0);
// 	}
// 
// 	//ret=Y  Y-->F
	for(PIXEL u=0; u!=N1; ++u){
		if(u==0){
			//ret->Array[0][u] = ret->Array[0][u]/sqrt(N1);
			ret->Array[0][u] = std::complex<double>(temp[u]/sqrt(N1), 0);
		}
		else{
			//ret->Array[0][u] = ret->Array[0][u]*sqrt(2)/sqrt(N1);
			ret->Array[0][u] = std::complex<double>(temp[u]*sqrt(2)/sqrt(N1), 0);
		}
	}

	return ret;
}
////////////////////////////////////////////////////////////////////////////////
//        NAME: Cbmp::DCT2
// DESCRIPTION: DCT ��ά���ұ任
//   ARGUMENTS: const CbmpArray * inputarray - ������� Ӧ�������ݵ���չ
// USES GLOBAL: PIXEL	Height, PIXEL	Width	-	ԭͼ��ĳ�����任��Ҫ 
// MODIFIES GL: none
//     RETURNS: CbmpArray* ret- ����  ������ʾ��ͼ��
//      AUTHOR: Hxsong
// AUTHOR/DATE: -
//							CH 2014-12-05
////////////////////////////////////////////////////////////////////////////////
CComplexArray* Cbmp::DCT2(const CComplexArray * inputarray)
{
	PIXEL KWidth(0), KHeight(0);
	//ȡ��2�Ĵ���
	while (true)
	{
		int count(0);
		pow(2, KWidth)	< inputarray->Width	? ++KWidth  : ++count;
		pow(2, KHeight)	< inputarray->Height	? ++KHeight : ++count;
		if (count==2){break;}
	}
	//���������С
	CComplexArray * ret = new CComplexArray;
	ret->GenerateArray(pow(2, KHeight), pow(2, KWidth));

	//f(x,y) --> f(x,v)
	//fft1ȡһ��/��
	CComplexArray *dct1 = new CComplexArray;
	CComplexArray *dct0;
	dct1->Init();
	dct1->GenerateArray(1, ret->Width);
	
	for(PIXEL x=0;x!=ret->Height;++x){
		for(PIXEL i=0; i!=ret->Width; ++i){
			dct1->Array[0][i] = inputarray->Array[x][i];	//��һ�֣�ȡ��������
		}
		dct0 = DCT1(dct1, KWidth);	
		for(PIXEL i=0; i!=ret->Width; ++i){
			ret->Array[x][i] = dct0->Array[0][i];			//��һ�֣�inputarray  --> ret
		}
		delete dct0;
	}

	//f(x,v) --> f(u,v)
	dct1->Init();
	dct1->GenerateArray(1, ret->Height);
	
	for(PIXEL v=0;v!=ret->Width;++v){
		for(PIXEL j=0; j!=ret->Height; ++j){
			dct1->Array[0][j] = ret->Array[j][v];			//�ڶ��֣�ȡ����ret
		}
		dct0 = DCT1(dct1, KHeight);//K=6 --> 64  
		for(PIXEL j=0; j!=ret->Width; ++j){
			ret->Array[j][v] = dct0->Array[0][j];			//�ڶ��֣�ret  -->  ret
		}
		delete dct0;
	}
	//�����ѻ������ת�����
	delete dct1;
	if(ArrayOfDCT!=NULL){
		delete ArrayOfDCT;
	}
	ArrayOfDCT = ret;
	return ret;
}

////////////////////////////////////////////////////////////////////////////////
//        NAME: Cbmp::DCT
// DESCRIPTION: DCT ���ұ任
//   ARGUMENTS: const CbmpArray * inputarray - �������
// USES GLOBAL: PIXEL	Height, PIXEL	Width	-	ԭͼ��ĳ�����任��Ҫ 
// MODIFIES GL: none
//     RETURNS: CbmpArray* ret- ����  ������ʾ��ͼ��
//      AUTHOR: Hxsong
// AUTHOR/DATE: -
//							CH 2014-12-05
////////////////////////////////////////////////////////////////////////////////
CbmpArray* Cbmp::DCT(const CbmpArray * inputarray)
{
	CbmpArray *ret = new CbmpArray;
	ret->GenerateArray(inputarray->Height, inputarray->Width);

	//Ԥ����ԭ������չ��2�Ĵη�
	//�����С2����
	PIXEL KWidth(0), KHeight(0);
	while (true)
	{
		int count(0);
		pow(2, KWidth)	< inputarray->Width	 ? ++KWidth  : ++count;
		pow(2, KHeight)	< inputarray->Height ? ++KHeight : ++count;
		if (count==2){break;}
	}
	//������չ��2��K��,��չ������0
	CbmpArray *reshapeInput = Offset(inputarray, pow(2, KWidth)-inputarray->Width, pow(2, KHeight)-inputarray->Height, 1, 0);
	CComplexArray *ComArray = new CComplexArray;
	ComArray->GenerateArray(reshapeInput->Height, reshapeInput->Width);
	std::string OutputPath("G:\\2014�γ��ļ�\\����ͼ����\\�γ����\\");
	SaveAsGray(OutputPath+"��չdct.bmp", bmpHead, reshapeInput->Width, reshapeInput->Height, reshapeInput);

	//������תΪ��������[height][width]
	for(PIXEL i=0; i!=ComArray->Height; ++i){
		for(PIXEL j=0; j!=ComArray->Width; ++j){
			ComArray->Array[i][j] = std::complex<double>(reshapeInput->Array[i][j], 0);
		}
	}

	//DCT2�任�õ����� reshapeInput --> ArrayOfDCT
	DCT2(ComArray);
	//��һ��
	//����һ��DCT�任�õ�������ת��Ϊ��ʾ��Ƶ��ͼ ArrayOfDCT --> COLOR
	CbmpArray *ret0;
	ret0 = SwitchComplexToCbmpArray(ArrayOfDCT);

	//���������ų�ԭͼ���С
	ret = Scale(ret0, 0, 0, ret->Height, ret->Width, 1);
	delete ret0;
	delete ComArray;
	return ret;
}

////////////////////////////////////////////////////////////////////////////////
//        NAME: CbmpArray::init
// DESCRIPTION: ��ʼ��CbmpArray��
//   ARGUMENTS: none
// USES GLOBAL: none 
// MODIFIES GL: none
//     RETURNS: none
//      AUTHOR: Hxsong
// AUTHOR/DATE: -
//							CH 2014-10-22
////////////////////////////////////////////////////////////////////////////////
void CbmpArray::Init()
{
	if(Height!=0){
		for(int i=0;i!=Height;++i){
			delete []Array[i];
		}
		delete []Array;
		Width = Height = 0;
	}
}
CbmpArray::~CbmpArray()
{
	Init();
}

////////////////////////////////////////////////////////////////////////////////
//        NAME: CbmpArray::GenerateArray
// DESCRIPTION: ����ָ����С����
//   ARGUMENTS: PIXEL width - ͼ�����Ŀ��
//				PIXEL height - ͼ�����ĸ߶�
// USES GLOBAL: none 
// MODIFIES GL: none
//     RETURNS: none
//      AUTHOR: Hxsong
// AUTHOR/DATE: -
//							CH 2014-10-22
////////////////////////////////////////////////////////////////////////////////
void CbmpArray::GenerateArray(PIXEL height, PIXEL width)
{
	Init();
	//������ʽ[Height][Width]
	Height = height;
	Width = width;

	Array = new COLOR* [Height];

	for(int i=0;i!=Height;++i){
		Array[i] = new COLOR [Width];
	}
}

////////////////////////////////////////////////////////////////////////////////
//        NAME: CComplexArray::init
// DESCRIPTION: ��ʼ��CbmpArray��
//   ARGUMENTS: none
// USES GLOBAL: none 
// MODIFIES GL: none
//     RETURNS: none
//      AUTHOR: Hxsong
// AUTHOR/DATE: -
//							CH 2014-10-22
////////////////////////////////////////////////////////////////////////////////
void CComplexArray::Init()
{
	if(Height!=0){
		for(int i=0;i!=Height;++i){
			delete []Array[i];
		}
		delete []Array;
		Width = Height = 0;
	}
}
CComplexArray::~CComplexArray()
{
	Init();
}

////////////////////////////////////////////////////////////////////////////////
//        NAME: CComplexArray::GenerateArray
// DESCRIPTION: ����ָ����С����
//   ARGUMENTS: PIXEL width - ͼ�����Ŀ��
//				PIXEL height - ͼ�����ĸ߶�
// USES GLOBAL: none 
// MODIFIES GL: none
//     RETURNS: none
//      AUTHOR: Hxsong
// AUTHOR/DATE: -
//							CH 2014-10-22
////////////////////////////////////////////////////////////////////////////////
void CComplexArray::GenerateArray(PIXEL height, PIXEL width)
{
	//������ʽ[Height][Width]
	Height = height;
	Width = width;

	Array = new std::complex<double>* [Height];

	for(int i=0;i!=Height;++i){
		Array[i] = new std::complex<double> [Width];
	}
}
