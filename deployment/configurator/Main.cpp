#include "EnvironmentConfiguration.hpp"
#include "WizardBase.hpp"
#include "ConfigSchemaHelper.hpp"
#include "SchemaCommon.hpp"
#include "NodeHandlerDocumentation.hpp"
#include <iostream>
#include "jfile.hpp"

#define BUFF_SIZE 1024

CIArrayOf<CXSDNodeHandler>  CXSDNodeBase::s_callBackEntryHandlersArray;
CIArrayOf<CXSDNodeHandler>  CXSDNodeBase::s_callBackExitHandlersArray;

const char *pDefaultExt =  ".mod.xml";


void usage()
{
    std::cout << "configurator -f <build set file> -p <base dir path>" << std::endl;
    std::cout << "-f -file <build set file>         : buildset file name (required if base directory is specfied" << std::endl;
    std::cout << "-p -path <base dir path>          : base directory path (required if buildset file name is specified)" << std::endl;
    std::cout << "-x -xsd  <xsd file name>          : xsd file name (can be more than one)" << std::endl;
    std::cout << "-l -list                          : list available xsd files" << std::endl;
    std::cout << "-d -doc                           : generate docs" << std::endl;
    std::cout << "-a -all                           : genearate all docs" << std::endl;
    std::cout << "-e -extension  <file extension>   : write docs to files with extension (default " <<  pDefaultExt << ")" << std::endl;
    std::cout << "-t -target <target directory>     : directory to which to docs will be written. If not specified, then output will go to std::out" << std::endl;
    std::cout << "-u -use <schema xsd>              : use specified xsd schema instead of buildset file" << std::endl;
    std::cout << "-h -help                          : prints out this usuage" << std::endl;
}

int main(int argc, char *argv[])
{
    InitModuleObjects();

    int idx = 1;

    CConfigSchemaHelper *pSchemaHelper = NULL;

    char pBuildSetFile[BUFF_SIZE];
    char pBuildSetFileDir[BUFF_SIZE];
    char pTargetDocDir[BUFF_SIZE];
    char pTargetDocExt[BUFF_SIZE];
    char pOverrideSchema[BUFF_SIZE];

    memset(pBuildSetFile, 0, sizeof(pBuildSetFile));
    memset(pBuildSetFileDir, 0, sizeof(pBuildSetFileDir));
    memset(pTargetDocDir, 0, sizeof(pTargetDocDir));
    memset(pOverrideSchema, 0, sizeof(pOverrideSchema));

    strncpy(pTargetDocExt, pDefaultExt, sizeof(pTargetDocExt));


    bool bListXSDs = false;
    bool bGenAllDocs = false;
    bool bGenDocs = false;
    bool bWriteDocs = false;


    StringArray arrXSDs;

    if (argc == 1)
    {
        usage();
        return 0;
    }

    while(idx < argc)
    {
        if (stricmp(argv[idx], "-help") == 0 || stricmp(argv[idx], "-h") == 0)
        {
            usage();
            return 0;
        }
        else if (stricmp(argv[idx], "-file") == 0 || stricmp(argv[idx], "-f") == 0)
        {
            idx++;

            assert(argv[idx]);

            if (argv[idx] == NULL)
            {
                std::cout << "Missing file parameter!" << std::endl;
                return 0;
            }

            strncpy(pBuildSetFile, argv[idx], BUFF_SIZE);
        }
        else if (stricmp(argv[idx], "-path") == 0 || stricmp(argv[idx], "-p") == 0)
        {
            idx++;

            assert(argv[idx]);

            if (argv[idx] == NULL)
            {
                std::cout << "Missing path parameter!" << std::endl;
                return 0;
            }

            strncpy(pBuildSetFileDir, argv[idx], BUFF_SIZE);
        }
        else if (stricmp(argv[idx], "-xsd") == 0 || stricmp(argv[idx], "-x") == 0)
        {
            idx++;

            assert(argv[idx]);

            if (argv[idx] == NULL)
            {
                std::cout << "Missing XSD file!" << std::endl;
                return 0;
            }

            arrXSDs.append(argv[idx]);
        }
        else if (stricmp(argv[idx], "-list") == 0 || stricmp(argv[idx], "-l") == 0)
        {
            bListXSDs = true;
        }
        else if (stricmp(argv[idx], "-doc") == 0 || stricmp(argv[idx], "-d") == 0)
        {
            bGenDocs = true;
        }
        else if (stricmp(argv[idx], "-all") == 0 || stricmp(argv[idx], "-a") == 0)
        {
            bGenAllDocs = true;
        }
        else if (stricmp(argv[idx], "-target") == 0 || stricmp(argv[idx], "-t") == 0)
        {
            idx++;

            assert(argv[idx]);

            if (argv[idx] == NULL)
            {
                std::cout << "Missing target!" << std::endl;
                return 0;
            }

            strcpy(pTargetDocDir,argv[idx]);
        }
        else if (stricmp(argv[idx], "-extension") == 0 || stricmp(argv[idx], "-e") == 0)
        {
            idx++;

            assert(argv[idx]);

            if (argv[idx] == NULL)
            {
                std::cout << "Missing extension!" << std::endl;
                return 0;
            }

            if (argv[idx][0] != '.')
            {
                strcat(pTargetDocExt, ".");
                strcpy(&(pTargetDocExt[1]),argv[idx]);
            }
            else if (*pTargetDocExt == 0)
            {
                strcat(pTargetDocExt, "");
            }
            else
            {
                strcpy(pTargetDocExt,argv[idx]);
            }
        }
        else if (stricmp(argv[idx], "-use") == 0 | stricmp(argv[idx], "-u") == 0)
        {
            idx++;

            assert(argv[idx]);

            if (argv[idx] == NULL)
            {
                std::cout << "Missing schema xsd!" << std::endl;
                return 0;
            }
            else
            {
                strcpy(pOverrideSchema, argv[idx]);
                arrXSDs.append(argv[idx]);
            }
        }

        idx++;
    }

    if ( pBuildSetFile[0] != 0 ^ pBuildSetFileDir[0] != 0)
    {
        puts("-file and -path need to be both set or neither one!");
        return 0;
    }

    if ((bGenDocs == true || bGenAllDocs == true) && arrXSDs.length() == 0)
    {
        puts("No XSDs specified for doc generation!");
        return 0;
    }

    if (pBuildSetFile[0] == 0 && pOverrideSchema[0] == 0)
    {
        pSchemaHelper = CConfigSchemaHelper::getInstance();
    }
    else if (pBuildSetFile[0] == 0)
    {
        pSchemaHelper = CConfigSchemaHelper::getInstance("");
    }
    else
    {
        pSchemaHelper = CConfigSchemaHelper::getInstance(pBuildSetFile, pBuildSetFileDir);
    }

    assert(pSchemaHelper);

    if (pOverrideSchema[0] != 0)
    {
        static StringArray arrSchema;

        arrSchema.append(pOverrideSchema);
        pSchemaHelper->setBuildSetArray(arrSchema);
    }
    else if (pSchemaHelper->populateBuildSet() == false)
    {
        std::cout << "Failed to populate buildset.  Are the buildset file name and path set correctly?  Do they exist? Are permissions set properly?" << std::endl;
    }

    pSchemaHelper->populateSchema();
    pSchemaHelper->processExtensionArray();


    if (bListXSDs == true)
    {
        StringArray arrXSDs;

        pSchemaHelper->getBuildSetComponents(arrXSDs);

        if (arrXSDs.length() > 0)
        {
            std::cout << "XSD files (" << arrXSDs.length() << ")" << std::endl;
        }

        for (int idx = 0; idx < arrXSDs.length(); idx++)
        {
            std::cout << "(" << idx+1 << ") " << arrXSDs.item(idx) << ".xsd" << std::endl;
        }
    }

    for (int idx =  0; bGenDocs == true && idx < arrXSDs.length(); idx++)
    {
        if (pTargetDocDir[0] == 0)
        {
            std::cout << pSchemaHelper->printDocumentation(arrXSDs.item(idx));
        }
        else
        {
            Owned<IFile>   pFile;
            Owned<IFileIO> pFileIO;
            StringBuffer strTargetPath;

            const char *pXSDFile = strrchr(arrXSDs.item(idx), '/') == NULL ? arrXSDs.item(idx) : strrchr(arrXSDs.item(idx),'/');

            //strTargetPath.append(pTargetDocDir).append("/").append(arrXSDs.item(idx)).append(pTargetDocExt);
            strTargetPath.append(pTargetDocDir).append("/").append(pXSDFile).append(pTargetDocExt);

            pFile.setown(createIFile(strTargetPath.str()));
            pFileIO.setown(pFile->open(IFOcreaterw));

            const char *pDoc = pSchemaHelper->printDocumentation(arrXSDs.item(idx));

            if (pDoc == NULL)
            {
                continue;
            }

            pFileIO->write(0, strlen(pDoc), pDoc);
        }
    }
}

#ifdef false

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

#endif // false
