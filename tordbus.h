//
// tordbus.h
//
// Copyright 2013 by John Pietrzak  (jpietrzak8@gmail.com)
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

#ifndef TORDBUS_H
#define TORDBUS_H

#include <QObject>
#include <QString>
#include <QMetaType>
#include <QList>

class QDBusInterface;

// Some annoying nowhere-documented types for use with DBus:
struct DBusProperty
{
  QString name;
  bool added;
  bool removed;
};
Q_DECLARE_METATYPE(DBusProperty)
Q_DECLARE_METATYPE(QList<DBusProperty>)


class TorDBus: public QObject
{
  Q_OBJECT

public:
  TorDBus();
  ~TorDBus();

  bool coverCurrentlyClosed();

signals:
  void userClosedCover();

public slots:
  void cameraCoverPropertyModified(
    int count,
    QList<DBusProperty> properties);

private:
  QDBusInterface *halCameraShutter;
};

#endif // TORDBUS_H
