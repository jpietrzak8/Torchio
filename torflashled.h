//
// torflashled.h
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

#ifndef TORFLASHLED_H
#define TORFLASHLED_H

class TorFlashLED
{
public:
  TorFlashLED();

  ~TorFlashLED();

  // Torch controls:
  void toggleTorch();
  void turnTorchOn();
  void turnTorchOff();

  // Flash strobe controls:
  int getMinFlash();
  int getMaxFlash();
  int getMinTime();
  int getMaxTime();
  int getChosenTime();

  void setFlashBrightness(
    int brightness);

  void setFlashDuration(
    int duration);

  void strobe();

  // Indicator controls:
  void toggleIndicator();
  void turnIndicatorOn();
  void turnIndicatorOff();
  void setIndicatorBrightnessLevel(
    int brightness);

  bool ledsCurrentlyLit();
  void swapLEDs();

private:
  void openFlashDevice();

  void switchIndicator(
    int brightness);

  int fileDescriptor;

  int minTorch;
  int maxTorch;
  bool torchOn;

  int minFlash;
  int maxFlash;
  int chosenFlash;

  int minTime;
  int maxTime;
  int chosenTime;

  int minIndicator;
  int maxIndicator;
  int chosenIndicator;
  bool indicatorOn;
};

#endif // TORFLASHLED_H
