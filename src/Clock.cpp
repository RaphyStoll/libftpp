#include "../include/Clock.hpp"

#include <sys/time.h>

using namespace libftpp::time;

unsigned long long Clock::now_ms() {
    struct timeval tv;
    if (gettimeofday(&tv, 0) != 0) {
        return 0;
    }
    unsigned long long ms =
        (unsigned long long)tv.tv_sec * 1000ULL +
        (unsigned long long)tv.tv_usec / 1000ULL;
    return ms;
}

Timeout::Timeout(): _last_activity_ms(0), _timeout_ms(0)
{}

Timeout::Timeout(unsigned long long timeout_ms)
	: _last_activity_ms(0), _timeout_ms(timeout_ms)
{}

void Timeout::set_timeout_ms(unsigned long long timeout_ms) {
    _timeout_ms = timeout_ms;
}

unsigned long long Timeout::timeout_ms() const {
    return _timeout_ms;
}

void Timeout::touch(unsigned long long now) {
    _last_activity_ms = now;
}

unsigned long long Timeout::last_activity_ms() const {
    return _last_activity_ms;
}

unsigned long long Timeout::elapsed_since(unsigned long long now) const {
    if (now < _last_activity_ms)
        return 0ULL;
    return now - _last_activity_ms;
}

bool Timeout::expired(unsigned long long now) const {
    if (_timeout_ms == 0)
        return false;
    return elapsed_since(now) >= _timeout_ms;
}

unsigned long long Timeout::deadline_ms() const {
    if (_timeout_ms == 0)
        return 0ULL;
    return _last_activity_ms + _timeout_ms;
}
