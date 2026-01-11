#pragma once

namespace libftpp {
namespace time {

class Clock {
private:
    Clock();
    Clock(const Clock&);
    Clock& operator=(const Clock&);
    ~Clock();

public:
    static unsigned long long now_ms();
};

class Timeout {
private:
    unsigned long long _last_activity_ms;
    unsigned long long _timeout_ms;

public:
    Timeout();
    Timeout(unsigned long long timeout_ms);

    void set_timeout_ms(unsigned long long timeout_ms);
    unsigned long long timeout_ms() const;

    void touch(unsigned long long now_ms);
    unsigned long long last_activity_ms() const;

    unsigned long long elapsed_since(unsigned long long now_ms) const;
    bool expired(unsigned long long now_ms) const;

    unsigned long long deadline_ms() const;
};

}
}
