#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "WeatherAPI.h"
#include "RickAndMorty.h"
#include <QMainWindow>
#include <qstackedwidget.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void switchWindow();

private:
    Ui::MainWindow *ui;
    RickAndMorty *rickAndMortyWidget;
    WeatherAPI *weatherApiWidget;
    QStackedWidget *stackedWidget;
};

#endif // MAINWINDOW_H
