#include "RickAndMorty.h"
#include "ui_RickAndMorty.h"

#include <QResizeEvent>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QPixmap>
#include <QDebug>

RickAndMorty::RickAndMorty(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RickAndMorty)
    , m_scene(new QGraphicsScene(this))
{
    ui->setupUi(this);

    // Setzt die Szene, in der das Charakter-Bild angezeigt wird
    ui->graphicsView->setScene(m_scene);

    // Verbindet das Signal, das gesendet wird, wenn eine Anfrage fertig ist,
    // mit unserem Slot zum Verarbeiten der Antwort
    connect(&m_manager, &QNetworkAccessManager::finished,
            this, &RickAndMorty::onNetworkReplyFinished);
}

RickAndMorty::~RickAndMorty()
{
    delete ui;
}

void RickAndMorty::resizeEvent(QResizeEvent *event)
{
    // Ruft zuerst das Standard-Resize-Verhalten auf
    QWidget::resizeEvent(event);

    // Skaliert die Ansicht so, dass das Bildverhältnis beibehalten wird
    ui->graphicsView->fitInView(m_scene->itemsBoundingRect(), Qt::KeepAspectRatio);
}

void RickAndMorty::on_fetchButton_clicked()
{
    // Liest die eingegebene Charakter-ID (z.B. "1" für Rick, "2" für Morty, etc.)
    QString characterId = ui->InputField->text().trimmed();
    if (characterId.isEmpty()) {
        ui->textBrowser->setPlainText("Bitte eine Character-ID eingeben.");
        m_scene->clear();
        return;
    }

    // Baut die URL für die Rick and Morty API
    QString urlStr = QString("https://rickandmortyapi.com/api/character/%1")
                         .arg(characterId);
    QUrl url(urlStr);

    // Erstellt die Netzwerk-Anfrage
    QNetworkRequest request(url);

    // Führt die Anfrage aus
    QNetworkReply *reply = m_manager.get(request);

    // Kennzeichnet die Anfrage als "characterData"
    reply->setProperty("requestType", "characterData");

    // Gibt dem Benutzer Feedback, dass wir Daten abfragen
    ui->ResponseTextField->setPlainText("Charakterdaten werden abgerufen...");
}

void RickAndMorty::fetchCharacterImage(const QString &imageUrl)
{
    // Erstellt eine Anfrage für das Bild, das von der API zurückgegeben wurde
    QUrl url(imageUrl);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::UserAgentHeader, "MyQtApp/1.0");

    // Führt die Anfrage aus
    QNetworkReply *reply = m_manager.get(request);

    // Kennzeichnet die Anfrage als "characterImage"
    reply->setProperty("requestType", "characterImage");
}

void RickAndMorty::onNetworkReplyFinished(QNetworkReply *reply)
{
    // Liest den zuvor gesetzten Typ der Anfrage
    QString requestType = reply->property("requestType").toString();

    // Überprüft, ob ein Netzwerkfehler aufgetreten ist
    if (reply->error() != QNetworkReply::NoError) {
        // Leert die Szene
        m_scene->clear();

        // Zeigt ein Fehlerbild (falls vorhanden) an
        QPixmap pix(":/Images/Images/404Error.png");
        m_scene->addPixmap(pix);
        m_scene->setSceneRect(m_scene->itemsBoundingRect());
        ui->graphicsView->fitInView(m_scene->itemsBoundingRect(),
                                    Qt::KeepAspectRatio);

        // Zeigt den Fehler im UI an
        QString err = QString("Netzwerkfehler [%1]: %2")
                          .arg(requestType, reply->errorString());
        ui->ResponseTextField->setPlainText(err);
        ui->textBrowser->setPlainText("Bitte eine andere Character-ID eingeben.");
        reply->deleteLater();
        return;
    }

    // Falls kein Fehler vorliegt, verarbeiten wir die Antwort je nach Anfrage-Typ
    if (requestType == "characterData") {
        // 1) Lies die empfangenen Daten ein
        QByteArray responseData = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);

        // 2) Überprüfe, ob die Daten ein gültiges JSON-Objekt sind
        if (!jsonDoc.isObject()) {
            ui->ResponseTextField->setPlainText("Ungültiges JSON oder kein Objekt erhalten.");
            reply->deleteLater();
            return;
        }

        // 3) Gültiges Objekt -> Werte auslesen
        QJsonObject jsonObj = jsonDoc.object();

        // Extrahiere wichtige Felder aus dem JSON
        QString name     = jsonObj.value("name").toString();
        QString status   = jsonObj.value("status").toString();
        QString species  = jsonObj.value("species").toString();
        QString imageUrl = jsonObj.value("image").toString();

        // 4) Baue einen zusammenhängenden String mit allen empfangenen Feldern
        //    für die ausführliche Ausgabe
        QString fullOutput;
        for (auto it = jsonObj.constBegin(); it != jsonObj.constEnd(); ++it) {
            QString key = it.key();
            QJsonValue val = it.value();

            QString valAsString;
            if      (val.isString()) valAsString = val.toString();
            else if (val.isDouble()) valAsString = QString::number(val.toDouble());
            else if (val.isBool())   valAsString = val.toBool() ? "true" : "false";
            else if (val.isNull())   valAsString = "null";
            else if (val.isObject()) valAsString = "Objekt { ... }";
            else if (val.isArray())  valAsString = "Array [ ... ]";

            fullOutput.append(QString("%1: %2\n").arg(key, valAsString));
        }

        // Setze die ausführliche Antwort im ResponseTextField
        ui->ResponseTextField->setPlainText(
            QString("Vollständige Antwort vom Server:\n%1").arg(fullOutput));

        // 5) Zeige eine Kurzinfo im textBrowser (Name, Status, Spezies)
        if (!name.isEmpty()) {
            ui->textBrowser->setText(
                QString("Name: %1\nStatus: %2\nSpezies: %3")
                    .arg(name, status, species));
        } else {
            ui->ResponseTextField->setPlainText("Keine Charakterdaten gefunden.");
        }

        // 6) Falls es eine Bild-URL gibt, rufen wir das Bild ab, ansonsten löschen wir die Szene
        if (!imageUrl.isEmpty()) {
            fetchCharacterImage(imageUrl);
        } else {
            m_scene->clear();
        }

    } else if (requestType == "characterImage") {
        // Dies ist die Antwort, die das Charakterbild enthält
        QByteArray imageData = reply->readAll();
        QPixmap pixmap;

        // Versuche, die Bilddaten in ein QPixmap zu laden
        if (pixmap.loadFromData(imageData)) {
            // Alles OK -> Szene leeren und das neue Bild darstellen
            m_scene->clear();
            m_scene->addPixmap(pixmap);
            m_scene->setSceneRect(m_scene->itemsBoundingRect());

            // Ansicht zurücksetzen und Bild passend einpassen
            ui->graphicsView->resetTransform();
            ui->graphicsView->fitInView(m_scene->sceneRect(), Qt::KeepAspectRatio);
        }
    }

    // Antwortobjekt wird nicht mehr benötigt
    reply->deleteLater();
}
