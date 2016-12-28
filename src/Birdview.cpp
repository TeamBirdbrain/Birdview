/* 
 * Copyright (C) 2016 James Wrigley
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include <QRect>
#include <QStyle>
#include <QApplication>
#include <QDesktopWidget>

#include "Birdview.hpp"

Birdview::Birdview()
{
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
}

Birdview::~Birdview() { }
