/* 
 * Copyright (C) 2016 James Wrigley
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef BIRDVIEW_HPP
#define BIRDVIEW_HPP

#include <QColor>
#include <QString>
#include <QWidget>
#include <QTcpSocket>
#include <QPushButton>
#include <QVBoxLayout>

class Birdview : public QWidget
{
    Q_OBJECT

public:
    Birdview();
    ~Birdview();

    static const int PORT = 1998;

private:
    bool connected();
    void setConnected(bool);

    QString deviceIP;
    QTcpSocket deviceSocket;

    QVBoxLayout* mainLayout;
    QPushButton* connectionButton;

    const QColor buttonRed{"#FF8589"};
    const QColor buttonGreen{"#47B84B"};

private slots:
    void onConnectionButtonClicked();
};

#endif
