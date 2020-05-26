#include "AbstractItemDelegate.h"
#include "3rdparty/richtextlineedit.hpp"
#include <QPixmapCache>
#include <QModelIndex>
#include <QPainter>
#include <QMouseEvent>
#include <QApplication>
#include <QToolTip>
#include <QPen>
#include <QDebug>

ItemDelegate::ItemDelegate(QObject *parent):QStyledItemDelegate(parent)
{
    checkbox = new QCheckBox;
    checkbox->setFixedSize(
            qRound(1.3 * checkbox->sizeHint().height()),
            checkbox->sizeHint().height());             //(width,height)preventing it from ever growing or shrinking

    label = new QLabel;
    label->setTextFormat(Qt::AutoText);
    label->setWordWrap(false);

    pushButton = new QPushButton("&Download");
}

ItemDelegate::~ItemDelegate()
{
    delete checkbox;
    delete label;
    delete pushButton;
}

void ItemDelegate::paint(QPainter *painter,
        const QStyleOptionViewItem &option,
        const QModelIndex &index) const
{
    bool selected = option.state & QStyle::State_Selected;
    QPalette palette(option.palette);
    palette.setColor(QPalette::Active, QPalette::Window,
                     selected ? option.palette.highlight().color()
                              : option.palette.base().color());
    palette.setColor(QPalette::Active, QPalette::WindowText,
                     selected ? option.palette.highlightedText().color()
                              : option.palette.text().color());
    //checkbox 垂直居中
    int yOffset = checkbox->height() < option.rect.height() ? (option.rect.height() - checkbox->height()) / 2 : 0;

    QRect checkboxRect(option.rect.x(), option.rect.y() + yOffset, checkbox->width(), checkbox->height());
    checkbox->setPalette(palette);

    bool checked = index.model()->data(index, Qt::CheckStateRole).toInt() == Qt::Checked;
    checkbox->setChecked(checked);

    //label
    int xOffset = option.rect.x() + checkbox->width();
    int tWidth  = option.rect.width() - checkbox->width();
    QRect labelRect(xOffset, option.rect.y(), tWidth,option.rect.height());
    label->setPalette(palette);
    label->setFixedSize(qMax(0, labelRect.width()),labelRect.height());
    QString str = index.model()->data(index, Qt::DisplayRole).toString();
    label->setText(str);    

    //pushButton
    xOffset = xOffset + label->width()- pushButton->width();
    QRect buttonRect(xOffset, option.rect.y(), pushButton->width(),option.rect.height());
    pushButton->setPalette(palette);

    //
    QString checkboxKey = QString("CHECKBOX:%1.%2").arg(selected).arg(checked);
    paintWidget(painter, checkboxRect, checkboxKey, checkbox);

    QString labelKey = QString("LABEL:%1.%2.%3x%4").arg(selected).arg(str)
                                                   .arg(labelRect.width()).arg(labelRect.height());
    QString buttonKey = QString("BUTTON:%1.%2.%3x%4").arg(selected).arg(str)
                                                   .arg(buttonRect.width()).arg(buttonRect.height());

    paintWidget(painter, labelRect, labelKey, label);
    paintWidget(painter, buttonRect, buttonKey, pushButton);
    painter->setPen(QPen(Qt::blue,1));
    painter->drawRect(buttonRect);
}

void ItemDelegate::paintWidget(QPainter *painter,const QRect &rect,
                               const QString &cacheKey, QWidget *widget) const
{
    QPixmap pm(widget->size());

    if (!QPixmapCache::find(cacheKey, &pm)) {
        widget->render(&pm);
        QPixmapCache::insert(cacheKey, pm);
    }
    painter->drawPixmap(rect, pm);
}

QSize ItemDelegate::sizeHint(const QStyleOptionViewItem &option,
                             const QModelIndex &index) const
{
    QString str = index.model()->data(index, Qt::DisplayRole).toString();
    document.setDefaultFont(option.font);
    document.setHtml(str);
    return QSize(document.idealWidth(), option.fontMetrics.height());
}

QWidget *ItemDelegate::createEditor(QWidget *parent,const QStyleOptionViewItem &option, const QModelIndex&) const
{
    RichTextLineEdit *editor = new RichTextLineEdit(parent);
    editor->viewport()->setFixedHeight(option.rect.height());
    connect(editor, SIGNAL(returnPressed()),
            this, SLOT(closeAndCommitEditor()));

    return editor;
}

void ItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QString html = index.model()->data(index, Qt::DisplayRole).toString();
    RichTextLineEdit *lineEdit = qobject_cast<RichTextLineEdit*>(editor);
    Q_ASSERT(lineEdit);
    lineEdit->setHtml(html);
}


void ItemDelegate::setModelData(QWidget *editor,QAbstractItemModel *model, const QModelIndex &index) const
{
    RichTextLineEdit *lineEdit = qobject_cast<RichTextLineEdit*>(editor);
    Q_ASSERT(lineEdit);
    model->setData(index, lineEdit->toSimpleHtml());
}


void ItemDelegate::closeAndCommitEditor()
{
    RichTextLineEdit *lineEdit = qobject_cast<RichTextLineEdit*>(sender());
    Q_ASSERT(lineEdit);
    emit commitData(lineEdit);
    emit closeEditor(lineEdit);
}

bool ItemDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    int xOffset = option.rect.x() + checkbox->width()+ label->width()- pushButton->width();
    QRect decorationRect = QRect(xOffset, option.rect.y(), pushButton->width(),option.rect.height());

    QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
    
    if (event->type() == QEvent::MouseButtonPress && decorationRect.contains(mouseEvent->pos()))
    {
        QString str = index.model()->data(index, Qt::DisplayRole).toString();
        qDebug()<<"button clicked:"<<str;
    }

    return QStyledItemDelegate::editorEvent(event, model, option, index);
}


