#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QStackedWidget"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , stackedWidget(new QStackedWidget(this))
{
    ui->setupUi(this);

    // Instanzen der Widgets anlegen
    rickAndMortyWidget = new RickAndMorty(this);
    weatherApiWidget   = new WeatherAPI(this);

    setCentralWidget(stackedWidget);

    stackedWidget->addWidget(rickAndMortyWidget);
    stackedWidget->addWidget(weatherApiWidget);

    this->setWindowTitle(QString("RickAndMorty"));

    connect(rickAndMortyWidget,&RickAndMorty::on_ButtonSwitchAPI_clicked,this,&MainWindow::switchWindow);
    connect(weatherApiWidget,&WeatherAPI::on_ButtonSwitchAPI_clicked,this,&MainWindow::switchWindow);

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::switchWindow(){
    int currentIdx = stackedWidget->currentIndex();
    //  - Index 0 = RickAndMorty
    //  - Index 1 = WeatherAPI
    int newIdx = (currentIdx == 0) ? 1 : 0;

    stackedWidget->setCurrentIndex(newIdx);

    if (newIdx == 0) {
        setWindowTitle("RickAndMorty");
    } else {
        setWindowTitle("WeatherAPI");
    }
}
