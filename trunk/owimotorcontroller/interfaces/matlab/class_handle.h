//From matlabcentral posts by O. Woodford (8 Jun, 2010)
// Define types
#ifdef _MSC_VER
typedef __int32 int32_t;
typedef unsigned __int32 uint32_t;
typedef unsigned __int64 uint64_t;
#else
#include <stdint.h>
#endif

#define CLASS_HANDLE_SIGNATURE 0xa5a50f0f
template<class base> class class_handle: public base
{
    public:
        class_handle(int armid=0) : base(armid) { signature = CLASS_HANDLE_SIGNATURE; }
        ~class_handle() { signature = 0; }
        bool isValid() { return (signature == CLASS_HANDLE_SIGNATURE); }
    
    private:
        uint32_t signature;
};

template<class base> inline mxArray *convertPtr2Mat(class_handle<base> *ptr)
{
    mxArray *out = mxCreateNumericMatrix(1, 1, mxUINT64_CLASS, mxREAL);
    *((uint64_t *)mxGetData(out)) = reinterpret_cast<uint64_t>(ptr);
    return out;
}

template<class base> inline class_handle<base> *convertMat2Ptr(const mxArray *in)
{
    if (mxGetNumberOfElements(in) != 1)
        mexErrMsgTxt("Input must be a real uint64 scalar.");
    class_handle<base> *ptr = reinterpret_cast<class_handle<base> *>(*((uint64_t *)mxGetData(in)));
    if (!ptr->isValid())
        mexErrMsgTxt("Handle not valid.");
    return ptr;
}
