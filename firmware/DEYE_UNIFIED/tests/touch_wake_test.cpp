#include "../touch_logic.h"
#include "../v2_logic.h"
#include <assert.h>
#include <stdio.h>

int main() {
  assert(gt911_status_sample(0x01) == TouchSample::Pending);
  assert(gt911_status_sample(0x80) == TouchSample::Released);
  assert(gt911_status_sample(0x81) == TouchSample::Pressed);
  assert(gt911_status_sample(0x85) == TouchSample::Pressed);
  assert(gt911_status_sample(0x86) == TouchSample::Invalid);
  assert(gt911_status_sample(0xff) == TouchSample::Invalid);
  uint8_t point[8] = {0, 0xdf, 0x01, 0xdf, 0x01, 0, 0, 0};
  int16_t x = -1, y = -1;
  assert(gt911_decode_point(point, 480, 480, x, y) && x == 479 && y == 479);
  point[1] = 0xe0;
  assert(!gt911_decode_point(point, 480, 480, x, y));
  assert(x == 479 && y == 479); // rejected frames must not replace the last point
  point[1] = 0; point[2] = 0; point[3] = 0xe0;
  assert(!gt911_decode_point(point, 480, 480, x, y));
  point[3] = 0; point[4] = 0;
  assert(gt911_decode_point(point, 480, 480, x, y) && x == 0 && y == 0);
  point[1] = point[2] = 0xff;
  assert(!gt911_decode_point(point, 480, 480, x, y));

  TouchWakeFilter filter;
  V2SleepState sleep;
  assert(!sleep.update(true, 0, 60000));
  assert(sleep.update(true, 60000, 60000));
  // Reproduce a single phantom frame followed by repeated polls with no data.
  for (uint32_t now = 60000; now <= 61000; now += 20) {
    const TouchSample sample = now == 60000 ? TouchSample::Pressed : TouchSample::Pending;
    if (filter.update(sample, now)) sleep.touch(true, now);
    assert(sleep.update(true, now, 60000));
  }
  // Isolated spikes cannot accumulate into a confirmed gesture.
  filter.reset();
  for (uint32_t now = 0; now <= 1000; now += 200)
    assert(!filter.update(TouchSample::Pressed, now));
  filter.reset();
  assert(!filter.update(TouchSample::Pressed, 0));
  assert(!filter.update(TouchSample::Pressed, 20));
  assert(!filter.update(TouchSample::Released, 30));
  assert(!filter.update(TouchSample::Pressed, 80));
  assert(!filter.update(TouchSample::Invalid, 100)); // I2C/read/ack failure
  assert(!filter.update(TouchSample::Pressed, 120));
  assert(!filter.update(TouchSample::Pressed, 160));
  assert(filter.update(TouchSample::Pressed, 200));

  // Real gesture, including empty polls between fresh frames.
  filter.reset();
  assert(!filter.update(TouchSample::Pressed, 70000));
  assert(!filter.update(TouchSample::Pending, 70020));
  assert(!filter.update(TouchSample::Pressed, 70040));
  assert(!filter.update(TouchSample::Pressed, 70079));
  assert(filter.update(TouchSample::Pressed, 70080));
  sleep.touch(true, 70080);
  assert(!sleep.update(true, 130079, 60000));
  assert(sleep.update(true, 130080, 60000));

  // Two frames, even far enough apart, are not three confirmations.
  filter.reset();
  assert(!filter.update(TouchSample::Pressed, 0));
  assert(!filter.update(TouchSample::Pressed, 80));
  assert(!filter.update(TouchSample::Pending, 100));
  assert(filter.update(TouchSample::Pressed, 120));
  filter.reset();
  assert(!filter.update(TouchSample::Pressed, 0xfffffff0));
  assert(!filter.update(TouchSample::Pressed, 24));
  assert(filter.update(TouchSample::Pressed, 64)); // millis rollover
  filter.reset();
  assert(!filter.update(TouchSample::Pressed, 0));
  assert(!filter.update(TouchSample::Pending, 121));
  assert(!filter.update(TouchSample::Pressed, 122));
  puts("PASS: GT911 validation, phantom wake rejection, confirmed wake, expiry, millis rollover");
}
