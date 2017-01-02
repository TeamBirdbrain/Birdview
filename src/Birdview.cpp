/* 
 * Copyright (C) 2016 James Wrigley
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
    mainLayout->setAlignment(Qt::AlignHCenter);

    // Create widgets
    connectionButton = new QPushButton;
    connectionButton->setFlat(true);
    connect(connectionButton, &QPushButton::clicked,
            this, &Birdview::onConnectionButtonClicked);

    // Pack layouts
    mainLayout->addWidget(connectionButton);

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
        connect(&deviceSocket, &QTcpSocket::readyRead,
                this, &Birdview::onDataReceived);
    } else {
        deviceIP.clear();
        deviceSocket.disconnectFromHost();
        deviceSocket.disconnect();
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

template<typename T>
T Birdview::bytesToNumeric(char* data)
{
    // TODO: Check for endianness, right now we assume that `data` is big endian
    const std::size_t size{sizeof(T)};
    union { char bytes[size]; T value; };
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

    for (auto stamp : stamps) {
        outputTextstream << stamp.timestamp << " "
                         << stamp.x << " "
                         << stamp.y << " "
                         << stamp.z << "\n";
    }

    return true;
}

void Birdview::onDataReceived()
{
    AccelerationStamp stamp;
    char data[DEVICE_BUFFER_SIZE];
    deviceSocket.read(data, DEVICE_BUFFER_SIZE);

    stamp.x = bytesToNumeric<float>(data);
    stamp.y = bytesToNumeric<float>(&(data[4]));
    stamp.z = bytesToNumeric<float>(&(data[8]));
    stamp.timestamp = bytesToNumeric<long long>(&(data[12]));

    if (stamps.empty()) {
        stamps.push_front(stamp);
        lastStamp = stamps.begin();
    } else {
        stamps.insert_after(lastStamp, stamp);
        ++lastStamp;
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
