#ifndef _QMLMARKUP_HPP_
#define _QMLMARKUP_HPP_

class StringBuffer;

static const char* QML_START("\
 import QtQuick 2.1\n\
 import QtQuick.Controls 1.0\n\
 import QtQuick.Controls.Styles 1.0\n\
 import QtQuick.Particles 2.0\n\
 import QtQuick.Layouts 1.0\n\
 \n\
 Item {\n\
     id: root\n\
     width: 900\n\
     height: 700\n\
");

static const char* QML_END("\
        }\n\
");

static const char* QML_TEXT_FIELD_STYLE("\
property Component textfieldStyle: TextFieldStyle {\n\
  background: Rectangle {\n\
      implicitWidth: columnWidth\n\
      implicitHeight: 22\n\
      color: \"#f0f0f0\"\n\
      antialiasing: true\n\
      border.color: \"gray\"\n\
      radius: height/2\n\
      Rectangle {\n\
          anchors.fill: parent\n\
          anchors.margins: 1\n\
          color: \"transparent\"\n\
          antialiasing: true\n\
          border.color: \"#aaffffff\"\n\
          radius: height/2\n\
      }\n\
  }\n\
}n\
}\n\
");

static const char* QML_TAB_VIEW_BEGIN("\n\
    TabView {\n\
        Layout.row: 5\n\
        Layout.columnSpan: 3\n\
        Layout.fillWidth: true\n\
        implicitHeight: 700\n\
        implicitWidth: 1600\n\
");

static const char* QML_TAB_VIEW_END("}\n");


static const char* QML_TAB_VIEW_STYLE("\
    style: TabViewStyle {\n\
                frameOverlap: 1\n\
                tab: Rectangle {\n\
                    color: styleData.selected ? \"steelblue\" :\"lightsteelblue\"\n\
                    border.color:  \"steelblue\"\n\
                    implicitWidth: Math.max(text.width + 4, 80)\n\
                    implicitHeight: 20\n\
                    radius: 2\n\
                    Text {\n\
                        id: text\n\
                        anchors.centerIn: parent\n\
                        text: styleData.title\n\
                        color: styleData.selected ? \"white\" : \"black\"\n\
                    }\n\
                }\n\
                frame: Rectangle { color: \"steelblue\" }\n\
      }\n");

static const char* QML_TAB_BEGIN("\
        Tab {\n");

static const char* QML_TAB_TITLE_BEGIN("\ttitle: \"");
static const char* QML_TAB_TITLE_END("\"\n");

static const char* QML_TAB_END("\t}\n");


static const char* QML_TAB_TEXT_STYLE("\
          // Style delegates:\n\
\n\
          property Component textfieldStyle: TextFieldStyle {\n\
              background: Rectangle {\n\
                  implicitWidth: columnWidth\n\
                  implicitHeight: 22\n\
                  color: \"#f0f0f0\"\n\
                  antialiasing: true\n\
                  border.color: \"gray\"\n\
                  radius: height/2\n\
                  Rectangle {\n\
                      anchors.fill: parent\n\
                      anchors.margins: 1\n\
                      color: \"transparent\"\n\
                      antialiasing: true\n\
                      border.color: \"#aaffffff\"\n\
                      radius: height/2\n\
                  }\n\
              }\n\
          }\n\
\n\
");


static const char* QML_COLOR_BEGIN("\
        color: \"");

static const char* QML_COLOR_END("\"");

static const char* QML_ROW_BEGIN("\
        Row {\n");

static const char* QML_ROW_END("\n\
          }\n");

static const char* QML_RECTANGLE_LIGHT_STEEEL_BLUE_BEGIN("\
         Rectangle {\n\
            color: \"lightsteelblue\"\n");

static const char* QML_RECTANGLE_BEGIN("\n\
         Rectangle {\n\
                     ");
static const char* QML_RECTANGLE_END("\n\
            width: childrenRect.width\n\
            height: childrenRect.height\n\
          }\n");

static const char* QML_RECTANGLE_LIGHT_STEEEL_BLUE_END(QML_RECTANGLE_END);

static const char* QML_TEXT_BEGIN("\
      Row {\n\
        Rectangle {\n\
          color: \"lightsteelblue\"\n\
          Text {\n\
               width: 275\n\
               height: 25\n\
               verticalAlignment: Text.AlignVCenter\n\
               horizontalAlignment: Text.AlignHCenter\n\
               text: \"");

static const char* QML_TEXT_END("\"\n\
            }\n\
        width: childrenRect.width\n\
        height: childrenRect.height\n\
         }\n");

static const char* QML_TEXT_BEGIN_2("\
          Text {\n\
                 width: 275\n\
                 height: 25\n\
                 verticalAlignment: Text.AlignVCenter\n\
                 horizontalAlignment: Text.AlignHCenter\n\
                 text: \"");

static const char* QML_TEXT_END_2("\"\n\
           }\n");

static const char* QML_TEXT_FIELD_PLACE_HOLDER_TEXT_BEGIN("\n\
        placeholderText: \"");

static const char* QML_TEXT_FIELD_PLACE_HOLDER_TEXT_END("\"\n");

static const char* QML_TEXT_FIELD_BEGIN("\
        TextField {\n\
        style: TextFieldStyle {\n\
                textColor: \"black\"\n\
        background: Rectangle {\n\
             radius: 2\n\
             implicitWidth: 250\n\
             implicitHeight: 25\n\
             border.color: \"#333\"\n\
             border.width: 1\n\
            }\n\
        }\n\
        horizontalAlignment: Text.AlignLeft\n\
        implicitWidth: 250\n\
        text:\"\"");

static const char* QML_TEXT_FIELD_END("\n\
          }\n");

static const char* QML_LIST_MODEL_BEGIN("\
              model: ListModel {\n");

static const char* QML_LIST_MODEL_END("\
                }\n");

static const char* QML_COMBO_BOX_BEGIN("\
    ComboBox {\n\
        implicitWidth: 250\n\
");

static const char* QML_COMBO_BOX_END("\
              }\n");

static const char* QML_LIST_ELEMENT_BEGIN("\
                ListElement { text: \"");

static const char* QML_LIST_ELEMENT_END("\" }\n");



static const char* QML_GRID_LAYOUT_BEGIN("\
          GridLayout {\n\
              rowSpacing: 1\n\
              columnSpacing: 1\n\
              columns: 3\n\
              flow: GridLayout.LeftToRight\n");

static const char* QML_GRID_LAYOUT_END("\
          }\n");


static const char* QML_TOOLTIP_TEXT_BEGIN("\
\n\
              Rectangle {\n\
                  color: \"lightblue\"\n\
                  x: parent.x-275\n\
                  y: parent.y-50\n\
                  width: 250\n\
                  height: 50\n\
                  visible: false\n\
                  radius: 5\n\
                  smooth: true\n\
                  id: ");

static const char* QML_TOOLTIP_TEXT_PART_1("\n\
          }\n\
          Text {\n\
              verticalAlignment: Text.AlignVCenter\n\
              horizontalAlignment: Text.AlignHCenter\n\
              visible: true\n\
              width: 250\n\
              height: 50\n\
              wrapMode: Text.WordWrap\n\
              text: \"");

static const char* QML_TOOLTIP_TEXT_PART_2("\"\n\
          Timer {\n\
              id: ");

static const char* QML_TOOLTIP_TEXT_PART_3("\n\
              interval: 2000\n\
              repeat: false\n\
              running: false\n\
              onTriggered: {\n\
                ");

static const char* QML_TOOLTIP_TEXT_PART_4(".visible = true;\n\
                ");


static const char* QML_TOOLTIP_TEXT_PART_5(".enabled = true;\n\
                ");

static const char* QML_TOOLTIP_TEXT_PART_6(".start();\n\
                }\n\
            }\n\
    Timer {\n\
    id:");

static const char* QML_TOOLTIP_TEXT_PART_7("\n\
          interval: 2000\n\
          repeat: false\n\
          running: false\n\
          onTriggered: {\n\
            ");

//static const char* QML_TOOLTIP_TEXT_PART_8(".enabled = true;\n");
//static const char* QML_TOOLTIP_TEXT_PART_9(".visible = false;\n\
//static const char* QML_TOOLTIP_TEXT_PART_8(QML_TOOLTIP_TEXT_PART_5);
static const char* QML_TOOLTIP_TEXT_PART_8(".enabled = true;\n\
            ");
static const char* QML_TOOLTIP_TEXT_PART_9(QML_TOOLTIP_TEXT_PART_4);

static const char* QML_TOOLTIP_TEXT_PART_10(".stop();\n\
          }\n\
\n\
          MouseArea {\n\
              id: ");

static const char* QML_TOOLTIP_TEXT_PART_11("\n\
              height: 25\n\
              width: 250\n\
              visible: true\n\
              hoverEnabled: true;\n\
              onEntered: {\n\
                  enabled = false;\n\
                  mytimer1.start();\n\
              }\n\
              onExited:  {\n");

static const char* QML_TOOLTIP_TEXT_PART_12(".visible = false;\n\
            }\n");

static const char* QML_TOOLTIP_TEXT_END("\
    }\n");

class CQMLMarkupHelper
{
public:
    static void getTabQML(StringBuffer &strQML, const char *pName);

    static void getComboBoxListElement(const char* pLabel, StringBuffer &strID, const char* pDefault = "");

    static void getToolTipQML(StringBuffer &strQML, const char *pToolTip);

    static unsigned getRandomID(StringBuffer *pID = 0);

};


#endif // _QMLMARKUP_HPP_
