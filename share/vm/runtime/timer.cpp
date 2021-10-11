//
// Created by xyzjiao on 10/11/21.
//

#include "timer.hpp"

void TimeStamp::update() {

}

void TimeStamp::update_to(jlong ticks) {
    _counter = ticks;
    if (_counter == 0) _counter = 1;
    assert(is_update(), "must not look clear");
}
