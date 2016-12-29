/* 
 * Copyright (C) 2016 James Wrigley
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include <iostream>

#include <QTimer>

#include "Birdview.hpp"
#include "ConnectDialog.hpp"

ConnectDialog::ConnectDialog(QString* ipAddress, QTcpSocket* socket)
{
    this->socket = socket;
    this->ipAddress = ipAddress;

    label = new QLabel("Enter IP address of mobile device:");

    ipLineEdit = new QLineEdit;
    ipLineEdit->setPlaceholderText("IP address");

    connectButton = new QPushButton("Connect");
    connect(connectButton, &QPushButton::clicked,
            this, &ConnectDialog::onConnectButtonClicked);

    buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    buttonLayout->addWidget(connectButton);
    buttonLayout->addStretch();

    mainLayout = new QVBoxLayout;
    mainLayout->addWidget(label);
    mainLayout->addWidget(ipLineEdit);
    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);
    setWindowTitle("Connect to Device");
    setWindowFlags(Qt::Dialog);
}

void ConnectDialog::onConnectButtonClicked()
{
    int millisPassed{0};
    QTimer* timer{new QTimer(this)};

    connect(socket, static_cast<void(QAbstractSocket::*)(QAbstractSocket::SocketError)>(&QAbstractSocket::error),
            [&] (QAbstractSocket::SocketError) {
                timer->stop();
                std::cout << socket->errorString().toStdString() << std::endl;

                done(QDialog::Rejected);
            });

    connect(timer, &QTimer::timeout,
            [&] () {
                if (socket->isValid()) {
                    timer->stop();
                    std::cout << "Connected" << std::endl;
                    done(QDialog::Accepted);
                } else {
                    millisPassed += timer->interval();

                    if (millisPassed >= TIMEOUT_MILLIS) {
                        timer->stop();
                        std::cout << "Timed out" << std::endl;
                        done(QDialog::Rejected);
                    }
                }
            });

    socket->connectToHost(ipLineEdit->text(), Birdview::PORT);
    timer->start(500);
}
