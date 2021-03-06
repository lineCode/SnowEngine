#pragma once
// XmlResource.h : API to use load Xml files from the Resource Cache

#include "ResCache.h"
#include <tinyxml.h>


class XmlResourceExtraData : public IResourceExtraData
{
    TiXmlDocument m_xmlDocument;

public:
    virtual std::string VToString() { return "XmlResourceExtraData"; }
    void ParseXml(char* pRawBuffer);
    TiXmlElement* GetRoot(void) { return m_xmlDocument.RootElement(); }
};


class XmlResourceLoader : public IResourceLoader
{
public:
    virtual bool VUseRawFile() { return false; }
	virtual bool VDiscardRawBufferAfterLoad() { return true; }
    virtual unsigned int VGetLoadedResourceSize(char *rawBuffer, unsigned int rawSize) { return rawSize; }
    virtual bool VLoadResource(char *rawBuffer, unsigned int rawSize, shared_ptr<ResHandle> handle);
    virtual std::string VGetPattern() { return "*.xml"; }

    //根据资源名直接加载Xml
    static TiXmlElement* LoadAndReturnRootXmlElement(const char* resourceString);
};


