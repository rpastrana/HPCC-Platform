#include "ConfigSchemaHelper.hpp"
#include "SchemaAttributes.hpp"
#include "SchemaElement.hpp"
#include "jptree.hpp"
#include "XMLTags.h"
#include <cstring>

#define LOOP_THRU_BUILD_SET for (int idx = 0; idx < m_buildSetArray.length(); idx++)

CConfigSchemaHelper* CConfigSchemaHelper::getInstance()
{
    // not thread safe!!!

    static CConfigSchemaHelper* pCConfigSchemaHelper = NULL;

    if (pCConfigSchemaHelper == NULL)
    {
       pCConfigSchemaHelper = new CConfigSchemaHelper();
    }

    return pCConfigSchemaHelper;
}

CConfigSchemaHelper::CConfigSchemaHelper(const char* pBuildSetPath) : m_buildSetPath(pBuildSetPath)
{

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

    m_buildSetTree.set(createPTreeFromXMLFile(m_buildSetPath.str()));

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

/*bool CConfigSchemaHelper::populateComponentAttributes()
{
    LOOP_THRU_BUILD_SET
    {
        const char *pSchemaFile = m_buildSetArray.item(idx).getSchema();
        const char *pCompName = m_buildSetArray.item(idx).getName();

        CAttributeArray *pArray = CAttributeArray::load(pSchemaFile);
        m_componentAttributesArrayMap.setValue(pCompName, pArray);
    }

    return true;
}

bool CConfigSchemaHelper::populateComponentAttributeGroups()
{
    LOOP_THRU_BUILD_SET
    {
        const char *pSchemaFile = m_buildSetArray.item(idx).getSchema();
        const char *pCompName = m_buildSetArray.item(idx).getName();

        CAttributeGroupArray *pArrayGroup = CAttributeGroupArray::load(pSchemaFile);
        m_componentAttributesGroupMap.setValue(pCompName, pArrayGroup);
    }

    return true;
}

bool CConfigSchemaHelper::populateElements()
{
    LOOP_THRU_BUILD_SET
    {
        const char *pSchemaFile = m_buildSetArray.item(idx).getSchema();
        const char *pCompName = m_buildSetArray.item(idx).getName();

        if (pSchemaFile != NULL)
        {
            CElementArray *pElementArray = CElementArray::load(pSchemaFile);
            m_componentElementArrayMap.setValue(pCompName, pElementArray);
        }
    }

    return true;
}
*/
bool CConfigSchemaHelper::populateSchema()
{
    LOOP_THRU_BUILD_SET
    {
        const char *pSchemaFile = m_buildSetArray.item(idx).getSchema();
        const char *pCompName = m_buildSetArray.item(idx).getName();

        if (pSchemaFile != NULL)
        {
            CSchema *pSchema = CSchema::load(pSchemaFile, NULL);
            m_schemaMap.setValue(pSchemaFile, pSchema);
        }
    }

    return true;
}
/*
const CAttributeArray* CConfigSchemaHelper::getComponentAttributes(const char* pCompName)
{
    const CAttributeArray* ptr = (m_componentAttributesArrayMap.getValue(pCompName));
    return ptr;
}

const CAttributeGroupArray* CConfigSchemaHelper::getComponentAttributeGroups(const char* pCompName)
{
    const CAttributeGroupArray* ptr = (m_componentAttributesGroupMap.getValue(pCompName));
    return ptr;
}
*/
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
                //strXML.append(pSchema->dump(std::cout));
                pSchema->dump(std::cout);
            }
        }
    }
/*    CElementArray *pElemArray;

    LOOP_THRU_BUILD_SET
    {
        CElementArray *pElementArray = m_componentElementArrayMap.getValue(m_buildSetArray.item(idx).getSchema());
        //CAttributeGroupArray *pAttriubuteGroupArray

        if (pElementArray != NULL)
        {
            pElementArray->dump(std::cout);
        }
    }
    */
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
    //assert (pName != NULL);

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
    //assert (pName != NULL);

    if (pName == NULL || pComplexType == NULL)
    {
        return;
    }

    m_complexTypePtrsMap.setValue(pName, pComplexType);
}

void CConfigSchemaHelper::addExtensionToBeProcessed(CExtension *pExtension)
{
    assert(pExtension != NULL);

    if (pExtension != NULL)
    {
        m_extensionArray.append(*pExtension);
    }
}

void CConfigSchemaHelper::processExtensionArray()
{
    int length = m_extensionArray.length();

    for (int idx = 0; idx < length; idx++)
    {
        CExtension &Extension = (m_extensionArray.item(idx));
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
