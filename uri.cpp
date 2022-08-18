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

#include "uri.hpp"
using namespace std;
Uri::Uri(const string &uri)
{
    typedef string::const_iterator iterator_t;
    this->Port = -1;
    if (uri.length() == 0)
	throw("URI string is empty");

    iterator_t uriEnd = uri.end();

    // get query start
    iterator_t queryStart = find(uri.begin(), uriEnd, '?');

    // protocol
    iterator_t protocolStart = uri.begin();
    iterator_t protocolEnd = find(protocolStart, uriEnd, ':');            //"://");

    if (protocolEnd != uriEnd)
    {
        string prot = &*(protocolEnd);
        if ((prot.length() > 3) && (prot.substr(0, 3) == "://"))
        {
            this->Protocol = string(protocolStart, protocolEnd);
            protocolEnd += 3;   //      ://
        }
        else
            protocolEnd = uri.begin();  // no protocol
    }
    else
        protocolEnd = uri.begin();  // no protocol

    // host
    iterator_t hostStart = protocolEnd;
    iterator_t pathStart = find(hostStart, uriEnd, '/');  // get pathStart

    iterator_t hostEnd = find(protocolEnd,
        (pathStart != uriEnd) ? pathStart : queryStart,
        L':');  // check for port

    this->Host = string(hostStart, hostEnd);

    // port
    if ((hostEnd != uriEnd) && ((&*(hostEnd))[0] == ':'))  // we have a port
    {
        hostEnd++;
        iterator_t portEnd = (pathStart != uriEnd) ? pathStart : queryStart;
        this->Port = stoi(string(hostEnd, portEnd));
    }

    // path
    if (pathStart != uriEnd)
        this->Path = string(pathStart, queryStart);

    // query
    if (queryStart != uriEnd)
        this->QueryString = string(queryStart, uri.end());

    if (this->Host.length() == 0) throw("URI string is malformed: hostname not found");

}   // Parse
