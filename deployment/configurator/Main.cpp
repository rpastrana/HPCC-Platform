#include "EnvironmentConfiguration.hpp"
#include "WizardBase.hpp"
#include "ConfigSchemaHelper.hpp"
#include <iostream>

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

    StringBuffer strXML;

/*    if (schemaHelper.getXMLFromSchema(strXML, NULL) == true)
    {
        std::cout << strXML.str();
    }
*/
    StringBuffer str;
    schemaHelper.printConfigSchema(str);
    {
        std::cout << str.str();
    }

    //std::cout << t
    return 0;
}
