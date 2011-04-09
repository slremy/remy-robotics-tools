//Code inspired by example of the playermex project
#include "mex.h"
#include "owimotorcontroller.h"
#include <string.h>

#define mxIsValidStruct(mx) (mxIsScalar(mx) && mxIsStruct(mx))
#define mxIsScalar(mx) \
    ( (2 == mxGetNumberOfDimensions(mx)) \
        && (1 == mxGetM(mx)) && (1 == mxGetN(mx)) )
#define mxIsNumericScalar(mx) ( mxIsScalar(mx) && mxIsNumeric(mx) )
#define mxIsUint32Scalar(mx) ( mxIsScalar(mx) && mxIsUint32(mx) )

#define mxGetArm(x) *(usbowiarm **) mxGetPr(mxGetField(x, 0, "_cself"))

const char *arm_fields[] = {"_cself", "_type"};
const unsigned int arm_fields_count = 2;


void arm_create		(int, mxArray *[], int, const mxArray *[]);
void arm_destroy	(int, mxArray *[], int, const mxArray *[]);
void halt_motors	(int, mxArray *[], int, const mxArray *[]);
void set_control	(int, mxArray *[], int, const mxArray *[]);
void get_control	(int, mxArray *[], int, const mxArray *[]);
void setup_LEDON	(int, mxArray *[], int, const mxArray *[]);
void setup_LEDOFF	(int, mxArray *[], int, const mxArray *[]);
void setup_LEDTOGGLE	(int, mxArray *[], int, const mxArray *[]);
void setup_motorforward	(int, mxArray *[], int, const mxArray *[]);
void setup_motorreverse	(int, mxArray *[], int, const mxArray *[]);
void setup_motoroff	(int, mxArray *[], int, const mxArray *[]);

void test		(int, mxArray *[], int, const mxArray *[]);

void mexFunction
    (int nlhs, mxArray * plhs[], int nrhs, const mxArray * prhs[]) {

    int ret;
    char *funName, *funNameEnd;

    if (nrhs < 1)
        mexErrMsgTxt("Not enough input arguments");

    funName = (char *) mxArrayToString(prhs[0]);
    funNameEnd = funName + strlen(funName);

    if (!strcmp(funName, "arm_create"))
        arm_create(nlhs, plhs, nrhs, prhs);
    else if (!strcmp(funName, "arm_destroy"))
        arm_destroy(nlhs, plhs, nrhs, prhs);
    else if (!strcmp(funName, "halt_motors"))
        halt_motors(nlhs, plhs, nrhs, prhs);
    else if (!strcmp(funName, "set_control"))
        set_control(nlhs, plhs, nrhs, prhs);
    else if (!strcmp(funName, "get_control"))
        get_control(nlhs, plhs, nrhs, prhs);
    else if (!strcmp(funName, "setup_LEDON"))
        setup_LEDON(nlhs, plhs, nrhs, prhs);
    else if (!strcmp(funName, "setup_LEDOFF"))
        setup_LEDOFF(nlhs, plhs, nrhs, prhs);
    else if (!strcmp(funName, "setup_LEDTOGGLE"))
        setup_LEDTOGGLE(nlhs, plhs, nrhs, prhs);
    else if (!strcmp(funName, "setup_motorforward"))
        setup_motorforward(nlhs, plhs, nrhs, prhs);
    else if (!strcmp(funName, "setup_motorreverse"))
        setup_motorreverse(nlhs, plhs, nrhs, prhs);
    else if (!strcmp(funName, "setup_motoroff"))
        setup_motoroff(nlhs, plhs, nrhs, prhs);
    else if (!strcmp(funName, "test"))
        test(nlhs, plhs, nrhs, prhs);
    else
        mexErrMsgTxt("Unknown method");
}


void halt_motors
    (int nlhs, mxArray * plhs[], int nrhs, const mxArray * prhs[]) {

    usbowiarm *arm;
    int ret;
    if (nrhs != 2 ||
        !mxIsUint32Scalar(prhs[1]) )
        mexErrMsgTxt("Not enough or invalid input arguments");

    arm = mxGetArm(prhs[1]);

    arm->halt_motors();

    mxSetM((mxArray *) prhs[1], 0); mxSetN((mxArray *) prhs[1], 0);
}

void set_control
    (int nlhs, mxArray * plhs[], int nrhs, const mxArray * prhs[]) {

    usbowiarm *arm;
    int ret;
    if (nrhs != 2 ||
        !mxIsUint32Scalar(prhs[1]) )
        mexErrMsgTxt("Not enough or invalid input arguments");

    arm = mxGetArm(prhs[1]);

    arm->set_control();

    mxSetM((mxArray *) prhs[1], 0); mxSetN((mxArray *) prhs[1], 0);
}

void get_control
    (int nlhs, mxArray * plhs[], int nrhs, const mxArray * prhs[]) {

    usbowiarm *arm;
    int ret;
    if (nrhs != 2 ||
        !mxIsUint32Scalar(prhs[1]) )
        mexErrMsgTxt("Not enough or invalid input arguments");

    arm = mxGetArm(prhs[1]);

    ret = arm->get_control();

    plhs[0] = mxCreateDoubleScalar(ret);
}

void setup_LEDON
    (int nlhs, mxArray * plhs[], int nrhs, const mxArray * prhs[]) {

    usbowiarm *arm;
    if (nrhs != 2 ||
        !mxIsUint32Scalar(prhs[1]) )
        mexErrMsgTxt("Not enough or invalid input arguments");

    arm = mxGetArm(prhs[1]);

    arm->setup_LEDON();

    mxSetM((mxArray *) prhs[1], 0); mxSetN((mxArray *) prhs[1], 0);
}

void setup_LEDOFF
    (int nlhs, mxArray * plhs[], int nrhs, const mxArray * prhs[]) {

    usbowiarm *arm;
    if (nrhs != 2 ||
        !mxIsUint32Scalar(prhs[1]) )
        mexErrMsgTxt("Not enough or invalid input arguments");

    arm = mxGetArm(prhs[1]);

    arm->setup_LEDOFF();

    mxSetM((mxArray *) prhs[1], 0); mxSetN((mxArray *) prhs[1], 0);
}

void setup_LEDTOGGLE
    (int nlhs, mxArray * plhs[], int nrhs, const mxArray * prhs[]) {

    usbowiarm *arm;
    if (nrhs != 2 ||
        !mxIsUint32Scalar(prhs[1]) )
        mexErrMsgTxt("Not enough or invalid input arguments");

    arm = mxGetArm(prhs[1]);

    arm->setup_LEDTOGGLE();

    mxSetM((mxArray *) prhs[1], 0); mxSetN((mxArray *) prhs[1], 0);
}

void setup_motorforward
    (int nlhs, mxArray * plhs[], int nrhs, const mxArray * prhs[]) {

    usbowiarm *arm;
    int motornum;
    if (nrhs != 3 ||
        !mxIsUint32Scalar(prhs[1]) || !mxIsNumericScalar(prhs[2]))
        mexErrMsgTxt("Not enough or invalid input arguments");

    motornum = int(mxGetScalar(prhs[2]));

    arm = mxGetArm(prhs[1]);

    arm->setup_motorforward(motornum);

    mxSetM((mxArray *) prhs[1], 0); mxSetN((mxArray *) prhs[1], 0);
}

void setup_motorreverse
    (int nlhs, mxArray * plhs[], int nrhs, const mxArray * prhs[]) {

    usbowiarm *arm;
    int motornum;
    if (nrhs != 3 ||
        !mxIsUint32Scalar(prhs[1]) || !mxIsNumericScalar(prhs[2]))
        mexErrMsgTxt("Not enough or invalid input arguments");

    motornum = int(mxGetScalar(prhs[2]));

    arm = mxGetArm(prhs[1]);

    arm->setup_motorreverse(motornum);

    mxSetM((mxArray *) prhs[1], 0); mxSetN((mxArray *) prhs[1], 0);
}

void setup_motoroff
    (int nlhs, mxArray * plhs[], int nrhs, const mxArray * prhs[]) {

    usbowiarm *arm;
    int motornum;
    if (nrhs != 3 ||
        !mxIsUint32Scalar(prhs[1]) || !mxIsNumericScalar(prhs[2]))
        mexErrMsgTxt("Not enough or invalid input arguments");

    motornum = int(mxGetScalar(prhs[2]));

    arm = mxGetArm(prhs[1]);

    arm->setup_motoroff(motornum);

    mxSetM((mxArray *) prhs[1], 0); mxSetN((mxArray *) prhs[1], 0);
}

void test (int nlhs, mxArray *plhs[],
                     int nrhs, const mxArray *prhs[])
{
        usbowiarm *arm;
        mxArray *data, *pr;
        int i;
        if (nrhs != 2 ||
            !mxIsValidStruct(prhs[1]))
            mexErrMsgTxt("Not enough or invalid input arguments");

        arm = mxGetArm(prhs[1]);

        arm->test();

        mxSetM((mxArray *) prhs[1], 0); mxSetN((mxArray *) prhs[1], 0);
}

void arm_destroy (int nlhs, mxArray *plhs[],
                     int nrhs, const mxArray *prhs[])
{
        usbowiarm *arm;
        mxArray *data, *pr;
        int i;
        if (nrhs != 2 ||
            !mxIsValidStruct(prhs[1]))
            mexErrMsgTxt("Not enough or invalid input arguments");

        data = mxGetField(prhs[1], 0, "_type");
        mxFree(mxGetPr(data));

        data = mxGetField(prhs[1], 0, "_cself");
        arm = *(usbowiarm **) mxGetPr(data);

        arm->~usbowiarm();
        delete(arm);


        for(i=0; i < arm_fields_count; i++) {
                 data = mxGetField(prhs[1], 0, arm_fields[i]);
                 mxSetData(data, (void *) 0);
                 mxSetM(data, 0); mxSetN(data, 0);
                /* Since we do some pointer magic we have to
                 * hide all that to let this struct be cleared */
        }
        mxSetM((mxArray *) prhs[1], 0); mxSetN((mxArray *) prhs[1], 0);
}
