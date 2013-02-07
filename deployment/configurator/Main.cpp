#include "EnvironmentConfiguration.hpp"
#include "WizardBase.hpp"
#include "ConfigSchemaHelper.hpp"
#include <iostream>

#define REQUIRED_ATTRIBUTES_ONLY

int main(int argc, char *argv[])
{
    InitModuleObjects();

    StringArray buildSetCompArray;

    CWizardBase baseWiz;

    CEnvironmentConfiguration *ptr = CEnvironmentConfiguration::getInstance();
    baseWiz.generate(ptr);

    CConfigSchemaHelper schemaHelper;

    schemaHelper.populateBuildSet();

    schemaHelper.getBuildSetComponents(buildSetCompArray);
    schemaHelper.populateComponentAttributes();

    for (int idx = 0; idx < buildSetCompArray.length(); idx++)
    {
        std::cout << idx+1 << "] " << buildSetCompArray.item(idx) << "\n";

        const CComponentAttributeArray *pCompAttribsArray = (schemaHelper.getComponentAttributes(buildSetCompArray.item(idx)));

        if (pCompAttribsArray == NULL)
            continue;

#ifndef REQUIRED_ATTRIBUTES_ONLY

        std::cout << "------Attributes:------";

        for (int idx = 0; idx < pCompAttribsArray->length(); idx++)
        {
            std::cout << "\n name: '" << pCompAttribsArray->item(idx).getName() << "'\t\t\ttype: '" <<  pCompAttribsArray->item(idx).getType() << "'\tuse: '" <<  pCompAttribsArray->item(idx).getUse() << "'\tdefault: '" <<  pCompAttribsArray->item(idx).getDefault() << "'";
        }

#else // REQUIRED_ATTRIBUTES_ONLY

        std::cout << "\n\n------Required Attributes:------";

        for (int idx = 0; idx < pCompAttribsArray->length(); idx++)
        {
            if (strcmp(pCompAttribsArray->item(idx).getUse(), TAG_REQUIRED) == 0)
            {
                std::cout << "\n name: '" << pCompAttribsArray->item(idx).getName() << "'\t\ttype: '" <<  pCompAttribsArray->item(idx).getType() << "'\tuse: '" <<  pCompAttribsArray->item(idx).getUse() << "'\tdefault: '" <<  pCompAttribsArray->item(idx).getDefault() << "'";
            }
        }

#endif // REQUIRED_ATTRIBUTES_ONLY

        std::cout << std::endl << std::endl;;

    }
    return 0;
}
