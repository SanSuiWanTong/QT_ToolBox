#ifndef ABSTRACTITEMDELEGATE_H
#define ABSTRACTITEMDELEGATE_H

#include <QStyledItemDelegate>
#include <QCheckBox>
#include <QLabel>
#include <QTextDocument>
#include <QPushButton>

class ItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit ItemDelegate(QObject *parent=0);
    ~ItemDelegate();

    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const;
    void setEditorData(QWidget *editor,
                       const QModelIndex &index) const;
    void setModelData(QWidget *editor,
                      QAbstractItemModel *model,
                      const QModelIndex &index) const;
    bool editorEvent(QEvent *event, QAbstractItemModel *model,
                     const QStyleOptionViewItem &option,
                     const QModelIndex &index);

    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const;
    QWidget *createEditor(QWidget *parent,
                          const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;    

private slots:
    void closeAndCommitEditor();   

private:
    void paintWidget(QPainter *painter, const QRect &rect,
                     const QString &cacheKey, QWidget *widget) const;

    QCheckBox   *checkbox;
    QLabel      *label;
    QPushButton *pushButton;
    mutable QTextDocument document;
};



#endif // ABSTRACTITEMDELEGATE_H
