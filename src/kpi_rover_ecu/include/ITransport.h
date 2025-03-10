#ifndef ITRANSPORT_H
#define ITRANSPORT_H

#include "config.h"

class ITransport {
public:

	virtual bool send(const vector<uint8_t>& data) = 0;
	virtual bool receive(vector<uint8_t>& data) = 0;
	virtual int init() = 0;
	virtual void start() = 0;
	virtual void destroy() = 0;
	//virtual ~ITransport() = default;
};


#endif