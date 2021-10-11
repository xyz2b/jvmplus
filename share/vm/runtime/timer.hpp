//
// Created by xyzjiao on 10/11/21.
//

#ifndef JVMPLUS_TIMER_HPP
#define JVMPLUS_TIMER_HPP

#include "../../../share/vm/utilities/globalDefinitions.hpp"

class TimeStamp {
private:
    jlong       _counter;

public:
    // 更新成当前时间戳
    void update();
    // 更新成给定的时间戳
    void update_to(jlong ticks);
    // 自创建或清除之后，时间戳是否更新
    bool is_update() const {return _counter != 0; }
};


#endif //JVMPLUS_TIMER_HPP
