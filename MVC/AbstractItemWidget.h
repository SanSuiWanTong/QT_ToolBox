#ifndef ABSTRACTITEMWIDGET_H
#define ABSTRACTITEMWIDGET_H
#include <QWidget>
#include <QScrollArea>
#include "./MVC/AbstractItemModel.h"

const int ExtraHeight = 5;
const int ExtraWidth = 5;
const int Invalid = -1;
enum {Year, Males, Females, Total};

class AbstractItemWidget;

class VisualizerHeader : public QWidget
{
    Q_OBJECT

public:
    explicit VisualizerHeader(QWidget *parent=0) : QWidget(parent) {}

    QSize minimumSizeHint() const;
    QSize sizeHint() const { return minimumSizeHint(); }

protected:
    void paintEvent(QPaintEvent *event);

private:
    void paintHeader(QPainter *painter, const int RowHeight);
    void paintHeaderItem(QPainter *painter, const QRect &rect,
                         const QString &text, bool selected);
};

class VisualizerView : public QWidget
{
    Q_OBJECT

public:
    explicit VisualizerView(QWidget *parent=0);

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

signals:
    void clicked(const QModelIndex&);

protected:
    bool eventFilter(QObject *target, QEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void paintEvent(QPaintEvent *event);

private:
    void paintRow(QPainter *painter, int row,int y,const int RowHeight);
    void paintItemBackground(QPainter *painter, const QRect &rect, bool selected);
    void paintYear(QPainter *painter, int row, const QRect &rect);
    void paintMaleFemale(QPainter *painter, int row, const QRect &rect);
    void paintTotal(QPainter *painter, int row, const QRect &rect);
    QLinearGradient maleFemaleGradient(qreal x1, qreal y1,qreal x2, qreal y2,
                                       qreal crossOver);
    AbstractItemWidget *visualizer;
};

class AbstractItemWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AbstractItemWidget(QWidget*parent=0);
    ~AbstractItemWidget();
    QAbstractItemModel *model() const { return m_model; }
    void setModel(QAbstractItemModel *model);

    QScrollArea *scrollArea()   const { return m_scrollArea; }

    int maximumPopulation()     const { return m_maximumPopulation; }
    int widthOfYearColumn()     const { return m_widthOfYearColumn; }
    int widthOfMaleFemaleColumn() const;
    int widthOfTotalColumn()    const { return m_widthOfTotalColumn; }

    int selectedRow()           const { return m_selectedRow; }
    void setSelectedRow(int row);
    int selectedColumn()        const { return m_selectedColumn; }
    void setSelectedColumn(int column);

    void paintItemBorder(QPainter *painter, const QPalette &palette, const QRect &rect);
    QString maleFemaleHeaderText()  const;
    int maleFemaleHeaderTextWidth() const;
    int xOffsetForMiddleOfColumn(int column) const;
    int yOffsetForRow(int row) const;

public slots:
    void setCurrentIndex(const QModelIndex &index);

signals:
    void clicked(const QModelIndex&);

private:
    QAbstractItemModel *m_model;
    QScrollArea *m_scrollArea;

    VisualizerHeader *header;
    VisualizerView   *view;

    int m_widthOfYearColumn;
    int m_widthOfTotalColumn;
    int m_selectedRow;
    int m_selectedColumn;
    int m_maximumPopulation;
};

#endif // ABSTRACTITEMWIDGET_H
