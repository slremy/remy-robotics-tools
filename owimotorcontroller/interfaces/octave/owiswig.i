/* owiswig.i */
%module owiswig
%{
/* Includes the header in the wrapper code */
#include "owimotorcontroller.h"
%}

/* Parse the header file to generate wrappers */
%include "owimotorcontroller.h"
