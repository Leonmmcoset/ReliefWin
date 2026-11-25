#ifndef RELIEFDIAGRAM_EXPORT_H
#define RELIEFDIAGRAM_EXPORT_H
#include "scene.h"
#include "../platform/worker.h"
namespace rd {
void paint_frame(const Frame &, double tx, double ty, double scale);
class PngJob {
  struct Work;
  Work *work_=nullptr;
  ro::Worker worker_;
  static void encode(void *);
  Bytes reply_;
  bool busy_ = false, ok_ = false;
  double deadline_ = 0;
  Text error_;
  void stop();

public:
  ~PngJob();
  bool start(const Frame &, Text &);
  bool poll();
  bool busy() const { return busy_; }
  bool take(Bytes &, Text &);
};
} // namespace rd
#endif
