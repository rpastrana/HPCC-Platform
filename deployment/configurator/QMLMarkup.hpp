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

static const char* QML_TAB_VIEW_BEGIN("\
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

static const char* QML_ROW_BEGIN("\
        Row {\n");

static const char* QML_ROW_END("\n\
          }\n");

static const char* QML_RECTANGLE_BEGIN("\
         Rectangle {\n\
            color: \"lightsteelblue\"\n");

static const char* QML_RECTANGLE_END("\n\
         width: childrenRect.width\n\
         height: childrenRect.height\n\
          }\n");

static const char* QML_TEXT_BEGIN("\
      Row {\n\
        Rectangle {\n\
          color: \"lightsteelblue\"\n\
          Text {\n\
               width: 200\n\
               height: 25\n\
               verticalAlignment: Text.AlignVCenter\n\
               text: \"");

static const char* QML_TEXT_END("\"\n\
            }\n\
        width: childrenRect.width\n\
        height: childrenRect.height\n\
         }\n");

static const char* QML_TEXT_BEGIN_2("\
          Text {\n\
                 width: 200\n\
                 height: 25\n\
                 verticalAlignment: Text.AlignVCenter\n\
                 text: \"");

static const char* QML_TEXT_END_2("\"\n\
           }\n");

static const char* QML_TEXT_FIELD_BEGIN("\
        TextField {\n\
        horizontalAlignment: Text.AlignLeft\n\
        implicitWidth: 250\n\
        text:\"");

static const char* QML_TEXT_FIELD_END("\"\n\
            //}\n\
          }\n");

static const char* QML_LIST_MODEL_BEGIN("\
              model: ListModel {\n");

static const char* QML_LIST_MODEL_END("\
                }\n");

static const char* QML_COMBO_BOX_BEGIN("\
    ComboBox {\n\
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
              flow: GridLayout.LeftToRight\n\
");

static const char* QML_GRID_LAYOUT_END("\
          }\n");

class CQMLMarkupHelper
{
public:
    static void getTabQML(StringBuffer &strJS, const char *pName);

    static void getComboBoxListElement(const char* pLabel, StringBuffer &strID, const char* pDefault = "");
};


#endif // _QMLMARKUP_HPP_
