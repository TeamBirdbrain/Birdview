/* 
 * Copyright (C) 2016 James Wrigley
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include <Qt>
#include <QRect>
#include <QStyle>
#include <QApplication>
#include <QInputDialog>
#include <QDesktopWidget>

#include "Birdview.hpp"

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
}

Birdview::~Birdview() { }

bool Birdview::connected()
{
    return deviceSocket.isValid();
}

void Birdview::setConnected(bool state)
{
    if (state) {

    } else {
        deviceIP.clear();        
    }

    QString buttonText{state ? "Connected to " + deviceIP : "Disconnected"};
    QColor buttonColor{state ? buttonGreen : buttonRed};

    connectionButton->setText(buttonText);
    connectionButton->setStyleSheet("QPushButton { background-color: " + buttonColor.name() + ";"
                                    "              border: none;"
                                    "              padding: 1em 1em 1em 1em; }"
                                    "QPushButton:hover:!pressed { background-color: " + buttonColor.lighter(110).name() + "; }"
                                    "QPushButton:pressed { background-color: " + buttonColor.darker(120).name() + "; }");
}

void Birdview::onConnectionButtonClicked()
{
    if (connected()) {
        setConnected(false);
    } else {
        bool entered{false};
        QString ip{QInputDialog::getText(this, "Connect to device",
                                         "Enter IP address of device:",
                                         QLineEdit::Normal, "", &entered,
                                         Qt::Dialog, Qt::ImhFormattedNumbersOnly)};

        if (entered) {

        }
    }
}
