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
#include <QDialog>
#include <QLineEdit>
#include <QTcpSocket>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

class ConnectDialog : public QDialog
{
    Q_OBJECT

public:
    ConnectDialog(QString*, QTcpSocket*);

private:
    QTcpSocket* socket;

    QLabel* label;
    QString* ipAddress;
    QLineEdit* ipLineEdit;
    QVBoxLayout* mainLayout;
    QHBoxLayout* buttonLayout;
    QPushButton* connectButton;

    const int TIMEOUT_MILLIS = 10000;

private slots:
    void onConnectButtonClicked();
};

#endif
