#include "ChatItemDelegate.h"

ChatItemDelegate::ChatItemDelegate(QAbstractItemView* view, QObject* parent):QItemDelegate(parent){}

QWidget* ChatItemDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	return nullptr;
}

QSize ChatItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	return QItemDelegate::sizeHint(option, index);
}
