//
// torcontroller.cpp
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

#include "torcontroller.h"
#include "torexception.h"

#include <QTextStream>

//#include <QDebug>

TorController::TorController(
  QStringList args)
  : pulse(No_Pulse),
    color(White_Color),
    ignoreCover(false),
    timeoutDuration(0),
    argList(args),
    morseRunning(false),
    morseFromStdin(false),
    inputStream(stdin)
{
  // Set up the timer:
  connect(
    &offTimer,
    SIGNAL(timeout()),
    this,
    SLOT(cleanupAndExit()));

  // Set up for a shutter closed event:
  connect(
    &dbus,
    SIGNAL(userClosedCover()),
    this,
    SLOT(cleanupAndExit()));

  // Also, when Morse code has finished:
  connect(
    &morse,
    SIGNAL(morseFinished()),
    this,
    SLOT(handleEndOfMorse()));

  // More Morse integration:
  connect(
    &morse,
    SIGNAL(turnTorchOn()),
    this,
    SLOT(turnOn()));

  connect(
    &morse,
    SIGNAL(turnTorchOff()),
    this,
    SLOT(turnOff()));
}


TorController::~TorController()
{
  // nothing to do here.
}


void TorController::parseArgs()
{
  QTextStream qts(stdout);

  int i = 1;
  while (i < argList.size())
  {
    if ( (argList.at(i) == "-h")
      || (argList.at(i) == "--help"))
    {
      // Print out the help info and quit:
      qts << "Torchio command-line N900 flashlight." << endl;
      qts << endl;
      qts << "Usage: " << argList.at(0) << " [options]" << endl;
      qts << endl;
      qts << "By default, Torchio turns the LEDs on when it starts" << endl;
      qts << "running, and turns them off when it exits." << endl;
      qts << endl;
      qts << "Options:" << endl;
      qts << "-p         Pulsed mode" << endl;
      qts << "--pulsed" << endl;
      qts << "-s         SOS mode" << endl;
      qts << "--sos" << endl;
      qts << "-m         Generate Morse code from standard input" << endl;
      qts << "--morse" << endl;
      qts << "-mf <filename>   Generate Morse code from text file" << endl;
      qts << "--morsefromfile <filename>" << endl;
      qts << endl;
      qts << "-d nnn     Set the dot duration to nnn milliseconds" << endl;
      qts << "--dotduration nnn                  (default is 100)" << endl;
      qts << endl;
      qts << "-w         Use white LEDs" << endl;
      qts << "--white" << endl;
      qts << "-r         Use red LED" << endl;
      qts << "--red" << endl;
      qts << endl;
      qts << "-i         Ignore camera cover" << endl;
      qts << "--ignorecover" << endl;
      qts << endl;
      qts << "-t nnn     Switch LEDs off and exit after nnn minutes" << endl;
      qts << "           (from 1 to 120 minutes supported)" << endl;
      qts << "--timeout nnn" << endl;
      qts << endl;
      qts << "-v         Print the version number" << endl;
      qts << "--version" << endl;
      qts << endl;
      qts << "-h         Show this help info" << endl;
      qts << "--help" << endl;
      qts << endl;
  
      emit controllerDone();
      return;
    }
    else if ((argList.at(i) == "-v")
      || (argList.at(i) == "--version"))
    {
      qts << "Torchio version 0.0.4" << endl;
      emit controllerDone();
      return;
    }
    else if ((argList.at(i) == "-p")
      || (argList.at(i) == "--pulsed"))
    {
      pulse = Simple_Pulse;
    }
    else if ((argList.at(i) == "-s")
      || (argList.at(i) == "--sos"))
    {
      pulse = SOS_Pulse;
    }
    else if ((argList.at(i) == "-m")
      || (argList.at(i) == "--morse"))
    {
      pulse = MorseFromStream_Pulse;
      morseFromStdin = true;
    }
    else if ((argList.at(i) == "-mf")
      || (argList.at(i) == "-morsefromfile"))
    {
      ++i;
      if (i >= argList.size())
      {
        // error: reached end of list
        qts << "Error: no filename provided" << endl;
        emit controllerDone();
        return;
      }
      else
      {
        pulse = MorseFromFile_Pulse;
        filename = argList.at(i);
        morseFromStdin = false;
      }
    }
    else if ((argList.at(i) == "-d")
      || (argList.at(i) == "--dotduration"))
    {
      ++i;
      if (i >= argList.size())
      {
        qts << "Error: no dot duration provided" << endl;
        emit controllerDone();
        return;
      }

      bool isANumber;
      int t = argList.at(i).toInt(&isANumber);
      if (!isANumber)
      {
        qts << "Error: couldn't parse timeout value" << endl;
        emit controllerDone();
        return;
      }

      if (t < 1)
      {
        qts << "Warning: dot duration value less than 1" << endl;
        qts << "Dot duration being set to 1 millisecond." << endl;
        morse.setDotDuration(1);
      }
      else if (t > 600000)
      {
        qts << "Warning: dot duration value greater than 10 minutes" << endl;
        qts << "Dot duration being set to 600000 (10 minutes)" << endl;
        morse.setDotDuration(600000);
      }
      else
      {
        morse.setDotDuration(t);
      }
    }
    else if ((argList.at(i) == "-w")
      || (argList.at(i) == "--white"))
    {
      color = White_Color;
    }
    else if ((argList.at(i) == "-r")
      || (argList.at(i) == "--red"))
    {
      color = Red_Color;
    }
    else if ((argList.at(i) == "-i")
      || (argList.at(i) == "--ignorecover"))
    {
      ignoreCover = true;
    }
    else if ((argList.at(i) == "-t")
      || (argList.at(i) == "--timeout"))
    {
      ++i;
      if (i >= argList.size())
      {
        // error: reached end of list
        qts << "Error: no timeout duration provided" << endl;
        emit controllerDone();
        return;
      }

      bool isANumber;
      int t = argList.at(i).toInt(&isANumber);
      if (!isANumber)
      {
        // error: couldn't parse duration value
        qts << "Error: couldn't parse timeout value" << endl;
        emit controllerDone();
        return;
      }

      if (t < 1)
      {
        // error: timeout too small
        qts << "Warning: timeout value less than 1." << endl;
        qts << "Timeout being set to 1 minute." << endl;
        timeoutDuration = 1;
      }
      else if (t > 120)
      {
        // error: timeout too large
        qts << "Warning: timeout value greater than 120." << endl;
        qts << "Timeout being set to 120 minutes." << endl;
        timeoutDuration = 120;
      }
      else
      {
        timeoutDuration = t;
      }
    }
    else
    {
      // Print out the "I didn't understand that" and quit:
      qts << "Error: argument \"" << argList.at(i);
      qts << "\" not supported" << endl;
      emit controllerDone();
      return;
    }

    ++i;
  }

  // So, on to the actual implementation:
  if (dbus.coverCurrentlyClosed() && !ignoreCover)
  {
    // Print out the "camera cover closed" message and quit:
    qts << "Error: camera cover is currently closed" << endl;
    emit controllerDone();
    return;
  }

  // Set up the timer:
  if (timeoutDuration)
  {
    unsigned int milliseconds = timeoutDuration * 60000;
    offTimer.start(milliseconds);
  }

  // Actually turn on the device:
  if (pulse == Simple_Pulse)
  {
    morse.startE();
    morseRunning = true;
  }
  else if (pulse == SOS_Pulse)
  {
    morse.startSOS();
    morseRunning = true;
  }
  else if (pulse == MorseFromStream_Pulse)
  {
    // We need to grab the first chunk of input and parse it:
    QString firstChunk = inputStream.readLine();
    if (firstChunk.isNull())
    {
      // No input.
      cleanupAndExit();
      return;
    }

    QTextStream lineStream(&firstChunk);

    morse.startMorseFromStream(lineStream);
  }
  else if (pulse == MorseFromFile_Pulse)
  {
    try
    {
      morse.startMorseFromFile(filename);
    }
    catch (TorException &e)
    {
      QTextStream qts(stderr);
      qts << e.getError() << endl;
      cleanupAndExit();
    }
    morseRunning = true;
  }
  else
  {
    turnOn();
  }
}


void TorController::turnOn()
{
  try
  {
    if (color == White_Color)
    {
      led.turnTorchOn();
    }
    else
    {
      led.turnIndicatorOn();
    }
  }
  catch (TorException &e)
  {
    QTextStream qts(stderr);
    qts << e.getError() << endl;
    cleanupAndExit();
  }
}


void TorController::turnOff()
{
  try
  {
    led.turnTorchOff();
    led.turnIndicatorOff();
  }
  catch (TorException &e)
  {
    QTextStream qts(stderr);
    qts << e.getError() << endl;
    cleanupAndExit();
  }
}


void TorController::handleEndOfMorse()
{
  if (!morseFromStdin)
  {
    // We were reading from a file, so just end it here.
    cleanupAndExit();
    return;
  }

  // We need to grab the next chunk of input and parse it:
  QString nextChunk = inputStream.readLine();
  if (nextChunk.isNull())
  {
    // No more input.
    cleanupAndExit();
    return;
  }

  QTextStream lineStream(&nextChunk);

  morse.startMorseFromStream(lineStream);
}


void TorController::cleanupAndExit()
{
  // Stop any pulsing:
  if (morseRunning)
  {
    morse.stopRunning();
    morseRunning = false;
  }

  // Turn off the LEDs:
//  turnOff();

  offTimer.stop();

  // Do we want to flash after timeout?
  // Otherwise, just exit here.
  emit controllerDone();
}
