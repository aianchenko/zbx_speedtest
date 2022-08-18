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

#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#include "speedtest.hpp"
#include "uri.hpp"
#include "connection.hpp"
#include "timer.hpp"

#define BUF_LEN 1024000
#define DETAIL_MS 100

//#define DEBUG

using namespace std;

int milliseconds_to_go = 0;

int main(int argc, char **argv)
    {
    size_t timer1;
    initialize();
    
    Tdata d; // initalized by zeroes by default
    
    if(argc < 3)
	{
	cerr << "Has no arguments! Need to be '" << argv[0] << " filename seconds'" << endl;
	return 2;
	}
    
    milliseconds_to_go = stoi(argv[2])*1000;
    
    Uri *addr;
    try
	{
	addr = new Uri(argv[1]);
	if(addr->Protocol.length() == 0) addr->Protocol = "http";
	if(addr->Protocol != "http") throw("Wrong protocol! 'http' needed.");
	if(addr->Port == -1) addr->Port = 80;
	if(addr->Path.length() == 0) addr->Path = "/";
	}
    catch(const char *msg)
	{
	cerr << "ERROR OCCURED! " << msg << endl;
	return 1;
	}

    auto handle = MakeConnection(addr);
    if(handle == -1)
	{
	cerr << "Error while connecting to remote host!" << endl;
	return 2;
	}

/* HTTP example:
GET /wiki/HTTP HTTP/1.0
Host: ru.wikipedia.org
*/

    string req;
    req = "GET " + addr->Path + " HTTP/1.1\r\nHost: " + addr->Host + "\r\n\r\n";
    send(handle, req.c_str(), req.length(), 0);

    int content_length = 0;
    int rcvd = 0;
    char char_arr[BUF_LEN];
    char *buf = char_arr;

/*    GET HEADER    */
    rcvd = recv(handle, buf, BUF_LEN, 0);
    if(rcvd > 0)
	{
	char *heap = buf;
	if((heap = strstr(buf, "Content-Length: ")) != NULL)
	    {
	    heap += 16;
	    while(*heap >= '0' && *heap <= '9')
		{
		content_length = (!content_length)?*heap-'0':content_length*10+(*heap-'0');
		heap++;
		};
	    }

/*    FIND END OF HEADER    */
	if((heap = strstr(heap, "\r\n\r\n")) != NULL)
	    {
	    heap+=4;
	    d.counter += rcvd-(heap-buf);
	    }
	}

    timer1 = start_timer(DETAIL_MS, tmr, TIMER_PERIODIC, &d);
    while(content_length == 0 || d.counter < content_length)
	{
	if(!d.end)
	    {
	    rcvd = recv(handle, buf, BUF_LEN, 0);
	    if(rcvd > 0)
		{
		d.counter += rcvd;
		}
	    }
	else // end reached
	    {
	    stop_timer(timer1);
	    close(handle);
	    
	    // export in certain format to stdout
	    //{"100ms":{"min":10, "max":20, "avg":15}, "500ms":{"min":10, "max":20, "avg":15}, "1s":{"min":10, "max":20, "avg":15}}
	    cout << "{\"100ms\":{\"min\":" << d.avg_100ms.min << ",\"max\":" << d.avg_100ms.max << ",\"avg\":" << d.avg_100ms.avg << "},"
		 << "\"500ms\":{\"min\":" << d.avg_500ms.min << ",\"max\":" << d.avg_500ms.max << ",\"avg\":" << d.avg_500ms.avg << "},"
		 << "\"1s\":{\"min\":" << d.avg_1s.min << ",\"max\":" << d.avg_1s.max << ",\"avg\":" << d.avg_1s.avg << "}}" << endl;
	    
	    exit(0);
	    }
	}

    delete(buf);
    return 0;
    }

void calc(unsigned int counter, Tavg *a, unsigned int detail)
    {
    unsigned int delta = counter - a->last_counter;
    a->last_counter = counter;
    unsigned int curr_speed = delta*(1000/detail)*8/1000;
    
    if(!a->min && !a->max && !a->avg) // first accurance
	{
	a->min = curr_speed;
	a->max = curr_speed;
	a->avg = curr_speed;
	}
    
    if(curr_speed > a->max) a->max = curr_speed;
    if(curr_speed < a->min) a->min = curr_speed;
    a->avg = (a->avg + curr_speed) / 2;

#ifdef DEBUG
    cerr << delta << ", " << curr_speed << " Kbit/s   {min: " << a->min << ", max: " << a->max << ", avg: " << a->avg << "}" << endl;
#endif
    }

void tmr(size_t timer_id, void * data)
    {
    Tdata *d = (Tdata*)data;
    d->ms += DETAIL_MS;

    if(!(d->ms % 100)) // every 100ms
	{
#ifdef DEBUG
	cerr << "+100ms: ";
#endif
	calc(d->counter, &d->avg_100ms, 100);
	}
    if(!(d->ms % 500)) // every 500ms
	{
#ifdef DEBUG
	cerr << "+500ms: ";
#endif
	calc(d->counter, &d->avg_500ms, 500);
	}
    if(!(d->ms % 1000)) // every 1s
	{
#ifdef DEBUG
	cerr << "+1s: ";
#endif
	calc(d->counter, &d->avg_1s, 1000);
	}
    if(d->ms == milliseconds_to_go) d->end = true;
    }

