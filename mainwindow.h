#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QStandardItemModel>
#include <QFile>
#include <QtNetwork/QTcpSocket>
#include <iostream>

namespace Ui {
class MainWindow;
}

enum logLineType {LOG, RECV, SEND};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QTcpSocket* connection;
public slots:
    void connectClicked();
    void disconnectClicked();
    void sendClicked();
    void readTcpData();
    void addLogLine(const QString& line, logLineType type);
    void onDisconnected();
    void addTogled(bool val);
    void deleteTogled(bool val);
};

#endif // MAINWINDOW_H
