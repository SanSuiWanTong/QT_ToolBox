#include "QcharWidget.h"
#include "ui_QcharWidget.h"

#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QtCharts/QAbstractBarSeries>
#include <QtCharts/QPercentBarSeries>
#include <QtCharts/QStackedBarSeries>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QLineSeries>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QAreaSeries>
#include <QtCharts/QLegend>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtCore/QRandomGenerator>
#include <QtCharts/QBarCategoryAxis>
#include <QtWidgets/QApplication>
#include <QtCharts/QValueAxis>
#include <QTextStream>
#include <QDebug>
#include <QDate>
#include <QDateTimeAxis>

QcharWidget::QcharWidget(QWidget *parent): QWidget(parent),
    m_ui(new Ui::ThemeWidgetForm)
{    
    m_ui->setupUi(this);
    generateData();
    populateThemeBox();
    populateAnimationBox();
    populateLegendBox();

    //create charts
    QChartView *chartView;
    chartView = new QChartView(createAreaChart());
    m_ui->gridLayout->addWidget(chartView, 1, 0);
    m_charts << chartView;

    chartView = new QChartView(createPieChart());
    // Funny things happen if the pie slice labels do not fit the screen, so we ignore size policy
    chartView->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    m_ui->gridLayout->addWidget(chartView, 1, 1);
    m_charts << chartView;

    chartView = new QChartView(createLineChart());
    m_ui->gridLayout->addWidget(chartView, 1, 2);
    m_charts << chartView;

    chartView = new QChartView(createBarChart());
    m_ui->gridLayout->addWidget(chartView, 2, 0);
    m_charts << chartView;

    chartView = new QChartView(createSplineChart());
    m_ui->gridLayout->addWidget(chartView, 2, 1);
    m_charts << chartView;

    chartView = new QChartView(createScatterChart());
    m_ui->gridLayout->addWidget(chartView, 2, 2);
    m_charts << chartView;

    // Set defaults
    m_ui->antialiasCheckBox->setChecked(true);

    // Set the colors from the light theme as default ones
    QPalette pal = qApp->palette();
    pal.setColor(QPalette::Window, QRgb(0x40434a));
    pal.setColor(QPalette::WindowText, QRgb(0xd6d6d6));
    qApp->setPalette(pal);

    updateUI();
}

QcharWidget::~QcharWidget()
{
    delete m_ui;
}

QStringList QcharWidget::matchRegString(const QString& reg,const QString& str)
{
    QStringList dataList;
    QRegularExpression re_ins(reg);
    QRegularExpressionMatchIterator i = re_ins.globalMatch(str);
    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        dataList << match.captured(1);
    }
    //Q_ASSERT(dataList.length() >= 1);
    return dataList;
}
/* confirmed cases
 * death
 * dateTime for Axis
*/
void QcharWidget::generateData()
{
    QStringList dateList,confirmedList,deathList;
    DataList    confirmedDataList,deathDataList;

    const QString QRegExp_date      = "(.*?):";
    const QString QRegExp_confirmed = "cases \\((.*?)new";
    const QString QRegExp_death     = "deaths \\((.*?)new";

    QString filePath = "../QT_ToolBox/data/qchartData.txt";
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){ qDebug()<<"data file open fail!"; return;}

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line  = in.readLine();
        auto tList = matchRegString(QRegExp_date,line);
        if(tList.length() == 0)   continue;

        dateList      << tList.at(0);
        confirmedList << matchRegString(QRegExp_confirmed,line).at(0);
        deathList     << matchRegString(QRegExp_death,line).at(0);
    }
    Q_ASSERT(dateList.length() == confirmedList.length());
    for(int i = 0; i < dateList.size(); ++i){
        //x
        auto strList = dateList.at(i).split(".");
        int mm = strList.at(0).toInt();
        int dd = strList.at(1).toInt();
        QDate date(2020,mm,dd);
        QDateTime xValue;
        xValue.setDate(date);
        qreal x = xValue.toMSecsSinceEpoch();

        //y confirmed
        QString strNum = confirmedList.at(i);
        qreal y = strNum.remove(",").toInt();
        confirmedDataList.append(qMakePair(x, y));

        //y death
        strNum = deathList.at(i);
        y = strNum.remove(",").toInt();
        //
        deathDataList.append(qMakePair(x, y));
    }
    m_dataTable.insert("death",deathDataList);
    m_dataTable.insert("confirmed",confirmedDataList);
}

void QcharWidget::populateThemeBox()
{
    // add items to theme combobox
    m_ui->themeComboBox->addItem("Light"        , QChart::ChartThemeLight);
    m_ui->themeComboBox->addItem("Blue Cerulean", QChart::ChartThemeBlueCerulean);
    m_ui->themeComboBox->addItem("Dark"         , QChart::ChartThemeDark);
    m_ui->themeComboBox->addItem("Brown Sand"   , QChart::ChartThemeBrownSand);
    m_ui->themeComboBox->addItem("Blue NCS"     , QChart::ChartThemeBlueNcs);
    m_ui->themeComboBox->addItem("High Contrast", QChart::ChartThemeHighContrast);
    m_ui->themeComboBox->addItem("Blue Icy"     , QChart::ChartThemeBlueIcy);
    m_ui->themeComboBox->addItem("Qt"           , QChart::ChartThemeQt);
    m_ui->themeComboBox->setCurrentIndex(1);
}

void QcharWidget::populateAnimationBox()
{
    // add items to animation combobox
    m_ui->animatedComboBox->addItem("No Animations"      , QChart::NoAnimation);
    m_ui->animatedComboBox->addItem("GridAxis Animations", QChart::GridAxisAnimations);
    m_ui->animatedComboBox->addItem("Series Animations"  , QChart::SeriesAnimations);
    m_ui->animatedComboBox->addItem("All Animations"     , QChart::AllAnimations);
    m_ui->animatedComboBox->setCurrentIndex(3);
}

void QcharWidget::populateLegendBox()
{
    // add items to legend combobox
    m_ui->legendComboBox->addItem("No Legend "   , 0);
    m_ui->legendComboBox->addItem("Legend Top"   , Qt::AlignTop);
    m_ui->legendComboBox->addItem("Legend Bottom", Qt::AlignBottom);
    m_ui->legendComboBox->addItem("Legend Left"  , Qt::AlignLeft);
    m_ui->legendComboBox->addItem("Legend Right" , Qt::AlignRight);
    m_ui->legendComboBox->setCurrentIndex(2);
}

void QcharWidget::createCommonAxis(QChart *chart, QStringList *categories) const
{
    //X
    QAbstractAxis* axisX = nullptr;
    if(categories!=nullptr)
    {
        axisX = new QBarCategoryAxis();
        qobject_cast<QBarCategoryAxis*>(axisX)->append(*categories);
    }else{
        axisX = new QDateTimeAxis();
        qobject_cast<QDateTimeAxis*>(axisX)->setFormat("MM-dd");
    }
    chart->addAxis(axisX, Qt::AlignBottom);

    //Y
    QValueAxis *axisY = new QValueAxis;
    axisY->setRange(0,100000);
    axisY->setLabelFormat("%d");
    chart->addAxis(axisY,Qt::AlignLeft);

    //
    foreach(auto serier,chart->series()){
        serier->attachAxis(axisX);
        serier->attachAxis(axisY);
    }
}

QChart *QcharWidget::createAreaChart() const
{
    QChart *chart = new QChart();
    chart->setTitle("Area chart");

    QLineSeries *lowerSeries = 0;
    DataTable::const_iterator it = m_dataTable.constBegin();
    while (it != m_dataTable.constEnd()) {
        QLineSeries *upperSeries = new QLineSeries();

        for(int i = 0;i<it.value().length();i++){
            auto it_data = it.value().at(i);
            if (lowerSeries){
                const QVector<QPointF>& points = lowerSeries->pointsVector();
                upperSeries->append(QPointF(it_data.first, points[i].y() + it_data.second));
            }else{
                upperSeries->append(it_data.first, it_data.second);
            }
        }

        QAreaSeries *area = new QAreaSeries(upperSeries, lowerSeries);
        area->setName(it.key());
        chart->addSeries(area);
        lowerSeries = upperSeries;
        ++it;
    }
    createCommonAxis(chart);
    return chart;
}

QChart *QcharWidget::createBarChart() const
{   
    QChart *chart = new QChart();
    chart->setTitle("Bar chart");

    QStackedBarSeries *series = new QStackedBarSeries();
    QStringList categories;

    DataTable::const_iterator it = m_dataTable.constBegin();
    while (it != m_dataTable.constEnd()) {
        categories.clear();
        QBarSet *set = new QBarSet(it.key());
        foreach(auto it_data,it.value()){
            *set << it_data.second;
            QDateTime t = QDateTime::fromMSecsSinceEpoch(it_data.first);
            categories<<t.toString("MM-dd");
        }
        series->append(set);
        ++it;
    }

    chart->addSeries(series);
    createCommonAxis(chart,&categories);
    return chart;
}

QChart *QcharWidget::createLineChart() const
{
    QChart *chart = new QChart();
    chart->setTitle("Line chart");

    DataTable::const_iterator it = m_dataTable.constBegin();
    while (it != m_dataTable.constEnd()) {
        QLineSeries *series = new QLineSeries();
        series->setName(it.key());
        foreach(auto it_data,it.value()){
           series->append(it_data.first, it_data.second);
        }
        chart->addSeries(series);
        ++it;
    }
    createCommonAxis(chart);
    return chart;
}

QChart *QcharWidget::createPieChart() const
{
    QChart *chart = new QChart();
    chart->setTitle("Pie chart");
    QPieSeries *series = new QPieSeries(chart);
    for (const Data &data : m_dataTable.value("death")) {
        QDateTime t = QDateTime::fromMSecsSinceEpoch(data.first);
        series->append(t.toString("MM-dd"), data.second);
    }
    // Show the first slice exploded with label
    QPieSlice *slice = series->slices().last();
    slice->setLabelVisible();
    slice->setExploded();
    slice->setExplodeDistanceFactor(0.5);

    series->setPieSize(0.4);
    chart->addSeries(series);
    return chart;
}

QChart *QcharWidget::createSplineChart() const
{
    QChart *chart = new QChart();
    chart->setTitle("Spline chart");

    DataTable::const_iterator it = m_dataTable.constBegin();
    while (it != m_dataTable.constEnd()) {
        QSplineSeries *series = new QSplineSeries(chart);
        series->setName(it.key());
        foreach(auto it_data,it.value()){
           series->append(it_data.first, it_data.second);
        }
        chart->addSeries(series);
        ++it;
    }

    createCommonAxis(chart);
    return chart;
}

QChart *QcharWidget::createScatterChart() const
{
    // scatter chart
    QChart *chart = new QChart();
    chart->setTitle("Scatter chart");

    DataTable::const_iterator it = m_dataTable.constBegin();
    while (it != m_dataTable.constEnd()) {
        QScatterSeries *series = new QScatterSeries(chart);
        series->setName(it.key());
        foreach(auto it_data,it.value()){
           series->append(it_data.first, it_data.second);
        }
        chart->addSeries(series);
        ++it;
    }
    createCommonAxis(chart);
    return chart;
}

void QcharWidget::updateUI()
{    
    QChart::ChartTheme theme = static_cast<QChart::ChartTheme>(
                m_ui->themeComboBox->itemData(m_ui->themeComboBox->currentIndex()).toInt());

    const auto charts = m_charts;
    if (!m_charts.isEmpty() && m_charts.at(0)->chart()->theme() != theme) {
        for (QChartView *chartView : charts) {          
            chartView->chart()->setTheme(theme);            
        }

        // Set palette colors based on selected theme        
        QPalette pal = window()->palette();
        if (theme == QChart::ChartThemeLight) {
            pal.setColor(QPalette::Window, QRgb(0xf0f0f0));
            pal.setColor(QPalette::WindowText, QRgb(0x404044));        
        } else if (theme == QChart::ChartThemeDark) {
            pal.setColor(QPalette::Window, QRgb(0x121218));
            pal.setColor(QPalette::WindowText, QRgb(0xd6d6d6));
        } else if (theme == QChart::ChartThemeBlueCerulean) {
            pal.setColor(QPalette::Window, QRgb(0x40434a));
            pal.setColor(QPalette::WindowText, QRgb(0xd6d6d6));
        } else if (theme == QChart::ChartThemeBrownSand) {
            pal.setColor(QPalette::Window, QRgb(0x9e8965));
            pal.setColor(QPalette::WindowText, QRgb(0x404044));
        } else if (theme == QChart::ChartThemeBlueNcs) {
            pal.setColor(QPalette::Window, QRgb(0x018bba));
            pal.setColor(QPalette::WindowText, QRgb(0x404044));
        } else if (theme == QChart::ChartThemeHighContrast) {
            pal.setColor(QPalette::Window, QRgb(0xffab03));
            pal.setColor(QPalette::WindowText, QRgb(0x181818));
        } else if (theme == QChart::ChartThemeBlueIcy) {
            pal.setColor(QPalette::Window, QRgb(0xcee7f0));
            pal.setColor(QPalette::WindowText, QRgb(0x404044));
        } else {
            pal.setColor(QPalette::Window, QRgb(0xf0f0f0));
            pal.setColor(QPalette::WindowText, QRgb(0x404044));
        }
        window()->setPalette(pal);
    }

    // Update antialiasing    
    bool checked = m_ui->antialiasCheckBox->isChecked();
    for (QChartView *chart : charts)
        chart->setRenderHint(QPainter::Antialiasing, checked);   

    // Update animation options   
    QChart::AnimationOptions options(
                m_ui->animatedComboBox->itemData(m_ui->animatedComboBox->currentIndex()).toInt());
    if (!m_charts.isEmpty() && m_charts.at(0)->chart()->animationOptions() != options) {
        for (QChartView *chartView : charts)
            chartView->chart()->setAnimationOptions(options);
    }    

    // Update legend alignment   
    Qt::Alignment alignment(
                m_ui->legendComboBox->itemData(m_ui->legendComboBox->currentIndex()).toInt());

    if (!alignment) {
        for (QChartView *chartView : charts)
            chartView->chart()->legend()->hide();
    } else {
        for (QChartView *chartView : charts) {
            chartView->chart()->legend()->setAlignment(alignment);
            chartView->chart()->legend()->show();
        }
    }   
}

