#include "AbstractItemWidget.h"
#include <QPainter>
#include <QScrollBar>
#include <QVBoxLayout>
#include <QLocale>
#include <QResizeEvent>

//VisualizerView
VisualizerView::VisualizerView(QWidget *parent) : QWidget(parent)
{
    visualizer = qobject_cast<AbstractItemWidget*>(parent);
    Q_ASSERT(visualizer);
    setFocusPolicy(Qt::WheelFocus);
    setMinimumSize(minimumSizeHint());
}
QSize VisualizerView::minimumSizeHint() const
{
    return QSize(visualizer->widthOfYearColumn() +
                 visualizer->maleFemaleHeaderTextWidth() +
                 visualizer->widthOfTotalColumn(),
                 QFontMetrics(font()).height() + ExtraHeight);
}
QSize VisualizerView::sizeHint() const
{
    int rows = visualizer->model() ? visualizer->model()->rowCount() : 1;
    return QSize(visualizer->widthOfYearColumn() +
            qMax(100, visualizer->maleFemaleHeaderTextWidth()) +
            visualizer->widthOfTotalColumn(),
            visualizer->yOffsetForRow(rows));
}
bool VisualizerView::eventFilter(QObject *target, QEvent *event)
{
    if (QScrollArea *scrollArea = visualizer->scrollArea()) {
        if (target == scrollArea && event->type() == QEvent::Resize) {
            if (QResizeEvent *resizeEvent = static_cast<QResizeEvent*>(event)) {
                QSize size = resizeEvent->size();
                size.setHeight(sizeHint().height());
                int width = size.width() - (ExtraWidth + scrollArea->verticalScrollBar()->sizeHint().width());
                size.setWidth(width);
                resize(size);
            }
        }
    }
    return QWidget::eventFilter(target, event);
}
void VisualizerView::mousePressEvent(QMouseEvent *event)
{
    int row = static_cast<int>(event->y() / (QFontMetricsF(font()).height() + ExtraHeight));
    int column;
    if (event->x() < visualizer->widthOfYearColumn())
        column = Year;
    else if (event->x() < (visualizer->widthOfYearColumn() + visualizer->widthOfMaleFemaleColumn() / 2))
        column = Males;
    else if (event->x() < (visualizer->widthOfYearColumn() + visualizer->widthOfMaleFemaleColumn()))
        column = Females;
    else
        column = Total;
    visualizer->setSelectedRow(row);
    visualizer->setSelectedColumn(column);
    emit clicked(visualizer->model()->index(row, column));
}
void VisualizerView::keyPressEvent(QKeyEvent *event)
{
    if (visualizer->model()) {
        int row = Invalid;
        int column = Invalid;
        if (event->key() == Qt::Key_Left) {
            column = visualizer->selectedColumn();
            if (column == Males || column == Total)
                --column;
            else if (column == Females)
                column = Year;
        }
        else if (event->key() == Qt::Key_Right) {
            column = visualizer->selectedColumn();
            if (column == Year || column == Females)
                ++column;
            else if (column == Males)
                column = Total;
        }
        else if (event->key() == Qt::Key_Up)
            row = qMax(0, visualizer->selectedRow() - 1);
        else if (event->key() == Qt::Key_Down)
            row = qMin(visualizer->selectedRow() + 1,
                       visualizer->model()->rowCount() - 1);
        row = row == Invalid ? visualizer->selectedRow() : row;
        column = column == Invalid ? visualizer->selectedColumn()
                                   : column;
        if (row != visualizer->selectedRow() || column != visualizer->selectedColumn()) {
            QModelIndex index = visualizer->model()->index(row,column);
            visualizer->setCurrentIndex(index);
            emit clicked(index);
            return;
        }
    }
    QWidget::keyPressEvent(event);
}
void VisualizerView::paintEvent(QPaintEvent *event)
{
    if (!visualizer->model())
        return;
    QFontMetricsF fm(font());
    const int RowHeight = fm.height() + ExtraHeight;
    const int MinY = qMax(0, event->rect().y() - RowHeight);
    const int MaxY = MinY + event->rect().height() + RowHeight;

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing|
                           QPainter::TextAntialiasing);

    int row = MinY / RowHeight;
    int y = row * RowHeight;
    for (; row < visualizer->model()->rowCount(); ++row) {
        paintRow(&painter, row, y, RowHeight);
        y += RowHeight;
        if (y > MaxY)
            break;
    }
}
void VisualizerView::paintRow(QPainter *painter, int row, int y, const int RowHeight)
{
    paintYear(painter, row,
            QRect(0, y, visualizer->widthOfYearColumn(), RowHeight));
    paintMaleFemale(painter, row,
            QRect(visualizer->widthOfYearColumn(), y,
                  visualizer->widthOfMaleFemaleColumn(), RowHeight));
    paintTotal(painter, row,
            QRect(visualizer->widthOfYearColumn() +
                  visualizer->widthOfMaleFemaleColumn(), y,
                  visualizer->widthOfTotalColumn(), RowHeight));
}
void VisualizerView::paintYear(QPainter *painter, int row,
                                     const QRect &rect)
{
    paintItemBackground(painter, rect,
                        row == visualizer->selectedRow() &&
                        visualizer->selectedColumn() == Year);
    painter->drawText(rect,
            visualizer->model()->data(visualizer->model()->index(row, Year)).toString(),
            QTextOption(Qt::AlignCenter));
}
void VisualizerView::paintItemBackground(QPainter *painter,
        const QRect &rect, bool selected)
{
    painter->fillRect(rect, selected ? palette().highlight()
                                     : palette().base());
    visualizer->paintItemBorder(painter, palette(), rect);
    painter->setPen(selected ? palette().highlightedText().color()
                             : palette().windowText().color());
}
void VisualizerView::paintMaleFemale(QPainter *painter,int row, const QRect &rect)
{
    QRect rectangle(rect);
    QLocale locale;
    int males = locale.toInt(visualizer->model()->data(
            visualizer->model()->index(row, Males)).toString());
    int females = locale.toInt(visualizer->model()->data(
            visualizer->model()->index(row, Females)).toString());
    qreal total = males + females;
    int offset = qRound(
            ((1 - (total / visualizer->maximumPopulation())) / 2) *
             rectangle.width());

    painter->fillRect(rectangle,
            (row == visualizer->selectedRow() &&
             (visualizer->selectedColumn() == Females ||
              visualizer->selectedColumn() == Males))
            ? palette().highlight() : palette().base());

    visualizer->paintItemBorder(painter, palette(), rectangle);
    rectangle.setLeft(rectangle.left() + offset);
    rectangle.setRight(rectangle.right() - offset);
    int rectY = rectangle.center().y();
    painter->fillRect(rectangle.adjusted(0, 1, 0, -1),
            maleFemaleGradient(rectangle.left(), rectY,
                    rectangle.right(), rectY, males / total));
}
QLinearGradient VisualizerView::maleFemaleGradient(
        qreal x1, qreal y1, qreal x2, qreal y2, qreal crossOver)
{
    QLinearGradient gradient(x1, y1, x2, y2);
    QColor maleColor = Qt::green;
    QColor femaleColor = Qt::red;
    gradient.setColorAt(0, maleColor.darker());
    gradient.setColorAt(crossOver - 0.001, maleColor.lighter());
    gradient.setColorAt(crossOver + 0.001, femaleColor.lighter());
    gradient.setColorAt(1, femaleColor.darker());
    return gradient;
}
void VisualizerView::paintTotal(QPainter *painter, int row,const QRect &rect)
{
    paintItemBackground(painter, rect,
                        row == visualizer->selectedRow() &&
                        visualizer->selectedColumn() == Total);
 // allow right margin
    painter->drawText(rect.adjusted(0, 0, -5, 0),
            visualizer->model()->data(
                visualizer->model()->index(row, Total)).toString(),
            QTextOption(Qt::AlignVCenter|Qt::AlignRight));
}

//VisualizerHeader
QSize VisualizerHeader::minimumSizeHint() const
{
    AbstractItemWidget *visualizer = qobject_cast<AbstractItemWidget*>(parent());
    Q_ASSERT(visualizer);
    return QSize(visualizer->widthOfYearColumn() +
                 visualizer->maleFemaleHeaderTextWidth() +
                 visualizer->widthOfTotalColumn(),
                 QFontMetrics(font()).height() + ExtraHeight);
}
void VisualizerHeader::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing|QPainter::TextAntialiasing);
    paintHeader(&painter, height());
    painter.setPen(QPen(palette().button().color().darker(), 0.5));
    painter.drawRect(0, 0, width(), height());
}
void VisualizerHeader::paintHeader(QPainter *painter,const int RowHeight)
{
    const int Padding = 2;

    AbstractItemWidget *visualizer = qobject_cast<AbstractItemWidget*>(parent());
    Q_ASSERT(visualizer);
    paintHeaderItem(painter,
            QRect(0, 0, visualizer->widthOfYearColumn() + Padding,RowHeight),
            visualizer->model()->headerData(Year, Qt::Horizontal).toString(),
            visualizer->selectedColumn() == Year);

    paintHeaderItem(painter,
            QRect(visualizer->widthOfYearColumn() + Padding, 0,
                  visualizer->widthOfMaleFemaleColumn(), RowHeight),
            visualizer->maleFemaleHeaderText(),
            visualizer->selectedColumn() == Males || visualizer->selectedColumn() == Females);
    paintHeaderItem(painter,
            QRect(visualizer->widthOfYearColumn() + Padding +
                  visualizer->widthOfMaleFemaleColumn(),
                  0, visualizer->widthOfTotalColumn(), RowHeight),
            visualizer->model()->headerData(Total, Qt::Horizontal).toString(),
            visualizer->selectedColumn() == Total);
}
void VisualizerHeader::paintHeaderItem(QPainter *painter,
        const QRect &rect, const QString &text, bool selected)
{
    AbstractItemWidget *visualizer = qobject_cast<AbstractItemWidget*>(parent());
    Q_ASSERT(visualizer);
    int x = rect.center().x();
    QLinearGradient gradient(x, rect.top(), x, rect.bottom());
    QColor color = selected ? palette().highlight().color()
                            : palette().button().color();
    gradient.setColorAt(0,   color.darker(125));
    gradient.setColorAt(0.5, color.lighter(125));
    gradient.setColorAt(1,   color.darker(125));
    painter->fillRect(rect, gradient);
    visualizer->paintItemBorder(painter, palette(), rect);
    painter->setPen(selected ? palette().highlightedText().color()
                             : palette().buttonText().color());
    painter->drawText(rect, text, QTextOption(Qt::AlignCenter));
}

//AbstractItemWidget
AbstractItemWidget::AbstractItemWidget(QWidget *parent):QWidget(parent), m_model(0), m_selectedRow(Invalid),
    m_selectedColumn(Invalid), m_maximumPopulation(Invalid)
{
  QFontMetrics fm(font());
  m_widthOfYearColumn = fm.horizontalAdvance("W9999W");
  m_widthOfTotalColumn = fm.horizontalAdvance("W9,999,999W");

  view         = new VisualizerView(this);
  header       = new VisualizerHeader(this);
  m_scrollArea = new QScrollArea;

  m_scrollArea->setBackgroundRole(QPalette::Light);
  m_scrollArea->setWidget(view);
  m_scrollArea->installEventFilter(view);

  QVBoxLayout *layout = new QVBoxLayout;
  layout->addWidget(header);
  layout->addWidget(m_scrollArea);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);
  setLayout(layout);

  this->setModel(new AbstractItemModel());

  //signal - signal
  connect(view, SIGNAL(clicked(const QModelIndex&)),
          this, SIGNAL(clicked(const QModelIndex&)));
}
AbstractItemWidget::~AbstractItemWidget()
{

}
void AbstractItemWidget::setModel(QAbstractItemModel *model)
{
    if (model) {
        QLocale locale;
        bool ok;
        for (int row = 0; row < model->rowCount(); ++row) {
            int total = locale.toInt(model->data(model->index(row, Total)).toString(),&ok);
            if (ok && total > m_maximumPopulation)
                m_maximumPopulation = total;
        }
        QString population = QString::number(m_maximumPopulation);
        population = QString("%1%2")
                .arg(population.left(1).toInt() + 1)
                .arg(QString(population.length() - 1, QChar('0')));//8666-9000
        m_maximumPopulation = population.toInt();
        QFontMetrics fm(font());
        m_widthOfTotalColumn = fm.horizontalAdvance(QString("W%1%2W")
                                                    .arg(population)
                                                    .arg(QString(population.length() / 3, ',')));
    }
    m_model = model;
    header->update();
    view->update();
}

int AbstractItemWidget::widthOfMaleFemaleColumn() const
{
    return width() - (m_widthOfYearColumn + m_widthOfTotalColumn + ExtraWidth +
                      m_scrollArea->verticalScrollBar()->sizeHint().width());
}
void AbstractItemWidget::setSelectedRow(int row)
{
    m_selectedRow = row;
    view->update();
}
void AbstractItemWidget::setSelectedColumn(int column)
{
    m_selectedColumn = column;
    header->update();
}
void AbstractItemWidget::setCurrentIndex(const QModelIndex &index)
{
    setSelectedRow(index.row());
    setSelectedColumn(index.column());
    int x = xOffsetForMiddleOfColumn(index.column());
    int y = yOffsetForRow(index.row());
    m_scrollArea->ensureVisible(x, y, 10, 20);
}
int AbstractItemWidget::xOffsetForMiddleOfColumn(int column) const
{
    switch (column) {
        case Year: return widthOfYearColumn() / 2;
        case Males: return widthOfYearColumn() + (widthOfMaleFemaleColumn() / 4);
        case Females: return widthOfYearColumn() + ((widthOfMaleFemaleColumn() * 4) / 3);
        default: return widthOfYearColumn() +
                        widthOfMaleFemaleColumn() +
                        (widthOfTotalColumn() / 2);
    }
}
int AbstractItemWidget::yOffsetForRow(int row) const
{
    return static_cast<int>((QFontMetricsF(font()).height()
                             + ExtraHeight) * row);
}
void AbstractItemWidget::paintItemBorder(QPainter *painter,
        const QPalette &palette, const QRect &rect)
{
    painter->setPen(QPen(palette.button().color().darker(), 0.33));
    painter->drawLine(rect.bottomLeft(), rect.bottomRight());
    painter->drawLine(rect.bottomRight(), rect.topRight());
}
int AbstractItemWidget::maleFemaleHeaderTextWidth() const
{
    return QFontMetrics(font()).horizontalAdvance(maleFemaleHeaderText());
}
QString AbstractItemWidget::maleFemaleHeaderText() const
{
    if (!m_model)
        return " - ";
    return QString("%1 - %2")
           .arg(m_model->headerData(Males, Qt::Horizontal).toString())
           .arg(m_model->headerData(Females, Qt::Horizontal).toString());
}
