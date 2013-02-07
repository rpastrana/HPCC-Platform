project ( configurator )

set ( SRCS Main.cpp ConfigFileComponentUtils.cpp ConfigFileUtils.cpp ConfigFileUtilsObservable.cpp ConfigFileUtilsObserver.cpp EnvironmentConfiguration.cpp WizardBase.cpp ConfigSchemaHelper.cpp ComponentAttributes.cpp )

include_directories (
         ./../../system/include
         ./../../system/jlib
         ./../deploy/
)

ADD_DEFINITIONS ( -D_CONSOLE )

HPCC_ADD_EXECUTABLE ( configurator  ${SRCS} )
install ( TARGETS configurator DESTINATION sbin )
target_link_libraries ( configurator jlib )
