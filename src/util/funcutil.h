#ifndef FUNCUTIL_H
#define FUNCUTIL_H

#include <QString>

class QNetworkAccessManager;

QString numberConvert(int size, const QString &suffix, int prec = 3, int limit = 850);

void openFileInFolder(const QString &filePath);

#endif // FUNCUTIL_H
