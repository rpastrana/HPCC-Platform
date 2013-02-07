#ifndef _COMPONENT_ATTRIBUTES_HPP_
#define _COMPONENT_ATTRIBUTES_HPP_

#include "jhash.hpp"
#include "jatomic.hpp"
#include "XMLTags.h"

class CAttribute : public CInterface
{
public:

    IMPLEMENT_IINTERFACE

    CAttribute()
    {
    }

    CAttribute(const char* pName) : m_strName(pName)
    {
    }

    CAttribute(const char* pName, const char* pType, const char* pDefault, const char* pUse) : m_strName(pName), m_strType(pType), m_strDefault(pDefault), m_strUse(pUse)
    {
    }

    virtual ~CAttribute()
    {
    }

    const char* getName() const
    {
        return m_strName.str();
    }

    void setName(const char* pName)
    {
        m_strName.clear().append(pName);
    }

    const char* getType() const
    {
        return m_strType.str();
    }

    void setType(const char* pType)
    {
        m_strType.clear().append(pType);
    }

    const char* getDefault() const
    {
        return m_strDefault.str();
    }

    void setDefault(const char* pDefault)
    {
        m_strDefault.clear().append(pDefault);
    }

    const char* getUse() const
    {
        return m_strUse.str();
    }

    void setUse(const char* pUse)
    {
        m_strUse.clear().append(pUse);
    }


protected:

    StringBuffer m_strName;
    StringBuffer m_strType;
    StringBuffer m_strDefault;
    StringBuffer m_strUse;
};



class CComponentAttributeArray : public CIArrayOf<CAttribute>, public IInterface
{
public:

    CComponentAttributeArray()
    {
        atomic_set(&xxcount, 1);
    }

    virtual ~CComponentAttributeArray()
    {
    }

    virtual void Link() const
    {
         atomic_inc(&xxcount);
    }

    virtual bool Release() const
    {
        if (atomic_dec_and_test(&xxcount))
        {
            delete this;
            return true;
        }
        return false;
    }

protected:

private:

    mutable atomic_t xxcount;
};


class CComponentAttributes : public CInterface
{
public:

    IMPLEMENT_IINTERFACE

    CComponentAttributes();
    virtual ~CComponentAttributes();

    const CComponentAttributeArray* getAttribArray(const char* pCompName) const;

protected:

    virtual CComponentAttributeArray* loadComponentAttributes(const char* pSchemaFile, const char* pXPath = NULL);

private:
};


class CComponentAttributeGroups : public CInterface
{
public:

    IMPLEMENT_IINTERFACE

    CComponentAttributeGroups();
    virtual ~CComponentAttributeGroups();

    const CComponentAttributeArray* getAttribArray(const char* pCompName) const;

protected:

    CComponentAttributeArray* loadComponentAttributeGroup(const char* pSchemaFile);

private:
};

#endif // _COMPONENT_ATTRIBUTES_HPP_
