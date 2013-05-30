#include "jptree.hpp"
#include "SchemaRestriction.hpp"
#include "SchemaEnumeration.hpp"
#include "XMLTags.h"

void CRestriction::dump(std::ostream& cout, unsigned int offset) const
{
    offset+= STANDARD_OFFSET_1;

    QuickOutHeader(cout, XSD_RESTRICTION_STR, offset);

    QUICK_OUT_2(Base);
    QUICK_OUT_2(ID);

    // to do call base
    // base->dump(cout, offset);

    if (m_pEnumerationArray != NULL)
    {
        m_pEnumerationArray->dump(cout, offset);
    }

    QuickOutFooter(cout, XSD_RESTRICTION_STR, offset);
}

void CRestriction::getDocumentation(StringBuffer &strDoc) const
{
    if (m_pEnumerationArray != NULL)
    {
        m_pEnumerationArray->getDocumentation(strDoc);
    }
}

void CRestriction::getDojoJS(StringBuffer &strJS) const
{
    if (m_pEnumerationArray != NULL)
    {
        m_pEnumerationArray->getDojoJS(strJS);
    }
}

CRestriction* CRestriction::load(CXSDNodeBase* pParentNode, IPropertyTree *pSchemaRoot, const char* xpath)
{
    if (pParentNode == NULL || pSchemaRoot == NULL)
    {
        return NULL;
    }

    IPropertyTree *pTree = pSchemaRoot->queryPropTree(xpath);

    const char* pID =  NULL;
    const char* pBase =  NULL;

    // TODO: handle Base in restrictions

    pID = pTree->queryProp(XML_ATTR_ID);
    pBase = pTree->queryProp(XML_ATTR_BASE);


    CRestriction* pRestriction = new CRestriction(pParentNode, pID, pBase);

    StringBuffer strXPathExt(xpath);

    strXPathExt.append("/").append(XSD_TAG_ENUMERATION);

    CEnumerationArray *pEnumerationArray = CEnumerationArray::load(pRestriction, pSchemaRoot, strXPathExt.str());

    if (pEnumerationArray != NULL)
    {
        pRestriction->setEnumerationArray(pEnumerationArray);
    }

    return pRestriction;
}

void  CRestriction::traverseAndProcessNodes() const
{
    CXSDNodeBase::processEntryHandlers(this);
    CXSDNodeBase::processExitHandlers(this);
}

const char* CRestriction::getXML(const char* /*pComponent*/)
{
    if (m_strXML.length () == 0)
    {
        m_strXML.append("<").append(getBase()).append("\n");
        m_strXML.append("<").append(getID()).append("\n");

        // TODO
      /*  if (m_pBase != NULL)
        {
            m_strXML.append(m_pBase->getXML(NULL));
        }*/

        m_strXML.append("/>\n");
    }

    return m_strXML.str();
}
