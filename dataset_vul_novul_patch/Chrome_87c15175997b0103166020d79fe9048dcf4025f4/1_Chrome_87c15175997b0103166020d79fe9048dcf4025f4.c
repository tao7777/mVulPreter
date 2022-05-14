EventType EventTypeFromNative(const base::NativeEvent& native_event) {
  switch (native_event.message) {
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
    case WM_CHAR:
      return ET_KEY_PRESSED;
    case WM_KEYUP:
    case WM_SYSKEYUP:
      return ET_KEY_RELEASED;
    case WM_LBUTTONDBLCLK:
    case WM_LBUTTONDOWN:
    case WM_MBUTTONDBLCLK:
    case WM_MBUTTONDOWN:
    case WM_NCLBUTTONDBLCLK:
    case WM_NCLBUTTONDOWN:
    case WM_NCMBUTTONDBLCLK:
    case WM_NCMBUTTONDOWN:
    case WM_NCRBUTTONDBLCLK:
    case WM_NCRBUTTONDOWN:
    case WM_NCXBUTTONDBLCLK:
    case WM_NCXBUTTONDOWN:
    case WM_RBUTTONDBLCLK:
    case WM_RBUTTONDOWN:
    case WM_XBUTTONDBLCLK:
    case WM_XBUTTONDOWN:
      return ET_MOUSE_PRESSED;
    case WM_LBUTTONUP:
    case WM_MBUTTONUP:
    case WM_NCLBUTTONUP:
    case WM_NCMBUTTONUP:
    case WM_NCRBUTTONUP:
    case WM_NCXBUTTONUP:
    case WM_RBUTTONUP:
    case WM_XBUTTONUP:
      return ET_MOUSE_RELEASED;
    case WM_MOUSEMOVE:
      return IsButtonDown(native_event) ? ET_MOUSE_DRAGGED : ET_MOUSE_MOVED;
     case WM_NCMOUSEMOVE:
       return ET_MOUSE_MOVED;
     case WM_MOUSEWHEEL:
       return ET_MOUSEWHEEL;
     case WM_MOUSELEAVE:
     case WM_NCMOUSELEAVE:
      return ET_MOUSE_EXITED;
    case WM_VSCROLL:
    case WM_HSCROLL:
      return ET_SCROLL;
    default:
      break;
  }
  return ET_UNKNOWN;
}
