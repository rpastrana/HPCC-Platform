#ifndef _SCHEMA_SCHEMA_HPP_
#define _SCHEMA_SCHEMA_HPP_

#include "jarray.hpp"
#include "jstring.hpp"
#include "SchemaAll.hpp"
#include "SchemaCommon.hpp"

class CSchema : public InterfaceImpl, public CXSDNodeBase
{
public:

    virtual ~CSchema()
    {
    }

    virtual void dump(std::ostream& cout, unsigned int offset = 0) const;

    virtual void getDocumentation(StringBuffer &strDoc) const;

    virtual void traverseAndProcessNodes() const;

    virtual const char* getXML(const char* /*pComponent*/);

    GETTERSETTER(XMLNS_XS)
    GETTERSETTER(ElementFormDefault)
    GETTERSETTER(AttributeFormDefault)

    CXSDNode* getExtensionType(const char* pExtensionTypeName) const;

    const char* getSchemaLocation() const
    {
        return m_strSchemaLocation.str();
    }

    CComplexTypeArray* getComplexTypeArray() const
    {
        return m_pComplexTypeArray;
    }

    static CSchema* load(const char* pSchemaLocation, IPropertyTree *pSchemaRoot, const char* xpath);
    static CSchema* load(const char* pSchemaLocation, CXSDNodeBase* pParentNode);

protected:

    CSchema(const char * pSchemaLocation, const char* pXMLNS_XS = NULL, const char* pElementFormDefault = NULL, const char* pAttributeFormDefault = NULL,
            CElementArray* pElementArray = NULL, CComplexTypeArray* pComplexTypeArray = NULL, CAttributeGroupArray* pAttributeGroupArray = NULL,
            CSimpleTypeArray* pSimpleTypeArray = NULL, CIncludeArray* pIncludeArray = NULL) : CXSDNodeBase::CXSDNodeBase(NULL, XSD_SCHEMA), m_strSchemaLocation(pSchemaLocation),
                m_strXMLNS_XS(pXMLNS_XS), m_strElementFormDefault(pElementFormDefault), m_strAttributeFormDefault(pAttributeFormDefault),
                m_pElementArray(pElementArray), m_pComplexTypeArray(pComplexTypeArray), m_pAttributeGroupArray(pAttributeGroupArray),
                m_pSimpleTypeArray(pSimpleTypeArray), m_pIncludeArray(pIncludeArray)
    {
    }

    StringBuffer            m_strSchemaLocation;
    CElementArray*          m_pElementArray;
    CComplexTypeArray*      m_pComplexTypeArray;
    CAttributeGroupArray*   m_pAttributeGroupArray;
    CSimpleTypeArray*       m_pSimpleTypeArray;
    CIncludeArray*          m_pIncludeArray;

private:

};



#endif // _SCHEMA_SCHEMA_HPP_
