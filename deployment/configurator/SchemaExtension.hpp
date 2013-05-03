#ifndef _SCHEMA_EXTENSION_HPP_
#define _SCHEMA_EXTENSION_HPP_

#include "SchemaCommon.hpp"
#include "jstring.hpp"

class IPropertyTree;

class CExtension : public CXSDNode
{
public:

    virtual ~CExtension()
    {
    }

    GETTERSETTER(Base)

    virtual void dump(std::ostream& cout, unsigned int offset = 0) const;

    virtual const char* getXML(const char* /*pComponent*/);

    virtual void initExtension();

    void setBaseNode(CXSDNodeBase* pCXSDNode)
    {
        if (m_pXSDNode != NULL)
        {
            //m_pXSDNode->Release();
        }

        m_pXSDNode = pCXSDNode;
    }

    static CExtension* load(CXSDNodeBase* pParentNode, IPropertyTree *pSchemaRoot, const char* xpath = NULL);

protected:

    CExtension(CXSDNodeBase* pParentNode, const char* pBase = NULL, CXSDNode *pXSDNode = NULL) : CXSDNode::CXSDNode(pParentNode, XSD_EXTENSION), m_strBase(pBase), m_pXSDNode(pXSDNode)
    {
    }

    const CXSDNodeBase* getBaseNode() const
    {
        return m_pXSDNode;
    }

    CXSDNodeBase *m_pXSDNode;

private:

    CExtension() : CXSDNode::CXSDNode(NULL, XSD_EXTENSION)
    {
    }
};

#endif // _SCHEMA_EXTENSION_HPP_
