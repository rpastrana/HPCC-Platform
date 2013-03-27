#ifndef _CONFIG_SCHEMA_HELPER_HPP_
#define _CONFIG_SCHEMA_HELPER_HPP_

#include "jiface.hpp"
#include "jutil.hpp"
#include "jarray.hpp"
#include "jhash.hpp"
#include "SchemaAttributes.hpp"
#include "SchemaElement.hpp"
#include "SchemaSchema.hpp"
#include "BuildSet.hpp"

class CConfigSchemaHelper : public CInterface
{
public:

    IMPLEMENT_IINTERFACE

    static CConfigSchemaHelper* getInstance();

    virtual ~CConfigSchemaHelper();

    void getBuildSetComponents(StringArray& buildSetArray) const;
    //const CAttributeArray* getComponentAttributes(const char*);
    //const CAttributeGroupArray* getComponentAttributeGroups(const char*);
    bool populateBuildSet();
    //bool populateComponentAttributes();
    //bool populateComponentAttributeGroups();
    //bool populateElements();
    bool populateSchema();
    void printConfigSchema() const;

    CSchema* getSchemaForXSD(const char* pComponent)
    {
        return m_componentSchemaArrayMap.getValue(pComponent);
    }

    //test purposes
    bool getXMLFromSchema(StringBuffer& strXML, const char* pXSD);

    IPropertyTree* getSchema(const char* );

protected:

    CConfigSchemaHelper(const char* pBuildSetPath = DEFAULT_BUILD_SET_XML_PATH);

    Owned<IPropertyTree> m_buildSetTree;
    CIArrayOf<CBuildSet> m_buildSetArray; // buildset
    //MapStringToMyClass<CAttributeArray> m_componentAttributesArrayMap;
    //MapStringToMyClass<CAttributeGroupArray> m_componentAttributesGroupMap;
    //MapStringToMyClass<CElementArray> m_componentElementArrayMap;
    MapStringToMyClass<CSchema> m_componentSchemaArrayMap;
    StringBuffer m_buildSetPath;

private:
};

#endif // _CONFIG_SCHEMA_HELPER_HPP_
