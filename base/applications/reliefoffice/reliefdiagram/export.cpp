#include "export.h"
#include "../platform/posix.h"
#include "../fltk_support.h"
#include <FL/Fl_Image_Surface.H>
#include <errno.h>
#include <math.h>
#include <png.h>
#include <new>
namespace rd {
namespace {
double now() { return ro::monotonic(); }
void color(uint32_t c) {
  fl_color(fl_rgb_color(c >> 16, (c >> 8) & 255, c & 255));
}
class Picture : public Fl_Widget {
  const Frame &frame_;
  double scale_;

public:
  Picture(const Frame &f, int w, int h, double s)
      : Fl_Widget(0, 0, w, h), frame_(f), scale_(s) {}
  void draw() override {
    fl_push_clip(0, 0, w(), h());
    fl_color(FL_WHITE);
    fl_rectf(0, 0, w(), h());
    paint_frame(frame_, 24 - frame_.bounds.x * scale_,
                24 - frame_.bounds.y * scale_, scale_);
    fl_pop_clip();
  }
};
} // namespace
void paint_frame(const Frame &f, double tx, double ty, double scale) {
  int left, top, cw, ch;
  fl_clip_box(0, 0, 32760, 32760, left, top, cw, ch);
  auto clip_line = [&](Point a, Point b) {
    double dx = b.x - a.x, dy = b.y - a.y, lo = 0, hi = 1;
    double p[] = {-dx, dx, -dy, dy},
           q[] = {a.x - left, left + cw - a.x, a.y - top, top + ch - a.y};
    for (int k = 0; k < 4; ++k) {
      if (p[k] == 0) {
        if (q[k] < 0)
          return;
        continue;
      }
      double t = q[k] / p[k];
      if (p[k] < 0)
        lo = fmax(lo, t);
      else
        hi = fmin(hi, t);
    }
    if (lo > hi)
      return;
    fl_line(int(round(a.x + dx * lo)), int(round(a.y + dy * lo)),
            int(round(a.x + dx * hi)), int(round(a.y + dy * hi)));
  };
  for (size_t i = 0; i < f.paths.size(); ++i) {
    const Stroke &s = f.paths[i];
    if (!s.count)
      continue;
    double minx = 1e30, miny = 1e30, maxx = -1e30, maxy = -1e30;
    for (unsigned j = 0; j < s.count; ++j) {
      double x = tx + s.points[j].x * scale, y = ty + s.points[j].y * scale;
      minx = fmin(minx, x);
      maxx = fmax(maxx, x);
      miny = fmin(miny, y);
      maxy = fmax(maxy, y);
    }
    if (maxx < left - 8 || minx > left + cw + 8 || maxy < top - 8 ||
        miny > top + ch + 8)
      continue;
    if (s.filled) {
      color(s.fill);
      fl_begin_complex_polygon();
      for (unsigned j = 0; j < s.count; ++j)
        fl_vertex(tx + s.points[j].x * scale, ty + s.points[j].y * scale);
      fl_end_complex_polygon();
    }
    if (s.width > 0) {
      color(s.line);
      fl_line_style(FL_SOLID, int(fmax(1, round(s.width * scale))));
      if (minx > -16000 && maxx < 16000 && miny > -16000 && maxy < 16000) {
        if (s.closed)
          fl_begin_loop();
        else
          fl_begin_line();
        for (unsigned j = 0; j < s.count; ++j)
          fl_vertex(tx + s.points[j].x * scale, ty + s.points[j].y * scale);
        if (s.closed)
          fl_end_loop();
        else
          fl_end_line();
        continue;
      }
      for (unsigned j = 1; j < s.count; ++j)
        clip_line(
            {tx + s.points[j - 1].x * scale, ty + s.points[j - 1].y * scale},
            {tx + s.points[j].x * scale, ty + s.points[j].y * scale});
      if (s.closed)
        clip_line({tx + s.points[s.count - 1].x * scale,
                   ty + s.points[s.count - 1].y * scale},
                  {tx + s.points[0].x * scale, ty + s.points[0].y * scale});
    }
  }
  fl_line_style(0);
  for (size_t i = 0; i < f.labels.size(); ++i) {
    const Label &l = f.labels[i];
    double x = tx + l.at.x * scale, y = ty + l.at.y * scale;
    if (x > left + cw || x + l.text.size() * l.font * scale < left ||
        y + l.font * scale < top || y - l.font * scale > top + ch)
      continue;
    color(l.color);
    fl_font(l.bold ? FL_HELVETICA_BOLD : FL_HELVETICA,
            int(fmax(6, round(l.font * scale))));
    fl_draw(l.text.c_str(), int(round(tx + l.at.x * scale)),
            int(round(ty + l.at.y * scale)));
  }
}
struct PngJob::Work {
  int width=0, height=0, stride=0;
  Bytes pixels, encoded;
  bool ok=false;
};
void PngJob::encode(void *opaque) {
  Work &work=*static_cast<Work *>(opaque);
  png_image image{};
  image.version=PNG_IMAGE_VERSION;
  image.width=work.width;
  image.height=work.height;
  image.format=PNG_FORMAT_RGB;
  png_alloc_size_t count=0;
  work.ok=png_image_write_to_memory(&image,nullptr,&count,0,work.pixels.data(),work.stride,nullptr)!=0 &&
      count<=MaxFileBytes && work.encoded.resize(size_t(count));
  if (work.ok)
    work.ok=png_image_write_to_memory(&image,work.encoded.data(),&count,0,work.pixels.data(),work.stride,nullptr)!=0;
  if (work.ok) work.ok=work.encoded.resize(size_t(count));
  png_image_free(&image);
}
void PngJob::stop() {
  worker_.wait();
  delete work_;
  work_=nullptr;
  busy_=false;
}
PngJob::~PngJob() { stop(); }
bool PngJob::start(const Frame &frame, Text &err) {
  if (busy_) {
    err.set("正在编码 PNG，请稍后");
    return false;
  }
  double scale = fmin(1.5, fmin(2000.0 / fmax(1, frame.bounds.w),
                                2000.0 / fmax(1, frame.bounds.h)));
  int width = int(ceil(frame.bounds.w * scale)) + 48,
      height = int(ceil(frame.bounds.h * scale)) + 48;
  Fl_Group *group = Fl_Group::current();
  Fl_Group::current(nullptr);
  Picture picture(frame, width, height, scale);
  Fl_Group::current(group);
  Fl_Image_Surface surface(width, height);
  surface.set_current();
  surface.draw(&picture);
  Fl_RGB_Image *rgb = surface.image();
  Fl_Display_Device::display_device()->set_current();
  if (!rgb) {
    err.set("无法获取导出图像");
    return false;
  }
  Work *work=new (std::nothrow) Work;
  if (!work) { delete rgb; err.set("内存不足"); return false; }
  work->width=width; work->height=height; work->stride=width*3;
  size_t count=size_t(work->stride)*size_t(height);
  bool copied=rgb->d()==3 && count<=MaxFileBytes && work->pixels.resize(count);
  if (copied) {
    int source_stride=rgb->ld()?rgb->ld():width*3;
    for (int y=0;y<height;++y)
      memcpy(work->pixels.data()+size_t(y)*work->stride,rgb->data()[0]+size_t(y)*source_stride,size_t(work->stride));
  }
  delete rgb;
  if (!copied || !worker_.start(encode,work)) {
    delete work;
    err.set(copied?"无法启动 PNG 编码线程":"图像超过限制或内存不足");
    return false;
  }
  work_=work;
  reply_.clear();
  error_.clear();
  ok_ = false;
  busy_ = true;
  deadline_ = now() + 60;
  err.clear();
  return true;
}
bool PngJob::poll() {
  if (!busy_) return false;
  if (!worker_.finished()) {
    if (now()<=deadline_) return false;
    error_.set("PNG 编码超时"); stop(); return true;
  }
  worker_.wait();
  ok_=work_->ok && work_->encoded.size()>=8 &&
      !memcmp(work_->encoded.data(),"\x89PNG\r\n\x1a\n",8);
  if (ok_) reply_.swap(work_->encoded);
  else error_.set("PNG 编码失败");
  delete work_; work_=nullptr; busy_=false;
  return true;
}
bool PngJob::take(Bytes &out, Text &err) {
  if (busy_)
    return false;
  if (!ok_) {
    err = error_;
    return false;
  }
  out.swap(reply_);
  err.clear();
  ok_ = false;
  return true;
}
} // namespace rd
