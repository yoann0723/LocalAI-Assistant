#ifndef CHATLISTMODEL_H
#define CHATLISTMODEL_H

#include <QAbstractListModel>

class ChatListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit ChatListModel(QObject *parent = nullptr);
};

#endif // CHATLISTMODEL_H
