#include "mainwindow.h"
#include "ui_mainwindow.h"

void MainWindow::setWelcomePhoto() {
    int numberOfPhotos = 3;

    srand(time(0));
    int photoNum = rand() % numberOfPhotos;

    QString photoString = QString::number(photoNum);

    QString tempPath("../../../../CoverPhoto");
    tempPath.append(photoString);
    QString filePath = tempPath.append(".jpg");
    qDebug() << filePath;

    welcomePic.load(filePath);
    int width = ui->labelPic->width();
    int height = ui->labelPic->height();
    ui->labelPic->setPixmap(welcomePic.scaled(width, height, Qt::KeepAspectRatio));
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWelcomePhoto();
}

MainWindow::~MainWindow()
{
    delete ui;
}

