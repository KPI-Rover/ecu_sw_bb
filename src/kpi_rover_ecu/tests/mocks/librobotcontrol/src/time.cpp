#include <gmock/gmock.h>
#include <rc/time.h>

#include <memory>

class MockRCTime {
   public:
    MOCK_METHOD(void, nanosleep, (uint64_t ns));
    MOCK_METHOD(void, usleep, (unsigned int us));
    MOCK_METHOD(uint64_t, nanos_since_epoch, ());
    MOCK_METHOD(uint64_t, nanos_since_boot, ());
    MOCK_METHOD(uint64_t, timespec_to_micros, (struct timespec ts));
    MOCK_METHOD(uint64_t, timespec_to_nanos, (struct timespec ts));
    MOCK_METHOD(void, timespec_add, (struct timespec * start, double seconds));
};

static std::unique_ptr<MockRCTime> g_mock_rc_time;

MockRCTime& GetMockRCTime() {
    if (!g_mock_rc_time) {
        g_mock_rc_time = std::make_unique<MockRCTime>();
    }
    return *g_mock_rc_time;
}

void ResetMockRCTime() { g_mock_rc_time = std::make_unique<MockRCTime>(); }

void DestroyMockRCTime() { g_mock_rc_time.reset(); }

extern "C" {
void rc_nanosleep(uint64_t ns) { GetMockRCTime().nanosleep(ns); }

void rc_usleep(unsigned int us) { GetMockRCTime().usleep(us); }

uint64_t rc_nanos_since_epoch(void) { return GetMockRCTime().nanos_since_epoch(); }

uint64_t rc_nanos_since_boot(void) { return GetMockRCTime().nanos_since_boot(); }

uint64_t rc_timespec_to_micros(struct timespec ts) { return GetMockRCTime().timespec_to_micros(ts); }

uint64_t rc_timespec_to_nanos(struct timespec ts) { return GetMockRCTime().timespec_to_nanos(ts); }

void rc_timespec_add(struct timespec* start, double seconds) { GetMockRCTime().timespec_add(start, seconds); }
}
