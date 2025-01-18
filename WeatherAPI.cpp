#include "WeatherAPI.h"
#include "ui_WeatherAPI.h"

// Für JSON-Verarbeitung
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

WeatherAPI::WeatherAPI(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::WeatherAPI)
    , manager(nullptr)
{
    ui->setupUi(this);

    // Initialisiert das QNetworkAccessManager-Objekt
    manager = new QNetworkAccessManager(this);

    // Verbindet das "finished"-Signal des Managers mit unserem Slot
    connect(manager, &QNetworkAccessManager::finished,
            this, &WeatherAPI::onReplyFinished);
}

WeatherAPI::~WeatherAPI()
{
    delete ui;
}

void WeatherAPI::on_fetchButton_clicked()
{
    // Liest den vom Benutzer eingegebenen Stadt-/Ortsnamen
    QString ortsName = ui->InputField->text().trimmed();

    // Erzeugt die Geokodierungs-URL (um lat/lon zu erhalten)
    QString geoUrl = QString("http://api.openweathermap.org/geo/1.0/direct?"
                             "q=%1&limit=5&appid=9683950bae0b8b0a5b94c1527dd7acf9")
                         .arg(ortsName);

    // Sendet die erste Anfrage (Geokodierung)
    QNetworkReply *geoReply = manager->get(QNetworkRequest(QUrl(geoUrl)));

    // Markiert diese Anfrage als "geokodierung"
    geoReply->setProperty("requestType", "geokodierung");
}

void WeatherAPI::onReplyFinished(QNetworkReply *reply)
{
    // Falls ein Netzwerkfehler aufgetreten ist, geben wir diesen aus
    if (reply->error() != QNetworkReply::NoError) {
        ui->ResponseTextField->setPlainText("Fehler: " + reply->errorString());
        reply->deleteLater();
        return;
    }

    // Liest den zuvor festgelegten "Typ" der Anfrage
    QString requestType = reply->property("requestType").toString();

    // Liest die empfangenen Rohdaten als JSON
    QByteArray rawJson = reply->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(rawJson);

    // --- ERSTER ANFRAGETYP: "geokodierung" ---
    if (requestType == "geokodierung") {

        // Prüfen, ob es sich um ein gültiges JSON-Array handelt
        if (!jsonDoc.isNull() && jsonDoc.isArray()) {
            QJsonArray jsonArray = jsonDoc.array();

            // String zum Anzeigen der Geodaten
            QString geoErgebnis;

            // Wenn wir mindestens einen Eintrag haben, nehmen wir den ersten
            if (!jsonArray.isEmpty()) {
                QJsonObject erstesObjekt = jsonArray.first().toObject();

                // Extrahiert Name, lat, lon, Land
                QString name    = erstesObjekt.value("name").toString();
                double lat      = erstesObjekt.value("lat").toDouble();
                double lon      = erstesObjekt.value("lon").toDouble();
                QString country = erstesObjekt.value("country").toString();

                // Baut einen formatierten String für die erste Anzeige
                geoErgebnis += QString("Name: %1\nBreitengrad (lat): %2\nLängengrad (lon): %3\nLand: %4\n\n")
                                   .arg(name)
                                   .arg(lat)
                                   .arg(lon)
                                   .arg(country);

                // Zeigt die Geokodierungsdaten im ersten Textfeld an
                ui->ResponseTextField->setPlainText(geoErgebnis);

                // Baut nun die URL für die Wetteranfrage
                QString wetterUrl = QString("https://api.openweathermap.org/data/2.5/weather?"
                                            "lat=%1&lon=%2&appid=9683950bae0b8b0a5b94c1527dd7acf9&units=metric&lang=de")
                                        .arg(lat)
                                        .arg(lon);

                // Sendet die zweite Anfrage (Wetter)
                QNetworkReply *wetterReply = manager->get(QNetworkRequest(QUrl(wetterUrl)));
                wetterReply->setProperty("requestType", "wetter");
            } else {
                // Falls kein Eintrag gefunden wurde
                ui->ResponseTextField->setPlainText("Keine Ergebnisse bei der Geokodierung.");
            }
        } else {
            ui->ResponseTextField->setPlainText("Ungültiges Geokodierungs-JSON oder kein Array.");
        }

        // --- ZWEITER ANFRAGETYP: "wetter" ---
    } else if (requestType == "wetter") {

        // Prüfen, ob wir ein gültiges JSON-Objekt haben
        if (!jsonDoc.isNull() && jsonDoc.isObject()) {
            QJsonObject jsonObj = jsonDoc.object();

            // "main"-Objekt enthält Temperaturdaten
            QJsonObject mainObj = jsonObj.value("main").toObject();
            double temp         = mainObj.value("temp").toDouble();
            double feelsLike    = mainObj.value("feels_like").toDouble();
            double tempMin      = mainObj.value("temp_min").toDouble();
            double tempMax      = mainObj.value("temp_max").toDouble();

            // "weather"-Array enthält Wetterbeschreibung
            QString beschreibung;
            QJsonArray wetterArray = jsonObj.value("weather").toArray();
            if (!wetterArray.isEmpty()) {
                QJsonObject wObj = wetterArray.first().toObject();
                beschreibung     = wObj.value("description").toString();
            }

            // Baut einen formatierten String mit den Wetterdaten
            QString wetterErgebnis = QString("Aktuelle Wetterdaten:\n")
                                     + QString("Temperatur: %1°C\n").arg(temp)
                                     + QString("Gefühlt wie: %1°C\n").arg(feelsLike)
                                     + QString("Min: %1°C\n").arg(tempMin)
                                     + QString("Max: %1°C\n").arg(tempMax)
                                     + QString("Beschreibung: %1\n").arg(beschreibung);

            // Zeigt die Wetterergebnisse im zweiten Textfeld an
            ui->ResponseTextFieldR->setPlainText(wetterErgebnis);

        } else {
            ui->ResponseTextFieldR->setPlainText("Ungültiges Wetter-JSON oder kein Objekt.");
        }
    }

    // Gibt den Speicher für das Reply-Objekt frei
    reply->deleteLater();
}
