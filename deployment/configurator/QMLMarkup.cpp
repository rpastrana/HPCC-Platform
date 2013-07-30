#include "QMLMarkup.hpp"
#include "jstring.hpp"
#include "jutil.hpp"
#include "jdebug.hpp"

void CQMLMarkupHelper::getTabQML(StringBuffer &strJS, const char *pName)
{
    assert(pName != NULL);

    strJS.append(QML_TAB_BEGIN).append(QML_TAB_TITLE_BEGIN).append(pName).append(QML_TAB_TITLE_END).append(QML_TAB_END);
}


void CQMLMarkupHelper::getComboBoxListElement(const char* pLabel, StringBuffer &strID, const char* pDefault)
{
    static StringBuffer strBuf;

    strBuf.append(QML_LIST_ELEMENT_BEGIN).append(pLabel);
}
