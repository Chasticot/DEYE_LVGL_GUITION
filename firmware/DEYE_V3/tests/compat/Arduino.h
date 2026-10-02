#pragma once
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
template<class T> T constrain(T value, T low, T high) { return value < low ? low : value > high ? high : value; }
struct TestESP { void restart() { abort(); } };
static TestESP ESP;
