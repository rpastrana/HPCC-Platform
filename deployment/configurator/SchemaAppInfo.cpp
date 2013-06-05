#include "jptree.hpp"
#include "XMLTags.h"
#include "SchemaAppInfo.hpp"
#include "DocumentationMarkup.hpp"

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

    StringBuffer strXPathDocInclude(xpath);
    strXPathDocInclude.append("/").append(TAG_DOCINCLUDE);

    StringBuffer strXPathViewChildNodes(xpath);
    strXPathViewChildNodes.append("/").append(TAG_VIEWCHILDNODES);

    StringBuffer strXPathXPath(xpath);
    strXPathXPath.append("/").append(TAG_XPATH);

    StringBuffer strViewType;
    StringBuffer strColIndex;
    StringBuffer strToolTip;
    StringBuffer strTitle;
    StringBuffer strWidth;
    StringBuffer strAutoGenForWizard;
    StringBuffer strAutoGenDefaultValue;
    StringBuffer strAutoGenDefaultForMultiNode;
    StringBuffer strDocInclude;
    StringBuffer strViewChildNodes;
    StringBuffer strXPath;

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
    if (pSchemaRoot->queryPropTree(strXPathDocInclude.str()) != NULL)
    {
        strDocInclude.append(pSchemaRoot->queryPropTree(strXPathDocInclude.str())->queryProp(""));
    }
    if (pSchemaRoot->queryPropTree(strXPathViewChildNodes.str()) != NULL)
    {
        strViewChildNodes.append(pSchemaRoot->queryPropTree(strXPathViewChildNodes.str())->queryProp(""));
    }
    if (pSchemaRoot->queryPropTree(strXPathXPath.str()) != NULL)
    {
        strXPath.append(pSchemaRoot->queryPropTree(strXPathXPath.str())->queryProp(""));
    }

    CAppInfo *pAppInfo = new CAppInfo(pParentNode, strViewType.str(),  strColIndex.str(), strToolTip.str(), strTitle.str(), strWidth.str(), strAutoGenForWizard.str(), strAutoGenDefaultValue.str(), NULL, strViewChildNodes.str(), strDocInclude.str(), strXPath.str());

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
    QUICK_OUT(cout, DocInclude, offset);
    QUICK_OUT(cout, ViewChildNodes, offset);
    QUICK_OUT(cout, XPath, offset);

    QuickOutFooter(cout, XSD_APP_INFO_STR, offset);
}

void CAppInfo::traverseAndProcessNodes() const
{
    CXSDNodeBase::processEntryHandlers(this);
    CXSDNodeBase::processExitHandlers(this);
}

void CAppInfo::getDocumentation(StringBuffer &strDoc) const
{
    if (this->getDocInclude() != NULL && this->getDocInclude()[0] != 0)
    {
        strDoc.appendf("<%s %s />\n", DM_XI_INCLUDE, this->getDocInclude());
    }
}

void CAppInfo::getDojoJS(StringBuffer &strJS) const
{
}
