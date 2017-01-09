/* 
 * Copyright (C) 2017 Te Ropu Awhina (Victoria University of Wellington)
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include <iostream>

#include <Qt>
#include <QFile>
#include <QRect>
#include <QStyle>
#include <QTextStream>
#include <QApplication>
#include <QInputDialog>
#include <QDesktopWidget>

#include "Birdview.hpp"
#include "ConnectDialog.hpp"

Birdview::Birdview()
{
    // Create layouts
    mainLayout = new QVBoxLayout;

    // Create widgets
    connectionButton = new QPushButton;
    connectionButton->setFlat(true);
    connect(connectionButton, &QPushButton::clicked,
            this, &Birdview::onConnectionButtonClicked);

    ys = new QCPDataMap;
    plot = new QCustomPlot;
    plot->addGraph();
    plot->graph()->setData(ys);
    plot->graph()->setAdaptiveSampling(true);
    plot->xAxis->setLabel("Time");
    plot->yAxis->setLabel("Acceleration");
    plot->xAxis->setRange(0, 1);
    plot->yAxis->setRange(0, 1);
    plot->replot();

    // Pack layouts
    mainLayout->addWidget(connectionButton);
    mainLayout->addWidget(plot);

    setLayout(mainLayout);
    setWindowTitle("Birdview");

    // Set window size
    QDesktopWidget* desktopWidget{QApplication::desktop()};
    QRect screenSize{desktopWidget->availableGeometry(this)};
    resize(screenSize.width() * 0.75, screenSize.height() * 0.75);

    // Center window
    QWidget* mainWindow{QWidget::window()};
    mainWindow->setGeometry(QStyle::alignedRect(Qt::LeftToRight,
                                                Qt::AlignCenter,
                                                size(),
                                                desktopWidget->availableGeometry()));

    // Start off in a disconnected state
    setConnected(false);
    replot = true;
    connect(&deviceSocket, static_cast<void(QTcpSocket::*)(QTcpSocket::SocketError)>(&QTcpSocket::error),
            this, &Birdview::onSocketError);
}

Birdview::~Birdview()
{
    if (connected()) {
        deviceSocket.disconnectFromHost();
    }
}

bool Birdview::connected()
{
    return deviceSocket.state() == QAbstractSocket::ConnectedState;
}

void Birdview::setConnected(bool state)
{
    if (state) {
        deviceIP = deviceSocket.peerName();
        connect(&deviceDataSocket, &QUdpSocket::readyRead,
                this, &Birdview::onDataReceived);
        deviceDataSocket.bind(QHostAddress::Any, PORT);
    } else {
        deviceIP.clear();
        deviceSocket.disconnectFromHost();
        deviceSocket.disconnect();
        deviceDataSocket.close();
        std::cout << "Disconnected" << std::endl;
    }

    QColor buttonColor{state ? buttonGreen : buttonRed};
    QString buttonText{state ? "Connected to " + deviceIP : "Disconnected"};

    connectionButton->setText(buttonText);
    connectionButton->setStyleSheet("QPushButton { background-color: " + buttonColor.name() + ";"
                                    "              border: none;"
                                    "              padding: 1em 1em 1em 1em; }"
                                    "QPushButton:hover:!pressed { background-color: " + buttonColor.lighter(110).name() + "; }"
                                    "QPushButton:pressed { background-color: " + buttonColor.darker(120).name() + "; }");
}

float Birdview::bytesToFloat(char* data)
{
    // Note that `data` is big endian (network byte order)
    const std::size_t size{sizeof(float)};
    union { char bytes[size]; float value; };
    for (auto i{0u}; i < size; ++i) {
        bytes[i] = data[size - i - 1];
    }

    return value;
}

bool Birdview::exportData(QString file)
{
    QFile outputFile{file};
    if (!outputFile.open(QIODevice::WriteOnly)) {
        return false;
    }

    QTextStream outputTextstream{&outputFile};
    outputTextstream << "timestamp x y z\n";

    for (auto timestamp : xs.keys()) {
        outputTextstream << timestamp << " "
                         << xs.value(timestamp).value << " "
                         << ys->value(timestamp).value << " "
                         << zs.value(timestamp).value << "\n";
    }

    return true;
}

void Birdview::onDataReceived()
{
    while (deviceDataSocket.hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(deviceDataSocket.pendingDatagramSize());
        deviceDataSocket.readDatagram(datagram.data(), datagram.size());

        double x{bytesToFloat(datagram.data())};
        double y{bytesToFloat(datagram.data() + 4)};
        double z{bytesToFloat(datagram.data() + 8)};
        double timestamp{bytesToFloat(datagram.data() + 12)};

        xs.insert(timestamp, QCPData(timestamp, x));
        ys->insert(timestamp, QCPData(timestamp, y));
        zs.insert(timestamp, QCPData(timestamp, z));

        if (y < currentMinY) {
            currentMinY = y;
        } else if (y > currentMaxY) {
            currentMaxY = y;
        }

        plot->xAxis->setRange(ys->isEmpty() ? timestamp : ys->firstKey(), timestamp);
        plot->yAxis->setRange(currentMinY, currentMaxY);

        if (replot) {
            plot->replot();
        } else {
            replot = false;
        }
    }
}

void Birdview::onConnectionButtonClicked()
{
    if (connected()) {
        setConnected(false);
    } else {
        ConnectDialog getIpAddress(&deviceIP, &deviceSocket);

        // Note that the rejected case (when an error occurs) is handled by
        // onSocketError()
        if (getIpAddress.exec() == QDialog::Accepted) {
            setConnected(true);
        }
    }
}

void Birdview::onSocketError(QAbstractSocket::SocketError error)
{
    setConnected(false);
    connectionButton->setText("Disconnected: Socket error " + QString::number(error));
}
