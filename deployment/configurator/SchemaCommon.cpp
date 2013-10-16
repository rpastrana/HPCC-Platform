#include "SchemaCommon.hpp"

CXSDNodeBase::CXSDNodeBase(CXSDNodeBase* pParentNode, NODE_TYPES eNodeType) : m_pParentNode(pParentNode),  m_eNodeType(eNodeType)
{
   assert(eNodeType != XSD_ERROR);

   switch (eNodeType)
   {
   case(XSD_ERROR):
       strcpy(m_pNodeType, XSD_ERROR_STR);
       break;
   case(XSD_ANNOTATION):
       strcpy(m_pNodeType, XSD_ANNOTATION_STR);
       break;
   case(XSD_APP_INFO):
       strcpy(m_pNodeType, XSD_APP_INFO_STR);
       break;
   case(XSD_ATTRIBUTE):
       strcpy(m_pNodeType, XSD_ATTRIBUTE_STR);
       break;
   case(XSD_ATTRIBUTE_ARRAY):
       strcpy(m_pNodeType, XSD_ATTRIBUTE_ARRAY_STR);
       break;
   case(XSD_ATTRIBUTE_GROUP):
       strcpy(m_pNodeType, XSD_ATTRIBUTE_GROUP_STR);
       break;
   case(XSD_ATTRIBUTE_GROUP_ARRAY):
       strcpy(m_pNodeType, XSD_ATTRIBUTE_GROUP_ARRAY_STR);
       break;
   case(XSD_CHOICE):
       strcpy(m_pNodeType, XSD_CHOICE_STR);
       break;
   case(XSD_COMPLEX_CONTENT):
       strcpy(m_pNodeType, XSD_COMPLEX_CONTENT_STR);
       break;
   case(XSD_COMPLEX_TYPE):
       strcpy(m_pNodeType, XSD_COMPLEX_TYPE_STR);
       break;
   case(XSD_COMPLEX_TYPE_ARRAY):
       strcpy(m_pNodeType, XSD_COMPLEX_TYPE_ARRAY_STR);
       break;
   case(XSD_DOCUMENTATION):
       strcpy(m_pNodeType, XSD_DOCUMENTATION_STR);
       break;
   case(XSD_ELEMENT):
       strcpy(m_pNodeType, XSD_ELEMENT_STR);
       break;
   case(XSD_ELEMENT_ARRAY):
       strcpy(m_pNodeType, XSD_ELEMENT_ARRAY_STR);
       break;
   case(XSD_EXTENSION):
       strcpy(m_pNodeType, XSD_EXTENSION_STR);
       break;
   case(XSD_INCLUDE):
       strcpy(m_pNodeType, XSD_INCLUDE_STR);
       break;
   case(XSD_INCLUDE_ARRAY):
       strcpy(m_pNodeType, XSD_INCLUDE_ARRAY_STR);
       break;
   case(XSD_RESTRICTION):
       strcpy(m_pNodeType, XSD_RESTRICTION_STR);
       break;
   case(XSD_SCHEMA):
       strcpy(m_pNodeType, XSD_SCHEMA_STR);
       break;
   case(XSD_SEQUENCE):
       strcpy(m_pNodeType, XSD_SEQUENCE_STR);
       break;
   case(XSD_SIMPLE_TYPE):
       strcpy(m_pNodeType, XSD_SIMPLE_TYPE_STR);
       break;
   case(XSD_SIMPLE_TYPE_ARRAY):
       strcpy(m_pNodeType, XSD_SIMPLE_TYPE_ARRAY_STR);
       break;
   case(XSD_ENUMERATION):
       strcpy(m_pNodeType, XSD_ENUMERATION_STR);
       break;
   case(XSD_ENUMERATION_ARRAY):
       strcpy(m_pNodeType, XSD_ENUMERATION_ARRAY_STR);
       break;
   default:
       assert(false); // should never get here
       strcpy(m_pNodeType, XSD_ERROR_STR);
       break;
   }
}

CXSDNodeBase::~CXSDNodeBase()
{
}

void CXSDNodeBase::dumpStdOut() const
{
   dump(std::cout);
}

const CXSDNodeBase* CXSDNodeBase::getConstAncestorNode(unsigned iLevel) const
{
   CXSDNodeBase *pAncestorNode = const_cast<CXSDNodeBase*>(this);

   if (iLevel == 0)
   {
       return this;
   }

   do
   {
       pAncestorNode = const_cast<CXSDNodeBase*>(pAncestorNode->getConstParentNode());
       iLevel--;

   } while (iLevel > 0 && pAncestorNode != NULL);

   return pAncestorNode;
}

const CXSDNodeBase* CXSDNodeBase::getParentNodeByType(NODE_TYPES eNodeType, const CXSDNodeBase *pParent) const
{
   if (this->m_eNodeType == eNodeType && pParent != NULL)
   {
       return this;
   }

   if (this->getConstParentNode() != NULL)
   {
       return this->getConstParentNode()->getParentNodeByType(eNodeType, this);
   }

   return NULL;
}

const CXSDNodeBase* CXSDNodeBase::getNodeByTypeAndNameAscending(NODE_TYPES eNodeType, const char *pName) const
{
   const CXSDNodeBase* pMatchingNode = NULL;
   int len = 0;

   const CIArray *pThisArray = dynamic_cast<const CIArray*>(this);

   if (pThisArray!= NULL)
   {
       len = pThisArray->length()-1;
   }
   else
   {
       assert(dynamic_cast<const CIArray*>(this) != NULL);
       return NULL;
   }

   for (int idx = 0; idx < len; idx++)
   {

       const CXSDNodeBase* pNode = dynamic_cast<CXSDNodeBase*>(&(pThisArray->item(idx)));

       if (pNode == NULL)
       {
           return NULL;
       }

       pMatchingNode = pNode->getNodeByTypeAndNameDescending(eNodeType, pName);

       if (pMatchingNode != NULL)
       {
           return pMatchingNode;
       }

       pMatchingNode = pNode->getNodeByTypeAndNameAscending(eNodeType, pName);

       if (pMatchingNode != NULL)
       {
           return pMatchingNode;
       }
   }

   return NULL;  // nothing found
}

const CXSDNodeBase* CXSDNodeBase::getNodeByTypeAndNameDescending(NODE_TYPES eNodeType, const char *pName) const
{
   const CXSDNodeBase* pMatchingNode = NULL;
   int len = 0;

   const CIArray *pThisArray = dynamic_cast<const CIArray*>(this);

   if (pThisArray!= NULL)
   {
       len = pThisArray->length()-1;
   }
   else
   {
       assert(dynamic_cast<const CIArray*>(this) != NULL);
       return NULL;
   }

   for (int idx = 0; idx < len; idx++)
   {
       const CXSDNodeBase* pNode = dynamic_cast<const CXSDNodeBase*>(&(pThisArray->item(idx)));

       if (pNode == NULL)
       {
           return NULL;
       }

       pMatchingNode = pNode->getNodeByTypeAndNameDescending(eNodeType, pName);

       if (pMatchingNode != NULL)
       {
           return pMatchingNode;
       }
   }

   return NULL;  // nothing found
}

void CXSDNodeBase::processEntryHandlers(const CXSDNodeBase *pBase)
{
    assert(pBase != NULL);

    if (pBase == NULL)
    {
        return;
    }

    for (int idx = 0; idx < s_callBackEntryHandlersArray.length(); idx++ )
    {
        s_callBackEntryHandlersArray.item(idx).onEventEntry(pBase);
    }
}

void CXSDNodeBase::processExitHandlers(const CXSDNodeBase *pBase)
{
    assert(pBase != NULL);

    if (pBase == NULL)
    {
        return;
    }

    for (int idx = 0; idx < s_callBackExitHandlersArray.length(); idx++ )
    {
        s_callBackExitHandlersArray.item(idx).onEventExit(pBase);
    }
}


CXSDNode::CXSDNode(CXSDNodeBase *pParentNode, NODE_TYPES pNodeType) : CXSDNodeBase::CXSDNodeBase(pParentNode, pNodeType)
{
}


bool CXSDNode::checkSelf(NODE_TYPES eNodeType, const char *pName, const char* pCompName) const
{
  if (eNodeType & this->getNodeType() && (pName != NULL ? !strcmp(pName, this->getNodeTypeStr()) : true))
  {
      assert(pName != NULL); // for now pName should always be populated

      return this;
  }

  return NULL;
}


const CXSDNodeBase* CXSDNode::getParentNodeByType(NODE_TYPES eNodeType) const
{
  if (this->m_eNodeType == eNodeType)
  {
      return this;
  }

  if (this->getConstParentNode() != NULL)
  {
      return this->getConstParentNode()->getParentNodeByType(eNodeType);
  }

  return NULL;
}


const CXSDNodeBase* CXSDNode::getNodeByTypeAndNameAscending(NODE_TYPES eNodeType, const char *pName) const
{
  assert(this->m_eNodeType != eNodeType); //  Base functionality just blindly searchs upstream

  if (this->getConstParentNode() != NULL)
  {
      return this->getConstParentNode()->getNodeByTypeAndNameAscending(eNodeType, pName);
  }

  return NULL;
}

const CXSDNodeBase* CXSDNode::getNodeByTypeAndNameDescending(NODE_TYPES eNodeType, const char *pName) const
{
  assert(false);  // Derived classes need to hanldes this

  return NULL;
}
