#include "QMLMarkup.hpp"
#include "jstring.hpp"
#include "jutil.hpp"
#include "jdebug.hpp"

void CQMLMarkupHelper::getTabQML(StringBuffer &strJS, const char *pName)
{
    assert(pName != NULL);

    strJS.append(QML_TAB_BEGIN).append(QML_TAB_TITLE_BEGIN).append(pName).append(QML_TAB_TITLE_END);
}


void CQMLMarkupHelper::getComboBoxListElement(const char* pLabel, StringBuffer &strID, const char* pDefault)
{
    static StringBuffer strBuf;

    strBuf.append(QML_LIST_ELEMENT_BEGIN).append(pLabel);
}

void CQMLMarkupHelper::getToolTipQML(StringBuffer &strQML, const char *pToolTip)
{
    return;
    assert(pToolTip != NULL);

    StringBuffer strTimer1("timer1");
    StringBuffer strTimer2("timer2");
    StringBuffer strMouseArea("mousearea");
    StringBuffer strRectangle("rectangle");
    StringBuffer strToolTip(pToolTip);

    strToolTip.replace('\"','\'');

    CQMLMarkupHelper::getRandomID(&strTimer1);
    CQMLMarkupHelper::getRandomID(&strTimer2);
    CQMLMarkupHelper::getRandomID(&strMouseArea);
    CQMLMarkupHelper::getRandomID(&strRectangle);

    strQML.append(QML_TOOLTIP_TEXT_BEGIN).append(strRectangle).append(QML_TOOLTIP_TEXT_PART_1).append(strToolTip).append(QML_TOOLTIP_TEXT_PART_2);
    strQML.append(strTimer1).append(QML_TOOLTIP_TEXT_PART_3).append(strRectangle).append(QML_TOOLTIP_TEXT_PART_4).append(strMouseArea).append(QML_TOOLTIP_TEXT_PART_5).append(strTimer2).append(QML_TOOLTIP_TEXT_PART_6).append("\t").append(strTimer2);
    strQML.append(QML_TOOLTIP_TEXT_PART_7).append(strMouseArea).append(QML_TOOLTIP_TEXT_PART_8).append("\t\t\t").append(strRectangle).append(QML_TOOLTIP_TEXT_PART_9).append("\t\t\t").append(strTimer1).append(QML_TOOLTIP_TEXT_PART_10).append(strMouseArea).append(QML_TOOLTIP_TEXT_PART_11).append(strRectangle).append(QML_TOOLTIP_TEXT_PART_12);
    strQML.append(QML_TOOLTIP_TEXT_END);
}


unsigned CQMLMarkupHelper::getRandomID(StringBuffer *pID)
{
    Owned<IRandomNumberGenerator> random = createRandomNumberGenerator();
    random->seed(get_cycles_now());

    unsigned int retVal =  (random->next() % UINT_MAX);

    if (pID != NULL)
    {
        pID->trim();
        pID->append(retVal);
    }

    return retVal;
}
