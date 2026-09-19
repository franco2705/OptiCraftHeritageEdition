#include "platform/RenderAPI.h"
#include "switch/render/SwitchGraphicsContext.h"
#include <algorithm>
#include <cstring>
namespace { int nextTexture=1; int viewport[4]={0,0,1280,720}; float clearColor[4]={0,0,0,1}; const unsigned char vendor[]="libnx native framebuffer"; }
#define NOOP1(name,T) void name(T){}
void renderEnable(RenderCapability){} void renderDisable(RenderCapability){} void renderBlendFunc(RenderBlendFactor,RenderBlendFactor){} void renderDepthMask(bool){} void renderDepthFunc(RenderCompare){} void renderAlphaFunc(RenderCompare,float){} void renderCullFace(RenderFace){} void renderColorMask(bool,bool,bool,bool){} void renderBindTexture(int){} void renderSetActiveTextureUnit(int){} void renderSetClientActiveTextureUnit(int){} void renderSetMultiTextureCoord(int,float,float){} void renderSetLightmapColors(const std::uint32_t*,int){} void renderColor4f(float,float,float,float){} void renderColor3f(float,float,float){} void renderNormal3f(float,float,float){}
void renderGenerateTextures(int n,int*t){while(n--)*t++=nextTexture++;} void renderDeleteTextures(int,const int*){} void renderTextureSubImageRgba(int,int,int,int,int,const void*){} void renderTextureImageRgba(int,int,int,const void*){} void renderTextureParameters(bool,bool,bool){}
#if PLATFORM_TEXTURE_QUALITY_CONTROLS
void renderApplyTextureQuality(bool,int,bool,int){}
#endif
int renderGetMaxAnisotropy(){return 1;} int renderGetMaxSamples(){return 1;} bool renderTextureBeginUpload(int,int,int,int,bool,bool,bool,bool){return true;} bool renderTextureIsValid(int t){return t>0;} void renderResetResources(){} void renderFogf(RenderFogParameter,float){} void renderFogi(RenderFogParameter,RenderFogMode){} void renderFogColor(const float*){} void renderLightfv(int,RenderLightParameter,const float*){} void renderLightModelAmbient(const float*){} void renderColorMaterial(RenderFace,RenderColorMaterialMode){} void renderShadeModel(RenderShadeModel){}
// The shared renderer still uses legacy retained handles for sky and chunk
// bookkeeping. Keep a handle namespace until the Switch terrain renderer owns
// those meshes directly; no desktop GL implementation is pulled into the NRO.
namespace { int nextDisplayList = 1; int nextQuery = 1; }
int renderGenerateDisplayLists(int count){const int first=nextDisplayList;nextDisplayList+=count;return first;}
void renderDeleteDisplayLists(int,int){} void renderBeginDisplayList(int){} void renderEndDisplayList(){}
void renderCallDisplayList(int){} void renderCallDisplayLists(int,const int*){}
void renderGenerateOcclusionQueries(int count,int* queries){while(count--)*queries++=nextQuery++;}
void renderBeginOcclusionQuery(int){} void renderEndOcclusionQuery(){}
bool renderOcclusionQueryResultAvailable(int){return true;} unsigned int renderOcclusionQueryResult(int){return 1;}
bool renderReadPixelsRgb(int,int,int,int,void*){return false;}
void renderClear(unsigned int mask){if(!(mask&RenderClearMask::Color))return;auto*p=SwitchGraphicsContext::instance().pixels();if(!p)return;auto cv=[](float f){return(unsigned)(std::max(0.f,std::min(1.f,f))*255.f);};std::uint32_t c=cv(clearColor[0])|(cv(clearColor[1])<<8)|(cv(clearColor[2])<<16)|(cv(clearColor[3])<<24);std::fill(p,p+1280*720,c);} void renderFinishGpu(){} void renderSubmitFrame(){SwitchGraphicsContext::instance().present();} void renderClearColor(float r,float g,float b,float a){clearColor[0]=r;clearColor[1]=g;clearColor[2]=b;clearColor[3]=a;} void renderClearDepth(double){} void renderPolygonOffset(float,float){} void renderLineWidth(float){} void renderViewport(int x,int y,int w,int h){viewport[0]=x;viewport[1]=y;viewport[2]=w;viewport[3]=h;} void renderGetViewport(int*v){std::memcpy(v,viewport,sizeof(viewport));}
void renderGetMatrix(RenderMatrixQuery,float*v){std::fill(v,v+16,0.f);v[0]=v[5]=v[10]=v[15]=1.f;} const unsigned char* renderGetString(RenderStringQuery){return vendor;} bool renderSupportsFeature(RenderFeature){return false;} unsigned int renderGetError(){return 0;} void renderFogHint(RenderHintMode){} void renderMatrixMode(RenderMatrixMode){} void renderLoadIdentity(){} void renderPushMatrix(){} void renderPopMatrix(){} void renderTranslate(float,float,float){} void renderRotate(float,float,float,float){} void renderScale(float,float,float){} void renderScaleDouble(double,double,double){} void renderFrustum(double,double,double,double,double,double){} void renderOrtho(double,double,double,double,double,double){}
bool renderCopyFramebufferToBoundTexture(int,int,int,int){return false;} void renderSetLegacyPresentationGamma(bool){}
bool renderDrawInterleaved(const RenderInterleavedMesh&m){return m.data&&m.count>0;} bool renderCaptureInterleaved(const RenderInterleavedMesh&m,RenderCapturedMesh&o,bool append){if(!m.data||m.count<=0||m.stride<=0)return false;if(!append)o.clear();const auto*n=(const std::int32_t*)m.data;std::size_t words=(std::size_t)m.count*m.stride/4;o.raw.insert(o.raw.end(),n,n+words);o.vertexCount+=m.count;o.stride=m.stride;o.primitive=m.primitive;o.positionShort=m.positionShort;o.hasTexture=m.hasTexture;o.texCoordOffset=m.texCoordOffset;o.hasColor=m.hasColor;o.colorOffset=m.colorOffset;o.hasNormals=m.hasNormals;o.normalOffset=m.normalOffset;o.hasBrightness=m.hasBrightness;o.brightnessOffset=m.brightnessOffset;return true;} bool renderDrawCaptured(const RenderCapturedMesh&m){return !m.empty();}
