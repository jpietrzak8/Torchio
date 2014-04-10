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
    argList(args)
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
  QTextStream cout(stdout);

  int i = 1;
  while (i < argList.size())
  {
    if ( (argList.at(i) == "-h")
      || (argList.at(i) == "--help"))
    {
      // Print out the help info and quit:
      cout << "Torchio command-line N900 flashlight." << endl;
      cout << endl;
      cout << "Usage: " << argList.at(0) << " [options]" << endl;
      cout << endl;
      cout << "By default, Torchio turns the LEDs on when it starts" << endl;
      cout << "running, and turns them off when it exits." << endl;
      cout << endl;
      cout << "Options:" << endl;
      cout << "-h         Show this help info" << endl;
      cout << "--help" << endl;
      cout << endl;
      cout << "-p         Pulsed mode" << endl;
      cout << "--pulsed" << endl;
      cout << "-s         SOS mode" << endl;
      cout << "--sos" << endl;
      cout << endl;
      cout << "-w         Use white LEDs" << endl;
      cout << "--white" << endl;
      cout << "-r         Use red LED" << endl;
      cout << "--red" << endl;
      cout << endl;
      cout << "-i         Ignore camera cover" << endl;
      cout << "--ignorecover" << endl;
      cout << endl;
      cout << "-t nnn     Switch LEDs off and exit after nnn minutes" << endl;
      cout << "           (from 1 to 120 minutes supported)" << endl;
      cout << "--timeout nnn" << endl;
  
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
        cout << "Error: no timeout duration given" << endl;
        emit controllerDone();
        return;
      }
      else
      {
        bool isANumber;
        int t = argList.at(i).toInt(&isANumber);
        if (!isANumber)
        {
          // error: couldn't parse duration value
          cout << "Error: couldn't parse timeout value" << endl;
          emit controllerDone();
          return;
        }
        else
        {
          if (t < 1)
          {
            // error: timeout too small
            cout << "Warning: timeout value less than 1." << endl;
            cout << "Timeout being set to 1 minute." << endl;
            timeoutDuration = 1;
          }
          else if (t > 120)
          {
            // error: timeout too large
            cout << "Warning: timeout value greater than 120." << endl;
            cout << "Timeout being set to 120 minutes." << endl;
            timeoutDuration = 120;
          }
          else
          {
            timeoutDuration = t;
          }
        }
      }
    }
    else
    {
      // Print out the "I didn't understand that" and quit:
      cout << "Error: argument \"" << argList.at(i);
      cout << "\" not supported" << endl;
      emit controllerDone();
      return;
    }

    ++i;
  }

  // So, on to the actual implementation:
  if (dbus.coverCurrentlyClosed() && !ignoreCover)
  {
    // Print out the "camera cover closed" message and quit:
    cout << "Error: camera cover is currently closed";
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
    loopRunning = true;
  }
  else if (pulse == SOS_Pulse)
  {
    morse.startSOS();
    loopRunning = true;
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
    QTextStream cout(stdout);
    cout << e.getError();
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
    QTextStream cout(stdout);
    cout << e.getError();
    cleanupAndExit();
  }
}


void TorController::cleanupAndExit()
{
  // Stop any pulsing:
  if (loopRunning)
  {
    morse.stopLooping();
    loopRunning = false;
  }

  // Turn off the LEDs:
//  turnOff();

  offTimer.stop();

  // Do we want to flash after timeout?
  // Otherwise, just exit here.
  emit controllerDone();
}
