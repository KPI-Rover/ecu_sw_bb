#ifndef ITRANSPORT_H
#define ITRANSPORT_H

#include <cstdint>
#include <vector>

class ITransport {
   protected:
    ITransport() = default;

   public:
    virtual bool Send(const std::vector<std::uint8_t>& data) = 0;
    virtual bool Receive(std::vector<std::uint8_t>& data) = 0;
    virtual int Init() = 0;
    virtual void Start() = 0;
    virtual void Destroy() = 0;
    virtual ~ITransport() = default;

    ITransport(const ITransport&) = delete;
    ITransport& operator=(const ITransport&) = delete;
    ITransport(ITransport&&) = delete;
    ITransport& operator=(ITransport&&) = delete;
};

#endif
