//
// main.cpp
//
// Copyright 2014 by John Pietrzak (jpietrzak8@gmail.com)
//
// This file is part of Torchio.
//
// Torchio is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// Torchio is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Torchio; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//

#include <QtCore/QCoreApplication>

#include <QTimer>
#include <torcontroller.h>
#include <signal.h> // to catch SIGINT


void signalhandler(
  int s)
{
  if (s == SIGINT)
  {
    qApp->quit();
  }
}


int main(
  int argc,
  char *argv[])
{
  signal(SIGINT, signalhandler);

  QCoreApplication a(argc, argv);

  QStringList argList = a.arguments();

  TorController controller(argList);

  // set up the mechanism for the controller to call it quits:
  QObject::connect(
    &controller,
    SIGNAL(controllerDone()),
    &a,
    SLOT(quit()));

  QTimer::singleShot(0, &controller, SLOT(parseArgs()));
//  controller.parseArgs(argList);

  return a.exec();
}
