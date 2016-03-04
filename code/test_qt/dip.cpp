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

	//��ʼ����Ա����
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


	//��ʼ����������
	ResetParameters();
	ui.imgType->setVisible(false);
	ui.selectGroup->setVisible(true);
	ui.resetButton->setVisible(true);
	ui.okButton->setVisible(false);
	connect(ui.resetButton, SIGNAL(clicked()), this, SLOT(ImgOpen()));

	//�趨������
		//File��������
	QList<QAction*> fileMenuActions = ui.fileMenu->actions();
	connect(fileMenuActions[0], SIGNAL(triggered()), this, SLOT(ImgOpen()));
	connect(fileMenuActions[1], SIGNAL(triggered()), this, SLOT(ImgSave()));
		//���򲿷ֵ�����
	QList<QAction*> pixelMenuActions = ui.pixelMenu->actions();
	connect(pixelMenuActions[0], SIGNAL(triggered()), this, SLOT(MirrorSlots()));
	connect(pixelMenuActions[1], SIGNAL(triggered()), this, SLOT(OffsetSlots()));
	connect(pixelMenuActions[2], SIGNAL(triggered()), this, SLOT(CropSlots()));
	connect(pixelMenuActions[3], SIGNAL(triggered()), this, SLOT(ScaleSlots()));
	connect(pixelMenuActions[4], SIGNAL(triggered()), this, SLOT(RotSlots()));
	connect(pixelMenuActions[5], SIGNAL(triggered()), this, SLOT(PixelFilterSlots()));
	connect(pixelMenuActions[6], SIGNAL(triggered()), this, SLOT(HistogramSlots()));
		//Ƶ���򲿷ֵ�����
	QList<QAction*> freMenuActions = ui.freMenu->actions();
	connect(freMenuActions[0], SIGNAL(triggered()), this, SLOT(FFTSlots()));
	connect(freMenuActions[1], SIGNAL(triggered()), this, SLOT(IFFTSlots()));
	connect(freMenuActions[2], SIGNAL(triggered()), this, SLOT(FreFilterSlots()));
	connect(freMenuActions[3], SIGNAL(triggered()), this, SLOT(DCTSlots()));
		//����
	QList<QAction*> aboutMenuActions = ui.aboutMenu->actions();
	connect(aboutMenuActions[0], SIGNAL(triggered()), this, SLOT(AuthorSlots()));

		//����ͼ����ʽ
	connect(ui.radioImgType_1, SIGNAL(clicked()), this, SLOT(SetImgType()));
	connect(ui.radioImgType_2, SIGNAL(clicked()), this, SLOT(SetImgType()));

		//���ȡɫ����
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
		QMessageBox::information(this, QStringLiteral("����ִ��"), QStringLiteral("�������Ѵ򿪵��ļ�!"));
		return;
	}
	if(isFirstRun){
		return;
	}
	isFirstRun = true;
	//���ǳ������У������Ƿ��ɫ���� ����ͼ��
		//���õ�һ������ĻҶ�ͼƬ����Դͼ�񴰿�
	ui.sourceScrollArea->setWidget(labelOutput);//setWidget��Label��Ϊ���Ӷ��󣬸�������ʱ���������¼���Ӷ����ַ�� --> ���imgSource
	labelSource = new myLabel(this);
	connect(labelSource, SIGNAL(clicked()), this, SLOT(GetColor()));
	imgSource = imgOutput;
	
	labelSource->setMinimumHeight(imgSource->height());
	labelSource->setMinimumWidth(imgSource->width());
	labelSource->setMaximumHeight(imgSource->height());
	labelSource->setMaximumWidth(imgSource->width());
	labelSource->setPixmap(QPixmap::fromImage(*imgSource));//����Ҫ�����wdget������
	ui.sourceScrollArea->setWidget(labelSource);	//--> ���labelOutput
	imgOutput = new QImage;
	labelOutput = new myLabel(this);
	
}

void dip::DisplayOutputImg(std::string filePath)
{
	QString filename = QString::fromLocal8Bit(filePath.c_str());
	BMPOutput->Open(filePath);
	if(! ( imgOutput->load(filename) ) ) //����ͼ��
	{
		QMessageBox::information(this, QStringLiteral("��ȡͼƬʧ��"), QStringLiteral("��ȡͼƬʧ��!"));
		delete imgOutput;
		return;
	}
	//��ʾ���������
	/*��ӹ�����*/
	labelOutput->setWindowTitle(QStringLiteral("��ʾͼ��"));
	labelOutput->setMinimumHeight(imgOutput->height());
	labelOutput->setMinimumWidth(imgOutput->width());
	labelOutput->setMaximumHeight(imgOutput->height());
	labelOutput->setMaximumWidth(imgOutput->width());
	labelOutput->setPixmap(QPixmap::fromImage(*imgOutput));//����Ҫ�����wdget������
	ui.outputSrcollArea->setWidget(labelOutput);

	//���ȡɫ�¼�
	connect(labelOutput, SIGNAL(clicked()), this, SLOT(GetColor_1()));
	//labelOutput->setCursor(Qt::CrossCursor);//move��������
}


/***************			�۵Ķ���			*******************/
void dip::DIPRun()
{
	//ѡ��flag
	QString lineEditText[5];
	int radioValue(1);
	if(flag == OFFSET){
		//��ȡѡ���ģ
		QButtonGroup *buttonGroup_1 = new QButtonGroup;
		buttonGroup_1->addButton(ui.radioButton_1, 0);
		buttonGroup_1->addButton(ui.radioButton_2, 1);
		radioValue = buttonGroup_1->checkedId();

		//��ȡ������
		QList<QLineEdit*>	lineEidtChildren = ui.inputGroup->findChildren<QLineEdit*>();
		lineEditText[0] = lineEidtChildren[0]->text();
		lineEditText[1] = lineEidtChildren[1]->text();
		lineEditText[2] = lineEidtChildren[2]->text();
		if(lineEditText[0].isEmpty() || lineEditText[1].isEmpty() || lineEditText[2].isEmpty()){
			QMessageBox::information(this, QStringLiteral("����ִ��"), QStringLiteral("���벻��Ϊ��!"));
			return;
		}
		Offset(lineEditText[0].toInt(), lineEditText[1].toInt(), radioValue , lineEditText[2].toInt() );

	}
	else if(flag == CROP){
		//��ȡѡ���ģʽ

		//��ȡ������
		QList<QLineEdit*>	lineEidtChildren = ui.inputGroup->findChildren<QLineEdit*>();
		for(int i=0; i!=5; ++i){
			lineEditText[i] = lineEidtChildren[i]->text();
		}

		if(lineEditText[0].isEmpty() || lineEditText[1].isEmpty() || lineEditText[2].isEmpty() ||
			lineEditText[3].isEmpty() || lineEditText[4].isEmpty()){
				QMessageBox::information(this, QStringLiteral("����ִ��"), QStringLiteral("���벻��Ϊ��!"));
				return;
		}
		Crop(lineEditText[0].toInt(), lineEditText[1].toInt(), lineEditText[2].toInt() ,
			lineEditText[3].toInt(), lineEditText[4].toInt());
	}
	else if(flag == SCALE){
		//��ȡѡ���ģ
		QButtonGroup *buttonGroup_1 = new QButtonGroup;
		buttonGroup_1->addButton(ui.radioButton_1, 0);
		buttonGroup_1->addButton(ui.radioButton_2, 1);
		radioValue = buttonGroup_1->checkedId();

		//��ȡ������
		QList<QLineEdit*>	lineEidtChildren = ui.inputGroup->findChildren<QLineEdit*>();
		for(int i=0; i!=4; ++i){
			lineEditText[i] = lineEidtChildren[i]->text();
		}

		if(lineEditText[0].isEmpty() || lineEditText[1].isEmpty() || lineEditText[2].isEmpty() ||
			lineEditText[3].isEmpty() ){
				QMessageBox::information(this, QStringLiteral("����ִ��"), QStringLiteral("���벻��Ϊ��!"));
				return;
		}
		if(lineEditText[0].toDouble()>10 || lineEditText[1].toDouble()>10){
			QMessageBox::information(this, QStringLiteral("����ִ��"), QStringLiteral("���ű�������!"));
			return;
		}
		Scale(lineEditText[0].toDouble(), lineEditText[1].toDouble(), lineEditText[2].toInt() ,
			lineEditText[3].toInt(), radioValue);
	}
	else if(flag == ROT){
		//��ȡѡ���ģ
		QButtonGroup *buttonGroup_1 = new QButtonGroup;
		buttonGroup_1->addButton(ui.radioButton_1, 0);
		buttonGroup_1->addButton(ui.radioButton_2, 1);
		radioValue = buttonGroup_1->checkedId();

		//��ȡ������
		QList<QLineEdit*>	lineEidtChildren = ui.inputGroup->findChildren<QLineEdit*>();
		for(int i=0; i!=4; ++i){
			lineEditText[i] = lineEidtChildren[i]->text();
		}

		if(lineEditText[0].isEmpty() || lineEditText[1].isEmpty() || lineEditText[2].isEmpty() ||
			lineEditText[3].isEmpty() ){
				QMessageBox::information(this, QStringLiteral("����ִ��"), QStringLiteral("���벻��Ϊ��!"));
				return;
		}
		if(lineEditText[0].toInt()>BMP->Array_Gray->Width-1 || lineEditText[1].toInt()>BMP->Array_Gray->Height-1){
			QMessageBox::information(this, QStringLiteral("����ִ��"), QStringLiteral("��ת�㳬������!"));
			return;
		}
		Rot(lineEditText[0].toInt(), lineEditText[1].toInt(), lineEditText[3].toDouble()*M_PI/180, radioValue,
			lineEditText[2].toInt());
	}
	else if(flag == FILTER){
		//��ȡѡ���ģ
		QButtonGroup *buttonGroup_1 = new QButtonGroup;
		buttonGroup_1->addButton(ui.radioButton_1, 1);
		buttonGroup_1->addButton(ui.radioButton_2, 2);
		buttonGroup_1->addButton(ui.radioButton_3, 3);
		buttonGroup_1->addButton(ui.radioButton_4, 4);
		radioValue = buttonGroup_1->checkedId();

		//�����˵�����
		QButtonGroup *buttonGroup_2 = new QButtonGroup;
		buttonGroup_2->addButton(ui.radioButton_5, 1);
		buttonGroup_2->addButton(ui.radioButton_6, 2);
		buttonGroup_2->addButton(ui.radioButton_7, 3);
		int radioValue2 = buttonGroup_2->checkedId();

		//��ȡ������

		PixelFilter(radioValue, radioValue2/* =0 */);
	}
	else if(flag == FLAGFreFilter){
		//��ȡѡ���ģʽ
		QButtonGroup *buttonGroup_1 = new QButtonGroup;
		buttonGroup_1->addButton(ui.radioButton_1, 1);
		buttonGroup_1->addButton(ui.radioButton_2, 5);
		radioValue = buttonGroup_1->checkedId();

		//��ȡ�����˵�
		int radioValue1;
		QButtonGroup *buttonGroup_2 = new QButtonGroup;
		buttonGroup_2->addButton(ui.radioButton_5, 0);
		buttonGroup_2->addButton(ui.radioButton_6, 1);
		buttonGroup_2->addButton(ui.radioButton_7, 2);
		buttonGroup_2->addButton(ui.radioButton_8, 3);
		radioValue1 = buttonGroup_2->checkedId();

		radioValue += radioValue1;

		//��ȡ������
		QList<QLineEdit*>	lineEidtChildren = ui.inputGroup->findChildren<QLineEdit*>();
		lineEditText[0] = lineEidtChildren[0]->text();
		lineEditText[1] = "0";
		if(radioValue==4 || radioValue==8){
			lineEditText[1] = lineEidtChildren[1]->text();
		}

		if(lineEditText[0].isEmpty() || (lineEditText[1].isEmpty()&&radioValue==4) || (lineEditText[1].isEmpty()&&radioValue==8) ){
			QMessageBox::information(this, QStringLiteral("����ִ��"), QStringLiteral("���벻��Ϊ��!"));
			return;
		}
		FreFilter(lineEditText[0].toDouble(), lineEditText[1].toDouble(), 1, radioValue);		
	}


}
void dip::ResetParameters()
{
	//���ò�������
	//��ѡ�˵�����
	ui.radioGroup->setWindowTitle(QStringLiteral("��ѡ"));	
	QList<QRadioButton*> radioChilren = ui.radioGroup->findChildren<QRadioButton*>();
	for(QList<QRadioButton*>::iterator i=radioChilren.begin(); i!=radioChilren.end(); ++i){
		(*i)->setText(QStringLiteral("default"));
		(*i)->setVisible(false);
		//ɾ���������
		disconnect((*i), SIGNAL(clicked()), 0, 0);
	}
	ui.radioGroup->setVisible(false);



	//�����˵�����
	ui.comboGroup->setWindowTitle(QStringLiteral("��ѡ2"));
	QList<QRadioButton*> radioChilren1 = ui.comboGroup->findChildren<QRadioButton*>();
	for(QList<QRadioButton*>::iterator i=radioChilren1.begin(); i!=radioChilren1.end(); ++i){
		(*i)->setText(QStringLiteral("default"));
		(*i)->setVisible(false);
		disconnect((*i), SIGNAL(clicked()), 0, 0);
	}
	ui.comboGroup->setVisible(false);
	//����˵�����
	ui.inputGroup->setWindowTitle(QStringLiteral("����"));
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
	//ѡ��˵�����
	ui.selectGroup->setVisible(false);

}
void dip::ImgOpen()
{
	//��ȡͼ��
	QString filename;
	filename = QFileDialog::getOpenFileName(
		NULL,
		QStringLiteral("ѡ��ͼ��"),
		"",//������Ĭ���ļ���
 		QStringLiteral("Images (*.bmp)"));//"Images (*.bmp *.jpg *.tif *.GIF *.cpp )"

	if(filename.isEmpty())	{
		return;
	}
	else	{
		
		isImgOpen = true;
		//��ԴͼƬ  -- ��������������ڶ�Ӧͼ��
		//����·��ת��  std::string  <---->  Qstring
		std::string tran = filename.toLocal8Bit();
		filename = QString::fromLocal8Bit(tran.c_str());

		BMP->Open(tran);

		isImgOpen = true;

		//��¼���봰�ڵ�ͼ��
		filename = defaultSavePath.c_str()+QStringLiteral("\\outputColor.bmp");
		tran = filename.toLocal8Bit();
		BMP->SaveAsColor(tran, BMP->bmpHead, BMP->Width, BMP->Height, BMP->Array_Blue, BMP->Array_Green, BMP->Array_Red);
		BMPSource->Open(tran);

		//���ص�Դͼ�񴰿�
		if(!( imgSource->load(filename) ) ) //����ͼ��
		{
			QMessageBox::information(this, QStringLiteral("��ͼ��ʧ��"), QStringLiteral("��ͼ��ʧ��!"));
			delete imgSource;
			return;
		}		
			/*��ӵ�������*/
		labelSource->setWindowTitle(QStringLiteral("��ʾͼ��"));
		labelSource->setMinimumHeight(imgSource->height());
		labelSource->setMinimumWidth(imgSource->width());
		labelSource->setMaximumHeight(imgSource->height());
		labelSource->setMaximumWidth(imgSource->width());
		labelSource->setPixmap(QPixmap::fromImage(*imgSource));//����Ҫ�����wdget������
		ui.sourceScrollArea->setWidget(labelSource);

		
		//��¼������ڵ�ͼ��
		filename = defaultSavePath.c_str()+QStringLiteral("\\outputGray.bmp");
		tran = filename.toLocal8Bit();
		BMP->SaveAsGray(tran, BMP->bmpHead, BMP->Width, BMP->Height, BMP->Array_Gray);	
		BMPOutput->Open(tran);
		//���ص��������
		if(! ( imgOutput->load(filename) ) ) //����ͼ��
		{
			QMessageBox::information(this, QStringLiteral("��ȡͼƬʧ��"), QStringLiteral("��ȡͼƬʧ��!"));
			delete imgOutput;
			return;
		}
		/*��ӹ�����*/
		labelOutput->setWindowTitle(QStringLiteral("��ʾͼ��"));
		labelOutput->setMinimumHeight(imgOutput->height());
		labelOutput->setMinimumWidth(imgOutput->width());
		labelOutput->setMaximumHeight(imgOutput->height());
		labelOutput->setMaximumWidth(imgOutput->width());
		labelOutput->setPixmap(QPixmap::fromImage(*imgOutput));//����Ҫ�����wdget������
		ui.outputSrcollArea->setWidget(labelOutput);

		//ȡɫ���������
		ui.colorGrid->setTitle(QStringLiteral("ȡɫ��-[%1,%2]").arg(BMP->Height).arg(BMP->Width));
		//ȡɫ�ź�����
		connect(labelSource, SIGNAL(clicked()), this, SLOT(GetColor()));
		connect(labelOutput, SIGNAL(clicked()), this, SLOT(GetColor_1()));
		//labelSource->setCursor(Qt::CrossCursor);
		//labelOutput->setCursor(Qt::CrossCursor);

		//�״δ�ͼ��ͼ������
		ui.selectGroup->setVisible(false);
		ui.resetButton->setVisible(false);
		ui.imgType->setVisible(true);
		//�����־
		isFirstRun = false;
		flag = 0;
	}
}
void dip::ImgSave()
{
	if(!IsImgOpen()){
		QMessageBox::information(this, QStringLiteral("����ִ��"), QStringLiteral("�������Ѵ򿪵��ļ�!"));
		return;
	}
	QString filename;
	filename=QFileDialog::getSaveFileName(
		NULL,
		QStringLiteral("���Ϊ"),
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
	QMessageBox::information(this, QStringLiteral("����"), QStringLiteral("�ѱ���!"));
}
void dip::SetImgType()
{
	QButtonGroup *buttonGroup = new QButtonGroup;
	buttonGroup->addButton(ui.radioImgType_1, 0);
	buttonGroup->addButton(ui.radioImgType_2, 1);
	imgType = buttonGroup->checkedId();
	delete buttonGroup;
}

void dip::MirrorSlots() //�ۺ�����ɴ��ڱ任�Լ���������
{
	//IsFirstRun();
	if(!IsImgOpen()){
		QMessageBox::information(this, QStringLiteral("����ִ��"), QStringLiteral("�������Ѵ򿪵��ļ�!"));
		return;
	}
	ResetParameters();
	flag = 0;
	Mirror();
}
void dip::Mirror()
{
	std::string savePath(defaultSavePath + QStringLiteral("\\outputMirror.bmp").toStdString());
	//�ж��ǲ�ɫ���ǻҶ�	
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
	//���������ͼ��
	BMPSource->Open(savePath);
	DisplayOutputImg(savePath);
}

void dip::OffsetSlots()
{
	//IsFirstRun();
	if(!IsImgOpen()){
		QMessageBox::information(this, QStringLiteral("����ִ��"), QStringLiteral("�������Ѵ򿪵��ļ�!"));
		return;
	}
	ResetParameters();
	flag = OFFSET;
//������Ҫ�Ĵ���
	//��ѡ�˵�
	ui.radioGroup->setTitle(QStringLiteral("ƽ��ģʽ"));
	ui.radioGroup->setVisible(true);
	ui.radioButton_1->setText(QStringLiteral("����ԭ��С"));
	ui.radioButton_2->setText(QStringLiteral("����������ɫ"));
	ui.radioButton_2->setChecked(true);
	ui.radioButton_1->setVisible(true);
	ui.radioButton_2->setVisible(true);
	//�����˵�
	
	//����˵�
	ui.inputGroup->setTitle(QStringLiteral("ƽ����x��,y��,����ԭ��"));
	ui.inputGroup->setVisible(true);
	QList<QLabel*>		labelChildren	 = ui.inputGroup->findChildren<QLabel*>();
	QList<QLineEdit*>	lineEidtChildren = ui.inputGroup->findChildren<QLineEdit*>();
	labelChildren[0]->setText(QStringLiteral("��� x"));
	labelChildren[1]->setText(QStringLiteral("�߶� y"));
	labelChildren[2]->setText(QStringLiteral("���ɫ"));
	labelChildren[0]->setVisible(true);
	labelChildren[1]->setVisible(true);
	labelChildren[2]->setVisible(true);
	lineEidtChildren[0]->setVisible(true);
	lineEidtChildren[1]->setVisible(true);
	lineEidtChildren[2]->setVisible(true);

	//ѡ��˵�
	ui.selectGroup->setVisible(true);

	//Offset();
}
void dip::Offset(int x_0, int y_0, bool mode/* =false */, COLOR fill/* =0 */)
{
	std::string savePath(defaultSavePath + QStringLiteral("\\outputOffset.bmp").toStdString());

	//�ж��ǲ�ɫ���ǻҶ�	
	if(imgType){
		//��ת�����ȴ�����
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
	
	//���������ͼ��  ����
	BMPSource->Open(savePath);
	DisplayOutputImg(savePath);
}

void dip::CropSlots()
{
	//IsFirstRun();
	if(!IsImgOpen()){
		QMessageBox::information(this, QStringLiteral("����ִ��"), QStringLiteral("�������Ѵ򿪵��ļ�!"));
		return;
	}
	ResetParameters();
	flag = CROP;
//������Ҫ�Ĳ˵�ѡ��
	//��ѡ�˵�   -->   ����Ҫ
	
	//�����˵�   -->   ����Ҫ

	//����˵�
	ui.inputGroup->setTitle(QStringLiteral("���з�Χx��,y��,����ԭ��"));
	ui.inputGroup->setVisible(true);
	QList<QLabel*>		labelChildren	 = ui.inputGroup->findChildren<QLabel*>();
	QList<QLineEdit*>	lineEidtChildren = ui.inputGroup->findChildren<QLineEdit*>();
	labelChildren[0]->setText(QStringLiteral("��� x0"));
	labelChildren[1]->setText(QStringLiteral("��� y0"));
	labelChildren[2]->setText(QStringLiteral("�յ� x1"));
	labelChildren[3]->setText(QStringLiteral("�յ� y1"));
	labelChildren[4]->setText(QStringLiteral("���ɫ"));
	for(int i=0; i!=5; ++i){
		labelChildren[i]->setVisible(true);
		lineEidtChildren[i]->setVisible(true);
	}

	//ѡ��˵�
	ui.selectGroup->setVisible(true);

	//Crop();
}
void dip::Crop(int x1, int y1, int x2, int y2, COLOR fill/* =0 */)
{
	std::string savePath(defaultSavePath + QStringLiteral("\\outputCrop.bmp").toStdString());
	
	//�ж��ǲ�ɫ���ǻҶ�	
	if(imgType){
		//��ת�����ȴ�����
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

	//���������ͼ��  ����
	BMPSource->Open(savePath);
	DisplayOutputImg(savePath);
}

void dip::ScaleSlots()
{
	//IsFirstRun();
	if(!IsImgOpen()){
		QMessageBox::information(this, QStringLiteral("����ִ��"), QStringLiteral("�������Ѵ򿪵��ļ�!"));
		return;
	}
	ResetParameters();
	flag = SCALE;

	//��ѡ�˵�   -->   ����Ҫ
	ui.radioGroup->setTitle(QStringLiteral("��ֵ��ʽ"));
	ui.radioGroup->setVisible(true);
	ui.radioButton_1->setText(QStringLiteral("����ڲ�ֵ"));
	ui.radioButton_2->setText(QStringLiteral("˫���Բ�ֵ"));
	ui.radioButton_2->setChecked(true);
	ui.radioButton_1->setVisible(true);
	ui.radioButton_2->setVisible(true);
	//�����˵�   -->   ����Ҫ

	//����˵�
	ui.inputGroup->setTitle(QStringLiteral("����-���Ȱ��������ţ���α���"));
	ui.inputGroup->setVisible(true);
	QList<QLabel*>		labelChildren	 = ui.inputGroup->findChildren<QLabel*>();
	QList<QLineEdit*>	lineEidtChildren = ui.inputGroup->findChildren<QLineEdit*>();
	labelChildren[0]->setText(QStringLiteral("x�������"));
	labelChildren[1]->setText(QStringLiteral("y�������"));
	labelChildren[2]->setText(QStringLiteral("x��������"));
	labelChildren[3]->setText(QStringLiteral("y��������"));
	//������������
	QRegExp regExp1("[0-9][.]{0,1}[0-9]{0,2}");
	lineEidtChildren[0]->setValidator(new QRegExpValidator(regExp1, NULL));
	lineEidtChildren[1]->setValidator(new QRegExpValidator(regExp1, NULL));
	QRegExp regExp2("[0-9]{1,4}");
	lineEidtChildren[2]->setValidator(new QRegExpValidator(regExp2, NULL));
	lineEidtChildren[3]->setValidator(new QRegExpValidator(regExp2, NULL));
	//�ɼ�
	for(int i=0; i!=4; ++i){
		labelChildren[i]->setVisible(true);
		lineEidtChildren[i]->setVisible(true);
	}

	//ѡ��˵�
	ui.selectGroup->setVisible(true);

	//Scale();
}
void dip::Scale(double nscale_x, double nscale_y, PIXEL nheight/* =0 */, PIXEL nwidth/* =0 */, bool resampling/* =false */)
{
	std::string savePath(defaultSavePath + QStringLiteral("\\outputScale.bmp").toStdString());

	//�ж��ǲ�ɫ���ǻҶ�	
	if(imgType){
		//��ת�����ȴ�����
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

	//���������ͼ��  ����
	BMPSource->Open(savePath);
	DisplayOutputImg(savePath);

}

void dip::RotSlots()
{
	//IsFirstRun();
	if(!IsImgOpen()){
		QMessageBox::information(this, QStringLiteral("����ִ��"), QStringLiteral("�������Ѵ򿪵��ļ�!"));
		return;
	}
	ResetParameters();
	flag = ROT;

	//��ѡ�˵�   -->   ����Ҫ
	ui.radioGroup->setTitle(QStringLiteral("��ֵ��ʽ"));
	ui.radioGroup->setVisible(true);
	ui.radioButton_1->setText(QStringLiteral("����ڲ�ֵ"));
	ui.radioButton_2->setText(QStringLiteral("˫���Բ�ֵ"));
	ui.radioButton_2->setChecked(true);
	ui.radioButton_1->setVisible(true);
	ui.radioButton_2->setVisible(true);
	//�����˵�   -->   ����Ҫ

	//����˵�
	ui.inputGroup->setTitle(QStringLiteral("��ת����"));
	ui.inputGroup->setVisible(true);
	QList<QLabel*>		labelChildren	 = ui.inputGroup->findChildren<QLabel*>();
	QList<QLineEdit*>	lineEidtChildren = ui.inputGroup->findChildren<QLineEdit*>();
	labelChildren[0]->setText(QStringLiteral("��ת����x"));
	labelChildren[1]->setText(QStringLiteral("��ת����y"));
	labelChildren[2]->setText(QStringLiteral("�������"));
	labelChildren[3]->setText(QStringLiteral("��ת�Ƕ�"));
	//������������
	QRegExp regExp1("[0-9]{0,4}");
	lineEidtChildren[0]->setValidator(new QRegExpValidator(regExp1, NULL));
	lineEidtChildren[1]->setValidator(new QRegExpValidator(regExp1, NULL));
	lineEidtChildren[2]->setValidator(new QRegExpValidator(regExp1, NULL));
	lineEidtChildren[3]->setValidator(new QRegExpValidator(regExp1, NULL));
	//�ɼ���Ŀ
	for(int i=0; i!=4; ++i){
		labelChildren[i]->setVisible(true);
		lineEidtChildren[i]->setVisible(true);
	}

	//ѡ��˵�
	ui.selectGroup->setVisible(true);


	//Rot();
}
void dip::Rot(PIXEL x_0/* =0 */, PIXEL y_0/* =0 */, double degree/* =M_PI_2 */, bool resampling/* =0 */, COLOR fill/* =0 */)
{
	std::string savePath(defaultSavePath + QStringLiteral("\\outputRot.bmp").toStdString());

	//�ж��ǲ�ɫ���ǻҶ�	
	if(imgType){
		//��ת�����ȴ�����
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

	//���������ͼ��  ����
	BMPSource->Open(savePath);
	DisplayOutputImg(savePath);

}


void dip::PixelFilterSlots()
{
	//IsFirstRun();
	if(!IsImgOpen()){
		QMessageBox::information(this, QStringLiteral("����ִ��"), QStringLiteral("�������Ѵ򿪵��ļ�!"));
		return;
	}
	ResetParameters();
	flag = FILTER;

	//Ƶ��ʹ��ʱ��ѡ2������
	connect(ui.radioButton_1, SIGNAL(clicked()), this, SLOT(ResetFilter()));
	connect(ui.radioButton_2, SIGNAL(clicked()), this, SLOT(ResetFilter()));
	connect(ui.radioButton_3, SIGNAL(clicked()), this, SLOT(ResetFilter()));
	connect(ui.radioButton_4, SIGNAL(clicked()), this, SLOT(SetFilter()));

	//��ѡ�˵�   -->   ����Ҫ
	ui.radioGroup->setTitle(QStringLiteral("�˲���ʽ"));
	ui.radioGroup->setVisible(true);
	ui.radioButton_1->setText(QStringLiteral("��ͨ�˲�"));
	ui.radioButton_2->setText(QStringLiteral("��ֵ�˲�"));
	ui.radioButton_3->setText(QStringLiteral("��ͨ�˲�"));
	ui.radioButton_4->setText(QStringLiteral("΢���˲�"));
	ui.radioButton_1->setChecked(true);
	ui.radioButton_1->setVisible(true);
	ui.radioButton_2->setVisible(true);
	ui.radioButton_3->setVisible(true);
	ui.radioButton_4->setVisible(true);

	//�����˵�   -->   ����Ҫδ����΢���˲���ǰ����Ҫ

	//����˵�

	//�ɼ���Ŀ

	//ѡ��˵�
	ui.selectGroup->setVisible(true);
}
void dip::PixelFilter(char mode, char diff_mode/* =0 */)
{
	std::string savePath(defaultSavePath + QStringLiteral("\\outputPixelFilter.bmp").toStdString());

	//�ж��ǲ�ɫ���ǻҶ�	
	if(imgType){
		//��ת�����ȴ�����
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

	//���������ͼ��  ����
	BMPSource->Open(savePath);
	DisplayOutputImg(savePath);

}

void dip::HistogramSlots()
{
	//IsFirstRun();
	if(!IsImgOpen()){
		QMessageBox::information(this, QStringLiteral("����ִ��"), QStringLiteral("�������Ѵ򿪵��ļ�!"));
		return;
	}
	ResetParameters();
	flag = HISTOGRAM;
	Histogram();
}
void dip::Histogram()
{
	std::string savePath(defaultSavePath + QStringLiteral("\\outputHistogram.bmp").toStdString());

	//�ж��ǲ�ɫ���ǻҶ�	
	if(imgType){
		//��ת�����ȴ�����
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

	//���������ͼ��  ����
	BMPSource->Open(savePath);
	DisplayOutputImg(savePath);


	//�������������ɢ�Ĵ���ͼ��  --  ֱ��ͼ�Ա�ͼ
	QGroupBox *histogram = new QGroupBox(NULL);
	QHBoxLayout *layoutHis = new QHBoxLayout;
	QLabel *preHis =new QLabel;
	QLabel *afterHis =new QLabel;
	QImage *input1 = new QImage;
	QImage *input2 = new QImage;
	QString filename1 = QStringLiteral("%1\\OldHistogram.bmp").arg(defaultSavePath.c_str());
	QString filename2 = QStringLiteral("%1\\NewHistogram.bmp").arg(defaultSavePath.c_str());

	histogram->setWindowTitle(QStringLiteral("ֱ��ͼ�Ƚ�"));

	input1->load(filename1); //����ͼ��
	preHis->setMinimumHeight(input1->height());
	preHis->setMinimumWidth(input1->width());
	preHis->setPixmap(QPixmap::fromImage(*input1));

	input2->load(filename2); //����ͼ��
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
		QMessageBox::information(this, QStringLiteral("����ִ��"), QStringLiteral("�������Ѵ򿪵��ļ�!"));
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

	//���������ͼ��
	DisplayOutputImg(savePath);
}

void dip::IFFTSlots()
{
	//IsFirstRun();
	if(!IsImgOpen()){
		QMessageBox::information(this, QStringLiteral("����ִ��"), QStringLiteral("�������Ѵ򿪵��ļ�!"));
		return;
	}
	ResetParameters();
	if(!ifFFT){
		QMessageBox::information(this, QStringLiteral("����ִ��"), QStringLiteral("δ���й�����Ҷ�任!"));
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
	//ȡ��任ʵ�� *pow(-1, i+j) ��ת����
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

	//���������ͼ��
	DisplayOutputImg(savePath);
}

void dip::FreFilterSlots()
{
	//IsFirstRun();
	if(!IsImgOpen()){
		QMessageBox::information(this, QStringLiteral("����ִ��"), QStringLiteral("�������Ѵ򿪵��ļ�!"));
		return;
	}
	ResetParameters();
	if(!ifFFT){
		QMessageBox::information(this, QStringLiteral("����ִ��"), QStringLiteral("δ���й�����Ҷ�任!"));
		return;
	}
	flag = FLAGFreFilter;
	ResetParameters();

	//������Ҫ������
	connect(ui.radioButton_1, SIGNAL(clicked()), this, SLOT(SetLPF()));
	connect(ui.radioButton_2, SIGNAL(clicked()), this, SLOT(SetHPF()));

	//��ѡ�˵�   -->   ����Ҫ
	ui.radioGroup->setTitle(QStringLiteral("ѡ��Ƶ�����˲���"));
	ui.radioGroup->setVisible(true);
	ui.radioButton_1->setText(QStringLiteral("��ͨ�˲���"));
	ui.radioButton_2->setText(QStringLiteral("��ͨ�˲���"));
	ui.radioButton_1->setChecked(true);
	ui.radioButton_1->setVisible(true);
	ui.radioButton_2->setVisible(true);

	SetLPF();
	//�����˵�   -->   ����Ҫ

	//����˵�

	//������������

	//�ɼ�

	//ѡ��˵�
	//ui.selectGroup->setVisible(true);
	//

}
void dip::FreFilter(double D0/* =0 */, double D1/* =0 */, int n/* =1 */, int filter/* =1 */)
{
	std::string savePath(defaultSavePath + QStringLiteral("\\outputFreFilter.bmp").toStdString());

	CComplexArray *ImageFFTFilter = BMP->FFT_Filter(BMP->ArrayOfFFT, D0, D1, n, filter);

	//����Ż����Ƶ��ͼ
	CbmpArray *ret,*ret0;
	ret0 = BMP->SwitchComplexToCbmpArray(ImageFFTFilter,1);
	ret = BMP->Scale(ret0, 0, 0, BMP->Height, BMP->Width, 1);

	BMP->SaveAsGray(defaultSavePath + QStringLiteral("\\outputFreFilter1.bmp").toStdString(), BMP->bmpHead, ret->Width, ret->Height, ret);
	
	QLabel *FreFilter1 =new QLabel(NULL);
	QImage *input1 = new QImage(NULL);
	QString filename1 = defaultSavePath.c_str() + QStringLiteral("\\outputFreFilter1.bmp");
	FreFilter1->setWindowTitle(QStringLiteral("�˲���Ƶ��ͼ"));
	input1->load(filename1); //����ͼ��
	FreFilter1->setMinimumHeight(input1->height());
	FreFilter1->setMinimumWidth(input1->width());
	FreFilter1->setPixmap(QPixmap::fromImage(*input1));
	FreFilter1->show();


	//����˲������任
	CComplexArray *IFFT_Filter = BMP->IFFT2(ImageFFTFilter);
	//ȡ��任ʵ�� *pow(-1, i+j) ��ת����
	CbmpArray *ImageIFFT = new CbmpArray;
	ImageIFFT->GenerateArray(IFFT_Filter->Height, IFFT_Filter->Width);

	for(PIXEL i=0; i!=BMP->ArrayOfFFT->Height; ++i){
		for(PIXEL j=0; j!=BMP->ArrayOfFFT->Width; ++j){
			ImageIFFT->Array[i][j] = static_cast<COLOR>(IFFT_Filter->Array[i][j].real()*pow(-1, i+j));
		}
	}
	CbmpArray *ImageIFFT1 = BMP->Scale(ImageIFFT, 0, 0, BMP->Height, BMP->Width);
	BMP->SaveAsGray(savePath, BMP->bmpHead, ImageIFFT1->Width, ImageIFFT1->Height, ImageIFFT1);

	//���������ͼ��
	DisplayOutputImg(savePath);
	ui.radioImgType_2->setChecked(true);

}

void dip::DCTSlots()
{
	//IsFirstRun();
	if(!IsImgOpen()){
		QMessageBox::information(this, QStringLiteral("����ִ��"), QStringLiteral("�������Ѵ򿪵��ļ�!"));
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

	//���������ͼ��
	DisplayOutputImg(savePath);
}

void dip::AuthorSlots()
{
	QLabel *author = new QLabel(NULL);
	author->setWindowTitle(QStringLiteral("С���Ա"));
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
	ui.comboGroup->setTitle(QStringLiteral("΢���˲���"));
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
	//ɾ��������
	QList<QRadioButton*> radioChilren1 = ui.comboGroup->findChildren<QRadioButton*>();
	for(QList<QRadioButton*>::iterator i=radioChilren1.begin(); i!=radioChilren1.end(); ++i){
		disconnect((*i), SIGNAL(clicked()), 0, 0);
	}

	ui.comboGroup->setTitle(QStringLiteral("��ͨ�˲���"));
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
	//����˵�	��ʼ��Ϊ��ͨ�������
	SetILPF();

	//����������   ����2  -->  ����˵�
	connect(ui.radioButton_5, SIGNAL(clicked()), this, SLOT(SetILPF()));
	connect(ui.radioButton_6, SIGNAL(clicked()), this, SLOT(SetBLPF()));
	connect(ui.radioButton_7, SIGNAL(clicked()), this, SLOT(SetGLPF()));
	connect(ui.radioButton_8, SIGNAL(clicked()), this, SLOT(SetTLPF()));
	
}
void dip::SetHPF()
{
	//ɾ��������
	QList<QRadioButton*> radioChilren1 = ui.comboGroup->findChildren<QRadioButton*>();
	for(QList<QRadioButton*>::iterator i=radioChilren1.begin(); i!=radioChilren1.end(); ++i){
		disconnect((*i), SIGNAL(clicked()), 0, 0);
	}

	ui.comboGroup->setTitle(QStringLiteral("��ͨ�˲���"));
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
	//����˵�	��ʼ��Ϊ��ͨ�������
	SetIHPF();
	//����������   ����2  -->  ����˵�
	connect(ui.radioButton_5, SIGNAL(clicked()), this, SLOT(SetIHPF()));
	connect(ui.radioButton_6, SIGNAL(clicked()), this, SLOT(SetBHPF()));
	connect(ui.radioButton_7, SIGNAL(clicked()), this, SLOT(SetGHPF()));
	connect(ui.radioButton_8, SIGNAL(clicked()), this, SLOT(SetTHPF()));

}

void dip::SetILPF()
{
	//����˵�
	ui.inputGroup->setTitle(QStringLiteral("�����ͨ�˲�������"));
	ui.inputGroup->setVisible(true);
	QList<QLabel*>		labelChildren	 = ui.inputGroup->findChildren<QLabel*>();
	QList<QLineEdit*>	lineEidtChildren = ui.inputGroup->findChildren<QLineEdit*>();
	labelChildren[0]->setText(QStringLiteral("�뾶 D0"));
	//������������
	QRegExp regExp1("[0-9]{0,4}");
	lineEidtChildren[0]->setValidator(new QRegExpValidator(regExp1, NULL));

	//�ɼ���Ŀ i
	int k(1);
	for(int i=0; i!=5; ++i){
		labelChildren[i]->setVisible( i<k );
		lineEidtChildren[i]->setVisible( i<k );
	}

	//ѡ��˵��ɼ�
	ui.selectGroup->setVisible(true);
}
void dip::SetBLPF()
{
	//����˵�
	ui.inputGroup->setTitle(QStringLiteral("������˹��ͨ����"));
	ui.inputGroup->setVisible(true);
	QList<QLabel*>		labelChildren	 = ui.inputGroup->findChildren<QLabel*>();
	QList<QLineEdit*>	lineEidtChildren = ui.inputGroup->findChildren<QLineEdit*>();

	labelChildren[0]->setText(QStringLiteral("�뾶 D0"));
	//������������
	QRegExp regExp1("[0-9]{0,4}");
	lineEidtChildren[0]->setValidator(new QRegExpValidator(regExp1, NULL));

	//�ɼ���Ŀ i
	int k(1);
	for(int i=0; i!=5; ++i){
		labelChildren[i]->setVisible( i<k );
		lineEidtChildren[i]->setVisible( i<k );
	}

	//ѡ��˵��ɼ�
	ui.selectGroup->setVisible(true);
}
void dip::SetGLPF()
{
	//����˵�
	ui.inputGroup->setTitle(QStringLiteral("��˹��ͨ����"));
	ui.inputGroup->setVisible(true);
	QList<QLabel*>		labelChildren	 = ui.inputGroup->findChildren<QLabel*>();
	QList<QLineEdit*>	lineEidtChildren = ui.inputGroup->findChildren<QLineEdit*>();

	labelChildren[0]->setText(QStringLiteral("�뾶 D0"));
	//������������
	QRegExp regExp1("[0-9]{0,4}");
	lineEidtChildren[0]->setValidator(new QRegExpValidator(regExp1, NULL));

	//����ɼ���Ŀ i
	int k(1);
	for(int i=0; i!=5; ++i){
		labelChildren[i]->setVisible( i<k );
		lineEidtChildren[i]->setVisible( i<k );
	}

	//ѡ��˵��ɼ�
	ui.selectGroup->setVisible(true);
}
void dip::SetTLPF()
{
	//����˵�
	ui.inputGroup->setTitle(QStringLiteral("���ε�ͨ����"));
	ui.inputGroup->setVisible(true);
	QList<QLabel*>		labelChildren	 = ui.inputGroup->findChildren<QLabel*>();
	QList<QLineEdit*>	lineEidtChildren = ui.inputGroup->findChildren<QLineEdit*>();

	labelChildren[0]->setText(QStringLiteral("�뾶 D0"));
	labelChildren[1]->setText(QStringLiteral("�뾶 D1"));
	//������������
	QRegExp regExp1("[0-9]{0,4}");
	lineEidtChildren[0]->setValidator(new QRegExpValidator(regExp1, NULL));

	//�ɼ���Ŀ i
	int k(2);
	for(int i=0; i!=5; ++i){
		labelChildren[i]->setVisible( i<k );
		lineEidtChildren[i]->setVisible( i<k );
	}

	//ѡ��˵��ɼ�
	ui.selectGroup->setVisible(true);
}
void dip::SetIHPF()
{
	//����˵�
	ui.inputGroup->setTitle(QStringLiteral("�����ͨ�˲�������"));
	ui.inputGroup->setVisible(true);
	QList<QLabel*>		labelChildren	 = ui.inputGroup->findChildren<QLabel*>();
	QList<QLineEdit*>	lineEidtChildren = ui.inputGroup->findChildren<QLineEdit*>();
	labelChildren[0]->setText(QStringLiteral("�뾶 D0"));
	//������������
	QRegExp regExp1("[0-9]{0,4}");
	lineEidtChildren[0]->setValidator(new QRegExpValidator(regExp1, NULL));

	//�ɼ���Ŀ i
	int k(1);
	for(int i=0; i!=5; ++i){
		labelChildren[i]->setVisible( i<k );
		lineEidtChildren[i]->setVisible( i<k );
	}

	//ѡ��˵��ɼ�
	ui.selectGroup->setVisible(true);
}
void dip::SetBHPF()
{
	//����˵�
	ui.inputGroup->setTitle(QStringLiteral("������˹��ͨ����"));
	ui.inputGroup->setVisible(true);
	QList<QLabel*>		labelChildren	 = ui.inputGroup->findChildren<QLabel*>();
	QList<QLineEdit*>	lineEidtChildren = ui.inputGroup->findChildren<QLineEdit*>();

	labelChildren[0]->setText(QStringLiteral("�뾶 D0"));
	//������������
	QRegExp regExp1("[0-9]{0,4}");
	lineEidtChildren[0]->setValidator(new QRegExpValidator(regExp1, NULL));

	//�ɼ���Ŀ i
	int k(1);
	for(int i=0; i!=5; ++i){
		labelChildren[i]->setVisible( i<k );
		lineEidtChildren[i]->setVisible( i<k );
	}

	//ѡ��˵��ɼ�
	ui.selectGroup->setVisible(true);
}
void dip::SetGHPF()
{
	//����˵�
	ui.inputGroup->setTitle(QStringLiteral("��˹��ͨ����"));
	ui.inputGroup->setVisible(true);
	QList<QLabel*>		labelChildren	 = ui.inputGroup->findChildren<QLabel*>();
	QList<QLineEdit*>	lineEidtChildren = ui.inputGroup->findChildren<QLineEdit*>();

	labelChildren[0]->setText(QStringLiteral("�뾶 D0"));
	//������������
	QRegExp regExp1("[0-9]{0,4}");
	lineEidtChildren[0]->setValidator(new QRegExpValidator(regExp1, NULL));

	//�ɼ���Ŀ i
	int k(1);
	for(int i=0; i!=5; ++i){
		labelChildren[i]->setVisible( i<k );
		lineEidtChildren[i]->setVisible( i<k );
	}

	//ѡ��˵��ɼ�
	ui.selectGroup->setVisible(true);
}
void dip::SetTHPF()
{
	//����˵�
	ui.inputGroup->setTitle(QStringLiteral("���θ�ͨ����"));
	ui.inputGroup->setVisible(true);
	QList<QLabel*>		labelChildren	 = ui.inputGroup->findChildren<QLabel*>();
	QList<QLineEdit*>	lineEidtChildren = ui.inputGroup->findChildren<QLineEdit*>();

	labelChildren[0]->setText(QStringLiteral("�뾶 D0"));
	labelChildren[1]->setText(QStringLiteral("�뾶 D1"));
	//������������
	QRegExp regExp1("[0-9]{0,4}");
	lineEidtChildren[0]->setValidator(new QRegExpValidator(regExp1, NULL));

	//�ɼ���Ŀ i
	int k(2);
	for(int i=0; i!=5; ++i){
		labelChildren[i]->setVisible( i<k );
		lineEidtChildren[i]->setVisible( i<k );
	}

	//ѡ��˵��ɼ�
	ui.selectGroup->setVisible(true);
}

void dip::GetColor()
{
	QString filename = (defaultSavePath+"\\GetColor.bmp").c_str();

	//�����Լ���ǰ�Ҷ�ֵ
	int pointX(labelSource->posx),pointY(BMP->Height-labelSource->posy);
	pointX = (pointX>BMP->Width-1)?(BMP->Width-1):pointX;
	pointY = (pointY>BMP->Height-1)?(BMP->Height-1):pointY;
	QString textColor = QStringLiteral("ȡɫ��:[%1,%2]").arg(pointX).arg(pointY);
	ui.colorGrid->setTitle(textColor);
	textColor = QStringLiteral("R :%1\nB :%2\nG :%3\n��:%4").arg(BMP->Array_Red->Array[pointY][pointX])\
		.arg(BMP->Array_Blue->Array[pointY][pointX]).arg(BMP->Array_Green->Array[pointY][pointX]).arg(BMP->Array_Gray->Array[pointY][pointX]);	
	ui.labelColor_1->setText(textColor);
	ui.labelColor_1->show();
	//����ͼ�����Ŵ�С --> Label
	CbmpArray *retb = BMP->Crop(BMP->Array_Blue, pointX-15, pointY-15, pointX+15, pointY+15, 0);
	CbmpArray *retg = BMP->Crop(BMP->Array_Green, pointX-15, pointY-15, pointX+15, pointY+15, 0);
	CbmpArray *retr = BMP->Crop(BMP->Array_Red, pointX-15, pointY-15, pointX+15, pointY+15, 0);
	CbmpArray *retB  = BMP->Scale(retb, 0, 0, 110, 110, 0);
	CbmpArray *retG  = BMP->Scale(retg, 0, 0, 110, 110, 0);
	CbmpArray *retR  = BMP->Scale(retr, 0, 0, 110, 110, 0);
	//��ʮ����
	for(PIXEL i=54;i!=56; ++i){
		for(PIXEL j=35;j!=75;++j){
			retR->Array[i][j] = 255;
			retR->Array[j][i] = 255;
		}
	}
	BMP->SaveAsColor(filename.toStdString(), BMP->bmpHead, retR->Width, retR->Height,retB, retG, retR);

	//ȡɫ����ʾСͼ��
	QImage *imgGetColor = new QImage;
	if(! ( imgGetColor->load(filename) ) ) //����ͼ��
	{
		QMessageBox::information(this, QStringLiteral("��ȡͼƬʧ��"), QStringLiteral("��ȡͼƬʧ��!"));
		delete imgGetColor;
		return;
	}
	//��ʾ���������
	ui.labelColor_2->setMinimumHeight(imgGetColor->height());
	ui.labelColor_2->setMinimumWidth(imgGetColor->width());
	ui.labelColor_2->setMaximumHeight(imgGetColor->height());
	ui.labelColor_2->setMaximumWidth(imgGetColor->width());
	ui.labelColor_2->setPixmap(QPixmap::fromImage(*imgGetColor));//����Ҫ�����wdget������
	ui.labelColor_2->show();
	delete imgGetColor, retB, retG, retR, retb, retg, retr;
}
void dip::GetColor_1()
{
	QString filename = (defaultSavePath+"\\GetColor_1.bmp").c_str();

	//�����Լ���ǰ�Ҷ�ֵ
	int pointX(labelOutput->posx),pointY(BMPOutput->Height-labelOutput->posy);
	pointX = (pointX>BMPOutput->Width-1)?(BMPOutput->Width-1):pointX;
	pointY = (pointY>BMPOutput->Height-1)?(BMPOutput->Height-1):pointY;
	QString textColor = QStringLiteral("ȡɫ��:[%1,%2]").arg(pointX).arg(pointY);
	ui.colorGrid->setTitle(textColor);
	textColor = QStringLiteral("R :%1\nB :%2\nG :%3\n��:%4").arg(BMPOutput->Array_Red->Array[pointY][pointX])\
		.arg(BMPOutput->Array_Blue->Array[pointY][pointX]).arg(BMPOutput->Array_Green->Array[pointY][pointX]).arg(BMPOutput->Array_Gray->Array[pointY][pointX]);	
	ui.labelColor_1->setText(textColor);
	ui.labelColor_1->show();
	//����ͼ�����Ŵ�С --> Label
	CbmpArray *retb = BMPOutput->Crop(BMPOutput->Array_Blue, pointX-15, pointY-15, pointX+15, pointY+15, 0);
	CbmpArray *retg = BMPOutput->Crop(BMPOutput->Array_Green, pointX-15, pointY-15, pointX+15, pointY+15, 0);
	CbmpArray *retr = BMPOutput->Crop(BMPOutput->Array_Red, pointX-15, pointY-15, pointX+15, pointY+15, 0);
	CbmpArray *retB  = BMPOutput->Scale(retb, 0, 0, 110, 110, 0);
	CbmpArray *retG  = BMPOutput->Scale(retg, 0, 0, 110, 110, 0);
	CbmpArray *retR  = BMPOutput->Scale(retr, 0, 0, 110, 110, 0);
	//��ʮ����
	for(PIXEL i=54;i!=56; ++i){
		for(PIXEL j=35;j!=75;++j){
			retR->Array[i][j] = 255;
			retR->Array[j][i] = 255;
		}
	}
	BMPOutput->SaveAsColor(filename.toStdString(), BMPOutput->bmpHead, retR->Width, retR->Height,retB, retG, retR);
	
	//ȡɫ����ʾСͼ��
	QImage *imgGetColor = new QImage;
	if(! ( imgGetColor->load(filename) ) ) //����ͼ��
	{
		QMessageBox::information(this, QStringLiteral("��ȡͼƬʧ��"), QStringLiteral("��ȡͼƬʧ��!"));
		delete imgGetColor;
		return;
	}
	//��ʾ���������
	ui.labelColor_2->setMinimumHeight(imgGetColor->height());
	ui.labelColor_2->setMinimumWidth(imgGetColor->width());
	ui.labelColor_2->setMaximumHeight(imgGetColor->height());
	ui.labelColor_2->setMaximumWidth(imgGetColor->width());
	ui.labelColor_2->setPixmap(QPixmap::fromImage(*imgGetColor));//����Ҫ�����wdget������
	ui.labelColor_2->show();
	delete imgGetColor, retB, retG, retR, retb, retg, retr;
}