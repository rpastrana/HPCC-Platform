#ifndef _DOJOJSMARKUP_HPP_
#define _DOJOJSMARKUP_HPP_

#include <climits>
#include "jstring.hpp"
#include "jutil.hpp"
#include "jdebug.hpp"


static const char* DJ_START_TEST("define([\n\
                                 \"dojo/_base/declare\",\n\
                                 \"dojo/dom\",\n\
\n\
                                 \"dojox/layout/TableContainer\",\n\
                                 \"dojox/grid/DataGrid\",\n\
\n\
                                 \"dgrid/Grid\",\n\
                                 \"dgrid/Keyboard\",\n\
                                 \"dgrid/Selection\",\n\
\n\
                                 \"dijit/Tooltip\",\n\
                                 \"dijit/layout/TabContainer\",\n\
                                 \"dijit/layout/ContentPane\",\n\
                                 \"dijit/form/Form\",\n\
                                 \"dijit/form/TextBox\",\n\
                                 \"dijit/_TemplatedMixin\",\n\
                                 \"dijit/_WidgetsInTemplateMixin\",\n\
                                 \"dijit/form/Select\",\n\
                                 \"dijit/registry\",\n\
\n\
                                 \"hpcc/_TabContainerWidget\",\n\
\n\
                                 \"dojo/text!../templates/GlebWidget4.html\"\n\
\n\
                             ], function (declare, dom,\n\
                                     TableContainer, DataGrid,\n\
                                     DGrid, Keyboard, Selection,\n\
                                     Tooltip, TabContainer, ContentPane, Form, TextBox,\n\
                                     _TemplatedMixin, _WidgetsInTemplateMixin, Select, registry,\n\
                                     _TabConterWidget,\n\
                                     template) {\n\
\n\
                                 return declare(\"GlebWidget4\", [_TabContainerWidget, _TemplatedMixin, _WidgetsInTemplateMixin], {\n\
                                     templateString: template,\n\
                                     baseClass: \"GlebWidget4\",\n\
                                 name: \"GlebWidget4\",\n\
\n\
\n\
                                     postCreate: function (args) {\n\
                                         this.inherited(arguments);\n\
                                     },\n\
\n\
                                     resize: function (args) {\n\
                                         this.inherited(arguments);\n\
                                     },\n\
\n\
                                     layout: function (args) {\n\
                                         this.inherited(arguments);\n\
                                     },\n\
\n\
                                     _initControls: function () {\n\
                                         var context = this;\n\
                                         this.targetSelectControl = registry.byId(this.id + \"TargetSelect\");\n\
                                     top.mystuff = this;\n\
                                     top.mystuff2 = registry.byId(\"tc1\");\n\
                                     top.registry = registry;\n\
                                     top.form = Form;\n\
\n\
");

static const char* DJ_FINISH_TEST("\n},\n        init: function (params) {\n\
                                         if (this.initalized)\n\
                                             return;\n\
                                         this.initalized = true;\n\
\n\
                                     this._initControls();\n\
                                     }\n\
                                 });\n\
                             });");




static const char* DJ_TAB_PART_1("\nvar cp = new ContentPane({\n\
title: \"");

static const char* DJ_TAB_PART_2("\",\n\
style: \"overflow: auto; width: 100%;\",\n\
doLayout: \"true\",\n\
id: \"");


static const char* DJ_TAB_PART_3("\", });\n");

static const char* DJ_TABLE_PART_1("\nvar tc = new dojox.layout.TableContainer(\n\
{ cols: 2,\n\
customClass : \"labelsAndValues\",\n\
\"labelWidth\" : \"175\" });\n");

static const char* DJ_TABLE_PART_2("\n\
if (cp != null)\n\
    cp.placeAt(\"stubTabContainer\");\n\
if (tc != null)\n\
{\n\
    cp.addChild(tc);\n\
}\n\
var temp_cp = cp;\n\
cp = null;\n\
tc = null;\n\
");

static const char* DJ_TABLE_ROW_PART_1("\nvar txt = new dijit.form.TextBox({label: \"");
static const char* DJ_TABLE_ROW_PART_PLACE_HOLDER("\", placeHolder: \"");
static const char* DJ_TABLE_ROW_PART_ID_BEGIN("\", id: \"");
static const char* DJ_TABLE_ROW_PART_ID_END("\", /*style: { width: '400px' }*/});\n");
static const char* DJ_TABLE_ROW_PART_2("\"});\n\
tc.addChild(txt);");

static const char* DJ_ADD_CHILD("\ntc.addChild(txt);");

static const char* DJ_TOOL_TIP_BEGIN("\nvar mytip = new dijit.Tooltip({");
static const char* DJ_TOOL_TIP_CONNECT_ID_BEGIN(" connectId: [\"");
static const char* DJ_TOOL_TIP_CONNECT_ID_END("\"], ");
static const char* DJ_TOOL_TIP_LABEL_BEGIN("label: \"");
static const char* DJ_TOOL_TIP_LABEL_END("\"");
static const char* DJ_TOOL_TIP_END("});");


static const char* DJ_LAYOUT_CONCAT_BEGIN("\nif (typeof(layout) == 'undefined')\n\tvar layout = [[]];\nlayout[0] = layout[0].concat(");
static const char* DJ_LAYOUT_CONCAT_END(");\n");

static const char* DJ_LAYOUT_BEGIN("\nvar layout = [[]];\n");
static const char* DJ_LAYOUT_END("\nvar CustomGrid = declare([ DGrid, Keyboard, Selection ]);\n\
\n\
var grid = new CustomGrid({\n\
columns: layout[0],\n\
selectionMode: \"single\",\n\
cellNavigation: false\n\
});\n\
temp_cp.addChild(grid);\n\
grid.startup();\n");

static const char* createDojoColumnLayout(const char* pName, unsigned uFieldId, const char* pWidth = "100px")
{
    assert(pName != NULL);
    assert(pWidth != NULL);

    static StringBuffer strBuf;

    strBuf.clear();
    strBuf.appendf("{'name': '%s', 'field': '%s', 'width': '%s', 'id': 'COLID_%u'}", pName, pName, pWidth, uFieldId);

    return strBuf.str();
}

static unsigned getRandomID()
{
    Owned<IRandomNumberGenerator> random = createRandomNumberGenerator();
    random->seed(get_cycles_now());

    return (random->next() % UINT_MAX);
}

static void genTabDojoJS(StringBuffer &strJS, const char *pName)
{
    assert(pName != NULL);

    if (pName == NULL)
    {
        return;
    }


    StringBuffer id;

    id.appendf("X%d",getRandomID());

    strJS.append(DJ_TAB_PART_1).append(pName).append(DJ_TAB_PART_2).append(id.str()).append(DJ_TAB_PART_3);
}

static void genTableRow(StringBuffer &strJS, const char* pName)
{

}


#endif // _DOJOJSMARKUP_HPP_
