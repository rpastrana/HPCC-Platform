#include "jptree.hpp"
#include "SchemaRestriction.hpp"
#include "XMLTags.h"

void CRestriction::dump(std::ostream& cout, unsigned int offset) const
{
    offset+= STANDARD_OFFSET_1;

    QuickOutHeader(cout, XSD_RESTRICTION_STR, offset);

    QUICK_OUT_2(Base);
    QUICK_OUT_2(ID);

    // to do call base
    // base->dump(cout, offset);

    QuickOutFooter(cout, XSD_RESTRICTION_STR, offset);
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

    pID = pTree->queryProp(XML_ATTR_ID);
    pBase = pTree->queryProp(XML_ATTR_BASE);

    // new to load BASE!!!

    CRestriction* pRestriction = new CRestriction(pParentNode, pID, pBase);

    return pRestriction;
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
