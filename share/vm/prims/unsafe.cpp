//
// Created by ziya on 2021/10/23.
//
#include "../../../share/vm/utilities/globalDefinitions.hpp"

#define GET_FIELD(obj, offset, type_name, v) \
  type_name v = *(type_name*)((address)obj + offset)

#define SET_FIELD(obj, offset, type_name, x) \
  *(type_name*)((address)obj + offset) = x

