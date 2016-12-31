/* 
 * Copyright (C) 2016 James Wrigley
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#ifndef CONNECTDIALOG_HPP
#define CONNECTDIALOG_HPP

#include <Qt>
#include <QLabel>
#include <QTimer>
#include <QDialog>
#include <QLineEdit>
#include <QTcpSocket>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QProgressBar>
#include <QStackedLayout>

class ConnectDialog : public QDialog
{
    Q_OBJECT

public:
    ConnectDialog(QString*, QTcpSocket*);

private:
    QString* ipAddress;
    QTcpSocket* socket;

    QTimer* timer;
    QLabel* connectLabel;
    QLabel* connectingLabel;
    QLineEdit* ipLineEdit;
    QPushButton* connectButton;
    QProgressBar* connectingBar;

    QWidget* connectWidget;
    QWidget* connectingWidget;

    QHBoxLayout* buttonLayout;
    QVBoxLayout* connectLayout;
    QVBoxLayout* connectingLayout;
    QStackedLayout* widgetStack;

    const int TIMEOUT_MILLIS = 10000;

private slots:
    void onConnectButtonClicked();
};

#endif
