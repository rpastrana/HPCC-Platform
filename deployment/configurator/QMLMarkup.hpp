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
     width: 300\n\
     height: 200\n\
 \n\
     property int columnWidth: 120\n\
     GridLayout {\n\
         rowSpacing: 12\n\
         columnSpacing: 30\n\
         anchors.top: parent.top\n\
         anchors.horizontalCenter: parent.horizontalCenter\n\
         anchors.margins: 30\n\
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
            implicitHeight: 600\n\
            implicitWidth: 1400\n\
");

static const char* QML_TAB_VIEW_END("}\n}\n");


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

static const char* QML_TAB_BEGIN("\n\
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

static const char* QML_TEXT_FIELD_BEGIN("");
static const char* QML_TEXT_FIELD_END("");

static const char* QML_LIST_MODEL_BEGIN("            ListModel {\n\
                                 id: cbItems");

static const char* QML_LIST_MODEL_END("}\n");

static const char* QML_LIST_ELEMENT_BEGIN("ListElement { text: \"");
static const char* QML_LIST_ELEMENT_END("\" }\n");

class CQMLMarkupHelper
{
public:
    static void getTabQML(StringBuffer &strJS, const char *pName);

    static void getComboBoxListElement(const char* pLabel, StringBuffer &strID, const char* pDefault = "");
};


#endif // _QMLMARKUP_HPP_
