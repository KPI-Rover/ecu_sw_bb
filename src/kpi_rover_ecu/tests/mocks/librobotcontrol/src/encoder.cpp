#include <rc/encoder.h>
#include <gmock/gmock.h>
#include <memory>

class MockRCEncoder {
public:
    MOCK_METHOD(int, init, ());
    MOCK_METHOD(int, cleanup, ());
    MOCK_METHOD(int, read, (int encoder));
    MOCK_METHOD(int, write, (int encoder, int pos));
    MOCK_METHOD(int, eqep_read, (int ch));
    MOCK_METHOD(int, pru_read, (int ch));
};

static std::unique_ptr<MockRCEncoder> g_mock_rc_encoder;

MockRCEncoder& GetMockRCEncoder() {
    if (!g_mock_rc_encoder) {
        g_mock_rc_encoder = std::make_unique<MockRCEncoder>();
    }
    return *g_mock_rc_encoder;
}

void ResetMockRCEncoder() {
    g_mock_rc_encoder = std::make_unique<MockRCEncoder>();
}

void DestroyMockRCEncoder() {
    g_mock_rc_encoder.reset();
}

extern "C" {
    int rc_encoder_init(void) {
        return GetMockRCEncoder().init();
    }

    int rc_encoder_cleanup(void) {
        return GetMockRCEncoder().cleanup();
    }

    int rc_encoder_read(int encoder) {
        return GetMockRCEncoder().read(encoder);
    }

    int rc_encoder_write(int encoder, int pos) {
        return GetMockRCEncoder().write(encoder, pos);
    }

    int rc_encoder_eqep_read(int ch) {
        return GetMockRCEncoder().eqep_read(ch);
    }

    int rc_encoder_pru_read(int ch) {
        return GetMockRCEncoder().pru_read(ch);
    }
}
