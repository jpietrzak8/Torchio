//
// torcontroller.h
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

#ifndef TORCONTROLLER_H
#define TORCONTROLLER_H

#include "torflashled.h"
#include "tordbus.h"
#include "tormorse.h"
#include <QObject>
#include <QStringList>
#include <QTimer>

enum TorPulseType
{
  No_Pulse,
  Simple_Pulse,
  SOS_Pulse,
  MorseFromStream_Pulse,
  MorseFromFile_Pulse
};


enum TorColorType
{
  Red_Color,
  White_Color
};


class TorController: public QObject
{
  Q_OBJECT

public:
  TorController(
    QStringList args);

  ~TorController();

signals:
  void controllerDone();

public slots:
  void parseArgs();

  void turnOn();
  void turnOff();

private slots:
  void handleEndOfMorse();
  void cleanupAndExit();

private:
  TorPulseType pulse;
  TorColorType color;
  bool ignoreCover;
  int timeoutDuration;
  QStringList argList;
  bool morseRunning;
  bool morseFromStdin;

  QString filename;
  QTextStream inputStream;
  QTimer offTimer;

  TorFlashLED led;
  TorDBus dbus;
  TorMorse morse;
};

#endif // TORCONTROLLER_H
