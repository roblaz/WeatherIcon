#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    myIcon = new QPixmap("C:/Users/Robert/Desktop/Cplusplus/OpenWeatherIcon/256x256/Empty.png");
    //myIcon = new QPixmap("./256x256/Empty.png");
    ui->label_iconID->setPixmap(*myIcon);
    delete myIcon;

    netManager = new QNetworkAccessManager(this);

    connect(netManager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_Get_clicked()
{
    QString qsCityName= ui->comboBox->currentText();
    QString strID;
    if (qsCityName == "Tallinn")
       strID = "588409";
    else if (qsCityName == "Tartu")
        strID = "588335";
    else if (qsCityName == "Narva")
       strID = "590031";
    else if (qsCityName == "Parnu")
       strID = "589580";
    else if (qsCityName == "Valga")
       strID = "587876";

    QString strRequest = "https://api.openweathermap.org/data/2.5/weather?id=" +
            strID + "&units=metric&appid=732a21594c62a625fa05abb9a35b8028";
    qDebug() << "Request is ---------------------";
    qDebug() << strRequest;

    QUrl address;

    address.setUrl(strRequest, QUrl::StrictMode);

    if (address.isValid())
        netManager->get(QNetworkRequest(address));
}

void MainWindow::replyFinished(QNetworkReply *reply)
{
   if ( reply->error() != QNetworkReply::NoError){
        qWarning() <<"ErrorNo: "<< reply->error() << "for url: " << reply->url().toString();
        qDebug() << "Request failed, " << reply->errorString();
        qDebug() << "Headers:"<<  reply->rawHeaderList()<< "content:" << reply->readAll();
        return;
    }

    QList<QByteArray> replyHeaders = reply->rawHeaderList();
    QString qsHeadersList;
    QByteArray baContent = reply->readAll();
    QString qsContent = baContent;
    QPair<QByteArray, QByteArray> HeaderPair;

    if (!qsContent.isEmpty()){
        parseJson(qsContent.toLatin1());
    }
}

void MainWindow::parseJson(QByteArray inputArray)
{
    qDebug() << "parseJson";
    QJsonParseError parError;
    QJsonDocument mainJson = QJsonDocument::fromJson(inputArray,&parError);

    if (parError.error != QJsonParseError::NoError) {
        qDebug() << "Error in parsing" << parError.errorString();
        return;
    }

    QJsonObject myObj = mainJson.object();

    QJsonArray listArray = myObj["weather"].toArray();
    //qDebug() << "All key:" << myObj.keys();
    QJsonArray weatherArray;
    QJsonValue listValue, weatherValue;
    QJsonObject listObj, weatherObj;
    QJsonObject mainObj;
    QString strResult = "";
    QString strIconFileName;

    for (int il = 0; il < listArray.size(); il++) {
        listValue = listArray.at(il);
        listObj = listValue.toObject();
        qDebug() << "All key:" << listObj.keys();
        strResult = listObj["main"].toString();
        if (strResult.toLower().contains("cloud"))
            strIconFileName = "cloudiness.png";
        else if (strResult.toLower().contains("clear"))
            strIconFileName = "sun.png";
        else if (strResult.toLower().contains("fog"))
            strIconFileName = "fog.png";
        else if (strResult.toLower().contains("rain"))
            strIconFileName = "rain.png";
        else if (strResult.toLower().contains("snow"))
            strIconFileName = "snow.png";
        else if (strResult.toLower().contains("thunderstorm"))
            strIconFileName = "thunderstorm.png";
        else
            strIconFileName = "Empty.png";

        myIcon = new QPixmap("C:/Users/Robert/Desktop/Cplusplus/OpenWeatherIcon/256x256"+ strIconFileName);
        ui->label_iconID->setPixmap(*myIcon);
        delete myIcon;

    }

}
