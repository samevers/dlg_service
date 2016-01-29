#ifndef __MYCONFIGURATOR_USE_TINYXML_STL_H
#define __MYCONFIGURATOR_USE_TINYXML_STL_H

class TiXmlDocument;
class TiXmlElement;

class XmlConfig {
    TiXmlDocument *pConfigDoc;

    TiXmlElement *getLastElement(const char *path);

public:
    XmlConfig();
    ~XmlConfig();

public:
    bool open(const char *config);

    void close();

    bool readStr(const char *path, const char * &v);

    bool readInt(const char *path, int &n);

    bool readDouble(const char *path, double &f);
};

#endif
