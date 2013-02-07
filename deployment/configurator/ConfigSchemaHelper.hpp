#ifndef _CONFIG_SCHEMA_HELPER_HPP_
#define _CONFIG_SCHEMA_HELPER_HPP_

#include "jiface.hpp"
#include "jutil.hpp"
#include "jarray.hpp"
#include "jhash.hpp"
#include "ComponentAttributes.hpp"

static const char* DEFAULT_BUILD_SET_XML_PATH("/opt/HPCCSystems/componentfiles/configxml/buildset.xml");

class CBuildSet : public CInterface
{
public:

    IMPLEMENT_IINTERFACE

    CBuildSet(const char* pInstallSet = NULL, const char* pName = NULL, const char* pProcessName = NULL, const char* pSchema = NULL) : m_pInstallSet(pInstallSet), m_pName(pName), m_pProcessName(pProcessName), m_pSchema(pSchema)
    {
    }

    virtual ~CBuildSet()
    {
    }

    const char* getInstallSet() const
    {
        return m_pInstallSet;
    }
    const char* getName() const
    {
        return m_pName;
    }
    const char* getProcessName() const
    {
        return m_pProcessName;
    }
    const char* getSchema() const
    {
        return m_pSchema;
    }

protected:

    CBuildSet();
    CBuildSet(const CBuildSet& buildSet);

    const char* m_pInstallSet;
    const char* m_pName;
    const char* m_pProcessName;
    const char* m_pSchema;

private:

};

class CConfigSchemaHelper : public CComponentAttributes
{
public:

    CConfigSchemaHelper(const char* pBuildSetPath = DEFAULT_BUILD_SET_XML_PATH);
    virtual ~CConfigSchemaHelper();

    void getBuildSetComponents(StringArray& buildSetArray) const;
    const CComponentAttributeArray* getComponentAttributes(const char*);
    bool populateBuildSet();
    bool populateComponentAttributes();
    bool populateAttributeGroups();

    IPropertyTree* getSchema(const char* );

protected:

    Owned<IPropertyTree> m_buildSetTree;
    CIArrayOf<CBuildSet> m_buildSetArray; // buildset
    MapStringToMyClass<CComponentAttributeArray> m_componentSchemaAttributesArrayMap;
    StringBuffer m_buildSetPath;

private:
};

#endif // _CONFIG_SCHEMA_HELPER_HPP_
