#include "test_qt.h"
#include "fre.h"
#include "dip.h"
//使用动态载入的两个文件
#include <QFile>
#include <QtUiTools/QUiLoader>

#include <QtWidgets/QScrollArea>
#include <QMessageBox>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QFileDialog>
#include <QTextCodec>
#include <QPushButton>
#include <QSlider>
#include <QSpinBox>
#include <QHBoxLayout>
#include <string>

#include "Cbmp.h"

//#pragma comment(lib, "Qt5UiTools.lib")
#pragma comment(lib, "Qt5UiToolsd.lib")

void bmpTest();

int main(int argc, char *argv[])
{
	
	QApplication::addLibraryPath("./plugins");
	//bmpTest();
	QApplication app(argc, argv);

	//输出中文以及html格式文件
	
// 	QLabel *label = new QLabel (QStringLiteral("<h2><i>黄显淞</i><font color=red>中文字符</h2><h2><i>黄显淞</i><font color=yellow>中文字符</h2>"));
// 	label->setWindowTitle(QStringLiteral("测试中文"));
// 	label->show();

/********************   dip窗口   ********************/
	
	QMainWindow *dipWindow = new dip;
	dipWindow->show();



	//显示图像
	
	QString filename(QStringLiteral("G:\\2014课程文件\\数字图像处理\\课程设计\\lena256.bmp"));
// 	filename=QFileDialog::getOpenFileName(
// 		NULL,
// 		QStringLiteral("选择图像"),
// 		"",
//  		QStringLiteral("Images (*.png *.bmp *.jpg *.tif *.GIF )"));

// 	if(filename.isEmpty())	{
// 		return 0;
// 	}
// 	else	{
// 		QImage* img=new QImage;
// 		if(! ( img->load(filename) ) ) //加载图像
// 		{
// 			QMessageBox::information(
// 				NULL,
// 				QStringLiteral("打开图像失败"),
// 				QStringLiteral("打开图像失败!"));
// 			delete img;
// 			return 0;
// 		}
		//添加滚动条
// 		QScrollArea *pArea= new QScrollArea(NULL);
// 		QLabel *labelImg = new QLabel(pArea);
// 
// 		//labelImg->setGeometry(100,100,400,400);//在窗口的(width,height,width2,height2)区域画出该部件
// 		labelImg->setGeometry(0,0,700,400);//这是label区域所占的大小，图像比之大，显示滚动条，图像小于这，嵌入区域中
// 		labelImg->setWindowTitle(QStringLiteral("显示图像"));
// // 		labelImg->setMinimumHeight(img->height());
// // 		labelImg->setMinimumWidth(img->width());
// 		labelImg->setPixmap(QPixmap::fromImage(*img));//必须要把这个wdget画出来
// 		pArea->setWidget(labelImg);//这里设置滚动窗口qw，
// 
// 		pArea->show();

//	}


	//载入窗口部件   --  从ui文件编译之后再导入
// 	QDialog *testDialog = new fre;
// 	testDialog->setWindowTitle(QStringLiteral("测试中文"));
// 	testDialog->show();
// 	//动态载入窗口部件--从ui文件直接导入
// 	QUiLoader uiLoader;
// 	QFile file("UI_load.ui");
// 	//QDialog *loadDialog = uiLoader.load(&file);//不能QWidget *类型的值不能用于初始化 Dialog *类型的实体
// 	QWidget * loadWidget = uiLoader.load(&file);
// 	loadWidget->setWindowFlags(Qt::Dialog);
// 	loadWidget->show();


	//事件以及信号、槽
// 	QPushButton *button = new QPushButton(QStringLiteral("退出"));
// 	QObject::connect(button,SIGNAL(clicked()),&app,SLOT(quit()));
// 	button->show()

	//窗口部件的布局
	
// 	QWidget *window = new QWidget;
// 	window->setWindowTitle(QStringLiteral("年龄输入"));
// 	//定义需要的窗口
// 	QSlider *slider = new QSlider(Qt::Horizontal);	//设置滑块方向
// 	slider->setRange(0,128);
// 	QSpinBox * spin = new QSpinBox;					//微调框
// 	spin->setRange(0,128);
// 	//设置信号与槽
// 	QObject::connect(slider,SIGNAL(valueChanged(int)),spin,SLOT(setValue(int)));
// 	QObject::connect(spin,SIGNAL(valueChanged(int)),slider,SLOT(setValue(int)));
// 	//spin->setValue(20);
// 	//set layout
// 	QHBoxLayout *layout = new QHBoxLayout;
// 	layout->addWidget(spin);
// 	layout->addWidget(slider);
// 	window->setLayout(layout);
// 
// 	window->show();



	return app.exec();
}
/*
void bmpTest()
{
	std::string ImagePath("G:\\2014课程文件\\数字图像处理\\课程设计\\lena256.bmp");
	std::string OutputPath("G:\\2014课程文件\\数字图像处理\\课程设计\\");
	Cbmp *BMP = new Cbmp;
	BMP->Open(ImagePath);
	// 	//DFT
	CbmpArray *ImageDFT = BMP->FFT(BMP->Array_Gray);
	BMP->SaveAsGray(OutputPath+"FFT_qt.bmp", BMP->bmpHead, ImageDFT->Width, ImageDFT->Height, ImageDFT);

	//频域滤波
	CComplexArray *F_Filter = BMP->FFT_Filter(BMP->ArrayOfFFT, 0.2*BMP->ArrayOfFFT->Width);
	delete ImageDFT;
	ImageDFT = BMP->SwitchComplexToCbmpArray(F_Filter);
	BMP->SaveAsGray(OutputPath+"频域滤波后频谱_qt.bmp", BMP->bmpHead, ImageDFT->Width, ImageDFT->Height, ImageDFT);
	//FFT2逆变换
	CComplexArray *IFFT_Filter = BMP->IFFT2(F_Filter);
	//CComplexArray *IFFT_Filter = BMP->IFFT2(BMP->ArrayOfFFT);

	//取逆变换实部 *pow(-1, i+j) 逆转方向
	CbmpArray *ImageDFT1 = new CbmpArray;
	ImageDFT1->GenerateArray(IFFT_Filter->Height, IFFT_Filter->Width);

	for(PIXEL i=0; i!=BMP->ArrayOfFFT->Height; ++i){
		for(PIXEL j=0; j!=BMP->ArrayOfFFT->Width; ++j){
			ImageDFT1->Array[i][j] = static_cast<COLOR>(IFFT_Filter->Array[i][j].real()*pow(-1, i+j));
		}
	}

	//BMP->SaveAsGray(OutputPath+"频域滤波扩展大小.bmp", BMP->bmpHead, ImageDFT1->Width, ImageDFT1->Height, ImageDFT1);
	delete ImageDFT;
	ImageDFT = BMP->Scale(ImageDFT1, 0, 0, BMP->Height, BMP->Width, 1);
	BMP->SaveAsGray(OutputPath+"频域滤波逆变换_qt.bmp", BMP->bmpHead, ImageDFT->Width, ImageDFT->Height, ImageDFT);
	delete ImageDFT,ImageDFT1;
	ImageDFT = NULL;
}

*/