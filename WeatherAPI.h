#ifndef WEATHERAPI_H
#define WEATHERAPI_H

#include <QWidget>
#include <QNetworkAccessManager>
#include <QNetworkReply>

QT_BEGIN_NAMESPACE
namespace Ui { class WeatherAPI; }
QT_END_NAMESPACE

class WeatherAPI : public QWidget
{
    Q_OBJECT

public:
    explicit WeatherAPI(QWidget *parent = nullptr);
    ~WeatherAPI();

signals:
    void on_ButtonSwitchAPI_clicked();

private slots:
    // Slot, der ausgeführt wird, wenn der "Fetch"-Button geklickt wird
    void on_fetchButton_clicked();

    // Slot, der ausgeführt wird, wenn eine Anfrage (Reply) fertig ist
    void onReplyFinished(QNetworkReply *reply);

private:
    Ui::WeatherAPI *ui;

    // Verwalter für alle Netzwerk-Anfragen
    QNetworkAccessManager *manager;
};

#endif // WEATHERAPI_H
