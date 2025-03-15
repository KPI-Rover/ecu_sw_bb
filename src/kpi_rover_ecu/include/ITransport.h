#ifndef ITRANSPORT_H
#define ITRANSPORT_H

//#include "config.h"
#include <iostream>
#include <vector>

using namespace std;


class ITransport {
   public:
    virtual bool Send(const vector<uint8_t>& data) = 0;
    virtual bool Receive(vector<uint8_t>& data) = 0;
    virtual int Init() = 0;
    virtual void Start() = 0;
    virtual void Destroy() = 0;
    // virtual ~ITransport() = default;
};

#endif