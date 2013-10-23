#include "jstring.hpp"
#include "ExceptionStrings.hpp"

const int nDefaultCode = 99;

IException *MakeExceptionFromMap(int code, enum eExceptionCodes eCode)
{
    static StringBuffer strExceptionMessage;

    strExceptionMessage.setf("Exception: %s\nPossible Action(s): %s",  pExceptionStringArray[eCode-1], pExceptionStringActionArray[eCode-1]);

    return MakeStringException(code, strExceptionMessage.str());
}

IException *MakeExceptionFromMap(enum eExceptionCodes eCode)
{
    return MakeExceptionFromMap(nDefaultCode, eCode);
}
