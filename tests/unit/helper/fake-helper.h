/*
 * Copyright 2016 Canonical Ltd.
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
 *   Charles Kerr <charles.kerr@canonical.com>
 */

#pragma once

#include <helper/helper.h>

class TestHelper: public Helper
{
    Q_OBJECT

public:

    TestHelper(QString const & appid = QString(), clock_func const & clock = default_clock, QObject *parent = nullptr): Helper{appid, clock, parent} {}

    ~TestHelper() =default;

    void record_data_transferred(qint64 n) {Helper::record_data_transferred(n);}
    void set_expected_size(qint64 n) {Helper::set_expected_size(n);}
};
