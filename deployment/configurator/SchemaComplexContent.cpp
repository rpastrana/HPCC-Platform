#include <cassert>
#include "XMLTags.h"
#include "SchemaComplexContent.hpp"
#include "SchemaExtension.hpp"


CComplexContent* CComplexContent::load(CXSDNodeBase* pParentNode, IPropertyTree *pSchemaRoot, const char* xpath)
{
    if (pSchemaRoot == NULL)
    {
        return NULL;
    }

    StringBuffer strXPathExt(xpath);
    strXPathExt.append("/").append(XSD_TAG_EXTENSION);

    CExtension* pExtension = CExtension::load(NULL, pSchemaRoot, strXPathExt.str());

    CComplexContent *pComplexContent = new CComplexContent(pParentNode, pExtension);

    SETPARENTNODE(pExtension, pComplexContent)

    return pComplexContent;
}

void CComplexContent::dump(std::ostream& cout, unsigned int offset) const
{
    offset+= STANDARD_OFFSET_1;

    if (m_pExtension != NULL)
    {
        m_pExtension->dump(cout, offset);
    }
}

const char* CComplexContent::getXML(const char* /*pComponent*/)
{
    if (m_strXML.length() == 0 && m_pExtension != NULL)
    {
        m_strXML.append(m_pExtension->getXML(NULL));
    }

    return m_strXML.str();
}
