#ifndef QCHARWIDGET_H
#define QCHARWIDGET_H

#include <QtWidgets/QWidget>
#include <QtCharts/QChartGlobal>
#include <QMap>

QT_CHARTS_BEGIN_NAMESPACE
class QChartView;
class QChart;
QT_CHARTS_END_NAMESPACE

typedef QPair<qreal, qreal>     Data;
typedef QList<Data>             DataList;
typedef QMap<QString,DataList>  DataTable;

QT_CHARTS_USE_NAMESPACE

namespace Ui {
class ThemeWidgetForm;
}


class QcharWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QcharWidget(QWidget *parent = 0);
    virtual ~QcharWidget();


private Q_SLOTS:
    void updateUI();

private:
    void generateData();
    void populateThemeBox();
    void populateAnimationBox();
    void populateLegendBox();
    void connectSignals();
    void createCommonAxis(QChart *chart, QStringList* categories=0)   const;

    QChart *createAreaChart()              const;
    QChart *createBarChart()               const;
    QChart *createPieChart()               const;
    QChart *createLineChart()              const;
    QChart *createSplineChart()            const;
    QChart *createScatterChart()           const;

    QStringList matchRegString(const QString& reg,const QString& str);

    Ui::ThemeWidgetForm *m_ui;

    QList<QChartView *> m_charts;
    DataTable           m_dataTable;

};

#endif // QCHARWIDGET_H
