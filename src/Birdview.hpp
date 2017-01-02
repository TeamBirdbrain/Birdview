/* 
 * Copyright (C) 2016 James Wrigley
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef BIRDVIEW_HPP
#define BIRDVIEW_HPP

#include <forward_list>

#include <QColor>
#include <QString>
#include <QWidget>
#include <QTcpSocket>
#include <QPushButton>
#include <QVBoxLayout>

struct AccelerationStamp
{
    float x;
    float y;
    float z;
    long long timestamp;
};

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
    template<typename T> T bytesToNumeric(char*);

    std::forward_list<AccelerationStamp> stamps;
    std::forward_list<AccelerationStamp>::iterator lastStamp;

    QString deviceIP;
    int deviceBufferSize;
    QTcpSocket deviceSocket;

    QVBoxLayout* mainLayout;
    QPushButton* connectionButton;

    const int BUFFER_SIZE = 3 * 4 + 8;

    const QColor buttonRed{"#FF8589"};
    const QColor buttonGreen{"#47B84B"};

private slots:
    void onDataReceived();
    void onConnectionButtonClicked();
    void onSocketError(QTcpSocket::SocketError);
};

#endif
