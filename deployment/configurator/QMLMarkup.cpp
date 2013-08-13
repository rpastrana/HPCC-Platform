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

    CQMLMarkupHelper::getToolTipRectangle(strQML, strToolTip.str(), strRectangle.str());
    CQMLMarkupHelper::getToolTipTimer(strQML, strToolTip.str(), strRectangle.str(), strTimer1.str(), strTimer2.str(), strMouseArea.str());
    CQMLMarkupHelper::getToolMouseArea(strQML, strToolTip.str(), strRectangle.str(), strTimer1.str(), strTimer2.str(), strMouseArea.str());
}

void CQMLMarkupHelper::getToolTipTimer(StringBuffer &strQML, const char *pToolTip, const char *pRectangleID, const char* pTimerID_1, const char* pTimerID_2, const char *pMouseAreaID)
{
    strQML.append(QML_TOOLTIP_TIMER_BEGIN).append(QML_STYLE_IDENT).append(pRectangleID).append(QML_TOOLTIP_TIMER_RECTANGLE_APPEND_TRUE).append(QML_STYLE_IDENT).append(pMouseAreaID).append(QML_TOOLTIP_TIMER_MOUSE_AREA_APPEND_TRUE).append(QML_STYLE_IDENT).append(pTimerID_2).append(QML_TOOLTIP_TIMER_TIMER_APPEND_START).append(pTimerID_1).append(QML_TOOLTIP_TIMER_END);

    strQML.append(QML_STYLE_NEW_LINE).append(QML_TOOLTIP_TIMER_BEGIN).append(QML_STYLE_IDENT).append(pRectangleID).append(QML_TOOLTIP_TIMER_RECTANGLE_APPEND_FALSE).append(QML_STYLE_IDENT).append(pMouseAreaID).append(QML_TOOLTIP_TIMER_MOUSE_AREA_APPEND_TRUE).append(QML_STYLE_IDENT).append(pTimerID_1).append(QML_TOOLTIP_TIMER_TIMER_APPEND_STOP).append(pTimerID_2).append(QML_TOOLTIP_TIMER_END);
}

void CQMLMarkupHelper::getToolTipRectangle(StringBuffer &strQML, const char *pToolTip, const char *pRectangleID)
{
    strQML.append(QML_TOOLTIP_TEXT_BEGIN).append(pRectangleID).append(QML_TOOLTIP_TEXT_PART_1).append(pToolTip).append(QML_TOOLTIP_TEXT_PART_2);
}

void CQMLMarkupHelper::getToolMouseArea(StringBuffer &strQML, const char *pToolTip, const char *pRectangleID, const char* pTimerID_1, const char* pTimerID_2, const char *pMouseAreaID)
{
    strQML.append(QML_MOUSE_AREA_BEGIN).append(pMouseAreaID).append(QML_MOUSE_AREA_ID_APPEND).append(QML_STYLE_IDENT).append(pTimerID_1).append(QML_MOUSE_AREA_TIMER_APPEND).append(QML_STYLE_IDENT).append(pRectangleID).append(QML_MOUSE_AREA_RECTANGLE_APPEND).append(QML_MOUSE_AREA_END);
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
