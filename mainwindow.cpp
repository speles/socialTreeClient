#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->btnConnect, SIGNAL(clicked()), this, SLOT(connectClicked()));
    connect(ui->btnSend, SIGNAL(clicked()), this, SLOT(sendClicked()));
    connect(ui->edtImage, SIGNAL(returnPressed()), this, SLOT(sendClicked()));
    connect(ui->btnDisconnect, SIGNAL(clicked()), this, SLOT(disconnectClicked()));
    connect(ui->rbtnAdd, SIGNAL(toggled(bool)), this, SLOT(addTogled(bool)));
    connect(ui->rbtnDelete, SIGNAL(toggled(bool)), this, SLOT(deleteTogled(bool)));
    connection = new QTcpSocket(this);
    connect(connection, SIGNAL(readyRead()), this, SLOT(readTcpData()));
    connect(connection, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
    ui->txtHistory->setTextInteractionFlags(0);
    addLogLine("Connect to server to send data", LOG);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::connectClicked()
{
    addLogLine("Connecting...", LOG);
    connection->connectToHost(ui->edtHost->text(), ui->edtPort->text().toUShort());
    ui->edtHost->setEnabled(false);
    ui->edtPort->setEnabled(false);
    ui->btnConnect->setEnabled(false);
    if (connection->waitForConnected())
    {
        ui->btnDisconnect->setEnabled(true);
        ui->btnSend->setEnabled(true);
        addLogLine("OK", LOG);
    } else
    {
        ui->edtHost->setEnabled(true);
        ui->edtPort->setEnabled(true);
        ui->btnConnect->setEnabled(true);
        addLogLine("FAIL :(", LOG);
        addLogLine(connection->errorString(), LOG);
    }
}

void MainWindow::disconnectClicked()
{
    addLogLine("Disconnecting...", LOG);
    connection->disconnectFromHost();
    ui->btnDisconnect->setEnabled(false);
    connection->waitForDisconnected(); //bug: waitForDisconnected() always return false
//    if (!connection->waitForDisconnected())
//    {
//        ui->btnDisconnect->setEnabled(true);
//        addLogLine("FAIL :(", LOG);
//        addLogLine(connection->errorString(), LOG);
//    }
}

void MainWindow::sendClicked()
{
    QString dataStr;
    if (ui->rbtnAdd->isChecked())
        dataStr.append("ADD "); else
        dataStr.append("DELETE ");
    dataStr.append(ui->edtName->text());
    if (ui->rbtnAdd->isChecked())
        dataStr.append(" ").append(ui->edtParent->text()).append(" ").append(ui->edtImage->text());
    addLogLine(dataStr, SEND);
    dataStr.append('\n');
    QByteArray data;
    data.append(dataStr);
    connection->write(data);
}

void MainWindow::readTcpData()
{
    while (connection->canReadLine())
    {
        QByteArray data = connection->readLine();
        QTextStream str(data);
        addLogLine(str.readAll(), RECV);
    }
}

void MainWindow::addLogLine(const QString& line, logLineType type)
{
    const QString htmlStartLog = "<font color=\"black\">";
    const QString htmlStartSend = "<font color=\"green\">";
    const QString htmlStartRecv = "<font color=\"blue\">";
    const QString htmlStartUnknown = "<font color=\"gray\">";
    const QString htmlEnd = "</font><br>";
    QString fullMessage;
    switch (type) {
    case LOG:
        fullMessage.append(htmlStartLog).append(" LOG: ");
        break;
    case RECV:
        fullMessage.append(htmlStartRecv).append("RECV: ");
        break;
    case SEND:
        fullMessage.append(htmlStartSend).append("SEND: ");
        break;
    default:
        fullMessage.append(htmlStartUnknown).append("????: ");
        break;
    }
    fullMessage.append(line).append(htmlEnd);
    ui->txtHistory->insertHtml(fullMessage);
}

void MainWindow::onDisconnected()
{
    ui->edtHost->setEnabled(true);
    ui->edtPort->setEnabled(true);
    ui->btnConnect->setEnabled(true);
    ui->btnSend->setEnabled(false);
    addLogLine("Disconnected", LOG);
}

void MainWindow::addTogled(bool val)
{
    ui->edtImage->setEnabled(true);
    ui->edtParent->setEnabled(true);
}

void MainWindow::deleteTogled(bool val)
{
    ui->edtImage->setEnabled(false);
    ui->edtParent->setEnabled(false);
}
