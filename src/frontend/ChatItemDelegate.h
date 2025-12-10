#ifndef CHATITEMDELEGATE_H
#define CHATITEMDELEGATE_H

#include <QItemDelegate>
#include <QAbstractItemView>
#include <QDateTime>

enum class MessageType{ DataRole = Qt::UserRole, SenderRole };
Q_DECLARE_METATYPE(MessageType)

struct ChatItemData {
	QString sender;
	QString message;
	QDateTime timestamp;
	// Add other relevant fields
};

class ChatItemDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    ChatItemDelegate(QAbstractItemView* view, QObject* parent = nullptr);
	~ChatItemDelegate() override = default;

	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

	// for setting row size
	QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};

#endif // CHATITEMDELEGATE_H
