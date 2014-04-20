//
// tormorse.h
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

#ifndef TORMORSE_H
#define TORMORSE_H

#include <QTimer>
#include <QString>
#include <QTextStream>

#include <list>
typedef std::list<bool> TorBoolList;

class TorMorse: public QObject
{
  Q_OBJECT

public:
  TorMorse();
  ~TorMorse();

  void setDotDuration(
    unsigned int dotDuration);

  void startSOS();

  void startE();

  void startMorseFromFile(
    QString filename);

  void startMorseFromStream(
    QTextStream &stream);

  void stopRunning();

signals:
  void turnTorchOn();
  void turnTorchOff();

  void morseFinished();

private slots:
  void runSOSCode();
  void runECode();
  void runMorseCode();

private:
  void translateTextToBits(
    QTextStream &stream);

  void dot();
  void dash();
  void threeUnitGap();
  void fourUnitGap();

  void setupSOSCode();
  void setupECode();

  void pushBits(
    TorBoolList &bits,
    bool value,
    unsigned int quantity);

  QTimer timer;

  bool runMorseContinuously;
  bool morseConnected;

  TorBoolList morseCodeBits;
  TorBoolList::const_iterator morseCodePosition;

  TorBoolList sosCodeBits;
  TorBoolList::const_iterator sosCodePosition;

  TorBoolList eCodeBits;
  TorBoolList::const_iterator eCodePosition;

  unsigned int dotDuration;
};

#endif // TORMORSE_H
