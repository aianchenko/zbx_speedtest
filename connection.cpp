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

#include "connection.hpp"

using namespace std;

int MakeConnection(Uri *addr)
    {
    int s;
    struct sockaddr_in ssin;
    struct hostent *hp;
    size_t pch;
    string strHlp;
    
    
    /* get host by name - resolve host name into IP address */
    if((hp = gethostbyname(addr->Host.c_str())) == NULL)
	{
	return -1;
	}
    
    bzero(&ssin, sizeof(ssin));
    bcopy(hp->h_addr, &ssin.sin_addr, hp->h_length);
    ssin.sin_family = hp->h_addrtype;
    ssin.sin_port = htons(addr->Port);
    
    /* allocate a socket */
    if((s = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
	return -1;
	}
    
    /* make a connection */
    if(connect(s, reinterpret_cast<const sockaddr*>(&ssin), sizeof(ssin)) == -1)
	{
	return -1;
	}
    
    return s; /* socket descriptor */
    }
