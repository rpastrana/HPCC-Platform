#include "EnvironmentConfiguration.hpp"
#include "WizardBase.hpp"
#include "ConfigSchemaHelper.hpp"
#include "SchemaCommon.hpp"
#include "NodeHandlerDocumentation.hpp"
#include <iostream>


CIArrayOf<CXSDNodeHandler> CXSDNodeBase::s_callBackEntryHandlersArray;
CIArrayOf<CXSDNodeHandler> CXSDNodeBase::s_callBackExitHandlersArray;

int main(int argc, char *argv[])
{
    InitModuleObjects();

    StringArray buildSetCompArray;

    CWizardBase baseWiz;

    CEnvironmentConfiguration *ptr = CEnvironmentConfiguration::getInstance();
    baseWiz.generate(ptr);

    CConfigSchemaHelper &schemaHelper = *(CConfigSchemaHelper::getInstance());

    schemaHelper.populateBuildSet();
    schemaHelper.getBuildSetComponents(buildSetCompArray);
    schemaHelper.populateSchema();
    //schemaHelper.printConfigSchema();
    schemaHelper.processExtensionArray();

    StringBuffer strXML;

/*    if (schemaHelper.getXMLFromSchema(strXML, NULL) == true)
    {
        std::cout << strXML.str();
    }
*/

    if (argc == 1)
    {
        StringBuffer str("dafilesrv.xsd");

        schemaHelper.printDocumentation(str);
        {
            std::cout << str.str();
        }

        str.clear().append("dali.xsd");
        schemaHelper.printDocumentation(str);
        {
            std::cout << str.str();
        }
    }
    else
    {
        StringBuffer str(argv[1]);

        schemaHelper.printDocumentation(str);
        std::cout << str.str();
    }


    NodeHandlerDocumentation docHandler;

    CXSDNodeBase::addEntryHandler(docHandler);
    CXSDNodeBase::addExitHandler(docHandler);

    //schemaHelper.traverseAndProcessArray("dafilesrv.xsd");

    return 0;
}
