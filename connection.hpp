/*
 * This file is part of the zbx_speedtest distribution (https://github.com/aianchenko/zbx_speedtest).
 * Copyright (c) 2019 Andrei Ianchenko.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CONNECTION_HPP

#include <string.h>
#include <netdb.h>

#include "uri.hpp"

int MakeConnection(Uri *addr);

#define CONNECTION_HPP
#endif
