//
// torflashled.cpp
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

#include "torflashled.h"

#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <fcntl.h>

// Error handling stuff:
#include "torexception.h"
#include <errno.h>
#include <QString>

//#include <QDebug>

// The Flash LEDs are tied into video device 0, along with the camera itself:
#define PATH_TO_FLASH_DEVICE "/dev/video0"

#include <iostream>

TorFlashLED::TorFlashLED()
  : fileDescriptor(-1),
    minTorch(0),
    maxTorch(1),
    torchOn(false),
    minFlash(12),
    maxFlash(19),
    chosenFlash(12),
    minTime(3000),
    maxTime(10000),
    chosenTime(0),
    minIndicator(0),
    maxIndicator(7),
    chosenIndicator(7),
    indicatorOn(false)
{
  openFlashDevice();
}


TorFlashLED::~TorFlashLED()
{
  if (torchOn) toggleTorch();
  if (indicatorOn) turnIndicatorOff();

  if (fileDescriptor >= 0)
  {
    if (close(fileDescriptor) == -1)
    {
      // Failed to close the Flash LED:
      QString ss;
      ss += "Failed to close flash LED device.\n";
      ss += "Error is: ";
      ss += strerror(errno);
      throw TorException(ss);
    }
  }
}


void TorFlashLED::toggleTorch()
{
  struct v4l2_control ctrl;

  // Sanity check:
  if (fileDescriptor == -1)
  {
    // Throw an error here?
    return;
  }

  ctrl.id = V4L2_CID_TORCH_INTENSITY;

  if (torchOn)
  {
    // Turn torch off:
    ctrl.value = minTorch;
    torchOn = false;
  }
  else
  {
    // Turn torch on:
    ctrl.value = maxTorch;
    torchOn = true;
  }

  if (ioctl(fileDescriptor, VIDIOC_S_CTRL, &ctrl) == -1)
  {
    QString ss;
    ss += "Failed to set torch intensity to ";
    ss += ctrl.value;
    ss += "\nError is ";
    ss += strerror(errno);
    throw TorException(ss);
  }
}


void TorFlashLED::turnTorchOn()
{
  if (!torchOn) toggleTorch();
}


void TorFlashLED::turnTorchOff()
{
  if (torchOn) toggleTorch();
}


int TorFlashLED::getMinFlash()
{
  return minFlash;
}


int TorFlashLED::getMaxFlash()
{
  return maxFlash;
}


int TorFlashLED::getMinTime()
{
  return minTime;
}


int TorFlashLED::getMaxTime()
{
  return maxTime;
}


int TorFlashLED::getChosenTime()
{
  return chosenTime;
}


void TorFlashLED::setFlashBrightness(
  int brightness)
{
  if (brightness < minFlash)
  {
    chosenFlash = minFlash;
  }
  else if (brightness > maxFlash)
  {
    chosenFlash = maxFlash;
  }
  else
  {
    chosenFlash = brightness;
  }
}


void TorFlashLED::setFlashDuration(
  int duration)
{
  if (duration < minTime)
  {
    chosenTime = minTime;
  }
  else if (duration > maxTime)
  {
    chosenTime = maxTime;
  }
  else
  {
    chosenTime = duration;
  }
}


void TorFlashLED::strobe()
{
  if (torchOn) toggleTorch();

  struct v4l2_control ctrl;

  // Sanity check:
  if (fileDescriptor == -1)
  {
    // Throw an error here?
    return;
  }

  ctrl.id = V4L2_CID_FLASH_INTENSITY;
  ctrl.value = chosenFlash;

  if (ioctl(fileDescriptor, VIDIOC_S_CTRL, &ctrl) == -1)
  {
    QString ss;
    ss += "Failed to set flash intensity to ";
    ss += chosenFlash;
    ss += "\nError is ";
    ss += strerror(errno);
    throw TorException(ss);
  }

  ctrl.id = V4L2_CID_FLASH_TIMEOUT;
  // For now, let's be a bit conservative and cut the max time in half:
  ctrl.value = chosenTime;

  if (ioctl(fileDescriptor, VIDIOC_S_CTRL, &ctrl) == -1)
  {
    QString ss;
    ss += "Failed to set flash timeout to ";
    ss += chosenTime;
    ss += "\nError is ";
    ss += strerror(errno);
    throw TorException(ss);
  }

  ctrl.id = V4L2_CID_FLASH_STROBE;

  if (ioctl(fileDescriptor, VIDIOC_S_CTRL, &ctrl) == -1)
  {
    QString ss;
    ss += "Failed to strobe flash.\nError is ";
    ss += strerror(errno);
    throw TorException(ss);
  }
}


void TorFlashLED::toggleIndicator()
{
  if (indicatorOn)
  {
    switchIndicator(minIndicator);
    indicatorOn = false;
  }
  else
  {
    switchIndicator(chosenIndicator);
    indicatorOn = true;
  }
}


void TorFlashLED::turnIndicatorOn()
{
  switchIndicator(chosenIndicator);
  indicatorOn = true;
}


void TorFlashLED::turnIndicatorOff()
{
  switchIndicator(minIndicator);
  indicatorOn = false;
}


void TorFlashLED::setIndicatorBrightnessLevel(
  int brightness)
{
  if (brightness < minIndicator)
  {
    chosenIndicator = minIndicator;
  }
  else if (brightness > maxIndicator)
  {
    chosenIndicator = maxIndicator;
  }
  else
  {
    chosenIndicator = brightness;
  }
}


bool TorFlashLED::ledsCurrentlyLit()
{
  return (torchOn || indicatorOn);
}


void TorFlashLED::swapLEDs()
{
  if (torchOn)
  {
    turnTorchOff();
    turnIndicatorOn();
  }
  else if (indicatorOn)
  {
    turnIndicatorOff();
    turnTorchOn();
  }
}


void TorFlashLED::openFlashDevice()
{
  // Not sure why "O_RDWR", but it seems to be necessary:
  fileDescriptor = open(PATH_TO_FLASH_DEVICE, O_RDWR | O_NONBLOCK, 0);
//  fileDescriptor = open(PATH_TO_FLASH_DEVICE, O_RDWR, 0);

  if (fileDescriptor == -1)
  {
    QString ss;
    ss += "Failed to connect to ";
    ss += PATH_TO_FLASH_DEVICE;
    ss += "\nError is ";
    ss += strerror(errno);
    throw TorException(ss);
  }

  // Find out the intensity values for the LED:

  struct v4l2_queryctrl qctrl;

  // Retrieve intensity values for strobe usage:
  qctrl.id = V4L2_CID_FLASH_INTENSITY;

  if (ioctl(fileDescriptor, VIDIOC_QUERYCTRL, &qctrl) == -1)
  {
    QString ss;
    ss += "Failed to retrieve flash LED intensity values.\n";
    ss += "Error is ";
    ss += strerror(errno);
    throw TorException(ss);
  }

  minFlash = qctrl.minimum;
  maxFlash = qctrl.maximum;
  chosenFlash = qctrl.minimum;

  // Retrieve timeout values for strobe usage:
  qctrl.id = V4L2_CID_FLASH_TIMEOUT;

  if (ioctl(fileDescriptor, VIDIOC_QUERYCTRL, &qctrl) == -1)
  {
    QString ss;
    ss += "Failed to retrieve flash timeout values.\n";
    ss += "Error is ";
    ss += strerror(errno);
    throw TorException(ss);
  }

  minTime = qctrl.minimum;
  maxTime = qctrl.maximum;
  chosenTime = qctrl.maximum / 2;

  // Retrieve intensity values for sustained usage:
  qctrl.id = V4L2_CID_TORCH_INTENSITY;

  if (ioctl(fileDescriptor, VIDIOC_QUERYCTRL, &qctrl) == -1)
  {
    QString ss;
    ss += "Failed to retrieve torch LED intensity values.\n";
    ss += "Error is ";
    ss += strerror(errno);
    throw TorException(ss);
  }

  minTorch = qctrl.minimum;
  maxTorch = qctrl.maximum;

  // Does this pick up the indicator LED?
  qctrl.id = V4L2_CID_INDICATOR_INTENSITY;

  if (ioctl(fileDescriptor, VIDIOC_QUERYCTRL, &qctrl) == -1)
  {
    QString ss;
    ss += "Failed to retrieve indicator LED intensity values.\n";
    ss += "Error is ";
    ss += strerror(errno);
    throw TorException(ss);
  }

  minIndicator = qctrl.minimum;
  maxIndicator = qctrl.maximum;
  chosenIndicator = qctrl.maximum;
}


void TorFlashLED::switchIndicator(
  int brightness)
{
  struct v4l2_control ctrl;

  // Sanity check:
  if (fileDescriptor == -1)
  {
    return;
  }

  ctrl.id = V4L2_CID_INDICATOR_INTENSITY;
  ctrl.value = brightness;

  if (ioctl(fileDescriptor, VIDIOC_S_CTRL, &ctrl) == -1)
  {
    QString ss;
    ss += "Failed to set indicator intensity to ";
    ss += ctrl.value;
    ss += "\nError is ";
    ss += strerror(errno);
    throw TorException(ss);
  }
}
