#include "dip.h"


#include <QMessageBox>
#include <QRadioButton>
#include <QLineEdit>
#include <QFileDialog>
#include <QScrollArea>
#include <QLayout>
#include <fstream>
#include <string>

dip::dip(QWidget *parent /* = 0 */)
	: QMainWindow(parent), isFileEmpty(0), ifFFT(0), isImgOpen(0), isFirstRun(0), imgType(true),
	flag(0), defaultSavePath(QStringLiteral("defaultSave").toStdString())
{
	ui.setupUi(this);

	//初始化成员变量
	BMP = new Cbmp;
	BMPOutput = new Cbmp;
	BMPSource = new Cbmp;
	imgSource = new QImage;
	imgOutput = new QImage;
	labelSource = new myLabel(this);
	labelOutput = new myLabel(this);

	labelSource->setMaximumHeight(1);
	labelSource->setMaximumWidth(1);
	labelOutput->setMaximumHeight(1);
	labelOutput->setMaximumWidth(1);
	ui.sourceScrollArea->setWidget(labelSource);
	ui.outputSrcollArea->setWidget(labelOutput);


	//初始化隐藏区域
	ResetParameters();
	ui.imgType->setVisible(false);
	ui.selectGroup->setVisible(true);
	ui.resetButton->setVisible(true);
	ui.okButton->setVisible(false);
	connect(ui.resetButton, SIGNAL(clicked()), this, SLOT(ImgOpen()));

	//设定各连接
		//File部分连接
	QList<QAction*> fileMenuActions = ui.fileMenu->actions();
	connect(fileMenuActions[0], SIGNAL(triggered()), this, SLOT(ImgOpen()));
	connect(fileMenuActions[1], SIGNAL(triggered()), this, SLOT(ImgSave()));
		//空域部分的连接
	QList<QAction*> pixelMenuActions = ui.pixelMenu->actions();
	connect(pixelMenuActions[0], SIGNAL(triggered()), this, SLOT(MirrorSlots()));
	connect(pixelMenuActions[1], SIGNAL(triggered()), this, SLOT(OffsetSlots()));
	connect(pixelMenuActions[2], SIGNAL(triggered()), this, SLOT(CropSlots()));
	connect(pixelMenuActions[3], SIGNAL(triggered()), this, SLOT(ScaleSlots()));
	connect(pixelMenuActions[4], SIGNAL(triggered()), this, SLOT(RotSlots()));
	connect(pixelMenuActions[5], SIGNAL(triggered()), this, SLOT(PixelFilterSlots()));
	connect(pixelMenuActions[6], SIGNAL(triggered()), this, SLOT(HistogramSlots()));
		//频率域部分的连接
	QList<QAction*> freMenuActions = ui.freMenu->actions();
	connect(freMenuActions[0], SIGNAL(triggered()), this, SLOT(FFTSlots()));
	connect(freMenuActions[1], SIGNAL(triggered()), this, SLOT(IFFTSlots()));
	connect(freMenuActions[2], SIGNAL(triggered()), this, SLOT(FreFilterSlots()));
	connect(freMenuActions[3], SIGNAL(triggered()), this, SLOT(DCTSlots()));
		//关于
	QList<QAction*> aboutMenuActions = ui.aboutMenu->actions();
	connect(aboutMenuActions[0], SIGNAL(triggered()), this, SLOT(AuthorSlots()));

		//设置图像处理方式
	connect(ui.radioImgType_1, SIGNAL(clicked()), this, SLOT(SetImgType()));
	connect(ui.radioImgType_2, SIGNAL(clicked()), this, SLOT(SetImgType()));

		//鼠标取色区域
	ui.labelColor_1->setVisible(false);
	ui.labelColor_2->setVisible(false);

}
dip::~dip()
{
	delete BMP, BMPOutput, BMPSource;
}

bool dip::IsFileEmpty()
{
	return isFileEmpty;
}
bool dip::IfFFT()
{
	return ifFFT;
}
bool dip::IsImgOpen()
{
	return isImgOpen;
}



void dip::IsFirstRun()
{
	if(!IsImgOpen()){
		QMessageBox::information(this, QStringLiteral("不可执行"), QStringLiteral("不存在已打开的文件!"));
		return;
	}
	if(isFirstRun){
		return;
	}
	isFirstRun = true;
	//不是初次运行，根据是否彩色操作 处理图像
		//利用第一次输出的灰度图片更改源图像窗口
	ui.sourceScrollArea->setWidget(labelOutput);//setWidget将Label作为其子对象，更换对象时清除“所记录的子对象地址” --> 清除imgSource
	labelSource = new myLabel(this);
	connect(labelSource, SIGNAL(clicked()), this, SLOT(GetColor()));
	imgSource = imgOutput;
	
	labelSource->setMinimumHeight(imgSource->height());
	labelSource->setMinimumWidth(imgSource->width());
	labelSource->setMaximumHeight(imgSource->height());
	labelSource->setMaximumWidth(imgSource->width());
	labelSource->setPixmap(QPixmap::fromImage(*imgSource));//必须要把这个wdget画出来
	ui.sourceScrollArea->setWidget(labelSource);	//--> 清除labelOutput
	imgOutput = new QImage;
	labelOutput = new myLabel(this);
	
}

void dip::DisplayOutputImg(std::string filePath)
{
	QString filename = QString::fromLocal8Bit(filePath.c_str());
	BMPOutput->Open(filePath);
	if(! ( imgOutput->load(filename) ) ) //加载图像
	{
		QMessageBox::information(this, QStringLiteral("读取图片失败"), QStringLiteral("读取图片失败!"));
		delete imgOutput;
		return;
	}
	//显示于输出区域
	/*添加滚动条*/
	labelOutput->setWindowTitle(QStringLiteral("显示图像"));
	labelOutput->setMinimumHeight(imgOutput->height());
	labelOutput->setMinimumWidth(imgOutput->width());
	labelOutput->setMaximumHeight(imgOutput->height());
	labelOutput->setMaximumWidth(imgOutput->width());
	labelOutput->setPixmap(QPixmap::fromImage(*imgOutput));//必须要把这个wdget画出来
	ui.outputSrcollArea->setWidget(labelOutput);

	//添加取色事件
	connect(labelOutput, SIGNAL(clicked()), this, SLOT(GetColor_1()));
	//labelOutput->setCursor(Qt::CrossCursor);//move里面设置
}


/***************			槽的定义			*******************/
void dip::DIPRun()
{
	//选择flag
	QString lineEditText[5];
	int radioValue(1);
	if(flag == OFFSET){
		//获取选择的模
		QButtonGroup *buttonGroup_1 = new QButtonGroup;
		buttonGroup_1->addButton(ui.radioButton_1, 0);
		buttonGroup_1->addButton(ui.radioButton_2, 1);
		radioValue = buttonGroup_1->checkedId();

		//获取输入数
		QList<QLineEdit*>	lineEidtChildren = ui.inputGroup->findChildren<QLineEdit*>();
		lineEditText[0] = lineEidtChildren[0]->text();
		lineEditText[1] = lineEidtChildren[1]->text();
		lineEditText[2] = lineEidtChildren[2]->text();
		if(lineEditText[0].isEmpty() || lineEditText[1].isEmpty() || lineEditText[2].isEmpty()){
			QMessageBox::information(this, QStringLiteral("不可执行"), QStringLiteral("输入不能为空!"));
			return;
		}
		Offset(lineEditText[0].toInt(), lineEditText[1].toInt(), radioValue , lineEditText[2].toInt() );

	}
	else if(flag == CROP){
		//获取选择的模式

		//获取输入数
		QList<QLineEdit*>	lineEidtChildren = ui.inputGroup->findChildren<QLineEdit*>();
		for(int i=0; i!=5; ++i){
			lineEditText[i] = lineEidtChildren[i]->text();
		}

		if(lineEditText[0].isEmpty() || lineEditText[1].isEmpty() || lineEditText[2].isEmpty() ||
			lineEditText[3].isEmpty() || lineEditText[4].isEmpty()){
				QMessageBox::information(this, QStringLiteral("不可执行"), QStringLiteral("输入不能为空!"));
				return;
		}
		Crop(lineEditText[0].toInt(), lineEditText[1].toInt(), lineEditText[2].toInt() ,
			lineEditText[3].toInt(), lineEditText[4].toInt());
	}
	else if(flag == SCALE){
		//获取选择的模
		QButtonGroup *buttonGroup_1 = new QButtonGroup;
		buttonGroup_1->addButton(ui.radioButton_1, 0);
		buttonGroup_1->addButton(ui.radioButton_2, 1);
		radioValue = buttonGroup_1->checkedId();

		//获取输入数
		QList<QLineEdit*>	lineEidtChildren = ui.inputGroup->findChildren<QLineEdit*>();
		for(int i=0; i!=4; ++i){
			lineEditText[i] = lineEidtChildren[i]->text();
		}

		if(lineEditText[0].isEmpty() || lineEditText[1].isEmpty() || lineEditText[2].isEmpty() ||
			lineEditText[3].isEmpty() ){
				QMessageBox::information(this, QStringLiteral("不可执行"), QStringLiteral("输入不能为空!"));
				return;
		}
		if(lineEditText[0].toDouble()>10 || lineEditText[1].toDouble()>10){
			QMessageBox::information(this, QStringLiteral("不可执行"), QStringLiteral("缩放比例过大!"));
			return;
		}
		Scale(lineEditText[0].toDouble(), lineEditText[1].toDouble(), lineEditText[2].toInt() ,
			lineEditText[3].toInt(), radioValue);
	}
	else if(flag == ROT){
		//获取选择的模
		QButtonGroup *buttonGroup_1 = new QButtonGroup;
		buttonGroup_1->addButton(ui.radioButton_1, 0);
		buttonGroup_1->addButton(ui.radioButton_2, 1);
		radioValue = buttonGroup_1->checkedId();

		//获取输入数
		QList<QLineEdit*>	lineEidtChildren = ui.inputGroup->findChildren<QLineEdit*>();
		for(int i=0; i!=4; ++i){
			lineEditText[i] = lineEidtChildren[i]->text();
		}

		if(lineEditText[0].isEmpty() || lineEditText[1].isEmpty() || lineEditText[2].isEmpty() ||
			lineEditText[3].isEmpty() ){
				QMessageBox::information(this, QStringLiteral("不可执行"), QStringLiteral("输入不能为空!"));
				return;
		}
		if(lineEditText[0].toInt()>BMP->Array_Gray->Width-1 || lineEditText[1].toInt()>BMP->Array_Gray->Height-1){
			QMessageBox::information(this, QStringLiteral("不可执行"), QStringLiteral("旋转点超出界限!"));
			return;
		}
		Rot(lineEditText[0].toInt(), lineEditText[1].toInt(), lineEditText[3].toDouble()*M_PI/180, radioValue,
			lineEditText[2].toInt());
	}
	else if(flag == FILTER){
		//获取选择的模
		QButtonGroup *buttonGroup_1 = new QButtonGroup;
		buttonGroup_1->addButton(ui.radioButton_1, 1);
		buttonGroup_1->addButton(ui.radioButton_2, 2);
		buttonGroup_1->addButton(ui.radioButton_3, 3);
		buttonGroup_1->addButton(ui.radioButton_4, 4);
		radioValue = buttonGroup_1->checkedId();

		//下拉菜单部分
		QButtonGroup *buttonGroup_2 = new QButtonGroup;
		buttonGroup_2->addButton(ui.radioButton_5, 1);
		buttonGroup_2->addButton(ui.radioButton_6, 2);
		buttonGroup_2->addButton(ui.radioButton_7, 3);
		int radioValue2 = buttonGroup_2->checkedId();

		//获取输入数

		PixelFilter(radioValue, radioValue2/* =0 */);
	}
	else if(flag == FLAGFreFilter){
		//获取选择的模式
		QButtonGroup *buttonGroup_1 = new QButtonGroup;
		buttonGroup_1->addButton(ui.radioButton_1, 1);
		buttonGroup_1->addButton(ui.radioButton_2, 5);
		radioValue = buttonGroup_1->checkedId();

		//获取下拉菜单
		int radioValue1;
		QButtonGroup *buttonGroup_2 = new QButtonGroup;
		buttonGroup_2->addButton(ui.radioButton_5, 0);
		buttonGroup_2->addButton(ui.radioButton_6, 1);
		buttonGroup_2->addButton(ui.radioButton_7, 2);
		buttonGroup_2->addButton(ui.radioButton_8, 3);
		radioValue1 = buttonGroup_2->checkedId();

		radioValue += radioValue1;

		//获取输入数
		QList<QLineEdit*>	lineEidtChildren = ui.inputGroup->findChildren<QLineEdit*>();
		lineEditText[0] = lineEidtChildren[0]->text();
		lineEditText[1] = "0";
		if(radioValue==4 || radioValue==8){
			lineEditText[1] = lineEidtChildren[1]->text();
		}

		if(lineEditText[0].isEmpty() || (lineEditText[1].isEmpty()&&radioValue==4) || (lineEditText[1].isEmpty()&&radioValue==8) ){
			QMessageBox::information(this, QStringLiteral("不可执行"), QStringLiteral("输入不能为空!"));
			return;
		}
		FreFilter(lineEditText[0].toDouble(), lineEditText[1].toDouble(), 1, radioValue);		
	}


}
void dip::ResetParameters()
{
	//重置参数窗口
	//单选菜单部分
	ui.radioGroup->setWindowTitle(QStringLiteral("单选"));	
	QList<QRadioButton*> radioChilren = ui.radioGroup->findChildren<QRadioButton*>();
	for(QList<QRadioButton*>::iterator i=radioChilren.begin(); i!=radioChilren.end(); ++i){
		(*i)->setText(QStringLiteral("default"));
		(*i)->setVisible(false);
		//删除相关连接
		disconnect((*i), SIGNAL(clicked()), 0, 0);
	}
	ui.radioGroup->setVisible(false);



	//下拉菜单部分
	ui.comboGroup->setWindowTitle(QStringLiteral("单选2"));
	QList<QRadioButton*> radioChilren1 = ui.comboGroup->findChildren<QRadioButton*>();
	for(QList<QRadioButton*>::iterator i=radioChilren1.begin(); i!=radioChilren1.end(); ++i){
		(*i)->setText(QStringLiteral("default"));
		(*i)->setVisible(false);
		disconnect((*i), SIGNAL(clicked()), 0, 0);
	}
	ui.comboGroup->setVisible(false);
	//输入菜单部分
	ui.inputGroup->setWindowTitle(QStringLiteral("输入"));
	QList<QLabel*> labelChildren = ui.inputGroup->findChildren<QLabel*>();
	for(QList<QLabel*>::iterator i=labelChildren.begin(); i!=labelChildren.end(); ++i){
		(*i)->setText("default");
		(*i)->setVisible(false);
	}
	QRegExp regExp("[-]{0,1}[0-9]{1,4}");
	QList<QLineEdit*> lineEditChildren = ui.inputGroup->findChildren<QLineEdit*>();
	for(QList<QLineEdit*>::iterator i=lineEditChildren.begin(); i!=lineEditChildren.end(); ++i){
		(*i)->setText("");
		(*i)->setValidator(new QRegExpValidator(regExp, NULL));
		(*i)->setVisible(false);
	}
	ui.inputGroup->setVisible(false);
	//选择菜单部分
	ui.selectGroup->setVisible(false);

}
void dip::ImgOpen()
{
	//获取图像
	QString filename;
	filename = QFileDialog::getOpenFileName(
		NULL,
		QStringLiteral("选择图像"),
		"",//这里是默认文件名
 		QStringLiteral("Images (*.bmp)"));//"Images (*.bmp *.jpg *.tif *.GIF *.cpp )"

	if(filename.isEmpty())	{
		return;
	}
	else	{
		
		isImgOpen = true;
		//打开源图片  -- 设置输入输出窗口对应图像
		//中文路径转换  std::string  <---->  Qstring
		std::string tran = filename.toLocal8Bit();
		filename = QString::fromLocal8Bit(tran.c_str());

		BMP->Open(tran);

		isImgOpen = true;

		//记录输入窗口的图像
		filename = defaultSavePath.c_str()+QStringLiteral("\\outputColor.bmp");
		tran = filename.toLocal8Bit();
		BMP->SaveAsColor(tran, BMP->bmpHead, BMP->Width, BMP->Height, BMP->Array_Blue, BMP->Array_Green, BMP->Array_Red);
		BMPSource->Open(tran);

		//加载到源图像窗口
		if(!( imgSource->load(filename) ) ) //加载图像
		{
			QMessageBox::information(this, QStringLiteral("打开图像失败"), QStringLiteral("打开图像失败!"));
			delete imgSource;
			return;
		}		
			/*添加到滚动条*/
		labelSource->setWindowTitle(QStringLiteral("显示图像"));
		labelSource->setMinimumHeight(imgSource->height());
		labelSource->setMinimumWidth(imgSource->width());
		labelSource->setMaximumHeight(imgSource->height());
		labelSource->setMaximumWidth(imgSource->width());
		labelSource->setPixmap(QPixmap::fromImage(*imgSource));//必须要把这个wdget画出来
		ui.sourceScrollArea->setWidget(labelSource);

		
		//记录输出窗口的图像
		filename = defaultSavePath.c_str()+QStringLiteral("\\outputGray.bmp");
		tran = filename.toLocal8Bit();
		BMP->SaveAsGray(tran, BMP->bmpHead, BMP->Width, BMP->Height, BMP->Array_Gray);	
		BMPOutput->Open(tran);
		//加载到输出区域
		if(! ( imgOutput->load(filename) ) ) //加载图像
		{
			QMessageBox::information(this, QStringLiteral("读取图片失败"), QStringLiteral("读取图片失败!"));
			delete imgOutput;
			return;
		}
		/*添加滚动条*/
		labelOutput->setWindowTitle(QStringLiteral("显示图像"));
		labelOutput->setMinimumHeight(imgOutput->height());
		labelOutput->setMinimumWidth(imgOutput->width());
		labelOutput->setMaximumHeight(imgOutput->height());
		labelOutput->setMaximumWidth(imgOutput->width());
		labelOutput->setPixmap(QPixmap::fromImage(*imgOutput));//必须要把这个wdget画出来
		ui.outputSrcollArea->setWidget(labelOutput);

		//取色框区域绘制
		ui.colorGrid->setTitle(QStringLiteral("取色框-[%1,%2]").arg(BMP->Height).arg(BMP->Width));
		//取色信号连接
		connect(labelSource, SIGNAL(clicked()), this, SLOT(GetColor()));
		connect(labelOutput, SIGNAL(clicked()), this, SLOT(GetColor_1()));
		//labelSource->setCursor(Qt::CrossCursor);
		//labelOutput->setCursor(Qt::CrossCursor);

		//首次打开图像图标隐藏
		ui.selectGroup->setVisible(false);
		ui.resetButton->setVisible(false);
		ui.imgType->setVisible(true);
		//清除标志
		isFirstRun = false;
		flag = 0;
	}
}
void dip::ImgSave()
{
	if(!IsImgOpen()){
		QMessageBox::information(this, QStringLiteral("不可执行"), QStringLiteral("不存在已打开的文件!"));
		return;
	}
	QString filename;
	filename=QFileDialog::getSaveFileName(
		NULL,
		QStringLiteral("另存为"),
		"",
		QStringLiteral("Images (*.bmp)"));
	if(filename.isEmpty()){
		return;
	}
	std::string tran = filename.toLocal8Bit();
	if(imgType){
		BMPOutput->SaveAsGray(tran, BMPOutput->bmpHead, BMPOutput->Width, BMPOutput->Height, BMP->Array_Gray);
	}
	else{
		BMPOutput->SaveAsColor(tran, BMPOutput->bmpHead, BMPOutput->Width, BMPOutput->Height, BMP->Array_Blue, BMP->Array_Green, BMP->Array_Red);
	}
	QMessageBox::information(this, QStringLiteral("保存"), QStringLiteral("已保存!"));
}
void dip::SetImgType()
{
	QButtonGroup *buttonGroup = new QButtonGroup;
	buttonGroup->addButton(ui.radioImgType_1, 0);
	buttonGroup->addButton(ui.radioImgType_2, 1);
	imgType = buttonGroup->checkedId();
	delete buttonGroup;
}

void dip::MirrorSlots() //槽函数完成窗口变换以及其他定义
{
	//IsFirstRun();
	if(!IsImgOpen()){
		QMessageBox::information(this, QStringLiteral("不可执行"), QStringLiteral("不存在已打开的文件!"));
		return;
	}
	ResetParameters();
	flag = 0;
	Mirror();
}
void dip::Mirror()
{
	std::string savePath(defaultSavePath + QStringLiteral("\\outputMirror.bmp").toStdString());
	//判定是彩色还是灰度	
	if(imgType){
		CbmpArray *ImageMirror = BMP->Mirror(BMP->Array_Gray, 0);	
		BMP->SaveAsGray(savePath, BMP->bmpHead, BMP->Width, BMP->Height, ImageMirror);
		delete ImageMirror;
	}
	else{
		CbmpArray *ImageB = BMP->Mirror(BMP->Array_Blue, 0);
		CbmpArray *ImageG = BMP->Mirror(BMP->Array_Green, 0);
		CbmpArray *ImageR = BMP->Mirror(BMP->Array_Red, 0);
		BMP->SaveAsColor(savePath, BMP->bmpHead, BMP->Width, BMP->Height, ImageB, ImageG, ImageR);
		delete ImageB, ImageG, ImageR;
	}
	//读入输出的图像
	BMPSource->Open(savePath);
	DisplayOutputImg(savePath);
}

void dip::OffsetSlots()
{
	//IsFirstRun();
	if(!IsImgOpen()){
		QMessageBox::information(this, QStringLiteral("不可执行"), QStringLiteral("不存在已打开的文件!"));
		return;
	}
	ResetParameters();
	flag = OFFSET;
//设置需要的窗口
	//单选菜单
	ui.radioGroup->setTitle(QStringLiteral("平移模式"));
	ui.radioGroup->setVisible(true);
	ui.radioButton_1->setText(QStringLiteral("保留原大小"));
	ui.radioButton_2->setText(QStringLiteral("超出部分填色"));
	ui.radioButton_2->setChecked(true);
	ui.radioButton_1->setVisible(true);
	ui.radioButton_2->setVisible(true);
	//下拉菜单
	
	//输入菜单
	ui.inputGroup->setTitle(QStringLiteral("平移量x→,y↑,左下原点"));
	ui.inputGroup->setVisible(true);
	QList<QLabel*>		labelChildren	 = ui.inputGroup->findChildren<QLabel*>();
	QList<QLineEdit*>	lineEidtChildren = ui.inputGroup->findChildren<QLineEdit*>();
	labelChildren[0]->setText(QStringLiteral("宽度 x"));
	labelChildren[1]->setText(QStringLiteral("高度 y"));
	labelChildren[2]->setText(QStringLiteral("填充色"));
	labelChildren[0]->setVisible(true);
	labelChildren[1]->setVisible(true);
	labelChildren[2]->setVisible(true);
	lineEidtChildren[0]->setVisible(true);
	lineEidtChildren[1]->setVisible(true);
	lineEidtChildren[2]->setVisible(true);

	//选择菜单
	ui.selectGroup->setVisible(true);

	//Offset();
}
void dip::Offset(int x_0, int y_0, bool mode/* =false */, COLOR fill/* =0 */)
{
	std::string savePath(defaultSavePath + QStringLiteral("\\outputOffset.bmp").toStdString());

	//判定是彩色还是灰度	
	if(imgType){
		//旋转参数等待输入
		CbmpArray *ImageOffset = BMP->Offset(BMP->Array_Gray, x_0, y_0, mode, fill);
		BMP->SaveAsGray(savePath, BMP->bmpHead, ImageOffset->Width, ImageOffset->Height, ImageOffset);
		delete ImageOffset;
	}
	else{
		CbmpArray *ImageB = BMP->Offset(BMP->Array_Blue, x_0, y_0, mode, fill);
		CbmpArray *ImageG = BMP->Offset(BMP->Array_Green, x_0, y_0, mode, fill);
		CbmpArray *ImageR = BMP->Offset(BMP->Array_Red, x_0, y_0, mode, fill);
		BMP->SaveAsColor(savePath, BMP->bmpHead, ImageB->Width, ImageB->Height, ImageB, ImageG, ImageR);
		delete ImageB, ImageG, ImageR;
	}
	
	//读入输出的图像  载入
	BMPSource->Open(savePath);
	DisplayOutputImg(savePath);
}

void dip::CropSlots()
{
	//IsFirstRun();
	if(!IsImgOpen()){
		QMessageBox::information(this, QStringLiteral("不可执行"), QStringLiteral("不存在已打开的文件!"));
		return;
	}
	ResetParameters();
	flag = CROP;
//设置需要的菜单选项
	//单选菜单   -->   不需要
	
	//下拉菜单   -->   不需要

	//输入菜单
	ui.inputGroup->setTitle(QStringLiteral("剪切范围x→,y↑,左下原点"));
	ui.inputGroup->setVisible(true);
	QList<QLabel*>		labelChildren	 = ui.inputGroup->findChildren<QLabel*>();
	QList<QLineEdit*>	lineEidtChildren = ui.inputGroup->findChildren<QLineEdit*>();
	labelChildren[0]->setText(QStringLiteral("起点 x0"));
	labelChildren[1]->setText(QStringLiteral("起点 y0"));
	labelChildren[2]->setText(QStringLiteral("终点 x1"));
	labelChildren[3]->setText(QStringLiteral("终点 y1"));
	labelChildren[4]->setText(QStringLiteral("填充色"));
	for(int i=0; i!=5; ++i){
		labelChildren[i]->setVisible(true);
		lineEidtChildren[i]->setVisible(true);
	}

	//选择菜单
	ui.selectGroup->setVisible(true);

	//Crop();
}
void dip::Crop(int x1, int y1, int x2, int y2, COLOR fill/* =0 */)
{
	std::string savePath(defaultSavePath + QStringLiteral("\\outputCrop.bmp").toStdString());
	
	//判定是彩色还是灰度	
	if(imgType){
		//旋转参数等待输入
		CbmpArray *ImageCrop = BMP->Crop(BMP->Array_Gray, x1, y1, x2, y2, fill);
		BMP->SaveAsGray(savePath, BMP->bmpHead, ImageCrop->Width, ImageCrop->Height, ImageCrop);
		delete ImageCrop;
	}
	else{
		CbmpArray *ImageB = BMP->Crop(BMP->Array_Blue, x1, y1, x2, y2, fill);
		CbmpArray *ImageG = BMP->Crop(BMP->Array_Green, x1, y1, x2, y2, fill);
		CbmpArray *ImageR = BMP->Crop(BMP->Array_Red, x1, y1, x2, y2, fill);
		BMP->SaveAsColor(savePath, BMP->bmpHead, ImageB->Width, ImageB->Height, ImageB, ImageG, ImageR);
		delete ImageB, ImageG, ImageR;
	}

	//读入输出的图像  载入
	BMPSource->Open(savePath);
	DisplayOutputImg(savePath);
}

void dip::ScaleSlots()
{
	//IsFirstRun();
	if(!IsImgOpen()){
		QMessageBox::information(this, QStringLiteral("不可执行"), QStringLiteral("不存在已打开的文件!"));
		return;
	}
	ResetParameters();
	flag = SCALE;

	//单选菜单   -->   不需要
	ui.radioGroup->setTitle(QStringLiteral("插值方式"));
	ui.radioGroup->setVisible(true);
	ui.radioButton_1->setText(QStringLiteral("最近邻插值"));
	ui.radioButton_2->setText(QStringLiteral("双线性插值"));
	ui.radioButton_2->setChecked(true);
	ui.radioButton_1->setVisible(true);
	ui.radioButton_2->setVisible(true);
	//下拉菜单   -->   不需要

	//输入菜单
	ui.inputGroup->setTitle(QStringLiteral("参数-优先按像素缩放，其次比例"));
	ui.inputGroup->setVisible(true);
	QList<QLabel*>		labelChildren	 = ui.inputGroup->findChildren<QLabel*>();
	QList<QLineEdit*>	lineEidtChildren = ui.inputGroup->findChildren<QLineEdit*>();
	labelChildren[0]->setText(QStringLiteral("x方向比例"));
	labelChildren[1]->setText(QStringLiteral("y方向比例"));
	labelChildren[2]->setText(QStringLiteral("x方向像素"));
	labelChildren[3]->setText(QStringLiteral("y方向像素"));
	//设置正则条件
	QRegExp regExp1("[0-9][.]{0,1}[0-9]{0,2}");
	lineEidtChildren[0]->setValidator(new QRegExpValidator(regExp1, NULL));
	lineEidtChildren[1]->setValidator(new QRegExpValidator(regExp1, NULL));
	QRegExp regExp2("[0-9]{1,4}");
	lineEidtChildren[2]->setValidator(new QRegExpValidator(regExp2, NULL));
	lineEidtChildren[3]->setValidator(new QRegExpValidator(regExp2, NULL));
	//可见
	for(int i=0; i!=4; ++i){
		labelChildren[i]->setVisible(true);
		lineEidtChildren[i]->setVisible(true);
	}

	//选择菜单
	ui.selectGroup->setVisible(true);

	//Scale();
}
void dip::Scale(double nscale_x, double nscale_y, PIXEL nheight/* =0 */, PIXEL nwidth/* =0 */, bool resampling/* =false */)
{
	std::string savePath(defaultSavePath + QStringLiteral("\\outputScale.bmp").toStdString());

	//判定是彩色还是灰度	
	if(imgType){
		//旋转参数等待输入
		CbmpArray *ImageScale = BMP->Scale(BMP->Array_Gray, nscale_x, nscale_y, nheight, nwidth, resampling);
		BMP->SaveAsGray(savePath, BMP->bmpHead, ImageScale->Width, ImageScale->Height, ImageScale);
		delete ImageScale;
	}
	else{
		CbmpArray *ImageB = BMP->Scale(BMP->Array_Blue, nscale_x, nscale_y, nheight, nwidth, resampling);
		CbmpArray *ImageG = BMP->Scale(BMP->Array_Green, nscale_x, nscale_y, nheight, nwidth, resampling);
		CbmpArray *ImageR = BMP->Scale(BMP->Array_Red, nscale_x, nscale_y, nheight, nwidth, resampling);
		BMP->SaveAsColor(savePath, BMP->bmpHead, ImageB->Width, ImageB->Height, ImageB, ImageG, ImageR);
		delete ImageB, ImageG, ImageR;
	}

	//读入输出的图像  载入
	BMPSource->Open(savePath);
	DisplayOutputImg(savePath);

}

void dip::RotSlots()
{
	//IsFirstRun();
	if(!IsImgOpen()){
		QMessageBox::information(this, QStringLiteral("不可执行"), QStringLiteral("不存在已打开的文件!"));
		return;
	}
	ResetParameters();
	flag = ROT;

	//单选菜单   -->   不需要
	ui.radioGroup->setTitle(QStringLiteral("插值方式"));
	ui.radioGroup->setVisible(true);
	ui.radioButton_1->setText(QStringLiteral("最近邻插值"));
	ui.radioButton_2->setText(QStringLiteral("双线性插值"));
	ui.radioButton_2->setChecked(true);
	ui.radioButton_1->setVisible(true);
	ui.radioButton_2->setVisible(true);
	//下拉菜单   -->   不需要

	//输入菜单
	ui.inputGroup->setTitle(QStringLiteral("旋转参数"));
	ui.inputGroup->setVisible(true);
	QList<QLabel*>		labelChildren	 = ui.inputGroup->findChildren<QLabel*>();
	QList<QLineEdit*>	lineEidtChildren = ui.inputGroup->findChildren<QLineEdit*>();
	labelChildren[0]->setText(QStringLiteral("旋转中心x"));
	labelChildren[1]->setText(QStringLiteral("旋转中心y"));
	labelChildren[2]->setText(QStringLiteral("填充像素"));
	labelChildren[3]->setText(QStringLiteral("旋转角度"));
	//设置正则条件
	QRegExp regExp1("[0-9]{0,4}");
	lineEidtChildren[0]->setValidator(new QRegExpValidator(regExp1, NULL));
	lineEidtChildren[1]->setValidator(new QRegExpValidator(regExp1, NULL));
	lineEidtChildren[2]->setValidator(new QRegExpValidator(regExp1, NULL));
	lineEidtChildren[3]->setValidator(new QRegExpValidator(regExp1, NULL));
	//可见数目
	for(int i=0; i!=4; ++i){
		labelChildren[i]->setVisible(true);
		lineEidtChildren[i]->setVisible(true);
	}

	//选择菜单
	ui.selectGroup->setVisible(true);


	//Rot();
}
void dip::Rot(PIXEL x_0/* =0 */, PIXEL y_0/* =0 */, double degree/* =M_PI_2 */, bool resampling/* =0 */, COLOR fill/* =0 */)
{
	std::string savePath(defaultSavePath + QStringLiteral("\\outputRot.bmp").toStdString());

	//判定是彩色还是灰度	
	if(imgType){
		//旋转参数等待输入
		CbmpArray *ImageRot = BMP->Rot(BMP->Array_Gray, x_0, y_0, degree, resampling, fill);
		BMP->SaveAsGray(savePath, BMP->bmpHead, ImageRot->Width, ImageRot->Height, ImageRot);
		delete ImageRot;
	}
	else{
		CbmpArray *ImageB = BMP->Rot(BMP->Array_Blue, x_0, y_0, degree, resampling, fill);
		CbmpArray *ImageG = BMP->Rot(BMP->Array_Green, x_0, y_0, degree, resampling, fill);
		CbmpArray *ImageR = BMP->Rot(BMP->Array_Red, x_0, y_0, degree, resampling, fill);
		BMP->SaveAsColor(savePath, BMP->bmpHead, ImageB->Width, ImageB->Height, ImageB, ImageG, ImageR);
		delete ImageB, ImageG, ImageR;
	}

	//读入输出的图像  载入
	BMPSource->Open(savePath);
	DisplayOutputImg(savePath);

}


void dip::PixelFilterSlots()
{
	//IsFirstRun();
	if(!IsImgOpen()){
		QMessageBox::information(this, QStringLiteral("不可执行"), QStringLiteral("不存在已打开的文件!"));
		return;
	}
	ResetParameters();
	flag = FILTER;

	//频率使用时候单选2的连接
	connect(ui.radioButton_1, SIGNAL(clicked()), this, SLOT(ResetFilter()));
	connect(ui.radioButton_2, SIGNAL(clicked()), this, SLOT(ResetFilter()));
	connect(ui.radioButton_3, SIGNAL(clicked()), this, SLOT(ResetFilter()));
	connect(ui.radioButton_4, SIGNAL(clicked()), this, SLOT(SetFilter()));

	//单选菜单   -->   不需要
	ui.radioGroup->setTitle(QStringLiteral("滤波方式"));
	ui.radioGroup->setVisible(true);
	ui.radioButton_1->setText(QStringLiteral("低通滤波"));
	ui.radioButton_2->setText(QStringLiteral("中值滤波"));
	ui.radioButton_3->setText(QStringLiteral("高通滤波"));
	ui.radioButton_4->setText(QStringLiteral("微分滤波"));
	ui.radioButton_1->setChecked(true);
	ui.radioButton_1->setVisible(true);
	ui.radioButton_2->setVisible(true);
	ui.radioButton_3->setVisible(true);
	ui.radioButton_4->setVisible(true);

	//下拉菜单   -->   不需要未定义微分滤波器前不需要

	//输入菜单

	//可见数目

	//选择菜单
	ui.selectGroup->setVisible(true);
}
void dip::PixelFilter(char mode, char diff_mode/* =0 */)
{
	std::string savePath(defaultSavePath + QStringLiteral("\\outputPixelFilter.bmp").toStdString());

	//判定是彩色还是灰度	
	if(imgType){
		//旋转参数等待输入
		CbmpArray *ImageFilter = BMP->Filter(BMP->Array_Gray, mode, diff_mode, 0, 0, 0, 255);
		BMP->SaveAsGray(savePath, BMP->bmpHead, ImageFilter->Width, ImageFilter->Height, ImageFilter);
		delete ImageFilter;
	}
	else{
		CbmpArray *ImageB = BMP->Filter(BMP->Array_Blue, mode, diff_mode, 0, 0, 0, 255);
		CbmpArray *ImageG = BMP->Filter(BMP->Array_Green, mode, diff_mode, 0, 0, 0, 255);
		CbmpArray *ImageR = BMP->Filter(BMP->Array_Red, mode, diff_mode, 0, 0, 0, 255);
		BMP->SaveAsColor(savePath, BMP->bmpHead, ImageB->Width, ImageB->Height, ImageB, ImageG, ImageR);
		delete ImageB, ImageG, ImageR;
	}

	//读入输出的图像  载入
	BMPSource->Open(savePath);
	DisplayOutputImg(savePath);

}

void dip::HistogramSlots()
{
	//IsFirstRun();
	if(!IsImgOpen()){
		QMessageBox::information(this, QStringLiteral("不可执行"), QStringLiteral("不存在已打开的文件!"));
		return;
	}
	ResetParameters();
	flag = HISTOGRAM;
	Histogram();
}
void dip::Histogram()
{
	std::string savePath(defaultSavePath + QStringLiteral("\\outputHistogram.bmp").toStdString());

	//判定是彩色还是灰度	
	if(imgType){
		//旋转参数等待输入
		CbmpArray *ImageHistogram = BMP->Histogram(BMP->Array_Gray, defaultSavePath + QStringLiteral("\\").toStdString());
		BMP->SaveAsGray(savePath, BMP->bmpHead, ImageHistogram->Width, ImageHistogram->Height, ImageHistogram);
		delete ImageHistogram;
	}
	else{
		CbmpArray *ImageB = BMP->Histogram(BMP->Array_Blue, defaultSavePath + QStringLiteral("\\").toStdString());
		CbmpArray *ImageG = BMP->Histogram(BMP->Array_Green, defaultSavePath + QStringLiteral("\\").toStdString());
		CbmpArray *ImageR = BMP->Histogram(BMP->Array_Red, defaultSavePath + QStringLiteral("\\").toStdString());
		BMP->SaveAsColor(savePath, BMP->bmpHead, ImageB->Width, ImageB->Height, ImageB, ImageG, ImageR);
		delete ImageB, ImageG, ImageR;
	}

	//读入输出的图像  载入
	BMPSource->Open(savePath);
	DisplayOutputImg(savePath);


	//额外输出两个离散的窗口图像  --  直方图对比图
	QGroupBox *histogram = new QGroupBox(NULL);
	QHBoxLayout *layoutHis = new QHBoxLayout;
	QLabel *preHis =new QLabel;
	QLabel *afterHis =new QLabel;
	QImage *input1 = new QImage;
	QImage *input2 = new QImage;
	QString filename1 = QStringLiteral("%1\\OldHistogram.bmp").arg(defaultSavePath.c_str());
	QString filename2 = QStringLiteral("%1\\NewHistogram.bmp").arg(defaultSavePath.c_str());

	histogram->setWindowTitle(QStringLiteral("直方图比较"));

	input1->load(filename1); //加载图像
	preHis->setMinimumHeight(input1->height());
	preHis->setMinimumWidth(input1->width());
	preHis->setPixmap(QPixmap::fromImage(*input1));

	input2->load(filename2); //加载图像
	afterHis->setMinimumHeight(input2->height());
	afterHis->setMinimumWidth(input2->width());
	afterHis->setPixmap(QPixmap::fromImage(*input2));

	layoutHis->addWidget(preHis);
	layoutHis->addStretch();
	layoutHis->addWidget(afterHis);
	histogram->setLayout(layoutHis);
	histogram->show();
}

void dip::FFTSlots()
{
	//IsFirstRun();
	if(!IsImgOpen()){
		QMessageBox::information(this, QStringLiteral("不可执行"), QStringLiteral("不存在已打开的文件!"));
		return;
	}
	ResetParameters();
	flag = FLAGFFT;
	FFT();
	ui.radioImgType_2->setChecked(true);
}
void dip::FFT()
{
	std::string savePath(defaultSavePath + QStringLiteral("\\outputFFT.bmp").toStdString());

	CbmpArray *ImageFFT = BMP->FFT(BMP->Array_Gray);
	BMP->SaveAsGray(savePath, BMP->bmpHead, ImageFFT->Width, ImageFFT->Height, ImageFFT);
	ifFFT = true;

	//载入输出的图像
	DisplayOutputImg(savePath);
}

void dip::IFFTSlots()
{
	//IsFirstRun();
	if(!IsImgOpen()){
		QMessageBox::information(this, QStringLiteral("不可执行"), QStringLiteral("不存在已打开的文件!"));
		return;
	}
	ResetParameters();
	if(!ifFFT){
		QMessageBox::information(this, QStringLiteral("不可执行"), QStringLiteral("未进行过傅里叶变换!"));
		return;
	}
	flag = FLAGIFFT;
	IFFT();
	ui.radioImgType_2->setChecked(true);
}
void dip::IFFT()
{
	std::string savePath(defaultSavePath + QStringLiteral("\\outputIFFT.bmp").toStdString());
	CComplexArray *IFFT_Filter = BMP->IFFT2(BMP->ArrayOfFFT);
	//取逆变换实部 *pow(-1, i+j) 逆转方向
	CbmpArray *ImageIFFT = new CbmpArray;
	ImageIFFT->GenerateArray(IFFT_Filter->Height, IFFT_Filter->Width);

	for(PIXEL i=0; i!=BMP->ArrayOfFFT->Height; ++i){
		for(PIXEL j=0; j!=BMP->ArrayOfFFT->Width; ++j){
			ImageIFFT->Array[i][j] = static_cast<COLOR>(IFFT_Filter->Array[i][j].real()*pow(-1, i+j));
		}
	}
	CbmpArray *ImageIFFT1 = BMP->Scale(ImageIFFT, 0, 0, BMP->Height, BMP->Width);
	BMP->SaveAsGray(savePath, BMP->bmpHead, ImageIFFT1->Width, ImageIFFT1->Height, ImageIFFT1);
	ifFFT = true;

	//载入输出的图像
	DisplayOutputImg(savePath);
}

void dip::FreFilterSlots()
{
	//IsFirstRun();
	if(!IsImgOpen()){
		QMessageBox::information(this, QStringLiteral("不可执行"), QStringLiteral("不存在已打开的文件!"));
		return;
	}
	ResetParameters();
	if(!ifFFT){
		QMessageBox::information(this, QStringLiteral("不可执行"), QStringLiteral("未进行过傅里叶变换!"));
		return;
	}
	flag = FLAGFreFilter;
	ResetParameters();

	//建立需要的连接
	connect(ui.radioButton_1, SIGNAL(clicked()), this, SLOT(SetLPF()));
	connect(ui.radioButton_2, SIGNAL(clicked()), this, SLOT(SetHPF()));

	//单选菜单   -->   不需要
	ui.radioGroup->setTitle(QStringLiteral("选择频率域滤波器"));
	ui.radioGroup->setVisible(true);
	ui.radioButton_1->setText(QStringLiteral("低通滤波器"));
	ui.radioButton_2->setText(QStringLiteral("高通滤波器"));
	ui.radioButton_1->setChecked(true);
	ui.radioButton_1->setVisible(true);
	ui.radioButton_2->setVisible(true);

	SetLPF();
	//下拉菜单   -->   不需要

	//输入菜单

	//设置正则条件

	//可见

	//选择菜单
	//ui.selectGroup->setVisible(true);
	//

}
void dip::FreFilter(double D0/* =0 */, double D1/* =0 */, int n/* =1 */, int filter/* =1 */)
{
	std::string savePath(defaultSavePath + QStringLiteral("\\outputFreFilter.bmp").toStdString());

	CComplexArray *ImageFFTFilter = BMP->FFT_Filter(BMP->ArrayOfFFT, D0, D1, n, filter);

	//输出优化后的频谱图
	CbmpArray *ret,*ret0;
	ret0 = BMP->SwitchComplexToCbmpArray(ImageFFTFilter,1);
	ret = BMP->Scale(ret0, 0, 0, BMP->Height, BMP->Width, 1);

	BMP->SaveAsGray(defaultSavePath + QStringLiteral("\\outputFreFilter1.bmp").toStdString(), BMP->bmpHead, ret->Width, ret->Height, ret);
	
	QLabel *FreFilter1 =new QLabel(NULL);
	QImage *input1 = new QImage(NULL);
	QString filename1 = defaultSavePath.c_str() + QStringLiteral("\\outputFreFilter1.bmp");
	FreFilter1->setWindowTitle(QStringLiteral("滤波后频谱图"));
	input1->load(filename1); //加载图像
	FreFilter1->setMinimumHeight(input1->height());
	FreFilter1->setMinimumWidth(input1->width());
	FreFilter1->setPixmap(QPixmap::fromImage(*input1));
	FreFilter1->show();


	//输出滤波后的逆变换
	CComplexArray *IFFT_Filter = BMP->IFFT2(ImageFFTFilter);
	//取逆变换实部 *pow(-1, i+j) 逆转方向
	CbmpArray *ImageIFFT = new CbmpArray;
	ImageIFFT->GenerateArray(IFFT_Filter->Height, IFFT_Filter->Width);

	for(PIXEL i=0; i!=BMP->ArrayOfFFT->Height; ++i){
		for(PIXEL j=0; j!=BMP->ArrayOfFFT->Width; ++j){
			ImageIFFT->Array[i][j] = static_cast<COLOR>(IFFT_Filter->Array[i][j].real()*pow(-1, i+j));
		}
	}
	CbmpArray *ImageIFFT1 = BMP->Scale(ImageIFFT, 0, 0, BMP->Height, BMP->Width);
	BMP->SaveAsGray(savePath, BMP->bmpHead, ImageIFFT1->Width, ImageIFFT1->Height, ImageIFFT1);

	//载入输出的图像
	DisplayOutputImg(savePath);
	ui.radioImgType_2->setChecked(true);

}

void dip::DCTSlots()
{
	//IsFirstRun();
	if(!IsImgOpen()){
		QMessageBox::information(this, QStringLiteral("不可执行"), QStringLiteral("不存在已打开的文件!"));
		return;
	}
	ResetParameters();
	flag = FLAGDCT;
	DCT();
	ui.radioImgType_2->setChecked(true);
}
void dip::DCT()
{
	std::string savePath(defaultSavePath + QStringLiteral("\\outputDCT.bmp").toStdString());
	CbmpArray *ImageFFT = BMP->DCT(BMP->Array_Gray);
	BMP->SaveAsGray(savePath, BMP->bmpHead, ImageFFT->Width, ImageFFT->Height, ImageFFT);

	//载入输出的图像
	DisplayOutputImg(savePath);
}

void dip::AuthorSlots()
{
	QLabel *author = new QLabel(NULL);
	author->setWindowTitle(QStringLiteral("小组成员"));
	QString textOutput("");
// 	QFile authorText("authorText.txt");
// 	authorText.open(QIODevice::ReadOnly);
// 	while (!authorText.atEnd()){
// 		textOutput += authorText.readLine() + "\r\n";
// 	}
	std::fstream fid_word;
	std::string author_text;
	fid_word.open("./authorText.txt", std::fstream::in);
	while(!fid_word.eof()){
		std::string temp;
		getline(fid_word, temp);
		author_text += temp;
	}


	textOutput = QString::fromLocal8Bit(author_text.c_str());
	author->setText(textOutput);
	author->show();
}

void dip::SetFilter()
{
	ui.comboGroup->setTitle(QStringLiteral("微分滤波器"));
	ui.comboGroup->setVisible(true);
	ui.radioButton_5->setText(QStringLiteral("PREWITT"));
	ui.radioButton_6->setText(QStringLiteral("SOBEL"));
	ui.radioButton_7->setText(QStringLiteral("LAPLACE"));
	ui.radioButton_5->setChecked(true);
	ui.radioButton_5->setVisible(true);
	ui.radioButton_6->setVisible(true);
	ui.radioButton_7->setVisible(true);
}
void dip::ResetFilter()
{
	ui.comboGroup->setVisible(false);
}

void dip::SetLPF()
{
	//删除旧连接
	QList<QRadioButton*> radioChilren1 = ui.comboGroup->findChildren<QRadioButton*>();
	for(QList<QRadioButton*>::iterator i=radioChilren1.begin(); i!=radioChilren1.end(); ++i){
		disconnect((*i), SIGNAL(clicked()), 0, 0);
	}

	ui.comboGroup->setTitle(QStringLiteral("低通滤波器"));
	ui.comboGroup->setVisible(true);
	ui.radioButton_5->setText(QStringLiteral("ILPF"));
	ui.radioButton_6->setText(QStringLiteral("BLPF"));
	ui.radioButton_7->setText(QStringLiteral("GLPF"));
	ui.radioButton_8->setText(QStringLiteral("TLPF"));
	ui.radioButton_5->setVisible(true);
	ui.radioButton_6->setVisible(true);
	ui.radioButton_7->setVisible(true);
	ui.radioButton_8->setVisible(true);

	ui.radioButton_5->setChecked(true);
	//输入菜单	初始化为低通的输入框
	SetILPF();

	//建立新连接   下拉2  -->  输入菜单
	connect(ui.radioButton_5, SIGNAL(clicked()), this, SLOT(SetILPF()));
	connect(ui.radioButton_6, SIGNAL(clicked()), this, SLOT(SetBLPF()));
	connect(ui.radioButton_7, SIGNAL(clicked()), this, SLOT(SetGLPF()));
	connect(ui.radioButton_8, SIGNAL(clicked()), this, SLOT(SetTLPF()));
	
}
void dip::SetHPF()
{
	//删除旧连接
	QList<QRadioButton*> radioChilren1 = ui.comboGroup->findChildren<QRadioButton*>();
	for(QList<QRadioButton*>::iterator i=radioChilren1.begin(); i!=radioChilren1.end(); ++i){
		disconnect((*i), SIGNAL(clicked()), 0, 0);
	}

	ui.comboGroup->setTitle(QStringLiteral("高通滤波器"));
	ui.comboGroup->setVisible(true);
	ui.radioButton_5->setText(QStringLiteral("IHPF"));
	ui.radioButton_6->setText(QStringLiteral("BHPF"));
	ui.radioButton_7->setText(QStringLiteral("GHPF"));
	ui.radioButton_8->setText(QStringLiteral("THPF"));
	ui.radioButton_5->setVisible(true);
	ui.radioButton_6->setVisible(true);
	ui.radioButton_7->setVisible(true);
	ui.radioButton_8->setVisible(true);

	ui.radioButton_5->setChecked(true);
	//输入菜单	初始化为高通的输入框
	SetIHPF();
	//建立新连接   下拉2  -->  输入菜单
	connect(ui.radioButton_5, SIGNAL(clicked()), this, SLOT(SetIHPF()));
	connect(ui.radioButton_6, SIGNAL(clicked()), this, SLOT(SetBHPF()));
	connect(ui.radioButton_7, SIGNAL(clicked()), this, SLOT(SetGHPF()));
	connect(ui.radioButton_8, SIGNAL(clicked()), this, SLOT(SetTHPF()));

}

void dip::SetILPF()
{
	//输入菜单
	ui.inputGroup->setTitle(QStringLiteral("理想低通滤波器参数"));
	ui.inputGroup->setVisible(true);
	QList<QLabel*>		labelChildren	 = ui.inputGroup->findChildren<QLabel*>();
	QList<QLineEdit*>	lineEidtChildren = ui.inputGroup->findChildren<QLineEdit*>();
	labelChildren[0]->setText(QStringLiteral("半径 D0"));
	//设置正则条件
	QRegExp regExp1("[0-9]{0,4}");
	lineEidtChildren[0]->setValidator(new QRegExpValidator(regExp1, NULL));

	//可见数目 i
	int k(1);
	for(int i=0; i!=5; ++i){
		labelChildren[i]->setVisible( i<k );
		lineEidtChildren[i]->setVisible( i<k );
	}

	//选择菜单可见
	ui.selectGroup->setVisible(true);
}
void dip::SetBLPF()
{
	//输入菜单
	ui.inputGroup->setTitle(QStringLiteral("巴特沃斯低通参数"));
	ui.inputGroup->setVisible(true);
	QList<QLabel*>		labelChildren	 = ui.inputGroup->findChildren<QLabel*>();
	QList<QLineEdit*>	lineEidtChildren = ui.inputGroup->findChildren<QLineEdit*>();

	labelChildren[0]->setText(QStringLiteral("半径 D0"));
	//设置正则条件
	QRegExp regExp1("[0-9]{0,4}");
	lineEidtChildren[0]->setValidator(new QRegExpValidator(regExp1, NULL));

	//可见数目 i
	int k(1);
	for(int i=0; i!=5; ++i){
		labelChildren[i]->setVisible( i<k );
		lineEidtChildren[i]->setVisible( i<k );
	}

	//选择菜单可见
	ui.selectGroup->setVisible(true);
}
void dip::SetGLPF()
{
	//输入菜单
	ui.inputGroup->setTitle(QStringLiteral("高斯低通参数"));
	ui.inputGroup->setVisible(true);
	QList<QLabel*>		labelChildren	 = ui.inputGroup->findChildren<QLabel*>();
	QList<QLineEdit*>	lineEidtChildren = ui.inputGroup->findChildren<QLineEdit*>();

	labelChildren[0]->setText(QStringLiteral("半径 D0"));
	//设置正则条件
	QRegExp regExp1("[0-9]{0,4}");
	lineEidtChildren[0]->setValidator(new QRegExpValidator(regExp1, NULL));

	//输入可见数目 i
	int k(1);
	for(int i=0; i!=5; ++i){
		labelChildren[i]->setVisible( i<k );
		lineEidtChildren[i]->setVisible( i<k );
	}

	//选择菜单可见
	ui.selectGroup->setVisible(true);
}
void dip::SetTLPF()
{
	//输入菜单
	ui.inputGroup->setTitle(QStringLiteral("梯形低通参数"));
	ui.inputGroup->setVisible(true);
	QList<QLabel*>		labelChildren	 = ui.inputGroup->findChildren<QLabel*>();
	QList<QLineEdit*>	lineEidtChildren = ui.inputGroup->findChildren<QLineEdit*>();

	labelChildren[0]->setText(QStringLiteral("半径 D0"));
	labelChildren[1]->setText(QStringLiteral("半径 D1"));
	//设置正则条件
	QRegExp regExp1("[0-9]{0,4}");
	lineEidtChildren[0]->setValidator(new QRegExpValidator(regExp1, NULL));

	//可见数目 i
	int k(2);
	for(int i=0; i!=5; ++i){
		labelChildren[i]->setVisible( i<k );
		lineEidtChildren[i]->setVisible( i<k );
	}

	//选择菜单可见
	ui.selectGroup->setVisible(true);
}
void dip::SetIHPF()
{
	//输入菜单
	ui.inputGroup->setTitle(QStringLiteral("理想高通滤波器参数"));
	ui.inputGroup->setVisible(true);
	QList<QLabel*>		labelChildren	 = ui.inputGroup->findChildren<QLabel*>();
	QList<QLineEdit*>	lineEidtChildren = ui.inputGroup->findChildren<QLineEdit*>();
	labelChildren[0]->setText(QStringLiteral("半径 D0"));
	//设置正则条件
	QRegExp regExp1("[0-9]{0,4}");
	lineEidtChildren[0]->setValidator(new QRegExpValidator(regExp1, NULL));

	//可见数目 i
	int k(1);
	for(int i=0; i!=5; ++i){
		labelChildren[i]->setVisible( i<k );
		lineEidtChildren[i]->setVisible( i<k );
	}

	//选择菜单可见
	ui.selectGroup->setVisible(true);
}
void dip::SetBHPF()
{
	//输入菜单
	ui.inputGroup->setTitle(QStringLiteral("巴特沃斯高通参数"));
	ui.inputGroup->setVisible(true);
	QList<QLabel*>		labelChildren	 = ui.inputGroup->findChildren<QLabel*>();
	QList<QLineEdit*>	lineEidtChildren = ui.inputGroup->findChildren<QLineEdit*>();

	labelChildren[0]->setText(QStringLiteral("半径 D0"));
	//设置正则条件
	QRegExp regExp1("[0-9]{0,4}");
	lineEidtChildren[0]->setValidator(new QRegExpValidator(regExp1, NULL));

	//可见数目 i
	int k(1);
	for(int i=0; i!=5; ++i){
		labelChildren[i]->setVisible( i<k );
		lineEidtChildren[i]->setVisible( i<k );
	}

	//选择菜单可见
	ui.selectGroup->setVisible(true);
}
void dip::SetGHPF()
{
	//输入菜单
	ui.inputGroup->setTitle(QStringLiteral("高斯高通参数"));
	ui.inputGroup->setVisible(true);
	QList<QLabel*>		labelChildren	 = ui.inputGroup->findChildren<QLabel*>();
	QList<QLineEdit*>	lineEidtChildren = ui.inputGroup->findChildren<QLineEdit*>();

	labelChildren[0]->setText(QStringLiteral("半径 D0"));
	//设置正则条件
	QRegExp regExp1("[0-9]{0,4}");
	lineEidtChildren[0]->setValidator(new QRegExpValidator(regExp1, NULL));

	//可见数目 i
	int k(1);
	for(int i=0; i!=5; ++i){
		labelChildren[i]->setVisible( i<k );
		lineEidtChildren[i]->setVisible( i<k );
	}

	//选择菜单可见
	ui.selectGroup->setVisible(true);
}
void dip::SetTHPF()
{
	//输入菜单
	ui.inputGroup->setTitle(QStringLiteral("梯形高通参数"));
	ui.inputGroup->setVisible(true);
	QList<QLabel*>		labelChildren	 = ui.inputGroup->findChildren<QLabel*>();
	QList<QLineEdit*>	lineEidtChildren = ui.inputGroup->findChildren<QLineEdit*>();

	labelChildren[0]->setText(QStringLiteral("半径 D0"));
	labelChildren[1]->setText(QStringLiteral("半径 D1"));
	//设置正则条件
	QRegExp regExp1("[0-9]{0,4}");
	lineEidtChildren[0]->setValidator(new QRegExpValidator(regExp1, NULL));

	//可见数目 i
	int k(2);
	for(int i=0; i!=5; ++i){
		labelChildren[i]->setVisible( i<k );
		lineEidtChildren[i]->setVisible( i<k );
	}

	//选择菜单可见
	ui.selectGroup->setVisible(true);
}

void dip::GetColor()
{
	QString filename = (defaultSavePath+"\\GetColor.bmp").c_str();

	//坐标以及当前灰度值
	int pointX(labelSource->posx),pointY(BMP->Height-labelSource->posy);
	pointX = (pointX>BMP->Width-1)?(BMP->Width-1):pointX;
	pointY = (pointY>BMP->Height-1)?(BMP->Height-1):pointY;
	QString textColor = QStringLiteral("取色框:[%1,%2]").arg(pointX).arg(pointY);
	ui.colorGrid->setTitle(textColor);
	textColor = QStringLiteral("R :%1\nB :%2\nG :%3\n灰:%4").arg(BMP->Array_Red->Array[pointY][pointX])\
		.arg(BMP->Array_Blue->Array[pointY][pointX]).arg(BMP->Array_Green->Array[pointY][pointX]).arg(BMP->Array_Gray->Array[pointY][pointX]);	
	ui.labelColor_1->setText(textColor);
	ui.labelColor_1->show();
	//剪切图像并缩放大小 --> Label
	CbmpArray *retb = BMP->Crop(BMP->Array_Blue, pointX-15, pointY-15, pointX+15, pointY+15, 0);
	CbmpArray *retg = BMP->Crop(BMP->Array_Green, pointX-15, pointY-15, pointX+15, pointY+15, 0);
	CbmpArray *retr = BMP->Crop(BMP->Array_Red, pointX-15, pointY-15, pointX+15, pointY+15, 0);
	CbmpArray *retB  = BMP->Scale(retb, 0, 0, 110, 110, 0);
	CbmpArray *retG  = BMP->Scale(retg, 0, 0, 110, 110, 0);
	CbmpArray *retR  = BMP->Scale(retr, 0, 0, 110, 110, 0);
	//加十字线
	for(PIXEL i=54;i!=56; ++i){
		for(PIXEL j=35;j!=75;++j){
			retR->Array[i][j] = 255;
			retR->Array[j][i] = 255;
		}
	}
	BMP->SaveAsColor(filename.toStdString(), BMP->bmpHead, retR->Width, retR->Height,retB, retG, retR);

	//取色框显示小图像
	QImage *imgGetColor = new QImage;
	if(! ( imgGetColor->load(filename) ) ) //加载图像
	{
		QMessageBox::information(this, QStringLiteral("读取图片失败"), QStringLiteral("读取图片失败!"));
		delete imgGetColor;
		return;
	}
	//显示于输出区域
	ui.labelColor_2->setMinimumHeight(imgGetColor->height());
	ui.labelColor_2->setMinimumWidth(imgGetColor->width());
	ui.labelColor_2->setMaximumHeight(imgGetColor->height());
	ui.labelColor_2->setMaximumWidth(imgGetColor->width());
	ui.labelColor_2->setPixmap(QPixmap::fromImage(*imgGetColor));//必须要把这个wdget画出来
	ui.labelColor_2->show();
	delete imgGetColor, retB, retG, retR, retb, retg, retr;
}
void dip::GetColor_1()
{
	QString filename = (defaultSavePath+"\\GetColor_1.bmp").c_str();

	//坐标以及当前灰度值
	int pointX(labelOutput->posx),pointY(BMPOutput->Height-labelOutput->posy);
	pointX = (pointX>BMPOutput->Width-1)?(BMPOutput->Width-1):pointX;
	pointY = (pointY>BMPOutput->Height-1)?(BMPOutput->Height-1):pointY;
	QString textColor = QStringLiteral("取色框:[%1,%2]").arg(pointX).arg(pointY);
	ui.colorGrid->setTitle(textColor);
	textColor = QStringLiteral("R :%1\nB :%2\nG :%3\n灰:%4").arg(BMPOutput->Array_Red->Array[pointY][pointX])\
		.arg(BMPOutput->Array_Blue->Array[pointY][pointX]).arg(BMPOutput->Array_Green->Array[pointY][pointX]).arg(BMPOutput->Array_Gray->Array[pointY][pointX]);	
	ui.labelColor_1->setText(textColor);
	ui.labelColor_1->show();
	//剪切图像并缩放大小 --> Label
	CbmpArray *retb = BMPOutput->Crop(BMPOutput->Array_Blue, pointX-15, pointY-15, pointX+15, pointY+15, 0);
	CbmpArray *retg = BMPOutput->Crop(BMPOutput->Array_Green, pointX-15, pointY-15, pointX+15, pointY+15, 0);
	CbmpArray *retr = BMPOutput->Crop(BMPOutput->Array_Red, pointX-15, pointY-15, pointX+15, pointY+15, 0);
	CbmpArray *retB  = BMPOutput->Scale(retb, 0, 0, 110, 110, 0);
	CbmpArray *retG  = BMPOutput->Scale(retg, 0, 0, 110, 110, 0);
	CbmpArray *retR  = BMPOutput->Scale(retr, 0, 0, 110, 110, 0);
	//加十字线
	for(PIXEL i=54;i!=56; ++i){
		for(PIXEL j=35;j!=75;++j){
			retR->Array[i][j] = 255;
			retR->Array[j][i] = 255;
		}
	}
	BMPOutput->SaveAsColor(filename.toStdString(), BMPOutput->bmpHead, retR->Width, retR->Height,retB, retG, retR);
	
	//取色框显示小图像
	QImage *imgGetColor = new QImage;
	if(! ( imgGetColor->load(filename) ) ) //加载图像
	{
		QMessageBox::information(this, QStringLiteral("读取图片失败"), QStringLiteral("读取图片失败!"));
		delete imgGetColor;
		return;
	}
	//显示于输出区域
	ui.labelColor_2->setMinimumHeight(imgGetColor->height());
	ui.labelColor_2->setMinimumWidth(imgGetColor->width());
	ui.labelColor_2->setMaximumHeight(imgGetColor->height());
	ui.labelColor_2->setMaximumWidth(imgGetColor->width());
	ui.labelColor_2->setPixmap(QPixmap::fromImage(*imgGetColor));//必须要把这个wdget画出来
	ui.labelColor_2->show();
	delete imgGetColor, retB, retG, retR, retb, retg, retr;
}