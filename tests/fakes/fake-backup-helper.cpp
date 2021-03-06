/*
 * Copyright (C) 2016 Canonical, Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3, as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 *     Charles Kerr <charles.kerr@canonical.com>
 */

#include "fake-backup-helper.h"

#include <qdbus-stubs/dbus-types.h>
#include <qdbus-stubs/keeper_helper_interface.h>

#include <QCoreApplication>
#include <QDebug>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QProcessEnvironment>
#include <QLocalSocket>

#include <unistd.h>

int
main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    const auto blob = QByteArray(FAKE_BACKUP_HELPER_PAYLOAD);

    // dump the inputs to stdout
    qDebug() << "argc:" << argc;
    for(int i=0; i<argc; ++i)
        qDebug() << "argv[" << i << "] is" << argv[i];
    const auto env = QProcessEnvironment::systemEnvironment();
    for(const auto& key : env.keys())
        qDebug() << "env" << qPrintable(key) << "is" << qPrintable(env.value(key));

    qDebug() << "Retrieving connection";
    // ask the service for a socket
    auto conn = QDBusConnection::connectToBus(QDBusConnection::SessionBus, DBusTypes::KEEPER_SERVICE);
    const auto object_path = QString::fromUtf8(DBusTypes::KEEPER_HELPER_PATH);
    DBusInterfaceKeeperHelper helper_iface (DBusTypes::KEEPER_SERVICE, object_path, conn);

    qDebug() << "Is valid:" << helper_iface.isValid();

    auto blob_size = blob.size();
#ifdef COMPILE_WITH_FAILURE
        // just remove 1 byte from the size to emulate a failure
        blob_size--;
#endif
    auto fd_reply = helper_iface.StartBackup(blob_size);
    fd_reply.waitForFinished();
    if (fd_reply.isError())
    {
        qFatal("Call to '%s.StartBackup() at '%s' call failed: %s",
            DBusTypes::KEEPER_SERVICE,
            qPrintable(object_path),
            qPrintable(fd_reply.error().message())
        );
    }
    const auto ufd = fd_reply.value();

    // write the blob
    const auto fd = ufd.fileDescriptor();
    const auto n_written_in = write(fd, blob.constData(), blob.size());
    qDebug() << "wrote" << n_written_in << "bytes";

    // create the mark file so we can check when it finished without upstart
    QFile markFile(SIMPLE_HELPER_MARK_FILE_PATH);
    markFile.open(QFile::WriteOnly);
    markFile.close();

    return EXIT_SUCCESS;
}
