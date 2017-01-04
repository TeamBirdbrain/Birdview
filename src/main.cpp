/* 
 * Copyright (C) 2017 Te Ropu Awhina (Victoria University of Wellington)
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include <QApplication>

#include "Birdview.hpp"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    Birdview birdview;
    birdview.show();

    return app.exec();
}
