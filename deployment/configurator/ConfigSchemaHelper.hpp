#ifndef _CONFIG_SCHEMA_HELPER_HPP_
#define _CONFIG_SCHEMA_HELPER_HPP_

#include "jiface.hpp"
#include "jutil.hpp"
#include "jarray.hpp"
#include "jhash.hpp"
#include "SchemaAttributes.hpp"
#include "SchemaElement.hpp"
#include "SchemaSchema.hpp"
#include "SchemaExtension.hpp"
#include "BuildSet.hpp"

class CSimpleType;

class CConfigSchemaHelper : public CInterface
{
public:

    IMPLEMENT_IINTERFACE

    static CConfigSchemaHelper* getInstance(const char* pDefaultDirOverride =  NULL);
    static CConfigSchemaHelper* getInstance(const char* pBuildSetFileName, const char *pBaseDirectory, const char *pDefaultDirOverride = NULL);

    virtual ~CConfigSchemaHelper();

    void getBuildSetComponents(StringArray& buildSetArray) const;
    bool populateBuildSet();
    bool populateSchema();
    void printConfigSchema(StringBuffer &str) const;

    CSchema* getSchemaForXSD(const char* pComponent);

    void setSchemaForXSD(const char* pComponent, CSchema *pSchema);

    CSimpleType* getSimpleTypeWithName(const char* pName);
    void setSimpleTypeWithName(const char* pName, CSimpleType *pSimpleType);

    CComplexType* getComplexTypeWithName(const char* pName);
    void setComplexTypeWithName(const char* pName, CComplexType *pComplexType);

    CAttributeGroup *getAttributeGroup(const char* pName);
    void setAttributeGroupTypeWithName(const char* pName, CAttributeGroup *pAttributeGroup);

    void addExtensionToBeProcessed(CExtension *pExtension);
    void addAttributeGroupToBeProcessed(CAttributeGroup *pAttributeGroup);

    //test purposes
    bool getXMLFromSchema(StringBuffer& strXML, const char* pXSD);

    IPropertyTree* getSchema(const char* );

    void processExtensionArr();
    void processAttributeGroupArr();

    void traverseAndProcessArray(const char *pXSDName = NULL);

    const char* printDocumentation(const char* comp);

    void setBuildSetArray(const StringArray &strArray);


protected:

    CConfigSchemaHelper(const char* pBuildSetFile = DEFAULT_BUILD_SET_XML_FILE, const char* pBuildSetDir = DEFAULT_BUILD_SET_DIRECTORY, const char* pDefaultDirOverride = NULL);

    Owned<IPropertyTree> m_buildSetTree;
    CIArrayOf<CBuildSet> m_buildSetArray;
    MapStringToMyClass<CSchema> m_schemaMap;
    MapStringTo<CSimpleType*> m_simpleTypePtrMap;
    MapStringTo<CComplexType*> m_complexTypePtrsMap;
    MapStringTo<CAttributeGroup*> m_attributeGroupTypePtrsMap;
    CIArrayOf<CExtension> m_extensionArr;
    CIArrayOf<CAttributeGroup> m_attributeGroupArr;
    StringBuffer m_buildSetPath;

private:

    const char *m_pDefaultDirOverride;
    static CConfigSchemaHelper* s_pCConfigSchemaHelper;
};

#endif // _CONFIG_SCHEMA_HELPER_HPP_
