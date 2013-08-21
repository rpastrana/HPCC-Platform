#include "QMLMarkup.hpp"
#include "SchemaCommon.hpp"
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
    StringBuffer strTextArea("textarea");
    StringBuffer strToolTip(pToolTip);

    strToolTip.replace('\"','\'');

    CQMLMarkupHelper::getRandomID(&strTimer1);
    CQMLMarkupHelper::getRandomID(&strTimer2);
    CQMLMarkupHelper::getRandomID(&strMouseArea);
    CQMLMarkupHelper::getRandomID(&strRectangle);
    CQMLMarkupHelper::getRandomID(&strTextArea);

    CQMLMarkupHelper::getToolTipRectangle(strQML, strToolTip.str(), strRectangle.str());
    DEBUG_MARK_QML;
    CQMLMarkupHelper::getToolTipTimer(strQML, strToolTip.str(), strRectangle.str(), strTimer1.str(), strTimer2.str(), strMouseArea.str());
    DEBUG_MARK_QML;
    CQMLMarkupHelper::getToolMouseArea(strQML, strToolTip.str(), strRectangle.str(), strTimer1.str(), strTimer2.str(), strMouseArea.str(), strTextArea.str());
    DEBUG_MARK_QML;

    strQML.append(QML_TEXT_FIELD_ID_BEGIN).append(strTextArea).append(QML_TEXT_FIELD_ID_END);
    DEBUG_MARK_QML;
}

void CQMLMarkupHelper::getToolTipTimer(StringBuffer &strQML, const char *pToolTip, const char *pRectangleID, const char* pTimerID_1, const char* pTimerID_2, const char *pMouseAreaID)
{

    strQML.append(QML_TOOLTIP_TIMER_BEGIN)\
            .append(QML_TOOLTIP_TIMER_ON_TRIGGERED_BEGIN)\
                .append(QML_STYLE_INDENT).append(pRectangleID).append(QML_TOOLTIP_TIMER_RECTANGLE_APPEND_TRUE)\
                .append(QML_STYLE_INDENT).append(pTimerID_2).append(QML_TOOLTIP_TIMER_TIMER_APPEND_START)\
            .append(QML_TOOLTIP_TIMER_ON_TRIGGERED_END)\
            .append(QML_TOOLTIP_TIMER_ID).append(pTimerID_1)\
           .append(QML_TOOLTIP_TIMER_END);
}

void CQMLMarkupHelper::getToolTipRectangle(StringBuffer &strQML, const char *pToolTip, const char *pRectangleID)
{
    strQML.append(QML_TOOLTIP_TEXT_BEGIN).append(pRectangleID).append(QML_TOOLTIP_TEXT_PART_1).append(pToolTip).append(QML_TOOLTIP_TEXT_PART_2);
}

void CQMLMarkupHelper::getToolMouseArea(StringBuffer &strQML, const char *pToolTip, const char *pRectangleID, const char* pTimerID_1, const char* pTimerID_2, const char *pMouseAreaID, const char* pTextAreaID)
{
    strQML.append(QML_MOUSE_AREA_BEGIN)\
            .append(pMouseAreaID).append(QML_MOUSE_AREA_ID_APPEND)\
                .append(QML_MOUSE_AREA_ON_ENTERED_BEGIN)
                    .append(QML_STYLE_INDENT).append(pTimerID_1).append(QML_TOOLTIP_TIMER_TIMER_APPEND_START)\
                .append(QML_MOUSE_AREA_ON_ENTERED_END)\
                .append(QML_MOUSE_AREA_ON_EXITED_BEGIN)\
                    .append(QML_STYLE_INDENT).append(pTimerID_1).append(QML_TOOLTIP_TIMER_STOP)\
                    .append(QML_STYLE_INDENT).append(pRectangleID).append(QML_MOUSE_AREA_RECTANGLE_VISIBLE_FALSE)\
                .append(QML_MOUSE_AREA_ON_EXITED_END)\
                .append(QML_MOUSE_AREA_ON_POSITION_CHANGED_BEGIN)\
                        .append(QML_STYLE_INDENT).append(pTimerID_1).append(QML_TOOLTIP_TIMER_RESTART)\
                        .append(QML_STYLE_INDENT).append(pRectangleID).append(QML_MOUSE_AREA_RECTANGLE_VISIBLE_FALSE)\
                .append(QML_MOUSE_AREA_ON_POSITION_CHANGED_END).append(QML_STYLE_INDENT)\
                .append(QML_MOUSE_AREA_ON_PRESSED_BEGIN)\
                        .append(QML_STYLE_INDENT).append(pTextAreaID).append(QML_TEXT_AREA_FORCE_FOCUS)\
                .append(QML_MOUSE_AREA_ON_PRESSED_END)\
            .append(QML_MOUSE_AREA_END);
}

void CQMLMarkupHelper::getTableViewColumn(StringBuffer &strQML, const char* colTitle)
{
    assert(colTitle != NULL);

    if (colTitle != NULL)
    {
        strQML.append(QML_TABLE_VIEW_COLUMN_BEGIN).append(QML_TABLE_VIEW_COLUMN_TITLE_BEGIN).append(colTitle).append(QML_TABLE_VIEW_COLUMN_TITLE_END).append(QML_TABLE_VIEW_COLUMN_END);
        DEBUG_MARK_QML;
    }
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
