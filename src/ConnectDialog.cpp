/* 
 * Copyright (C) 2017 Te Ropu Awhina (Victoria University of Wellington)
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include <iostream>

#include "Birdview.hpp"
#include "ConnectDialog.hpp"

ConnectDialog::ConnectDialog(QString* ipAddress, QTcpSocket* socket)
{
    millisPassed = 0u;
    this->socket = socket;
    this->ipAddress = ipAddress;

    timer = new QTimer(this);
    connectWidget = new QWidget;
    connectingWidget = new QWidget;
    widgetStack = new QStackedLayout;

    connectLabel = new QLabel("Enter IP address of mobile device:");
    connectingLabel = new QLabel("Connecting...");

    ipLineEdit = new QLineEdit;
    ipLineEdit->setPlaceholderText("IP address");

    connectButton = new QPushButton("Connect");
    connect(connectButton, &QPushButton::clicked,
            this, &ConnectDialog::onConnectButtonClicked);

    connectingBar = new QProgressBar;
    connectingBar->setRange(0, 0);

    buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    buttonLayout->addWidget(connectButton);
    buttonLayout->addStretch();

    connectLayout = new QVBoxLayout;
    connectLayout->addWidget(connectLabel);
    connectLayout->addWidget(ipLineEdit);
    connectLayout->addLayout(buttonLayout);

    connectingLayout = new QVBoxLayout;
    connectingLayout->addWidget(connectingLabel);
    connectingLayout->addWidget(connectingBar);

    connectWidget->setLayout(connectLayout);
    connectingWidget->setLayout(connectingLayout);
    widgetStack->addWidget(connectWidget);
    widgetStack->addWidget(connectingWidget);

    setLayout(widgetStack);
    setWindowTitle("Connect to Device");
    setWindowFlags(Qt::Dialog);
}

void ConnectDialog::onConnectButtonClicked()
{
    widgetStack->setCurrentIndex(1);

    connect(timer, &QTimer::timeout,
            [&] () {
                if (socket->state() == QTcpSocket::ConnectedState) {
                    std::cout << "Connected" << std::endl;
                    timer->stop();
                    done(QDialog::Accepted);
                } else {
                    millisPassed += timer->interval();

                    if (millisPassed >= TIMEOUT_MILLIS) {
                        std::cout << "Timed out" << std::endl;
                        timer->stop();
                        done(QDialog::Rejected);
                    } else if (socket->error() > -1) {
                        std::cout << "Socket error: " << socket->error() << std::endl;
                        timer->stop();
                        done(QDialog::Rejected);
                    }
                }
            });

    socket->connectToHost(ipLineEdit->text(), Birdview::PORT);
    timer->start(500);
}
