#ifndef _DOJOJSMARKUP_HPP_
#define _DOJOJSMARKUP_HPP_

#include "jstring.hpp"
#include "jutil.hpp"
#include "jdebug.hpp"

static const char* DJ_START_TEST("define([\n\
                                 \"dojo/_base/declare\",\n\
                                 \"dojo/dom\",\n\
\n\
                                 \"dojox/layout/TableContainer\",\n\
\n\
                                 \"dijit/layout/TabContainer\",\n\
                                 \"dijit/layout/ContentPane\",\n\
                                 \"dijit/form/Form\",\n\
                                 \"dijit/form/TextBox\",\n\
                                 \"dijit/layout/_LayoutWidget\",\n\
                                 \"dijit/_TemplatedMixin\",\n\
                                 \"dijit/_WidgetsInTemplateMixin\",\n\
                                 \"dijit/form/Select\",\n\
                                 \"dijit/registry\",\n\
\n\
\n\
                                 \"dojo/text!../templates/GlebWidget3.html\"\n\
\n\
                             ], function (declare, dom,\n\
                                     TableContainer,\n\
                                 TabContainer, ContentPane, Form, TextBox,\n\
                                 _LayoutWidget, _TemplatedMixin, _WidgetsInTemplateMixin, Select, registry,\n\
                                     template) {\n\
\n\
                                 return declare(\"GlebWidget4\", [_LayoutWidget, _TemplatedMixin, _WidgetsInTemplateMixin], {\n\
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
                                     //  Implementation  ---\n\
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
style: \"height: 50%; width: 90%x;\",\n \
cssText: \"width: 50%; height: 70%; showLabels: true; colspan: 2\",\n\
doLayout: \"true\",\n\
id: \"");


static const char* DJ_TAB_PART_3("\", }).placeAt(\"stubTabContainer\");\n");



static const char* DJ_TABLE_PART_1("\nvar tc = new dojox.layout.TableContainer(\n\
{ cols: 2,\n\
\"labelWidth\" : \"50\" });");

static const char* DJ_TABLE_PART_2("cp.addChild(tc); \n");

static const char* DJ_TABLE_ROW_PART_1("\nvar txt = new dijit.form.TextBox({label: \"");
static const char* DJ_TABLE_ROW_PART_2("\"});\n\
tc.addChild(txt);");

static void genTabDojoJS(StringBuffer &strJS, const char *pName)
{
    assert(pName != NULL);

    if (pName == NULL)
    {
        return;
    }

    Owned<IRandomNumberGenerator> random = createRandomNumberGenerator();

    random->seed(get_cycles_now());

    StringBuffer id;

    id.appendf("%d",random->next() % 123456);

   //strJS.append(DJ_TAB_PART_1).append(pName).append(DJ_TAB_PART_2).append(pName).append(DJ_TAB_PART_3);
    strJS.append(DJ_TAB_PART_1).append(pName).append(DJ_TAB_PART_2).append(id.str()).append(DJ_TAB_PART_3);
}

static void genTableRow(StringBuffer &strJS, const char* pName)
{

}


#endif // _DOJOJSMARKUP_HPP_
