/*
 * Copyright (C) 2017 Te Ropu Awhina (Victoria University of Wellington)
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include <iostream>

#include <Qt>
#include <QIcon>
#include <QFile>
#include <QSize>
#include <QRect>
#include <QStyle>
#include <QLabel>
#include <QComboBox>
#include <QSplitter>
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

    xs = Birdcage();
    ys = Birdcage();
    zs = Birdcage();
    plot = new QCustomPlot;
    plot->addGraph();
    plot->graph()->setAdaptiveSampling(true);
    plot->xAxis->setLabel("Time");
    plot->yAxis->setLabel("Acceleration");
    plot->xAxis->setRange(0, 1);
    plot->yAxis->setRange(0, 1);
    plot->replot();

    // Create group boxes
    QWidget* groupsWidget{new QWidget()};
    groupsBox = new QGroupBox("Groups");

    QGroupBox* graphBox = new QGroupBox("Graph");
    QLabel* axisLabel{new QLabel("Axis:")};
    QComboBox* axisComboBox{new QComboBox()};
    axisComboBox->addItem("X");
    axisComboBox->addItem("Y");
    axisComboBox->addItem("Z");
    axisComboBox->setCurrentIndex(1);
    connect(axisComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &Birdview::onAxisChanged);

    QSplitter* splitter{new QSplitter()};
    splitter->addWidget(plot);
    splitter->addWidget(groupsWidget);

    recordButton = new QPushButton();
    recordButton->setIcon(QIcon(":/start-record-icon"));
    recordButton->setIconSize(QSize(50, 50));
    connect(recordButton, &QPushButton::clicked,
            this, &Birdview::onRecordClicked);

    // Pack layouts
    QVBoxLayout* groupsLayout{new QVBoxLayout()};
    groupsLayout->addWidget(groupsBox);
    groupsLayout->addWidget(graphBox);
    groupsLayout->setStretch(0, 6);
    groupsLayout->setStretch(1, 4);
    groupsWidget->setLayout(groupsLayout);

    QVBoxLayout* graphBoxLayout{new QVBoxLayout()};
    QHBoxLayout* axisChooserLayout{new QHBoxLayout()};
    axisChooserLayout->addWidget(axisLabel);
    axisChooserLayout->addWidget(axisComboBox);
    axisChooserLayout->setStretch(0, 2);
    axisChooserLayout->setStretch(1, 8);
    graphBoxLayout->addWidget(recordButton);
    graphBoxLayout->setAlignment(recordButton, Qt::AlignHCenter);
    graphBoxLayout->addLayout(axisChooserLayout);
    graphBox->setLayout(graphBoxLayout);

    splitter->setStretchFactor(0, 90);
    splitter->setStretchFactor(1, 10);

    mainLayout->addWidget(connectionButton);
    mainLayout->addWidget(splitter);

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
    replot = true;
    recording = false;
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
    recordButton->setEnabled(state);

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

    auto xs_it{xs->constBegin()};
    auto ys_it{ys->constBegin()};
    auto zs_it{zs->constBegin()};
    for (; xs_it != xs->constEnd(); ++xs_it, ++ys_it, ++zs_it) {
        outputTextstream << xs_it->key << " "
                         << xs_it->value << " "
                         << ys_it->value << " "
                         << zs_it->value << "\n";
    }

    return true;
}

void Birdview::onAxisChanged(int index)
{
    if (index == 0) {
        plot->graph()->setData(xs);
    } else if (index == 1) {
        plot->graph()->setData(ys);
    } else if (index == 2) {
        plot->graph()->setData(zs);
    }

    plot->replot();
}

void Birdview::onDataReceived()
{
    while (deviceDataSocket.hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(deviceDataSocket.pendingDatagramSize());
        deviceDataSocket.readDatagram(datagram.data(), datagram.size());

        if (recording) {
            double x{bytesToFloat(datagram.data())};
            double y{bytesToFloat(datagram.data() + 4)};
            double z{bytesToFloat(datagram.data() + 8)};
            double timestamp{bytesToFloat(datagram.data() + 12)};

            xs->add(QCPGraphData(timestamp, x));
            ys->add(QCPGraphData(timestamp, y));
            zs->add(QCPGraphData(timestamp, z));

            if (y < currentMinY) {
                currentMinY = y;
            } else if (y > currentMaxY) {
                currentMaxY = y;
            }

            bool xRangeFound{false};
            bool yRangeFound{false};
            auto dataMap{plot->graph()->data()};
            auto xRange{dataMap->keyRange(xRangeFound)};
            auto yRange{dataMap->valueRange(yRangeFound)};

            if (xRangeFound && yRangeFound) {
                plot->xAxis->setRange(xRange);
                plot->yAxis->setRange(yRange);

                replot = !replot;
                if (replot) {
                    plot->replot();
                }
            }
        }
    }
}

void Birdview::onRecordClicked()
{
    recording = !recording;
    recordButton->setIcon(QIcon(recording ? ":/stop-record-icon" : ":/start-record-icon"));
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
