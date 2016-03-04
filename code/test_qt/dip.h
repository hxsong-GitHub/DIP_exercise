#ifndef DIP_H
#define DIP_H

#include <QtWidgets/QMainWindow>
#include "ui_dip.h"
#include "Cbmp.h"
#include<QMouseEvent>

#define	OFFSET		0x01
#define	CROP		0x02
#define	SCALE		0x03
#define	ROT			0x04
#define	FILTER		0x05
#define	HISTOGRAM	0x06

#define FLAGFFT			0x07
#define FLAGIFFT 		0x08
#define FLAGFreFilter	0x09
#define FLAGDCT			0x0a

class myLabel;

class dip : public QMainWindow
{
	Q_OBJECT

public:
	dip(QWidget *parent = 0);
	~dip();

public:
	Cbmp *BMP;				//源图像
	Cbmp *BMPOutput;		//输出窗口图像
	Cbmp *BMPSource;		//输入窗口图像
	QString sourcePath;		//源路径
	QImage	*imgSource;		//输入输出窗口的图像指针
	QImage	*imgOutput;
	myLabel	*labelSource;
	myLabel	*labelOutput;
	bool	imgType;		//图像处理方式 -彩色或者灰度

	void Open(std::string filePath);
	void IsFirstRun();
	void DisplayOutputImg(std::string filePath);
	void DisplaySourceImg(std::string filePath);

	//空域部分的处理函数
	void Mirror();
	void Offset(int x_0, int y_0, bool mode =false , COLOR fill =0 );
	void Crop(int x1, int y1, int x2, int y2, COLOR fill=0 );
	void Scale(double nscale_x, double nscale_y, PIXEL nheight=0, PIXEL nwidth=0, bool resampling=true);
	void Rot(PIXEL x_0=0, PIXEL y_0=0, double degree=M_PI_2, bool resampling=0, COLOR fill=0);
	void PixelFilter(char mode, char diff_mode/* =0 */);
	void Histogram();
	//频率域部分的处理函数
	void FFT();
	void IFFT();
	void FreFilter(double D0=0, double D1=0, int n=1, int filter=1);
	void DCT();

	bool IsFileEmpty();
	bool IfFFT();
	bool IsImgOpen();

signals:


private slots:
	//全局设置
	void DIPRun();
	void ResetParameters();
	void ImgOpen();
	void ImgSave();
	void SetImgType();

	//鼠标取色使用
	void GetColor();
	void GetColor_1();
	//空域部分的槽
	void MirrorSlots();
	void OffsetSlots();
	void CropSlots();
	void ScaleSlots();
	void RotSlots();
	void PixelFilterSlots();
	void HistogramSlots();

	void SetFilter();
	void ResetFilter();

	//频率域部分的槽
	void FFTSlots();
	void IFFTSlots();
	void FreFilterSlots();
	void DCTSlots();

	//关于菜单
	void AuthorSlots();

	void SetLPF();	//设置高通或者低通滤波菜单2
	void SetHPF();

	//下拉2		二级槽
		//低通
	void SetILPF();
	void SetBLPF();
	void SetGLPF();
	void SetTLPF();
		//高通
	void SetIHPF();
	void SetBHPF();
	void SetGHPF();
	void SetTHPF();

private:
	Ui::dipClass ui;

	std::string defaultSavePath;
	bool isFileEmpty;
	bool ifFFT;
	bool isImgOpen;
	bool isFirstRun;

	int	 flag;

};

/*******		Qlabel			*********/

//重写Qlabel,定义clicked响应响应事件
class myLabel :public QLabel
{
	Q_OBJECT

public:
	myLabel(QWidget * parent = 0)
		:QLabel(parent), posx(0), posy(0), tracking(1)
	{
		setMouseTracking(true);
		//connect(this, SIGNAL(clicked()), this, SLOT(slickedSlots()));
	}
	~myLabel(){};
	int posx,posy;
	bool tracking;


signals:
	void clicked();

public slots:

protected:
	void mouseMoveEvent ( QMouseEvent * event )
	{
		setCursor(Qt::CrossCursor);
		posx = event->pos().x();
		posy = event->pos().y();
		emit clicked();
	}
	void mousePressEvent ( QMouseEvent * event )
	{
		tracking = !tracking;
		setMouseTracking(tracking);
		setCursor(Qt::ArrowCursor);
		posx = event->pos().x();
		posy = event->pos().y();
		emit clicked();
	}

};




#endif // DIP_H
