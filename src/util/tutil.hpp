#include <QVariant>

template<typename Container, typename ... list>
inline QVariant value(const Container& c, const QString& s)
{
    return c.toMap().value(s);
}

template<typename Container, typename ... list>
inline QVariant value(const Container& c, const QString& s, const list&... Args)
{
    return value(c.toMap().value(s), Args...);
}

template<typename Container>
inline bool contains(const Container& c, const QString& s)
{
    return c.toMap().contains(s);
}

template<typename Container>
inline void clearQObjects(Container& c)
{
    for(auto &&object : c){
        object->setParent(nullptr);
        object->deleteLater();
    }
    c.clear();
}
