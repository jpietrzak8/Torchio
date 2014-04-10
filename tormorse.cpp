//
// tormorse.cpp
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

#include "tormorse.h"

#include <QTimer>
//#include <QFileDialog>
//#include <QFile>
//#include <QTextStream>
//#include <QTextDocument>


TorMorse::TorMorse()
{
  setupSOSCode();
  setupECode();
}


TorMorse::~TorMorse()
{
}


void TorMorse::startSOS()
{
  timer.stop();
  sosCodePosition = sosCodeBits.begin();
  connect (&timer, SIGNAL(timeout()), this, SLOT(runSOSCode()));
  timer.start(100);
}


void TorMorse::startE()
{
  timer.stop();
  eCodePosition = eCodeBits.begin();
  connect (&timer, SIGNAL(timeout()), this, SLOT(runECode()));
  timer.start(100);
}


void TorMorse::stopLooping()
{
  timer.stop();
}


/*
void LanMorseForm::on_morseButton_clicked()
{
  if (timer)
  {
    delete timer;
    timer = 0;
  }

  // Create the morseCodeBits:
  translateTextToBits();

  // Execute the morseCodeBits:
  morseCodePosition = morseCodeBits.begin();
  timer = new QTimer(this);
  connect (timer, SIGNAL(timeout()), this, SLOT(runMorseCode()));
  timer->start(mainWindow->dotDuration());

  // Flag that we are now transmitting morse:
  morseRunning = true;
  morsePaused = false;
}
*/


/*
void LanMorseForm::on_pauseButton_clicked()
{
  if (morsePaused)
  {
    // Stop any running timer:
    if (timer)
    {
      delete timer;
      timer = 0;
    }

    // Continue where we left off:
    timer = new QTimer(this);

    connect(timer, SIGNAL(timeout()), this, SLOT(runMorseCode()));
    timer->start(mainWindow->dotDuration());

    morsePaused = false;

    // Nothing more to do here, exit:
    return;
  }

  // Is the timer even running?  And, are we currently transmitting?
  if (!timer || !morseRunning)
  {
    // Nothing to be paused:
    return;
  }

  // Stop the timer, set the pause flag:
  delete timer;
  timer = 0;
  morsePaused = true;

  // Just in case we're currently "on", turn the light off:
  mainWindow->turnTorchOff();
}
*/


/*
void LanMorseForm::on_selectTextFileButton_clicked()
{
  QString filename = QFileDialog::getOpenFileName(
    this,
    tr("Choose Text File"),
    "/home/user/MyDocs/.documents");

  if (filename.isEmpty())
  {
    // Need to create a message of some sort here...
    return;
  }

  QFile file(filename);

  if (!file.open(QFile::ReadOnly | QFile::Text))
  {
    // Need to create a message of some sort here...
    return;
  }

  QTextStream in(&file);

  ui->morsePlainTextEdit->setPlainText(in.readAll());
}
*/


/*
void LanMorseForm::on_repeatCheckBox_toggled(bool checked)
{
  runMorseContinuously = checked;
}
*/


/*
void LanMorseForm::runMorseCode()
{
  if (morseCodePosition == morseCodeBits.end())
  {
    if (runMorseContinuously)
    {
      morseCodePosition = morseCodeBits.begin();
    }
    else
    {
      if (timer)
      {
        delete timer;
        timer = 0;
      }

      morseRunning = false;
      morsePaused = false;

      return;
    }
  }

  if (*morseCodePosition)
  {
    mainWindow->turnTorchOn();
  }
  else
  {
    mainWindow->turnTorchOff();
  }

  ++morseCodePosition;
}
*/


void TorMorse::runSOSCode()
{
  if (sosCodePosition == sosCodeBits.end())
  {
    sosCodePosition = sosCodeBits.begin();
  }

  if (*sosCodePosition)
  {
    emit turnTorchOn();
  }
  else
  {
    emit turnTorchOff();
  }

  ++sosCodePosition;
}


void TorMorse::runECode()
{
  if (eCodePosition == eCodeBits.end())
  {
    eCodePosition = eCodeBits.begin();
  }

  if (*eCodePosition)
  {
    emit turnTorchOn();
  }
  else
  {
    emit turnTorchOff();
  }

  ++eCodePosition;
}



//
// This method will convert the desired text into morse code bits:
//
/*
void LanMorseForm::translateTextToBits()
{
  morseCodeBits.clear();

  const QTextDocument *morseText = ui->morsePlainTextEdit->document();

  int index = 0;
  int count = morseText->characterCount();

  while (index < count)
  {
    switch (morseText->characterAt(index).toAscii())
    {
    case 'a':
    case 'A':
      dot(); dash(); break;

    case 'b':
    case 'B':
      dash(); dot(); dot(); dot(); break;

    case 'c':
    case 'C':
      dash(); dot(); dash(); dot(); break;

    case 'd':
    case 'D':
      dash(); dot(); dot(); break;

    case 'e':
    case 'E':
      dot(); break;

    case 'f':
    case 'F':
      dot(); dot(); dash(); dot(); break;

    case 'g':
    case 'G':
      dash(); dash(); dot(); break;

    case 'h':
    case 'H':
      dot(); dot(); dot(); dot(); break;

    case 'i':
    case 'I':
      dot(); dot(); break;

    case 'j':
    case 'J':
      dot(); dash(); dash(); dash(); break;

    case 'k':
    case 'K':
      dash(); dot(); dash(); break;

    case 'l':
    case 'L':
      dot(); dash(); dot(); dot(); break;

    case 'm':
    case 'M':
      dash(); dash(); break;

    case 'n':
    case 'N':
      dash(); dot(); break;

    case 'o':
    case 'O':
      dash(); dash(); dash(); break;

    case 'p':
    case 'P':
      dot(); dash(); dash(); dot(); break;

    case 'q':
    case 'Q':
      dash(); dash(); dot(); dash(); break;

    case 'r':
    case 'R':
      dot(); dash(); dot(); break;

    case 's':
    case 'S':
      dot(); dot(); dot(); break;

    case 't':
    case 'T':
      dash(); break;

    case 'u':
    case 'U':
      dot(); dot(); dash(); break;

    case 'v':
    case 'V':
      dot(); dot(); dot(); dash(); break;

    case 'w':
    case 'W':
      dot(); dash(); dash(); break;

    case 'x':
    case 'X':
      dash(); dot(); dot(); dash(); break;

    case 'y':
    case 'Y':
      dash(); dot(); dash(); dash(); break;

    case 'z':
    case 'Z':
      dash(); dash(); dot(); dot(); break;

    case '0':
      dash(); dash(); dash(); dash(); dash(); break;

    case '1':
      dot(); dash(); dash(); dash(); dash(); break;

    case '2':
      dot(); dot(); dash(); dash(); dash(); break;

    case '3':
      dot(); dot(); dot(); dash(); dash(); break;

    case '4':
      dot(); dot(); dot(); dot(); dash(); break;

    case '5':
      dot(); dot(); dot(); dot(); dot(); break;

    case '6':
      dash(); dot(); dot(); dot(); dot(); break;

    case '7':
      dash(); dash(); dot(); dot(); dot(); break;

    case '8':
      dash(); dash(); dash(); dot(); dot(); break;

    case '9':
      dash(); dash(); dash(); dash(); dot(); break;

    case '.':
      dot(); dash(); dot(); dash(); dot(); dash(); break;

    case ',':
      dash(); dash(); dot(); dot(); dash(); dash(); break;

    case '?':
      dot(); dot(); dash(); dash(); dot(); dot(); break;

    case '\'':
      dot(); dash(); dash(); dash(); dash(); dot(); break;

    case '!':
      dash(); dot(); dash(); dot(); dash(); dash(); break;

    case '/':
      dash(); dot(); dot(); dash(); dot(); break;

    case '(':
      dash(); dot(); dash(); dash(); dot(); break;

    case ')':
      dash(); dot(); dash(); dash(); dot(); dash(); break;

    case '&':
      dot(); dash(); dot(); dot(); dot(); break;

    case ':':
      dash(); dash(); dash(); dot(); dot(); dot(); break;

    case ';':
      dash(); dot(); dash(); dot(); dash(); dot(); break;

    case '=':
      dash(); dot(); dot(); dot(); dash(); break;

    case '+':
      dot(); dash(); dot(); dash(); dot(); break;

    case '-':
      dash(); dot(); dot(); dot(); dot(); dash(); break;

    case '_':
      dot(); dot(); dash(); dash(); dot(); dash(); break;

    case '"':
      dot(); dash(); dot(); dot(); dash(); dot(); break;

    case '$':
      dot(); dot(); dot(); dash(); dot(); dot(); dash(); break;

    case '@':
      dot(); dash(); dash(); dot(); dash(); dot(); break;

    case ' ':
      // End of a word, so need to add 4 units to the 3-unit character gap:
      fourUnitGap(); break;

    default:
      break;
    }

    // At the end of every character is a 3 unit gap:
    threeUnitGap();

    ++index;
  }
}
*/

/*
void LanMorseForm::dot()
{
  morseCodeBits.push_back(true);
  morseCodeBits.push_back(false);
}


void LanMorseForm::dash()
{
  pushBits(morseCodeBits, true, 3);
  morseCodeBits.push_back(false);
}


void LanMorseForm::threeUnitGap()
{
  pushBits(morseCodeBits, false, 3);
}


void LanMorseForm::fourUnitGap()
{
  pushBits(morseCodeBits, false, 4);
}
*/


void TorMorse::setupSOSCode()
{
  // We'll do the standard SOS, followed by a standard 7-dot word space.

  // S: Three dots, plus char space:
  sosCodeBits.push_back(true);
  sosCodeBits.push_back(false);
  sosCodeBits.push_back(true);
  sosCodeBits.push_back(false);
  sosCodeBits.push_back(true);
  pushBits(sosCodeBits, false, 4);

  // O: Three dashes, plus char space:
  pushBits(sosCodeBits, true, 3);
  sosCodeBits.push_back(false);
  pushBits(sosCodeBits, true, 3);
  sosCodeBits.push_back(false);
  pushBits(sosCodeBits, true, 3);
  pushBits(sosCodeBits, false, 4);

  // S: Three dots, plus word space:
  sosCodeBits.push_back(true);
  sosCodeBits.push_back(false);
  sosCodeBits.push_back(true);
  sosCodeBits.push_back(false);
  sosCodeBits.push_back(true);
  pushBits(sosCodeBits, false, 8);
}


void TorMorse::setupECode()
{
  // We'll do an "E" (single dot), followed by 7-dot word space:
  eCodeBits.push_back(true);
  pushBits(eCodeBits, false, 8);
}


void TorMorse::pushBits(
  TorBoolList &bits,
  bool value,
  unsigned int quantity)
{
  unsigned int index = 0;

  while (index < quantity)
  {
    bits.push_back(value);

    ++index;
  }
}
