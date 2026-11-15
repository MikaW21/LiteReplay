#pragma once
#include <windows.h>
#include <commctrl.h>
#include <map>
#include <vector>
#include <string>
// Native controls retain keyboard, focus, click and accessibility semantics.
// Painting is event driven: no animation timers and no GPU render loop.
namespace lr_ui {
inline constexpr COLORREF canvas=RGB(22,25,30),surface=RGB(30,35,42),input=RGB(39,45,54),edge=RGB(55,64,75),ink=RGB(231,237,244),muted=RGB(151,165,183),accent=RGB(88,215,184);
inline HBRUSH canvasBrush=CreateSolidBrush(canvas),surfaceBrush=CreateSolidBrush(surface),inputBrush=CreateSolidBrush(input);
inline std::map<HWND,std::vector<RECT>> sections;
inline HFONT heading=nullptr,small=nullptr;
inline void init(){heading=CreateFontW(-26,0,0,0,FW_SEMIBOLD,0,0,0,DEFAULT_CHARSET,0,0,CLEARTYPE_QUALITY,0,L"Microsoft YaHei UI");small=CreateFontW(-13,0,0,0,FW_NORMAL,0,0,0,DEFAULT_CHARSET,0,0,CLEARTYPE_QUALITY,0,L"Microsoft YaHei UI");}
inline void fill(HDC dc,RECT r,COLORREF color){HBRUSH b=CreateSolidBrush(color);FillRect(dc,&r,b);DeleteObject(b);}
inline void box(HDC dc,RECT r){auto b=SelectObject(dc,surfaceBrush);HPEN pen=CreatePen(PS_SOLID,1,edge);auto old=SelectObject(dc,pen);RoundRect(dc,r.left,r.top,r.right,r.bottom,12,12);SelectObject(dc,old);SelectObject(dc,b);DeleteObject(pen);}
inline RECT shifted(HWND h,RECT r){OffsetRect(&r,-GetScrollPos(h,SB_HORZ),-GetScrollPos(h,SB_VERT));return r;}
inline void paint(HWND h,HDC dc){RECT r;GetClientRect(h,&r);FillRect(dc,&r,canvasBrush);auto it=sections.find(h);if(it!=sections.end())for(auto card:it->second)box(dc,shifted(h,card));}
inline LRESULT color(HWND parent,UINT msg,WPARAM w,LPARAM l){HDC dc=reinterpret_cast<HDC>(w);COLORREF bg=canvas;HBRUSH brush=canvasBrush;
 if(msg==WM_CTLCOLOREDIT||msg==WM_CTLCOLORLISTBOX){bg=input;brush=inputBrush;}else{RECT r;GetWindowRect(reinterpret_cast<HWND>(l),&r);POINT pt{r.left,r.top};ScreenToClient(parent,&pt);auto it=sections.find(parent);if(it!=sections.end())for(auto card:it->second){card=shifted(parent,card);if(PtInRect(&card,pt)){bg=surface;brush=surfaceBrush;break;}}}
 SetTextColor(dc,IsWindowEnabled(reinterpret_cast<HWND>(l))?ink:muted);SetBkColor(dc,bg);return reinterpret_cast<LRESULT>(brush);}
inline LRESULT CALLBACK buttonProc(HWND h,UINT m,WPARAM w,LPARAM l,UINT_PTR,DWORD_PTR){
 if(m==WM_MOUSEMOVE){if(!GetPropW(h,L"lrHover")){SetPropW(h,L"lrHover",HANDLE(1));TRACKMOUSEEVENT t{sizeof t,TME_LEAVE,h,0};TrackMouseEvent(&t);InvalidateRect(h,nullptr,FALSE);}}
 if(m==WM_MOUSELEAVE){RemovePropW(h,L"lrHover");InvalidateRect(h,nullptr,FALSE);}
 if(m==WM_ENABLE||m==WM_SETFOCUS||m==WM_KILLFOCUS)InvalidateRect(h,nullptr,FALSE);
 if(m==WM_ERASEBKGND)return 1;
 if(m==WM_PAINT){PAINTSTRUCT ps;HDC dc=BeginPaint(h,&ps);RECT r;GetClientRect(h,&r);bool active=GetPropW(h,L"lrActive")!=nullptr,primary=GetPropW(h,L"lrPrimary")!=nullptr,enabled=IsWindowEnabled(h),hover=GetPropW(h,L"lrHover")!=nullptr;
  bool down=(SendMessageW(h,BM_GETSTATE,0,0)&BST_PUSHED)!=0;COLORREF bg=primary&&enabled?accent:active?RGB(36,70,68):hover?RGB(50,60,72):surface;if(down)bg=RGB(44,95,85);fill(dc,r,canvas);
  HBRUSH b=CreateSolidBrush(bg);HPEN pen=CreatePen(PS_SOLID,1,active?accent:edge);auto ob=SelectObject(dc,b);auto op=SelectObject(dc,pen);RoundRect(dc,0,0,r.right,r.bottom,8,8);SelectObject(dc,ob);SelectObject(dc,op);DeleteObject(b);DeleteObject(pen);
  if(active){RECT stripe{0,8,3,r.bottom-8};fill(dc,stripe,accent);}wchar_t title[512]{};GetWindowTextW(h,title,512);auto f=SelectObject(dc,reinterpret_cast<HFONT>(SendMessageW(h,WM_GETFONT,0,0)));SetBkMode(dc,TRANSPARENT);SetTextColor(dc,!enabled?muted:primary?canvas:ink);InflateRect(&r,-10,-2);DrawTextW(dc,title,-1,&r,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);if(GetFocus()==h){InflateRect(&r,-2,-3);DrawFocusRect(dc,&r);}SelectObject(dc,f);EndPaint(h,&ps);return 0;}
 if(m==WM_NCDESTROY){RemovePropW(h,L"lrHover");RemovePropW(h,L"lrPrimary");RemovePropW(h,L"lrActive");RemoveWindowSubclass(h,buttonProc,1);}
 return DefSubclassProc(h,m,w,l);
}
inline void primary(HWND h){SetPropW(h,L"lrPrimary",HANDLE(1));InvalidateRect(h,nullptr,FALSE);}
inline void active(HWND h,bool on){if(on)SetPropW(h,L"lrActive",HANDLE(1));else RemovePropW(h,L"lrActive");InvalidateRect(h,nullptr,FALSE);}
inline BOOL CALLBACK skin(HWND h,LPARAM){wchar_t cls[64]{};GetClassNameW(h,cls,64);if(lstrcmpiW(cls,L"Button")==0){auto type=GetWindowLongPtrW(h,GWL_STYLE)&BS_TYPEMASK;if(type==BS_PUSHBUTTON||type==BS_DEFPUSHBUTTON)SetWindowSubclass(h,buttonProc,1,0);}return TRUE;}
inline void apply(HWND root){EnumChildWindows(root,skin,0);}
inline void shutdown(){sections.clear();DeleteObject(heading);DeleteObject(small);DeleteObject(canvasBrush);DeleteObject(surfaceBrush);DeleteObject(inputBrush);}
}
