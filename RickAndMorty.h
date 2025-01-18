#ifndef RICKANDMORTY_H
#define RICKANDMORTY_H


#include <QWidget>
#include <QGraphicsScene>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QDebug>
#include <QResizeEvent>


QT_BEGIN_NAMESPACE
namespace Ui {
class RickAndMorty;
}
QT_END_NAMESPACE


class RickAndMorty : public QWidget
{
    Q_OBJECT

public:
    explicit RickAndMorty(QWidget *parent = nullptr);
    ~RickAndMorty();


protected:
    // Wird aufgerufen, wenn das Fenster/Widget in der Größe geändert wird.
    void resizeEvent(QResizeEvent *event) override;

private slots:
    // Slot, der ausgeführt wird, wenn der "Fetch"-Button geklickt wurde
    void on_fetchButton_clicked();

    // Slot, der ausgeführt wird, wenn eine Netzwerk-Antwort (Reply) fertig ist
    void onNetworkReplyFinished(QNetworkReply *reply);

signals:
    // Switcht Fenster
    void on_ButtonSwitchAPI_clicked();

private:
    Ui::RickAndMorty *ui;

    // Szene für die Darstellung von Charakter-Bildern
    QGraphicsScene *m_scene;

    // Netzwerk-Manager für alle Requests
    QNetworkAccessManager m_manager;

    // Hilfsfunktion, um das Bild eines Charakters abzurufen
    void fetchCharacterImage(const QString &imageUrl);
};

#endif
