/* owiswig.i */
%module owiswig
%{
/* Includes the header in the wrapper code */
#include "owimotorcontroller.hpp"
%}

/* Parse the header file to generate wrappers */
%include "owimotorcontroller.hpp"
