#include "ConfigSchemaHelper.hpp"
#include "SchemaAttributes.hpp"
#include "SchemaElement.hpp"
#include "jptree.hpp"
#include "XMLTags.h"
#include <cstring>

#define LOOP_THRU_BUILD_SET for (int idx = 0; idx < m_buildSetArray.length(); idx++)

CConfigSchemaHelper* CConfigSchemaHelper::s_pCConfigSchemaHelper = NULL;


CConfigSchemaHelper* CConfigSchemaHelper::getInstance(const char* pDefaultDirOverride)
{
    // not thread safe!!!

    if (s_pCConfigSchemaHelper == NULL)
    {
        s_pCConfigSchemaHelper = new CConfigSchemaHelper();

        if (s_pCConfigSchemaHelper != NULL && pDefaultDirOverride != NULL)
        {
            s_pCConfigSchemaHelper->m_pDefaultDirOverride = pDefaultDirOverride;
        }
    }

    return s_pCConfigSchemaHelper;
}

CConfigSchemaHelper* CConfigSchemaHelper::getInstance(const char* pBuildSetFileName, const char *pBaseDirectory, const char *pDefaultDirOverride)
{
    assert(pBuildSetFileName != NULL);
    assert(pBaseDirectory != NULL);

    if (s_pCConfigSchemaHelper == NULL && pBuildSetFileName != NULL && pBaseDirectory != NULL)
    {
       s_pCConfigSchemaHelper = new CConfigSchemaHelper(pBuildSetFileName, pBaseDirectory, pDefaultDirOverride);
    }

    return s_pCConfigSchemaHelper;

}

CConfigSchemaHelper::CConfigSchemaHelper(const char* pBuildSetFile, const char* pBuildSetDir, const char* pDefaultDirOverride)
{
    assert(pBuildSetFile != NULL);
    assert(pBuildSetDir != NULL);

    if (pBuildSetFile != NULL && pBuildSetDir != NULL)
    {
        m_buildSetPath.clear().appendf("%s%s%s", pBuildSetDir, pBuildSetDir[strlen(pBuildSetDir)-1] == '/' ? "" : "/", pBuildSetFile);
    }
}

CConfigSchemaHelper::~CConfigSchemaHelper()
{
}


void CConfigSchemaHelper::getBuildSetComponents(StringArray& buildSetArray) const
{
    LOOP_THRU_BUILD_SET
    {
        const char *pName = m_buildSetArray.item(idx).getName();
        buildSetArray.append(pName);
    }
}

bool CConfigSchemaHelper::populateBuildSet()
{
    StringBuffer xpath;

    if (m_buildSetTree.get() != NULL)
    {
        return false;
    }

    try
    {
        m_buildSetTree.set(createPTreeFromXMLFile(m_buildSetPath.str()));
    }
    catch(...)
    {
        return false;
    }

    xpath.appendf("./%s/%s/%s", XML_TAG_PROGRAMS, XML_TAG_BUILD, XML_TAG_BUILDSET);

    Owned<IPropertyTreeIterator> iter = m_buildSetTree->getElements(xpath.str());

    ForEach(*iter)
    {
        IPropertyTree* pTree = &iter->query();
        Owned<CBuildSet> pBuildSet = new CBuildSet(pTree->queryProp(XML_ATTR_INSTALLSET), pTree->queryProp(XML_ATTR_NAME), pTree->queryProp(XML_ATTR_PROCESS_NAME), pTree->queryProp(XML_ATTR_SCHEMA));

        m_buildSetArray.append(*pBuildSet.getLink());
    }

    return true;
}

bool CConfigSchemaHelper::populateSchema()
{
    LOOP_THRU_BUILD_SET
    {
        const char *pSchemaFile = m_buildSetArray.item(idx).getSchema();
        //const char *pCompName = m_buildSetArray.item(idx).getName();

        if (pSchemaFile != NULL)
        {
            CXSDNodeBase *pNull = NULL;
            CSchema *pSchema = CSchema::load(pSchemaFile, pNull, m_pDefaultDirOverride);
            m_schemaMap.setValue(pSchemaFile, pSchema);
        }
    }

    return true;
}

void CConfigSchemaHelper::printConfigSchema(StringBuffer &strXML) const
{
    const char *pComponent = NULL;
    CSchema* pSchema = NULL;
    LOOP_THRU_BUILD_SET
    {
        if (pComponent == NULL || strcmp(pComponent, m_buildSetArray.item(idx).getSchema()) == 0)
        {
            const char* pXSDSchema = m_buildSetArray.item(idx).getSchema();

            if (pXSDSchema == NULL)
            {
                continue;
            }

            pSchema = m_schemaMap.getValue(m_buildSetArray.item(idx).getSchema());

            if (pSchema != NULL)
            {
                if (strXML.length() > 0 ? strcmp(strXML.str(), pXSDSchema) == 0 : true)
                pSchema->dump(std::cout);
            }
        }
    }
}

const char* CConfigSchemaHelper::printDocumentation(const char* comp)
{
    assert(comp);

    if (comp == NULL)
    {
        return NULL;
    }

    CSchema* pSchema = NULL;

    LOOP_THRU_BUILD_SET
    {
        if (m_buildSetArray.item(idx).getSchema() != NULL && strcmp(comp, m_buildSetArray.item(idx).getSchema()) == 0)
        {
             pSchema = m_schemaMap.getValue(m_buildSetArray.item(idx).getSchema());

             assert(pSchema != NULL);

             if (pSchema != NULL)
             {
                static StringBuffer strDoc;
                pSchema->getDocumentation(strDoc);

                return strDoc.str();
             }
        }
    }

    return NULL;
}

//test purposes
bool CConfigSchemaHelper::getXMLFromSchema(StringBuffer& strXML, const char* pComponent)
{
    CAttributeArray *pAttributeArray = NULL;
    CElementArray *pElementArray = NULL;
    CSchema* pSchema = NULL;

    //std::cout << "\n<XML>\n<";

/*    LOOP_THRU_BUILD_SET
    {
        if (strcmp(pComponent, m_buildSetArray.item(idx).getName()) != 0)
        {

        pAttributeArray = m_componentAttributesArrayMap.getValue(m_buildSetArray.item(idx).getName());

        if (pAttributeArray != NULL)
        {
            strXML.clear().append(pAttributeArray->getXML(NULL));
            return true;
        }

        m_componentElementArrayMap


    }

    strXML.clear();
    LOOP_THRU_BUILD_SET
    {
        if (pComponent == NULL || strcmp(pComponent, m_buildSetArray.item(idx).getSchema()) == 0)
        {
            pElementArray = m_componentElementArrayMap.getValue(m_buildSetArray.item(idx).getSchema());

            if (pElementArray != NULL)
            {
                strXML.append(pElementArray->getXML(NULL));
            }
        }
    }
*/
    strXML.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<Environment>\n\t<Software>");
    LOOP_THRU_BUILD_SET
    {
        if (pComponent == NULL || strcmp(pComponent, m_buildSetArray.item(idx).getSchema()) == 0)
        {
            const char* pXSDSchema = m_buildSetArray.item(idx).getSchema();

            if (pXSDSchema == NULL)
            {
                continue;
            }

            pSchema = m_schemaMap.getValue(m_buildSetArray.item(idx).getSchema());

            if (pSchema != NULL)
            {
                strXML.append(pSchema->getXML(NULL));
            }
        }
    }
    strXML.append("\t</Software>\n</Environment>\n");

    return true;

    //std::cout << ">\n</XML>";

    //return false;
}

CSchema* CConfigSchemaHelper::getSchemaForXSD(const char* pComponent)
{
    return m_schemaMap.getValue(pComponent);
}

void CConfigSchemaHelper::setSchemaForXSD(const char* pComponent, CSchema *pSchema)
{
    assert(pSchema != NULL);

    if (pSchema != NULL)
    {
        m_schemaMap.setValue(pComponent, pSchema);
    }
}

CSimpleType* CConfigSchemaHelper::getSimpleTypeWithName(const char* pName)
{
    assert(pName != NULL);

    if (pName == NULL)
    {
        return NULL;
    }

    CSimpleType *pSimpleType = NULL;

    pSimpleType = *(m_simpleTypePtrMap.getValue(pName));

    assert(pSimpleType != NULL);

    return pSimpleType;
}

void CConfigSchemaHelper::setSimpleTypeWithName(const char* pName, CSimpleType *pSimpleType)
{
    assert (pSimpleType != NULL);

    if (pName == NULL || pSimpleType == NULL)
    {
        return;
    }

    m_simpleTypePtrMap.setValue(pName, pSimpleType);
}

CComplexType* CConfigSchemaHelper::getComplexTypeWithName(const char* pName)
{
    assert(pName != NULL);

    if (pName == NULL)
    {
        return NULL;
    }

    CComplexType *pComplexType = NULL;

    pComplexType = *(m_complexTypePtrsMap.getValue(pName));

    assert(pComplexType != NULL);

    return pComplexType;
}

void CConfigSchemaHelper::setComplexTypeWithName(const char* pName, CComplexType *pComplexType)
{
    assert (pComplexType != NULL);

    if (pName == NULL || pComplexType == NULL)
    {
        return;
    }

    m_complexTypePtrsMap.setValue(pName, pComplexType);
}

CAttributeGroup* CConfigSchemaHelper::getAttributeGroup(const char* pName)
{
    assert(pName != NULL);

    if (pName == NULL)
    {
        return NULL;
    }

    CAttributeGroup *pAttributeGroup = NULL;

    pAttributeGroup = *(m_attributeGroupTypePtrsMap.getValue(pName));

    assert(pAttributeGroup != NULL);

    return pAttributeGroup;
}

void CConfigSchemaHelper::setAttributeGroupTypeWithName(const char* pName, CAttributeGroup *pAttributeGroup)
{
    assert (pAttributeGroup != NULL);

    if (pName == NULL || pAttributeGroup == NULL)
    {
        return;
    }

    m_attributeGroupTypePtrsMap.setValue(pName, pAttributeGroup);
}

void CConfigSchemaHelper::addExtensionToBeProcessed(CExtension *pExtension)
{
    assert(pExtension != NULL);

    if (pExtension != NULL)
    {
        m_extensionArr.append(*pExtension);
    }
}

void CConfigSchemaHelper::addAttributeGroupToBeProcessed(CAttributeGroup *pAttributeGroup)
{
    assert(pAttributeGroup != NULL);

    if (pAttributeGroup != NULL)
    {
        m_attributeGroupArr.append(*pAttributeGroup);
    }
}

void CConfigSchemaHelper::processExtensionArr()
{
    int length = m_extensionArr.length();

    for (int idx = 0; idx < length; idx++)
    {
        CExtension &Extension = (m_extensionArr.item(idx));
        const char *pName = Extension.getBase();

        assert(pName != NULL);

        if (pName != NULL)
        {
            CXSDNodeBase *pNodeBase = NULL;

            pNodeBase = m_simpleTypePtrMap.getValue(pName) != NULL ? dynamic_cast<CSimpleType*>(*(m_simpleTypePtrMap.getValue(pName))) : NULL;

            if (pNodeBase == NULL)
            {
                pNodeBase = m_complexTypePtrsMap.getValue(pName) != NULL ? dynamic_cast<CComplexType*>(*(m_complexTypePtrsMap.getValue(pName))) : NULL ;
            }

            assert(pNodeBase != NULL);

            if (pNodeBase != NULL)
            {
                Extension.setBaseNode(pNodeBase);
            }
        }
    }
}

void CConfigSchemaHelper::processAttributeGroupArr()
{
    int length = m_attributeGroupArr.length();

    for (int idx = 0; idx < length; idx++)
    {
        CAttributeGroup &AttributeGroup = (m_attributeGroupArr.item(idx));
        const char *pRef = AttributeGroup.getRef();

        assert(pRef != NULL && pRef[0] != 0);

        if (pRef != NULL && pRef[0] != 0)
        {
            CAttributeGroup *pAttributeGroup = NULL;

            pAttributeGroup = *(m_attributeGroupTypePtrsMap.getValue(pRef));

            assert(pAttributeGroup != NULL);

            if (pAttributeGroup != NULL)
            {
                AttributeGroup.setRefNode(pAttributeGroup);
            }
        }
    }

}

void CConfigSchemaHelper::traverseAndProcessArray(const char *pXSDName)
{
    const char *pComponent = NULL;
    CSchema* pSchema = NULL;
    LOOP_THRU_BUILD_SET
    {
        if (pComponent == NULL || strcmp(pComponent, m_buildSetArray.item(idx).getSchema()) == 0)
        {
            const char* pXSDSchema = m_buildSetArray.item(idx).getSchema();

            if (pXSDSchema == NULL || (pXSDName != NULL && strcmp(pXSDName, pXSDSchema) != 0))
            {
                continue;
            }

            pSchema = m_schemaMap.getValue(m_buildSetArray.item(idx).getSchema());

            if (pSchema != NULL)
            {
                //strXML.append(pSchema->dump(std::cout));
                pSchema->traverseAndProcessNodes();
            }
        }
    }
}


void CConfigSchemaHelper::setBuildSetArray(const StringArray &strArray)
{
    m_buildSetArray.kill();

    //Owned<CBuildSet> pBSet = new CBuildSet((NULL, strArray.item(0), NULL, strArray.item(0)));
    Owned<CBuildSet> pBSet = new CBuildSet(NULL, strArray.item(0), NULL, strArray.item(0));

    assert (pBSet != NULL);

    m_buildSetArray.append(*pBSet.getClear());
}
