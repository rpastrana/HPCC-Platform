#include "ConfigSchemaHelper.hpp"
#include "jptree.hpp"
#include "XMLTags.h"
#include <cstring>

CBuildSet::CBuildSet(const CBuildSet& buildSet) : m_pInstallSet(buildSet.m_pInstallSet), m_pName(buildSet.m_pName), m_pProcessName(buildSet.m_pProcessName), m_pSchema(buildSet.m_pSchema)
{
}

CConfigSchemaHelper::CConfigSchemaHelper(const char* pBuildSetPath) : m_buildSetPath(pBuildSetPath)
{

}

CConfigSchemaHelper::~CConfigSchemaHelper()
{
}

void CConfigSchemaHelper::getBuildSetComponents(StringArray& buildSetArray) const
{
    for (int idx = 0; idx < m_buildSetArray.length(); idx++)
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

bool CConfigSchemaHelper::populateComponentAttributes()
{
    for (int idx = 0; idx < m_buildSetArray.length(); idx++)
    {
        const char *pSchemaFile = m_buildSetArray.item(idx).getSchema();
        const char *pCompName = m_buildSetArray.item(idx).getName();

        CComponentAttributeArray *pArray = loadComponentAttributes(pSchemaFile);
        m_componentSchemaAttributesArrayMap.setValue(pCompName, pArray);
    }

    return true;
}

bool CConfigSchemaHelper::populateAttributeGroups()
{
    return true;
}

const CComponentAttributeArray* CConfigSchemaHelper::getComponentAttributes(const char* pCompName)
{
    const CComponentAttributeArray* ptr = (m_componentSchemaAttributesArrayMap.getValue(pCompName));
    return ptr;
}
