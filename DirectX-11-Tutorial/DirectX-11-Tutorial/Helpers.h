
#define SAFE_INIT(obj, Class)   { (obj) = new Class(); if (!(obj)) return false;                   }
#define SAFE_RELEASE(obj)       { if (obj) { (obj)->Release();                  (obj) = nullptr; } }
#define SAFE_SHUTDOWN(obj)      { if (obj) { (obj)->Shutdown(); delete   (obj); (obj) = nullptr; } }
#define SAFE_DELETE(obj)        {                               delete   (obj); (obj) = nullptr;   }
#define SAFE_DELETE_ARRAY(obj)  {                               delete [](obj); (obj) = nullptr;   }
