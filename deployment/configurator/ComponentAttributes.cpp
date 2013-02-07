#include "ComponentAttributes.hpp"
#include "jptree.hpp"

const char* DEFAULT_SCHEMA_DIRECTORY("/opt/HPCCSystems/componentfiles/configxml/");
const char* DEFAULT_COMPONENT_ATTRIBUTE_XPATH("./xs:element/xs:complexType/xs:attribute");

CComponentAttributes::CComponentAttributes()
{
}

CComponentAttributes::~CComponentAttributes()
{
}

CComponentAttributeArray* CComponentAttributes::loadComponentAttributes(const char* pSchemaFile, const char* pXPath)
{
    if (pSchemaFile == NULL)
    {
        return false;
    }
    else
    {
        Linked<IPropertyTree> pSchemaRoot;

        StringBuffer schemaPath;
        schemaPath.appendf("%s%s", DEFAULT_SCHEMA_DIRECTORY, pSchemaFile);
        pSchemaRoot.setown(createPTreeFromXMLFile(schemaPath.str()));

        Owned<IPropertyTreeIterator> iter = pSchemaRoot->getElements( pXPath ? pXPath : DEFAULT_COMPONENT_ATTRIBUTE_XPATH );

        CComponentAttributeArray *pAttribArray = new CComponentAttributeArray();

        ForEach(*iter)
        {
          IPropertyTree &schemaElement = iter->query();

          Owned<IAttributeIterator> iterAttrib = schemaElement.getAttributes(true);

          CAttribute *pAttribute = new CAttribute();

          // Find the name of this xs:attribute node
          ForEach(*iterAttrib)
          {
              if (strcmp(iterAttrib->queryName(), XML_ATTR_NAME) == 0)
              {
                  pAttribute->setName(iterAttrib->queryValue());
              }
              else if (strcmp(iterAttrib->queryName(), XML_ATTR_TYPE) == 0)
              {
                  pAttribute->setType(iterAttrib->queryValue());
              }
              else if (strcmp(iterAttrib->queryName(), XML_ATTR_DEFAULT) == 0)
              {
                  pAttribute->setDefault(iterAttrib->queryValue());
              }
              else if (strcmp(iterAttrib->queryName(), XML_ATTR_USE) == 0)
              {
                  pAttribute->setUse(iterAttrib->queryValue());
              }
          }

          if (pAttribute != NULL)
          {
              pAttribArray->append(*pAttribute);
          }
        }

        return pAttribArray;
    }
    return NULL;
}
