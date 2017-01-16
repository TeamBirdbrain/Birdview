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
#include <QGroupBox>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QPushButton>
#include <QVBoxLayout>
#include <QSharedPointer>

#include "../qcustomplot/qcustomplot.h"

using Birdcage = QSharedPointer<QCPGraphDataContainer>;

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

    Birdcage xs;
    Birdcage ys;
    Birdcage zs;
    double currentMaxY{std::numeric_limits<double>::min()};
    double currentMinY{std::numeric_limits<double>::max()};

    QCustomPlot* plot;
    QGroupBox* groupsBox;
    QVBoxLayout* mainLayout;
    QPushButton* recordButton;
    QPushButton* connectionButton;

    bool replot;
    bool recording;
    QString deviceIP;
    QTcpSocket deviceSocket;
    QUdpSocket deviceDataSocket;
    const int DEVICE_BUFFER_SIZE = 4 * 4;

    const QColor buttonRed{"#FF8589"};
    const QColor buttonGreen{"#47B84B"};

private slots:
    void onDataReceived();
    void onRecordClicked();
    void onAxisChanged(int);
    void onConnectionButtonClicked();
    void onSocketError(QTcpSocket::SocketError);
};

#endif
