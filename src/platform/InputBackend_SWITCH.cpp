#include "platform/Input.h"
#include <switch.h>
namespace { PadState s_pad; bool s_ready=false; u64 s_pressed=0;
void poll(){ if(!s_ready){ padConfigureInput(1, HidNpadStyleSet_NpadStandard); padInitializeDefault(&s_pad); s_ready=true; } padUpdate(&s_pad); s_pressed=padGetButtonsDown(&s_pad); }
std::uint32_t text(u64 b){ std::uint32_t r=0; if(b&HidNpadButton_Left)r|=PLATFORM_TEXT_LEFT; if(b&HidNpadButton_Right)r|=PLATFORM_TEXT_RIGHT; if(b&HidNpadButton_Up)r|=PLATFORM_TEXT_UP; if(b&HidNpadButton_Down)r|=PLATFORM_TEXT_DOWN; if(b&HidNpadButton_A)r|=PLATFORM_TEXT_TYPE; if(b&HidNpadButton_X)r|=PLATFORM_TEXT_BACK; if(b&HidNpadButton_Minus)r|=PLATFORM_TEXT_SPACE; if(b&HidNpadButton_Y)r|=PLATFORM_TEXT_SHIFT; if(b&HidNpadButton_Plus)r|=PLATFORM_TEXT_ENTER; if(b&HidNpadButton_B)r|=PLATFORM_TEXT_CLOSE; return r; }
float axis(s32 value){ constexpr float scale=1.0f/32768.0f; return value*scale; }}
PlatformTextInputSnapshot platformTextInputSnapshot(int){ poll(); PlatformTextInputSnapshot o; o.connected=padIsConnected(&s_pad); o.held=text(padGetButtons(&s_pad)); o.pressed=text(s_pressed); return o; }
PlatformGamepadSnapshot platformRawGamepadSnapshot(int){ poll(); HidAnalogStickState l=padGetStickPos(&s_pad,0),r=padGetStickPos(&s_pad,1); return {padIsConnected(&s_pad),axis(l.x),-axis(l.y),axis(r.x),-axis(r.y)}; }
PlatformGamepadSnapshot platformGamepadSnapshot(int p){ return platformRawGamepadSnapshot(p); }
int platformMenuPad(){ return 0; } bool platformMenuPointerActive(){return true;} bool platformMenuCursorVisible(){return true;} void platformSetMenuCursor(int,int){}
const PlatformKeyboardHints& platformKeyboardHints(){ static const PlatformKeyboardHints h={{"A:type  X:del  Y:shift  -:space  +:ok  B:close",nullptr,nullptr},1}; return h; }
const char* platformInputDebugLine(){return "Joy-Con / Pro Controller";}
