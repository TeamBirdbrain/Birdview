/* 
 * Copyright (C) 2017 Te Ropu Awhina (Victoria University of Wellington)
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef BIRDVIEW_HPP
#define BIRDVIEW_HPP

#include <limits>

#include <QColor>
#include <QString>
#include <QWidget>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QPushButton>
#include <QVBoxLayout>
#include <qcustomplot.h>

class Birdview : public QWidget
{
    Q_OBJECT

public:
    Birdview();
    ~Birdview();

    static const int PORT = 1998;

private:
    bool connected();
    bool exportData(QString);
    void setConnected(bool);
    float bytesToFloat(char*);

    QCPDataMap xs;
    QCPDataMap* ys;
    QCPDataMap zs;
    double currentMaxY{std::numeric_limits<double>::min()};
    double currentMinY{std::numeric_limits<double>::max()};

    QCustomPlot* plot;
    QVBoxLayout* mainLayout;
    QPushButton* connectionButton;

    bool replot;
    QString deviceIP;
    QTcpSocket deviceSocket;
    QUdpSocket deviceDataSocket;
    const int DEVICE_BUFFER_SIZE = 4 * 4;

    const QColor buttonRed{"#FF8589"};
    const QColor buttonGreen{"#47B84B"};

private slots:
    void onDataReceived();
    void onConnectionButtonClicked();
    void onSocketError(QTcpSocket::SocketError);
};

#endif
