#include <cassert>
#include "XMLTags.h"
#include "jptree.hpp"
#include "SchemaAnnotation.hpp"
#include "SchemaDocumentation.hpp"
#include "SchemaAppInfo.hpp"

CAnnotation* CAnnotation::load(CXSDNodeBase* pParentNode, IPropertyTree *pSchemaRoot, const char* xpath)
{
    assert(pSchemaRoot != NULL);

    if (pSchemaRoot == NULL)
    {
        return NULL;
    }

    StringBuffer strXPathExt(xpath);

    strXPathExt.append("/").append(XSD_TAG_DOCUMENTATION);
    CDocumentation *pDocumentation = CDocumentation::load(NULL, pSchemaRoot, strXPathExt.str());

    strXPathExt.clear().append(xpath).append("/").append(XSD_TAG_APP_INFO);
    CAppInfo *pAnnInfo = CAppInfo::load(NULL, pSchemaRoot, strXPathExt.str());

    CAnnotation *pAnnotation = new CAnnotation(pParentNode, pDocumentation,pAnnInfo);

    SETPARENTNODE(pDocumentation, pAnnotation);
    SETPARENTNODE(pAnnInfo, pAnnotation);

    return pAnnotation;
}

void CAnnotation::dump(std::ostream& cout, unsigned int offset) const
{
    offset+= STANDARD_OFFSET_1;

    QuickOutHeader(cout, XSD_ANNOTATION_STR, offset);

    if (m_pAppInfo != NULL)
    {
        m_pAppInfo->dump(cout, offset);
    }

    if (m_pDocumentation != NULL)
    {
        m_pDocumentation->dump(cout, offset);
    }

    QuickOutFooter(cout, XSD_ANNOTATION_STR, offset);
}

void CAnnotation::traverseAndProcessNodes() const
{
     CXSDNodeBase::processEntryHandlers(this);

     if (m_pAppInfo != NULL)
     {
         m_pAppInfo->traverseAndProcessNodes();
     }

     if (m_pDocumentation != NULL)
     {
         m_pDocumentation->traverseAndProcessNodes();
     }

     CXSDNodeBase::processExitHandlers(this);
}
