////////////////////////////////////////////////////////////////////////////////
// Cbmp类的函数定义部分
// Modifications:
// 1. 读入修改	- 2014-10-10
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
	for(int i=0;i!=256;++i)               //初始化8位灰度调色板
	{
		Color[i].RGBBlue=i;
		Color[i].RGBGreen=i;
		Color[i].RGBRed=i;
		Color[i].RGBReserved=0;
	};
	//文件头
	bmpHead = new COLOR[54];
	//构造矩阵
	Array_Red = new CbmpArray;
	Array_Green = new CbmpArray;
	Array_Blue	= new CbmpArray;
	Array_Gray	= new CbmpArray;
	ArrayOfFFT	= NULL;
	ArrayOfDCT	= NULL;
}

Cbmp::~Cbmp()
{
	//销毁new 对象
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
// DESCRIPTION: 读入24位或8位灰度BMP图像
//   ARGUMENTS: std::string filepath - 图像路径
// USES GLOBAL: none
// MODIFIES GL: Head, FileSize, OffsetBytes, Width, Height, PixelBitCount ,ImageSize,
//				Array_Blue, Array_Green, Array_Red, Array_Gray
//     RETURNS: bool - 操作标志
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
	//取文件头
	fp.seekg(0, fp.beg);
	fp.read(reinterpret_cast<char *>(bmpHead), 54);

	//提取文件头信息
	FileSize = bmpHead[2]|bmpHead[3]<<8|bmpHead[4]<<16|bmpHead[5]<<24;
	OffsetBytes = bmpHead[10]|bmpHead[11]<<8|bmpHead[12]<<16|bmpHead[13]<<24;
	Width = bmpHead[18]|bmpHead[19]<<8|bmpHead[20]<<16|bmpHead[21]<<24;
	Height = bmpHead[22]|bmpHead[23]<<8|bmpHead[24]<<16|bmpHead[25]<<24;
	PixelBitCount = bmpHead[28]|bmpHead[29]<<8;
	ImageSize = bmpHead[34]|bmpHead[35]<<8|bmpHead[36]<<16|bmpHead[37]<<24;
	
	//提取三色矩阵 /灰度矩阵

	//**********		24位位图		*************//
	if(bmpHead[28]==24){
		IsGray = false;
		COLOR temp[3];
		PIXEL temp_width = Width*3;
		PIXEL fix_width_3b =(temp_width&0X03) ? ((temp_width&0XFFFFFFFC)+4) : (temp_width&0XFFFFFFFC);//补足后的每行字节数

		//生成矩阵
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
					//temp[2]*0.299 + temp[1]*0.587 +temp[0]*0.114;//采用视觉近似
			}
			fp.read(reinterpret_cast<char*>(temp), fix_width_3b - Width*3);
		}
	}

	//**********		16位位图		*************//

	//**********		8位灰度图		*************//
	if(bmpHead[28]==8){
		IsGray = true;
		COLOR temp[3];
		PIXEL temp_width = Width;
		PIXEL fix_width_1b =(temp_width&0X03) ? ((temp_width&0XFFFFFFFC)+4) : (temp_width&0XFFFFFFFC);

		//生成矩阵
		Array_Blue->GenerateArray(Height, Width);
		Array_Green->GenerateArray(Height, Width);
		Array_Red->GenerateArray(Height, Width);
		Array_Gray->GenerateArray(Height, Width);
		
		fp.seekg(OffsetBytes, fp.beg);//像素开始位置
		for(PIXEL i=0;i!=Height;++i){
			for (PIXEL j=0;j!=Width;++j){
				fp.read(reinterpret_cast<char*>(temp), 1);
				//Array_Gray->Array[Height-1-i][j] = Array_Blue->Array[Height-1-i][j] = Array_Green->Array[Height-1-i][j] = Array_Red->Array[Height-1-i][j] = temp[0];
				Array_Gray->Array[i][j] = Array_Blue->Array[i][j] = Array_Green->Array[i][j] = Array_Red->Array[i][j] = temp[0];
			}
			fp.read(reinterpret_cast<char*>(temp), fix_width_1b - Width);
		}
	}

	//**********		4位位图			*************//

	//**********		2位位图			*************//

	fp.close();
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
//        NAME: Cbmp::SaveAsColor
// DESCRIPTION: 保存为BMP彩色图像
//   ARGUMENTS: std::string save_path - 保存路径
//				PIXEL width - 保存的图像宽度
//				PIXEL height - 保存的图像高度
//				COLOR **&array_b - 图像的blue分量矩阵 height*width
//				COLOR **&array_g - 图像的green分量矩阵 height*width
//				COLOR **&array_r - 图像的red分量矩阵 height*width
// USES GLOBAL: bmpHead 
// MODIFIES GL: none
//     RETURNS: bool - 操作标志
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
	//新图像大小计算
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
	//颜色位数
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
		fp.write(temp, fix_width_3b - temp_width);	//补位
	}

	fp.close();

	return 0;
}

////////////////////////////////////////////////////////////////////////////////
//        NAME: Cbmp::SaveAsGray
// DESCRIPTION: 保存为8为BMP灰度图像
//   ARGUMENTS: std::string save_path - 保存路径
//				PIXEL width - 保存的图像宽度
//				PIXEL height - 保存的图像高度
//				COLOR **&array_grayscale - 图像的灰度矩阵 height*width
// USES GLOBAL: bmpHead 
// MODIFIES GL: none
//     RETURNS: bool - 操作标志
//      AUTHOR: Hxsong
// AUTHOR/DATE: -
//							CH 2014-10-10
////////////////////////////////////////////////////////////////////////////////
bool Cbmp::SaveAsGray(std::string save_path, COLOR *newhead, PIXEL width, PIXEL height, const CbmpArray *array_gray)//灰度存储
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
	BYTES newImageSize = fix_width * height;		//实际需要的像素存储空间
	NewHead[34] = 0XFF&(newImageSize);
	NewHead[35] = 0XFF&(newImageSize>>8);
	NewHead[36] = 0XFF&(newImageSize>>16);
	NewHead[37] = 0XFF&(newImageSize>>24);

	BYTES newSize = 54 + 1024 + newImageSize;
	NewHead[2] = 0XFF&newSize;
	NewHead[3] = 0XFF&(newSize>>8);
	NewHead[4] = 0XFF&(newSize>>16);
	NewHead[5] = 0XFF&(newSize>>24);
	//颜色位数
	NewHead[28] = 8;

	fp.write(reinterpret_cast<char*>(NewHead), 54);

	//插入调色板
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
// DESCRIPTION: 镜像图像
//   ARGUMENTS: const CbmpArray * inputarray - 需要镜像变换的矩阵
//				bool action - 镜像方式：0-左右镜像, 1-上下镜像
// USES GLOBAL: none 
// MODIFIES GL: none
//     RETURNS: CbmpArray* - 返回 new CbmpArray 指针，指向镜像完之后的矩阵
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
// DESCRIPTION: 图像平移，大小不变，空白出填充
//   ARGUMENTS: const CbmpArray * inputarray - 需要平移变换的矩阵
//				PIXEL x	- 平移的x坐标，负为向左，正为向右
//				PIXEL y - 平移的y坐标，负为向下，正为向上
//				bool mode	- 平移模式： 0-大小不变， 1-目标大小改变，保持原图大小
//				COLOR fill - 空白的填充灰度值
// USES GLOBAL: none 
// MODIFIES GL: none
//     RETURNS: CbmpArray* - 返回 new CbmpArray 指针，指向平移之后的矩阵，由使用者负责销毁
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
// DESCRIPTION: 图像剪切，空白处填充
//   ARGUMENTS: const CbmpArray * inputarray - 剪切变换的矩阵
//				int x1 - 左下角的x坐标
//				int x2 - 右上角的x坐标
//				int y1 - 左下角y坐标
//				int y2 - 右上角y坐标
//				COLOR fill - 空白的填充灰度值
// USES GLOBAL: none 
// MODIFIES GL: none
//     RETURNS: CbmpArray* - 返回 new CbmpArray 指针，指向剪切之后的矩阵，由使用者负责销毁
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
// DESCRIPTION: 图像缩放，采用插值
//   ARGUMENTS: const CbmpArray * inputarray - 输入矩阵
//				double nscale - 缩放比例
//				PIXEL nheight=0, PIXEL nwidth=0 - 均不为0时按此点直接生成比例
//				bool resampling - 插值方式： 0-最近邻插值   1-双线性插值
// USES GLOBAL: none 
// MODIFIES GL: none
//     RETURNS: CbmpArray* - 返回 new CbmpArray 指针，指向剪切之后的矩阵，由使用者负责销毁
//      AUTHOR: Hxsong
// AUTHOR/DATE: -
//							CH 2014-10-22
////////////////////////////////////////////////////////////////////////////////
CbmpArray* Cbmp::Scale(const CbmpArray * inputarray, double nscale_x, double nscale_y, PIXEL nheight/* =0 */, PIXEL nwidth/* =0 */, bool resampling/* =false */)
{

	COLOR **const &f	= inputarray->Array;
	double scale_x, scale_y;
	//以比例计算
	PIXEL newWidth, newHeight;
	//优先计算直接长宽
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
		newWidth	= static_cast<PIXEL>(floor(inputarray->Width*scale_x));	//图像往小取整
		newHeight	= static_cast<PIXEL>(floor(inputarray->Height*scale_y));
	}

	//直接换算
	CbmpArray *ret = new CbmpArray;
	ret->GenerateArray(newHeight, newWidth);
	PIXEL x(0), y(0);
	double u(0), v(0);//(x+u,y+v)→f[y+v][x+u]
	for(PIXEL i=0;i!=ret->Height;++i){
		for(PIXEL j=0;j!=ret->Width;++j){
			//最近邻插值,四舍五入到最近点
			if(!resampling){
				//(x,y)
				x = static_cast<PIXEL>(ceil((j)/scale_x - 0.5));
				y = static_cast<PIXEL>(ceil((i)/scale_y - 0.5));
				if((y)>inputarray->Height-1){ y=y-1;}
				if((x)>inputarray->Width-1){ x=x-1;}
				ret->Array[i][j] = f[y][x];
			}
			//双线性插值
			else{
				x = static_cast<PIXEL>(floor(j/scale_x));
				y = static_cast<PIXEL>(floor(i/scale_y));
				//边缘采用最近邻插值计算
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
// DESCRIPTION: 图像旋转，空白处插值
//   ARGUMENTS: const CbmpArray * inputarray - 输入矩阵
//				PIXEL x_0, PIXEL y_0 - 旋转中心
//				double beta - 旋转角度
//				bool resampling - 插值方式： 0-最近邻插值   1-双线性插值
// USES GLOBAL: none 
// MODIFIES GL: none
//     RETURNS: CbmpArray* - 返回 new CbmpArray 指针，指向剪切之后的矩阵，由使用者负责销毁
//      AUTHOR: Hxsong
// AUTHOR/DATE: -
//							CH 2014-10-22
////////////////////////////////////////////////////////////////////////////////
CbmpArray* Cbmp::Rot(const CbmpArray * inputarray, PIXEL x_0/* =0 */, PIXEL y_0/* =0 */, double degree/* =M_PI_2 */, bool resampling/* =0 */, COLOR fill/* =0 */)
{
	if(x_0<0 || x_0>inputarray->Width-1 || y_0<0 || y_0>inputarray->Height){
		return NULL;
	}
	COLOR **const &f	= inputarray->Array;//简化计算矩阵

	double beta = ((degree/M_PI)-floor(degree/M_PI))*M_PI;//约分beta
	//4顶点确定新矩阵大小
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
	double u(0), v(0);//(x+u,y+v)→f[y+v][x+u]
	double xOrigin,yOrigin;
	__int32 iOrigin,jOrigin;
	for (PIXEL i=0;i!=ret->Height;++i){
		for (PIXEL j=0;j!=ret->Width;++j){
			//坐标映射（j，i）→原坐标（xOrigin,yOrigin）→插值计算
			jOrigin	= j + p4x + 1;
			iOrigin	= i;
			xOrigin	= jOrigin*cos(beta)/(cos(beta)*cos(beta) + sin(beta)*sin(beta)) +\
				      iOrigin*sin(beta)/(cos(beta)*cos(beta) + sin(beta)*sin(beta));
			yOrigin	= (-1)*jOrigin*sin(beta)/(cos(beta)*cos(beta) + sin(beta)*sin(beta)) +\
					   iOrigin*cos(beta)/(cos(beta)*cos(beta) + sin(beta)*sin(beta));
			//映射到原图外的点填充
			if(xOrigin<0 || yOrigin<0 || xOrigin>inputarray->Width-1 || yOrigin>inputarray->Height-1){
				ret->Array[i][j] = fill;
			}
			//映射到原图的进行插值近似
			else{
				if(resampling){//双线性内插
					x	= static_cast<PIXEL>(floor(xOrigin));
					u	= xOrigin - x;
					y	= static_cast<PIXEL>(floor(yOrigin));
					v	= yOrigin - y;
					if((y+1)>inputarray->Height-1){ y=y-1;}
					if((x+1)>inputarray->Width-1){ x=x-1;}
					ret->Array[i][j] = static_cast<COLOR>(f[y][x]*(1-u)*(1-v) + f[y][x+1]*(u)*(1-v) + f[y][x+1]*(1-u)*(v) + f[y+1][x+1]*(u)*(v));
				}
				else{//最近邻插值
					x = static_cast<PIXEL>(ceil(xOrigin - 0.5));
					y = static_cast<PIXEL>(ceil(yOrigin - 0.5));
					if((y)>inputarray->Height-1){ y=y-1;}
					if((x)>inputarray->Width-1){ x=x-1;}
					ret->Array[i][j] = f[y][x];
				}
			}
		}
	}
	//平移旋转中心
	CbmpArray *retTemp = Offset(ret, x_0, y_0, true, fill);
	delete ret;
	ret = retTemp;

	return ret;
}

////////////////////////////////////////////////////////////////////////////////
//        NAME: Cbmp::Filter
// DESCRIPTION: 空域滤波
//   ARGUMENTS: const CbmpArray * inputarray - 输入矩阵
//				char mode - 滤波模式，低通，高通...
//				char diff_mode - 微分滤波使用，确定算子
//				char diff_reserve - 微分滤波使用，确定保留方式
//				COLOR threshold - 微分阈值
//				COLOR background - 背景灰度		背景取舍部分不做
//				COLOR outline - 轮廓灰度
// USES GLOBAL: none 
// MODIFIES GL: none
//     RETURNS: CbmpArray* - 返回 new CbmpArray 指针
//      AUTHOR: Hxsong
// AUTHOR/DATE: -
//							CH 2014-10-24
////////////////////////////////////////////////////////////////////////////////
CbmpArray* Cbmp::Filter(const CbmpArray * inputarray, char mode, char diff_mode/* =0 */, char diff_reserve/* =0 */, COLOR threshold/* =0 */, COLOR background/* =0 */, COLOR outline/* =255 */)
{
	COLOR **const &f = inputarray->Array;
	CbmpArray *ret = new CbmpArray;
	ret->GenerateArray(inputarray->Height,inputarray->Width);

	/* 低通滤波 */
	if (mode == LOW_PASS){
		for(PIXEL i=0;i!=ret->Height;++i){
			for(PIXEL j=0;j!=ret->Width;++j){
			//边缘处理:原值
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

	/* 中值滤波 */
	if (mode == MID_MEAN){
		COLOR temp[9];
		COLOR exc(0);
		for(PIXEL i=0;i!=ret->Height;++i){
			for(PIXEL j=0;j!=ret->Width;++j){
				//边缘处理:原值
				if(i==0 || i==ret->Height-1 || j==0 || j==ret->Width-1){
					ret->Array[i][j] = f[i][j];
				}
				else{
					//排序：3x3
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
					ret->Array[i][j] = temp[4];//中值为temp[4]
				}
			}
		}
	}

	/* 基本高通滤波 */
	if (mode == HIGH_PASS){
		for(PIXEL i=0;i!=ret->Height;++i){
			for(PIXEL j=0;j!=ret->Width;++j){
				//边缘处理:原值
				if(i==0 || i==ret->Height-1 || j==0 || j==ret->Width-1){
					ret->Array[i][j] = f[i][j];
				}
				else{
					double temp  =	(	f[i-1][j-1]*highPass[0][0]	+ f[i-1][j]*highPass[0][1]	+ f[i-1][j+1]*highPass[0][2] +\
										f[i][j-1]*highPass[1][0]	+ f[i][j]*highPass[1][1]	+ f[i][j+1]*highPass[1][2] +\
										f[i+1][j-1]*highPass[2][0]	+ f[i+1][j]*highPass[2][1]	+ f[i+1][j+1]*highPass[2][2]);
					//负数归零
					temp<=0?ret->Array[i][j]=0 : (temp>255? ret->Array[i][j]=255 : ret->Array[i][j]=static_cast<COLOR>(temp));	
				}
			}
		}
	}

	/* 微分滤波 */
	if (mode == DIFF){
		/* PREWITT */
		if (diff_mode == PREWITT){
			for(PIXEL i=0;i!=ret->Height;++i){
				for(PIXEL j=0;j!=ret->Width;++j){
					//边缘处理:原值
					if(i==0 || i==ret->Height-1 || j==0 || j==ret->Width-1){
						ret->Array[i][j] = f[i][j];
					}
					else{
						//水平方向梯度
						double temp1  =	(	f[i-1][j-1]*prewitt1[0][0]	+ f[i-1][j]*prewitt1[0][1]	+ f[i-1][j+1]*prewitt1[0][2] +\
											f[i][j-1]*prewitt1[1][0]	+ f[i][j]*prewitt1[1][1]	+ f[i][j+1]*prewitt1[1][2] +\
											f[i+1][j-1]*prewitt1[2][0]	+ f[i+1][j]*prewitt1[2][1]	+ f[i+1][j+1]*prewitt1[2][2]);
						double temp2  =	(	f[i-1][j-1]*prewitt2[0][0]	+ f[i-1][j]*prewitt2[0][1]	+ f[i-1][j+1]*prewitt2[0][2] +\
											f[i][j-1]*prewitt2[1][0]	+ f[i][j]*prewitt2[1][1]	+ f[i][j+1]*prewitt2[1][2] +\
											f[i+1][j-1]*prewitt2[2][0]	+ f[i+1][j]*prewitt2[2][1]	+ f[i+1][j+1]*prewitt2[2][2]);
						double temp	=sqrt(temp1*temp1 + temp2*temp2);
						//负数归零
						temp<=0?ret->Array[i][j]=0 : (temp>255? ret->Array[i][j]=255 : ret->Array[i][j]=static_cast<COLOR>(temp));
					}
				}
			}
		}
		/* SOBEL */
		if (diff_mode == SOBEL){
			for(PIXEL i=0;i!=ret->Height;++i){
				for(PIXEL j=0;j!=ret->Width;++j){
					//边缘处理:原值
					if(i==0 || i==ret->Height-1 || j==0 || j==ret->Width-1){
						ret->Array[i][j] = f[i][j];
					}
					else{
						//水平方向梯度
						double temp1  =	(	f[i-1][j-1]*sobel1[0][0]	+ f[i-1][j]*sobel1[0][1]	+ f[i-1][j+1]*sobel1[0][2] +\
							f[i][j-1]*sobel1[1][0]	+ f[i][j]*sobel1[1][1]	+ f[i][j+1]*sobel1[1][2] +\
							f[i+1][j-1]*sobel1[2][0]	+ f[i+1][j]*sobel1[2][1]	+ f[i+1][j+1]*sobel1[2][2]);
						double temp2  =	(	f[i-1][j-1]*sobel2[0][0]	+ f[i-1][j]*sobel2[0][1]	+ f[i-1][j+1]*sobel2[0][2] +\
							f[i][j-1]*sobel2[1][0]	+ f[i][j]*sobel2[1][1]	+ f[i][j+1]*sobel2[1][2] +\
							f[i+1][j-1]*sobel2[2][0]	+ f[i+1][j]*sobel2[2][1]	+ f[i+1][j+1]*sobel2[2][2]);
						double temp	=sqrt(temp1*temp1 + temp2*temp2);
						//负数归零
						temp<=0?ret->Array[i][j]=0 : (temp>255? ret->Array[i][j]=255 : ret->Array[i][j]=static_cast<COLOR>(temp));
					}
				}
			}
		}
		/* LAPLACE */
		if (diff_mode == LAPLACE){
			for(PIXEL i=0;i!=ret->Height;++i){
				for(PIXEL j=0;j!=ret->Width;++j){
					//边缘处理:原值
					if(i==0 || i==ret->Height-1 || j==0 || j==ret->Width-1){
						ret->Array[i][j] = f[i][j];
					}
					else{
						double temp  =	(	f[i-1][j-1]*laplace[0][0]	+ f[i-1][j]*laplace[0][1]	+ f[i-1][j+1]*laplace[0][2] +\
							f[i][j-1]*laplace[1][0]	+ f[i][j]*laplace[1][1]	+ f[i][j+1]*laplace[1][2] +\
							f[i+1][j-1]*laplace[2][0]	+ f[i+1][j]*laplace[2][1]	+ f[i+1][j+1]*laplace[2][2]);
						//负数归零
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
	// 灰度映射表
	unsigned char	bMap[256];
	// 计算灰度映射表
	for (int i = 0; i < 256; i++)
	{
		// 初始为0
		int lTemp = 0;		
		for (int j = 0; j <= i ; j++)
		{
			lTemp += count[j];
		}	
		// 计算对应的新灰度值
		bMap[i] = (unsigned char) (lTemp * 255 / inputarray->Height / inputarray->Width);
	}

	for(PIXEL i = 0; i < inputarray->Height; i++){
		for(PIXEL j = 0; j < inputarray->Width; j++){
			outputarray->Array[i][j] = bMap[inputarray->Array[i][j]];              // 计算新的灰度值	
		}
	}
}
CbmpArray* Cbmp::Histogram(const CbmpArray * inputarray, std::string output_path)
{
	CbmpArray *ret = new CbmpArray;
	ret->GenerateArray(inputarray->Height, inputarray->Width);

	histequalization(inputarray, ret);

	//画直方图
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
// DESCRIPTION: 将傅里叶或则余弦变换生成的复数矩阵归一化为图像显示范围
//   ARGUMENTS: const CComplexArray * inputarray - 输入矩阵， COLOR min=0, COLOR max=255 归一化的范围上下限
// USES GLOBAL: none
// MODIFIES GL: none
//     RETURNS: CbmpArray* - 返回 new CbmpArray
//      AUTHOR: Hxsong
// AUTHOR/DATE: -
//							CH 2014-12-09
////////////////////////////////////////////////////////////////////////////////
CbmpArray* Cbmp::SwitchComplexToCbmpArray(const CComplexArray * inputarray, bool mode/* =0 */, COLOR thresholdmin/* =0 */, COLOR thresholdmax/* =255 */)
{
	CbmpArray *ret = new CbmpArray;
	ret->GenerateArray(inputarray->Height, inputarray->Width);

	//取对数后归一化到 thresholdmin-thresholdmax  --> ret
	double DFT_Min(255),DFT_Max(0); //初始化最大最小值 --> 求对数之后上下限的值
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
	//归一化
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
// DESCRIPTION: 一维FFT 变换,输入的矩阵必须为 2、4、8、16、32、64、128、256、512--等2的冥
//   ARGUMENTS: const CbmpArray * inputarray - 输入矩阵 width*1,  PIXEL K - 迭代的层数, int W=-1 - 运算子的正负，为余弦变换使用
// USES GLOBAL: none 
// MODIFIES GL: none
//     RETURNS: CbmpArray* - 返回 new CbmpArray 指针，指向中心转换之后的频谱图
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
	
	//N!=2  递归分解
	if(N1!=2){
		CComplexArray *Xeven = new CComplexArray; //偶数列
		CComplexArray *Xodd = new CComplexArray; //奇数列
		Xeven->GenerateArray(1, N2);
		Xodd->GenerateArray(1, N2);
		CComplexArray *Xeven1; //偶数列
		CComplexArray *Xodd1; //奇数列
		//将inputarray分解为偶数列和奇数列
		for(PIXEL i=0; i!=N2; ++i){
			Xeven->Array[0][i] = inputarray->Array[0][i*2];
			Xodd->Array[0][i] = inputarray->Array[0][i*2+1];
		}
		//返回递归矩阵
		Xeven1 = FFT1(Xeven, K-1);
		Xodd1  = FFT1(Xodd, K-1);
		//写出递归公式
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
// DESCRIPTION: 一维FFT 逆变换
////////////////////////////////////////////////////////////////////////////////
//std::complex<double> Cbmp::FFT1(const CComplexArray * inputarray, PIXEL K)
CComplexArray* Cbmp::IFFT1(const CComplexArray * inputarray, PIXEL K)
{
	CComplexArray *ret = new CComplexArray;
	ret->GenerateArray(inputarray->Height, inputarray->Width);
	PIXEL N1 = pow(2, K);
	PIXEL N2(inputarray->Width/2);

	//N!=2  递归分解
	if(N1!=2){
		CComplexArray *Xeven = new CComplexArray; //偶数列
		CComplexArray *Xodd = new CComplexArray; //奇数列
		Xeven->GenerateArray(1, N2);
		Xodd->GenerateArray(1, N2);
		CComplexArray *Xeven1; //偶数列的输出
		CComplexArray *Xodd1 ; //奇数列的输出
		//将inputarray分解为偶数列和奇数列
		for(PIXEL i=0; i!=N2; ++i){
			Xeven->Array[0][i] = inputarray->Array[0][i*2];
			Xodd->Array[0][i] = inputarray->Array[0][i*2+1];
		}
		//返回递归矩阵
		Xeven1 = IFFT1(Xeven, K-1);
		Xodd1  = IFFT1(Xodd, K-1);
		//写出递归公式
		for(PIXEL i=0; i!=N2; ++i){
			double W_N_beta(2*M_PI*i/N1);		//W的符号
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
// DESCRIPTION: FFT2 变换
//   ARGUMENTS: const CbmpArray * inputarray - 输入矩阵
// USES GLOBAL: none 
// MODIFIES GL: none
//     RETURNS: CbmpArray* - 返回 new CbmpArray 指针，指向中心转换之后的频谱图
//      AUTHOR: Hxsong
// AUTHOR/DATE: -
//							CH 2014-10-22
////////////////////////////////////////////////////////////////////////////////
CComplexArray* Cbmp::FFT2(const CComplexArray * inputarray)
{
	CComplexArray *ret = new CComplexArray;
	ret->GenerateArray(inputarray->Height, inputarray->Width);

	PIXEL KWidth(0), KHeight(0);
	//取得2的次幂
	while (true)
	{
		int count(0);
		pow(2, KWidth)	< ret->Width	? ++KWidth  : ++count;
		pow(2, KHeight)	< ret->Height	? ++KHeight : ++count;
		if (count==2){break;}
	}

	//f(x,y) --> f(x,v)
	//fft1取一行/列
	CComplexArray *fft1 = new CComplexArray;
	fft1->Init();
	fft1->GenerateArray(1, ret->Width);
	CComplexArray *fft0;
	for(PIXEL x=0;x!=ret->Height;++x){
		for(PIXEL i=0; i!=ret->Width; ++i){
			fft1->Array[0][i] = inputarray->Array[x][i]; //第一轮，取输入的一行，处理后 --> ret
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
	//以上已获得输入转化后的
	delete fft1, fft0;
	if(ArrayOfFFT!=NULL){
		delete ArrayOfFFT;
	}
	ArrayOfFFT = ret;
	return ret;
}
////////////////////////////////////////////////////////////////////////////////
//        NAME: Cbmp::IFFT2
// DESCRIPTION: IFFT2 逆变换
////////////////////////////////////////////////////////////////////////////////
CComplexArray* Cbmp::IFFT2(const CComplexArray * inputarray)
{
	CComplexArray *ret =new CComplexArray;
	ret->GenerateArray(inputarray->Height, inputarray->Width);

	PIXEL KWidth(0), KHeight(0);
	//取得2的次幂
	while (true)
	{
		int count(0);
		pow(2, KWidth)	< ret->Width	? ++KWidth  : ++count;
		pow(2, KHeight)	< ret->Height	? ++KHeight : ++count;
		if (count==2){break;}
	}

	//F(u,v) --> F(u,x)
	//fft1取一行/列
	CComplexArray *fft1 = new CComplexArray;
	fft1->Init();
	fft1->GenerateArray(1, ret->Width);
	CComplexArray *fft0;

	for(PIXEL x=0;x!=ret->Height;++x){
		for(PIXEL i=0; i!=ret->Width; ++i){
			fft1->Array[0][i] = inputarray->Array[x][i]; //输入inputarray --> ret
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
	//以上已获得输入转化后的
	delete fft1, fft0;

	return ret;
}
////////////////////////////////////////////////////////////////////////////////
//        NAME: Cbmp::FFT
// DESCRIPTION: FFT2 变换
//   ARGUMENTS: const CbmpArray * inputarray - 输入矩阵
// USES GLOBAL: PIXEL	Height, PIXEL	Width	-	原图像的长宽，逆变换需要 
// MODIFIES GL: none
//     RETURNS: CbmpArray* ret- 返回  频谱图
//      AUTHOR: Hxsong
// AUTHOR/DATE: -
//							CH 2014-12-05
////////////////////////////////////////////////////////////////////////////////
CbmpArray* Cbmp::FFT(const CbmpArray * inputarray)
{
	CbmpArray *ret = new CbmpArray;
	ret->GenerateArray(inputarray->Height, inputarray->Width);

	//预处理，原矩阵扩展到2的次方，输入矩阵转换为复矩阵，并作 f(x, y)*(-1)^(x+y)平移
		//获得最小2的幂
	PIXEL KWidth(0), KHeight(0);
	while (true)
	{
		int count(0);
		pow(2, KWidth)	< inputarray->Width	 ? ++KWidth  : ++count;
		pow(2, KHeight)	< inputarray->Height ? ++KHeight : ++count;
		if (count==2){break;}
	}
		//矩阵扩展至2的K次,扩展部分填0
	CbmpArray *reshapeInput = Offset(inputarray, pow(2, KWidth)-inputarray->Width, pow(2, KHeight)-inputarray->Height, 1, 0);
	CComplexArray *ComArray = new CComplexArray;
	ComArray->GenerateArray(reshapeInput->Height, reshapeInput->Width);
	std::string OutputPath("G:\\2014课程文件\\数字图像处理\\课程设计\\");
	SaveAsGray(OutputPath+"扩展2基.bmp", bmpHead, reshapeInput->Width, reshapeInput->Height, reshapeInput);

		//将输入转为复数矩阵[height][width] *(-1)^(i+j)
	for(PIXEL i=0; i!=ComArray->Height; ++i){
		for(PIXEL j=0; j!=ComArray->Width; ++j){
			ComArray->Array[i][j] = std::complex<double>(reshapeInput->Array[i][j]*pow(-1, i+j), 0);
		}
	}
	
	//FFT2变换得到复矩阵 ret --> ArrayOfFFT
	FFT2(ComArray);//
	
	//将上一次傅里叶变换得到复矩阵转化为显示的频谱图 ArrayOfFFT --> COLOR
	CbmpArray *ret0;
	//初始化最大最小值 --> 求对数之后上下限的值
	//保留初始的FFT矩阵的最大最小值，频域滤波参考
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

	//按比例缩放成原图像大小
	ret = Scale(ret0, 0, 0, ret->Height, ret->Width, 1);
	delete ret0;
	return ret;
}
////////////////////////////////////////////////////////////////////////////////
//        NAME: Cbmp::FFT_Filter
// DESCRIPTION: 图像频率域滤波
//   ARGUMENTS: const CComplexArray * inputarray - 输入矩阵
// USES GLOBAL: none 
// MODIFIES GL: none
//     RETURNS: CbmpArray* ret- 返回  逆变换后的复数矩阵
//      AUTHOR: Hxsong
// AUTHOR/DATE: -
//							CH 2014-12-05
////////////////////////////////////////////////////////////////////////////////
CComplexArray* Cbmp::FFT_Filter(const CComplexArray * inputarray, double D0/* =0 */, double D1/* =0 */, int n/* =1 */, int filter/* =1 */)
{
	//复制输入的复矩阵
	CComplexArray * ret = new CComplexArray;
	ret->GenerateArray(inputarray->Height, inputarray->Width);
	for(PIXEL i=0; i!=ret->Height; ++i){
		for(PIXEL j=0; j!=ret->Width; ++j){
			ret->Array[i][j] =  inputarray->Array[i][j];
		}
	}

	//生成滤波器模板，H(U, V)
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
	//高通部分
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

	//模板相乘
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
// DESCRIPTION: DCT 一维余弦变换
//   ARGUMENTS: const CbmpArray * inputarray - 输入矩阵
// USES GLOBAL: PIXEL	Height, PIXEL	Width	-	原图像的长宽，逆变换需要 
// MODIFIES GL: none
//     RETURNS: CbmpArray* ret- 返回  余弦显示的图像
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
//非快速算法
	double * temp = new double[N1];
	for(PIXEL u=0; u!=N1; ++u){
		temp[u] = 0;
		for(PIXEL x=0; x!=N1; ++x){
			temp[u] += inputarray->Array[0][x].real()*cos((x+0.5)*M_PI*u/N1);
		}
	}
//快速算法
// 	//输入序列分解扩展为奇偶 fe /fo
// 	CComplexArray * fe = new CComplexArray;
// 	fe->GenerateArray(1, N1);
// 	CComplexArray * fo = new CComplexArray;
// 	fo->GenerateArray(1, N1);
// 	for(PIXEL i=0; i!=N1/2; ++i){
// 		fe->Array[0][i] = inputarray->Array[0][2*i];
// 		fo->Array[0][i] = inputarray->Array[0][2*i+1];
// 	}//扩展末尾0
// 	for(PIXEL i=N1/2; i!=N1; ++i){
// 		fe->Array[0][i] = fo->Array[0][i] = 0;
// 	}
// 	CComplexArray * y1 = new CComplexArray;
// 	y1->GenerateArray(1, N1);
// 	CComplexArray * y2 = new CComplexArray;
// 	y2->GenerateArray(1, N1);
// 
// 	//FFT
// 	y1 = FFT1(fe, KWidth, 1);//FFT的Wn项为负
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
// DESCRIPTION: DCT 二维余弦变换
//   ARGUMENTS: const CbmpArray * inputarray - 输入矩阵 应该已做幂的扩展
// USES GLOBAL: PIXEL	Height, PIXEL	Width	-	原图像的长宽，逆变换需要 
// MODIFIES GL: none
//     RETURNS: CbmpArray* ret- 返回  余弦显示的图像
//      AUTHOR: Hxsong
// AUTHOR/DATE: -
//							CH 2014-12-05
////////////////////////////////////////////////////////////////////////////////
CComplexArray* Cbmp::DCT2(const CComplexArray * inputarray)
{
	PIXEL KWidth(0), KHeight(0);
	//取得2的次幂
	while (true)
	{
		int count(0);
		pow(2, KWidth)	< inputarray->Width	? ++KWidth  : ++count;
		pow(2, KHeight)	< inputarray->Height	? ++KHeight : ++count;
		if (count==2){break;}
	}
	//定义输出大小
	CComplexArray * ret = new CComplexArray;
	ret->GenerateArray(pow(2, KHeight), pow(2, KWidth));

	//f(x,y) --> f(x,v)
	//fft1取一行/列
	CComplexArray *dct1 = new CComplexArray;
	CComplexArray *dct0;
	dct1->Init();
	dct1->GenerateArray(1, ret->Width);
	
	for(PIXEL x=0;x!=ret->Height;++x){
		for(PIXEL i=0; i!=ret->Width; ++i){
			dct1->Array[0][i] = inputarray->Array[x][i];	//第一轮，取的是输入
		}
		dct0 = DCT1(dct1, KWidth);	
		for(PIXEL i=0; i!=ret->Width; ++i){
			ret->Array[x][i] = dct0->Array[0][i];			//第一轮，inputarray  --> ret
		}
		delete dct0;
	}

	//f(x,v) --> f(u,v)
	dct1->Init();
	dct1->GenerateArray(1, ret->Height);
	
	for(PIXEL v=0;v!=ret->Width;++v){
		for(PIXEL j=0; j!=ret->Height; ++j){
			dct1->Array[0][j] = ret->Array[j][v];			//第二轮，取的是ret
		}
		dct0 = DCT1(dct1, KHeight);//K=6 --> 64  
		for(PIXEL j=0; j!=ret->Width; ++j){
			ret->Array[j][v] = dct0->Array[0][j];			//第二轮，ret  -->  ret
		}
		delete dct0;
	}
	//以上已获得输入转化后的
	delete dct1;
	if(ArrayOfDCT!=NULL){
		delete ArrayOfDCT;
	}
	ArrayOfDCT = ret;
	return ret;
}

////////////////////////////////////////////////////////////////////////////////
//        NAME: Cbmp::DCT
// DESCRIPTION: DCT 余弦变换
//   ARGUMENTS: const CbmpArray * inputarray - 输入矩阵
// USES GLOBAL: PIXEL	Height, PIXEL	Width	-	原图像的长宽，逆变换需要 
// MODIFIES GL: none
//     RETURNS: CbmpArray* ret- 返回  余弦显示的图像
//      AUTHOR: Hxsong
// AUTHOR/DATE: -
//							CH 2014-12-05
////////////////////////////////////////////////////////////////////////////////
CbmpArray* Cbmp::DCT(const CbmpArray * inputarray)
{
	CbmpArray *ret = new CbmpArray;
	ret->GenerateArray(inputarray->Height, inputarray->Width);

	//预处理，原矩阵扩展到2的次方
	//获得最小2的幂
	PIXEL KWidth(0), KHeight(0);
	while (true)
	{
		int count(0);
		pow(2, KWidth)	< inputarray->Width	 ? ++KWidth  : ++count;
		pow(2, KHeight)	< inputarray->Height ? ++KHeight : ++count;
		if (count==2){break;}
	}
	//矩阵扩展至2的K次,扩展部分填0
	CbmpArray *reshapeInput = Offset(inputarray, pow(2, KWidth)-inputarray->Width, pow(2, KHeight)-inputarray->Height, 1, 0);
	CComplexArray *ComArray = new CComplexArray;
	ComArray->GenerateArray(reshapeInput->Height, reshapeInput->Width);
	std::string OutputPath("G:\\2014课程文件\\数字图像处理\\课程设计\\");
	SaveAsGray(OutputPath+"扩展dct.bmp", bmpHead, reshapeInput->Width, reshapeInput->Height, reshapeInput);

	//将输入转为复数矩阵[height][width]
	for(PIXEL i=0; i!=ComArray->Height; ++i){
		for(PIXEL j=0; j!=ComArray->Width; ++j){
			ComArray->Array[i][j] = std::complex<double>(reshapeInput->Array[i][j], 0);
		}
	}

	//DCT2变换得到矩阵 reshapeInput --> ArrayOfDCT
	DCT2(ComArray);
	//归一化
	//将上一次DCT变换得到复矩阵转化为显示的频谱图 ArrayOfDCT --> COLOR
	CbmpArray *ret0;
	ret0 = SwitchComplexToCbmpArray(ArrayOfDCT);

	//按比例缩放成原图像大小
	ret = Scale(ret0, 0, 0, ret->Height, ret->Width, 1);
	delete ret0;
	delete ComArray;
	return ret;
}

////////////////////////////////////////////////////////////////////////////////
//        NAME: CbmpArray::init
// DESCRIPTION: 初始化CbmpArray类
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
// DESCRIPTION: 构建指定大小矩阵
//   ARGUMENTS: PIXEL width - 图像矩阵的宽度
//				PIXEL height - 图像矩阵的高度
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
	//矩阵形式[Height][Width]
	Height = height;
	Width = width;

	Array = new COLOR* [Height];

	for(int i=0;i!=Height;++i){
		Array[i] = new COLOR [Width];
	}
}

////////////////////////////////////////////////////////////////////////////////
//        NAME: CComplexArray::init
// DESCRIPTION: 初始化CbmpArray类
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
// DESCRIPTION: 构建指定大小矩阵
//   ARGUMENTS: PIXEL width - 图像矩阵的宽度
//				PIXEL height - 图像矩阵的高度
// USES GLOBAL: none 
// MODIFIES GL: none
//     RETURNS: none
//      AUTHOR: Hxsong
// AUTHOR/DATE: -
//							CH 2014-10-22
////////////////////////////////////////////////////////////////////////////////
void CComplexArray::GenerateArray(PIXEL height, PIXEL width)
{
	//矩阵形式[Height][Width]
	Height = height;
	Width = width;

	Array = new std::complex<double>* [Height];

	for(int i=0;i!=Height;++i){
		Array[i] = new std::complex<double> [Width];
	}
}
