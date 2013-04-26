#ifndef _BUILD_SET_HPP_
#define _BUILD_SET_HPP_

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

    CBuildSet(const CBuildSet& buildSet) : m_pInstallSet(buildSet.m_pInstallSet), m_pName(buildSet.m_pName), m_pProcessName(buildSet.m_pProcessName), m_pSchema(buildSet.m_pSchema)
    {
    }

    const char* m_pInstallSet;
    const char* m_pName;
    const char* m_pProcessName;
    const char* m_pSchema;

private:

};

#endif // _BUILD_SET_HPP_
