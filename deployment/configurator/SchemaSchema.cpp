#include "jptree.hpp"
#include "XMLTags.h"
#include "SchemaSchema.hpp"

CSchema* CSchema::load(const char* pSchemaLocation, IPropertyTree *pSchemaRoot, const char* xpath)
{
    CSchema* pSchema = new CSchema(pSchemaLocation);

    if (pSchemaRoot == NULL)
    {
        return NULL;
    }

    IPropertyTree *pTree = pSchemaRoot->queryPropTree(xpath);

    if (pTree == NULL)
    {
        return NULL;
    }

    //pSchema->setParentNode(NULL);  // this is the root node in this schema
    pSchema->setXMLNS_XS(pTree->queryProp(XML_ATTR_XMLNS_XS));
    pSchema->setElementFormDefault(pTree->queryProp(XML_ATTR_ELEMENTFORMDEFAULT));
    pSchema->setAttributeFormDefault(pTree->queryProp(XML_ATTR_ATTRIBUTEFORMDEFAULT));

    StringBuffer strXPathExt(xpath);

    strXPathExt.clear().append(xpath).append(XSD_TAG_INCLUDE);
    CIncludeArray* pIncludeArray = CIncludeArray::load(pSchema, pSchemaRoot, strXPathExt);

    strXPathExt.clear().append(xpath).append(XSD_TAG_SIMPLE_TYPE);
    CSimpleTypeArray* pSimpleTypeArray = CSimpleTypeArray::load(pSchema, pSchemaRoot, strXPathExt);

    strXPathExt.clear().append(xpath).append(XSD_TAG_COMPLEX_TYPE);
    CComplexTypeArray* pComplexTypeArray = CComplexTypeArray::load(pSchema, pSchemaRoot, strXPathExt);

    strXPathExt.clear().append(xpath).append(XSD_TAG_ELEMENT);
    CElementArray* pElemArray = CElementArray::load(pSchema, pSchemaRoot, strXPathExt.str());

    strXPathExt.clear().append(xpath).append(XSD_TAG_ATTRIBUTE_GROUP);
    CAttributeGroupArray* pAttributeGroupArray = CAttributeGroupArray::load(pSchema, pSchemaRoot, strXPathExt);


    //CSchema* pSchema = new CSchema(pXMLNS_XS, pElementFormDefault, pAttributeFormDefault, pElemArray, pComplexTypeArray, pAttributeGroupArray, pSimpleTypeArray, pIncludeArray);
    pSchema->m_pElementArray = pElemArray;
    pSchema->m_pComplexTypeArray = pComplexTypeArray;

    if (pSchema->m_pAttributeGroupArray == NULL)
    {
        pSchema->m_pAttributeGroupArray = pAttributeGroupArray;
    }
    else
    {
        // copy contents from from pAttributeGroupArray to pSchema->m_pAttributeGroupArray
    }
    pSchema->m_pSimpleTypeArray = pSimpleTypeArray;
    pSchema->m_pIncludeArray = pIncludeArray;

    return pSchema;
}
CSchema* CSchema::load(const char* pSchemaLocation, CXSDNodeBase* pParentNode)
{
    if (pSchemaLocation == NULL)
    {
        return NULL;
    }

    Linked<IPropertyTree> pSchemaRoot;

    StringBuffer schemaPath;

    schemaPath.appendf("%s%s", DEFAULT_SCHEMA_DIRECTORY, pSchemaLocation);

    pSchemaRoot.setown(createPTreeFromXMLFile(schemaPath.str()));

    CSchema *pSchema = CSchema::load(pSchemaLocation, pSchemaRoot, XSD_TAG_SCHEMA);

    SETPARENTNODE(pSchema, pParentNode)

    return pSchema;
}

/*CSchema* CSchema::load(const char* pSchemaLocation)
{
    if (pSchemaLocation == NULL)
    {
        return NULL;
    }

    Linked<IPropertyTree> pSchemaRoot;

    StringBuffer schemaPath;

    schemaPath.appendf("%s%s", DEFAULT_SCHEMA_DIRECTORY, pSchemaLocation);

    pSchemaRoot.setown(createPTreeFromXMLFile(schemaPath.str()));

    return CSchema::load(pSchemaLocation, pSchemaRoot, XSD_TAG_SCHEMA);
}
*/
/*bool CSchema::AddToComplexTypeArray(CSchema *pSchema, const char* pSchemaLocation)
{
    if (pSchema == NULL || pSchemaLocation == NULL)
    {
        return false;
    }

    Linked<IPropertyTree> pSchemaRoot;

    StringBuffer schemaPath;

    schemaPath.appendf("%s%s", DEFAULT_SCHEMA_DIRECTORY, pSchemaLocation);

    pSchemaRoot.setown(createPTreeFromXMLFile(schemaPath.str()));

    StringBuffer strXPathExt;

    strXPathExt.clear().append(XSD_TAG_COMPLEX_TYPE);

    CComplexTypeArray* pComplexTypeArray = CComplexTypeArray::load(pSchema, pSchemaRoot, strXPathExt);

    if (pComplexTypeArray != NULL)
    {
        int length = pComplexTypeArray->length();

        if (pSchema->m_pComplexTypeArray == NULL)
        {
            pSchema->m_pComplexTypeArray = pComplexTypeArray;
        }
        else
        {
            for (int idx = 0; idx < length; idx++)
            {
                pSchema->m_pComplexTypeArray->append(pComplexTypeArray->item(idx));
            }
        }
        //pComplexTypeArray->Release();
    }

    return true;
}
*/
void CSchema::dump(std::ostream& cout, unsigned int offset) const
{
    offset+= STANDARD_OFFSET_1;

    QuickOutHeader(cout, XSD_SCHEMA_STR, offset);

    QUICK_OUT_2(XMLNS_XS);
    QUICK_OUT_2(ElementFormDefault);
    QUICK_OUT_2(AttributeFormDefault);

    if (m_pElementArray != NULL)
    {
        m_pElementArray->dump(cout);
    }
    if (m_pComplexTypeArray != NULL)
    {
        m_pComplexTypeArray->dump(cout);
    }
    if (m_pAttributeGroupArray != NULL)
    {
        m_pAttributeGroupArray->dump(cout);
    }
    if (m_pSimpleTypeArray != NULL)
    {
        m_pSimpleTypeArray->dump(cout);
    }
    if (m_pIncludeArray != NULL)
    {
        m_pIncludeArray->dump(cout);
    }

    QuickOutFooter(cout, XSD_SCHEMA_STR, offset);
}

const char* CSchema::getXML(const char* /*pComponent*/)
{
    if (m_strXML.length() == 0)
    {
        int length =  m_pElementArray->length();

        //m_strXML.append("<Environment>\n\t<Software>");

        if (m_pElementArray != NULL)
        {
            for (int idx = 0; idx < length; idx++)
            {
                CElement &Element = m_pElementArray->item(idx);

                m_strXML.append(Element.getXML(NULL));

                if (idx+1 < length)
                {
                    m_strXML.append("\n");
                }
            }
        }

        if (m_pAttributeGroupArray != NULL)
        {
            length = m_pAttributeGroupArray->length();
            for (int idx = 0; idx < length; idx++)
            {
                CAttributeGroup &AttributeGroup  = m_pAttributeGroupArray->item(idx);

                m_strXML.append(AttributeGroup.getXML(NULL));

                if (idx+1 < length)
                {
                    m_strXML.append("\n");
                }
            }
        }

         m_strXML.append("/>\n");

        if (m_pComplexTypeArray != NULL)
        {
            length = m_pComplexTypeArray->length();
            for (int idx = 0; idx < length; idx++)
            {
                CComplexType &ComplexType = m_pComplexTypeArray->item(idx);

                m_strXML.append(ComplexType.getXML(NULL));

                if (idx+1 < length)
                {
                    m_strXML.append("\n");
                }
            }
        }

        if (m_pSimpleTypeArray != NULL)
        {
            length = m_pSimpleTypeArray->length();
            for (int idx = 0; idx < length; idx++)
            {
                CSimpleType &SimpleType = m_pSimpleTypeArray->item(idx);

                m_strXML.append(SimpleType.getXML(NULL));

                if (idx+1 < length)
                {
                    m_strXML.append("\n");
                }
            }
        }

        if (m_pIncludeArray != NULL)
        {
            length = m_pIncludeArray->length();
            for (int idx = 0; idx < length; idx++)
            {
                CInclude &Include = m_pIncludeArray->item(idx);

                m_strXML.append(Include.getXML(NULL));

                if (idx+1 < length)
                {
                    m_strXML.append("\n");
                }
            }
        }

     //   m_strXML.append("\t</Software>\n</Environment>\n");
    }

    return m_strXML.str();
}

CXSDNode* CSchema::getExtensionType(const char* pExtensionTypeName) const
{
    if (pExtensionTypeName == NULL)
    {
        return NULL;
    }

    if (m_pSimpleTypeArray != NULL)
    {
        int length = m_pSimpleTypeArray->length();

        for (int idx = 0; idx < length; idx++)
        {
            CSimpleType &SimpleType = m_pSimpleTypeArray->item(idx);

            if (strcmp(SimpleType.getName(), pExtensionTypeName) == 0)
            {
                return &SimpleType;
            }
        }
    }

    if (m_pComplexTypeArray != NULL)
    {
        int length = m_pComplexTypeArray->length();

        for (int idx = 0; idx < length; idx++)
        {
            CComplexType &ComplexType = m_pComplexTypeArray->item(idx);

            if (strcmp(ComplexType.getName(), pExtensionTypeName) == 0)
            {
                return &ComplexType;
            }
        }
    }

    return NULL;
}
