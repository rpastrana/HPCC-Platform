project ( configurator )

set ( SRCS Main.cpp ConfigFileComponentUtils.cpp ConfigFileUtils.cpp ConfigFileUtilsObservable.cpp
      ConfigFileUtilsObserver.cpp EnvironmentConfiguration.cpp WizardBase.cpp ConfigSchemaHelper.cpp
      SchemaAttributes.cpp SchemaComplexType.cpp SchemaSequence.cpp SchemaElement.cpp
      SchemaAppInfo.cpp SchemaAnnotation.cpp SchemaComplexContent.cpp SchemaComplexType.cpp SchemaDocumentation.cpp
      SchemaExtension.cpp SchemaChoice.cpp SchemaInclude.cpp SchemaSchema.cpp SchemaSimpleType.cpp SchemaRestriction.cpp NodeHandlerDocumentation.cpp
      SchemaEnumeration.cpp DocumentationMarkup.hpp DojoJSMarkup.hpp )

include_directories (
         ./../../system/include
         ./../../system/jlib
         ./../deploy/
)

ADD_DEFINITIONS ( -D_CONSOLE )

HPCC_ADD_EXECUTABLE ( configurator  ${SRCS} )
install ( TARGETS configurator DESTINATION sbin )
target_link_libraries ( configurator jlib )
