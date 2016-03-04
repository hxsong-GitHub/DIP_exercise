#include "test_qt.h"
#include "fre.h"
#include "dip.h"
//ʹ�ö�̬����������ļ�
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

	//��������Լ�html��ʽ�ļ�
	
// 	QLabel *label = new QLabel (QStringLiteral("<h2><i>������</i><font color=red>�����ַ�</h2><h2><i>������</i><font color=yellow>�����ַ�</h2>"));
// 	label->setWindowTitle(QStringLiteral("��������"));
// 	label->show();

/********************   dip����   ********************/
	
	QMainWindow *dipWindow = new dip;
	dipWindow->show();



	//��ʾͼ��
	
	QString filename(QStringLiteral("G:\\2014�γ��ļ�\\����ͼ����\\�γ����\\lena256.bmp"));
// 	filename=QFileDialog::getOpenFileName(
// 		NULL,
// 		QStringLiteral("ѡ��ͼ��"),
// 		"",
//  		QStringLiteral("Images (*.png *.bmp *.jpg *.tif *.GIF )"));

// 	if(filename.isEmpty())	{
// 		return 0;
// 	}
// 	else	{
// 		QImage* img=new QImage;
// 		if(! ( img->load(filename) ) ) //����ͼ��
// 		{
// 			QMessageBox::information(
// 				NULL,
// 				QStringLiteral("��ͼ��ʧ��"),
// 				QStringLiteral("��ͼ��ʧ��!"));
// 			delete img;
// 			return 0;
// 		}
		//��ӹ�����
// 		QScrollArea *pArea= new QScrollArea(NULL);
// 		QLabel *labelImg = new QLabel(pArea);
// 
// 		//labelImg->setGeometry(100,100,400,400);//�ڴ��ڵ�(width,height,width2,height2)���򻭳��ò���
// 		labelImg->setGeometry(0,0,700,400);//����label������ռ�Ĵ�С��ͼ���֮����ʾ��������ͼ��С���⣬Ƕ��������
// 		labelImg->setWindowTitle(QStringLiteral("��ʾͼ��"));
// // 		labelImg->setMinimumHeight(img->height());
// // 		labelImg->setMinimumWidth(img->width());
// 		labelImg->setPixmap(QPixmap::fromImage(*img));//����Ҫ�����wdget������
// 		pArea->setWidget(labelImg);//�������ù�������qw��
// 
// 		pArea->show();

//	}


	//���봰�ڲ���   --  ��ui�ļ�����֮���ٵ���
// 	QDialog *testDialog = new fre;
// 	testDialog->setWindowTitle(QStringLiteral("��������"));
// 	testDialog->show();
// 	//��̬���봰�ڲ���--��ui�ļ�ֱ�ӵ���
// 	QUiLoader uiLoader;
// 	QFile file("UI_load.ui");
// 	//QDialog *loadDialog = uiLoader.load(&file);//����QWidget *���͵�ֵ�������ڳ�ʼ�� Dialog *���͵�ʵ��
// 	QWidget * loadWidget = uiLoader.load(&file);
// 	loadWidget->setWindowFlags(Qt::Dialog);
// 	loadWidget->show();


	//�¼��Լ��źš���
// 	QPushButton *button = new QPushButton(QStringLiteral("�˳�"));
// 	QObject::connect(button,SIGNAL(clicked()),&app,SLOT(quit()));
// 	button->show()

	//���ڲ����Ĳ���
	
// 	QWidget *window = new QWidget;
// 	window->setWindowTitle(QStringLiteral("��������"));
// 	//������Ҫ�Ĵ���
// 	QSlider *slider = new QSlider(Qt::Horizontal);	//���û��鷽��
// 	slider->setRange(0,128);
// 	QSpinBox * spin = new QSpinBox;					//΢����
// 	spin->setRange(0,128);
// 	//�����ź����
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
	std::string ImagePath("G:\\2014�γ��ļ�\\����ͼ����\\�γ����\\lena256.bmp");
	std::string OutputPath("G:\\2014�γ��ļ�\\����ͼ����\\�γ����\\");
	Cbmp *BMP = new Cbmp;
	BMP->Open(ImagePath);
	// 	//DFT
	CbmpArray *ImageDFT = BMP->FFT(BMP->Array_Gray);
	BMP->SaveAsGray(OutputPath+"FFT_qt.bmp", BMP->bmpHead, ImageDFT->Width, ImageDFT->Height, ImageDFT);

	//Ƶ���˲�
	CComplexArray *F_Filter = BMP->FFT_Filter(BMP->ArrayOfFFT, 0.2*BMP->ArrayOfFFT->Width);
	delete ImageDFT;
	ImageDFT = BMP->SwitchComplexToCbmpArray(F_Filter);
	BMP->SaveAsGray(OutputPath+"Ƶ���˲���Ƶ��_qt.bmp", BMP->bmpHead, ImageDFT->Width, ImageDFT->Height, ImageDFT);
	//FFT2��任
	CComplexArray *IFFT_Filter = BMP->IFFT2(F_Filter);
	//CComplexArray *IFFT_Filter = BMP->IFFT2(BMP->ArrayOfFFT);

	//ȡ��任ʵ�� *pow(-1, i+j) ��ת����
	CbmpArray *ImageDFT1 = new CbmpArray;
	ImageDFT1->GenerateArray(IFFT_Filter->Height, IFFT_Filter->Width);

	for(PIXEL i=0; i!=BMP->ArrayOfFFT->Height; ++i){
		for(PIXEL j=0; j!=BMP->ArrayOfFFT->Width; ++j){
			ImageDFT1->Array[i][j] = static_cast<COLOR>(IFFT_Filter->Array[i][j].real()*pow(-1, i+j));
		}
	}

	//BMP->SaveAsGray(OutputPath+"Ƶ���˲���չ��С.bmp", BMP->bmpHead, ImageDFT1->Width, ImageDFT1->Height, ImageDFT1);
	delete ImageDFT;
	ImageDFT = BMP->Scale(ImageDFT1, 0, 0, BMP->Height, BMP->Width, 1);
	BMP->SaveAsGray(OutputPath+"Ƶ���˲���任_qt.bmp", BMP->bmpHead, ImageDFT->Width, ImageDFT->Height, ImageDFT);
	delete ImageDFT,ImageDFT1;
	ImageDFT = NULL;
}

*/