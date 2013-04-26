#include "jptree.hpp"
#include "XMLTags.h"
#include "SchemaAppInfo.hpp"

CAppInfo* CAppInfo::load(CXSDNodeBase* pParentNode, IPropertyTree *pSchemaRoot, const char* xpath)
{
    if (pSchemaRoot == NULL)
    {
        return NULL;
    }

    if (pSchemaRoot->queryPropTree(xpath) == NULL)
    {
        return NULL;   // No AppInfo node
    }

    StringBuffer strXPathViewType(xpath);
    strXPathViewType.append("/").append(TAG_VIEWTYPE);

    StringBuffer strXPathColIndex(xpath);
    strXPathColIndex.append("/").append(TAG_COLINDEX);

    StringBuffer strXPathToolTip(xpath);
    strXPathToolTip.append("/").append(TAG_TOOLTIP);

    StringBuffer strXPathTitle(xpath);
    strXPathTitle.append("/").append(TAG_TITLE);

    StringBuffer strXPathWidth(xpath);
    strXPathWidth.append("/").append(TAG_WIDTH);

    StringBuffer strXPathAutoGenDefaultValue(xpath);
    strXPathAutoGenDefaultValue.append("/").append(TAG_AUTOGENWIZARD);

    StringBuffer strXPathAutoGenForWizard(xpath);
    strXPathAutoGenForWizard.append("/").append(TAG_AUTOGENDEFAULTVALUE);

    StringBuffer strXPathAutoGenDefaultForMultiNode(xpath);
    strXPathAutoGenDefaultForMultiNode.append("/").append(TAG_AUTOGENDEFAULTVALUEFORMULTINODE);

    StringBuffer strViewType;
    StringBuffer strColIndex;
    StringBuffer strToolTip;
    StringBuffer strTitle;
    StringBuffer strWidth;
    StringBuffer strAutoGenForWizard;
    StringBuffer strAutoGenDefaultValue;
    StringBuffer strAutoGenDefaultForMultiNode;

    if (pSchemaRoot->queryPropTree(strXPathViewType.str()) != NULL)
    {
        strViewType.append(pSchemaRoot->queryPropTree(strXPathViewType.str())->queryProp(""));
    }
    if (pSchemaRoot->queryPropTree(strXPathColIndex.str()) != NULL)
    {
        strColIndex.append(pSchemaRoot->queryPropTree(strXPathColIndex.str())->queryProp(""));
    }
    if (pSchemaRoot->queryPropTree(strXPathToolTip.str()) != NULL)
    {
        strToolTip.append(pSchemaRoot->queryPropTree(strXPathToolTip.str())->queryProp(""));
    }
    if (pSchemaRoot->queryPropTree(strXPathTitle.str()) != NULL)
    {
        strTitle.append(pSchemaRoot->queryPropTree(strXPathTitle.str())->queryProp(""));
    }
    if (pSchemaRoot->queryPropTree(strXPathWidth.str()) != NULL)
    {
        strWidth.append(pSchemaRoot->queryPropTree(strXPathWidth.str())->queryProp(""));
    }
    if (pSchemaRoot->queryPropTree(strXPathAutoGenForWizard.str()) != NULL)
    {
        strAutoGenForWizard.append(pSchemaRoot->queryPropTree(strXPathAutoGenForWizard.str())->queryProp(""));
    }
    if (pSchemaRoot->queryPropTree(strXPathAutoGenDefaultValue.str()) != NULL)
    {
        strAutoGenDefaultValue.append(pSchemaRoot->queryPropTree(strXPathAutoGenDefaultValue.str())->queryProp(""));
    }
    if (pSchemaRoot->queryPropTree(strXPathAutoGenDefaultForMultiNode.str()) != NULL)
    {
        strAutoGenDefaultForMultiNode.append(pSchemaRoot->queryPropTree(strXPathAutoGenDefaultForMultiNode.str())->queryProp(""));
    }

    CAppInfo *pAppInfo = new CAppInfo(pParentNode, strViewType.str(),  strColIndex.str(), strToolTip.str(), strTitle.str(), strWidth.str(), strAutoGenForWizard.str(), strAutoGenDefaultValue.str());

    return pAppInfo;
}

void CAppInfo::dump(std::ostream &cout, unsigned int offset) const
{
    offset += STANDARD_OFFSET_1;

    QuickOutHeader(cout, XSD_APP_INFO_STR, offset);

    QUICK_OUT(cout, Title, offset);
    QUICK_OUT(cout, ViewType, offset);
    QUICK_OUT(cout, ToolTip, offset);
    QUICK_OUT(cout, ColIndex, offset);
    QUICK_OUT(cout, Width, offset);
    QUICK_OUT(cout, AutoGenForWizard, offset);
    QUICK_OUT(cout, AutoGenDefaultValue, offset);
    QUICK_OUT(cout, AutoGenDefaultValueForMultiNode, offset);

    QuickOutFooter(cout, XSD_APP_INFO_STR, offset);
}
