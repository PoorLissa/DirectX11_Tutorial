
#define SAFE_CREATE(obj, Class)            { (obj) = new Class();    if (!(obj)) return false;                  }
#define SAFE_CREATE_ARRAY(obj, Class, Qty) { (obj) = new Class[Qty]; if (!(obj)) return false;                  }
#define SAFE_RELEASE(obj)                  { if (obj) { (obj)->Release();                  (obj) = nullptr; }   }
#define SAFE_SHUTDOWN(obj)                 { if (obj) { (obj)->Shutdown(); delete   (obj); (obj) = nullptr; }   }
#define SAFE_DELETE(obj)                   {                               delete   (obj); (obj) = nullptr;     }
#define SAFE_DELETE_IF(obj)                { if (obj) {                    delete   (obj); (obj) = nullptr; }   }
#define SAFE_DELETE_ARRAY(obj)             {                               delete [](obj); (obj) = nullptr;     }
#define CHECK_RESULT(hwnd, res, str)       { if(!res) { MessageBox(hwnd, str, L"Error", MB_OK); return false; } }
#define CHECK_FAILED(res)                  { if(FAILED(res))                                    return false;   }
