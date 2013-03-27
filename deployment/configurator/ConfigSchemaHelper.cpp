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
            CSchema *pSchema = CSchema::load(pSchemaFile);
            m_componentSchemaArrayMap.setValue(pSchemaFile, pSchema);
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
void CConfigSchemaHelper::printConfigSchema() const
{
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

            pSchema = m_componentSchemaArrayMap.getValue(m_buildSetArray.item(idx).getSchema());

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
