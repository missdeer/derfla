#import <CoreServices/CoreServices.h>
#import <Foundation/Foundation.h>
#import <QString>
#import <QtCore/QtCore>
#import <sys/types.h>
#import <unistd.h>

bool QuickGetFilesByFileName(bool regexpEnabled, const QString &fileName,
                             QStringList &results,
                             const std::function<bool(bool)> &checker,
                             int maxCount) {
  QString queryString =
      QString("* == \"%1\"wcd || kMDItemFSName = \"%1\"c").arg(fileName);
  CFStringRef rawQuery = CFStringCreateWithCString(
      kCFAllocatorDefault, queryString.toStdString().c_str(),
      CFStringGetSystemEncoding());
  if (!rawQuery) {
    qDebug() << "creating CFString failed";
    return false;
  }

  MDQueryRef queryRef =
      MDQueryCreate(kCFAllocatorDefault, rawQuery, NULL, NULL);
  if (!queryRef) {
    qDebug() << "creating MDQuery failed";

    CFRelease(rawQuery);
    return false;
  }

  // go execute the query
  if (!MDQueryExecute(queryRef, kMDQuerySynchronous)) {
    qDebug() << "MDQueryExecute failed";

    CFRelease(rawQuery);
    CFRelease(queryRef);
    return false;
  }

  int count = MDQueryGetResultCount(queryRef);
  for (int i = 0; i < qMin(count, maxCount); i++) {
    MDItemRef item = (MDItemRef)MDQueryGetResultAtIndex(queryRef, i);
    NSString *name = (NSString *)MDItemCopyAttribute(item, kMDItemPath);
    results.append(QString::fromNSString(name));
    [name autorelease];
  }

  CFRelease(rawQuery);
  CFRelease(queryRef);

  return !results.isEmpty();
}
