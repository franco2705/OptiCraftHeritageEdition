#include "switch/render/SwitchGraphicsContext.h"
#include <switch.h>
namespace { Framebuffer s_framebuffer; }
SwitchGraphicsContext& SwitchGraphicsContext::instance(){ static SwitchGraphicsContext value; return value; }
bool SwitchGraphicsContext::initialize(){ if(alive_) return true; framebufferCreate(&s_framebuffer, nwindowGetDefault(), 1280, 720, PIXEL_FORMAT_RGBA_8888, 2); framebufferMakeLinear(&s_framebuffer); alive_=true; return true; }
void SwitchGraphicsContext::shutdown(){ if(alive_) framebufferClose(&s_framebuffer); pixels_=nullptr; alive_=false; }
bool SwitchGraphicsContext::alive() const { return alive_ && appletMainLoop(); }
std::uint32_t* SwitchGraphicsContext::pixels(){ u32 stride=0; pixels_=static_cast<std::uint32_t*>(framebufferBegin(&s_framebuffer, &stride)); return pixels_; }
void SwitchGraphicsContext::present(){ if(alive_) framebufferEnd(&s_framebuffer); }
