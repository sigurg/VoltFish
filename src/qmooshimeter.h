#pragma once

#include <QObject>
#include <QSettings>
#include <QStringList>

#include <atomic>
#include <memory>
#include <map>

#include "lib/mooshimeter.h"

class QMooshimeter : public QObject
{
    Q_OBJECT
public:
    explicit QMooshimeter(QObject *parent = 0);

    virtual ~QMooshimeter();

    enum class Mapping {
        VOLTAGE,
        CURRENT,
        TEMP,
        AUX_V,
        RESISTANCE,
        DIODE
    };
    Q_ENUM(Mapping)

    enum class Analysis {
        MEAN,
        RMS,
        BUFFER
    };
    Q_ENUM(Analysis)

    enum class TempUnit {
        KELVIN,
        CELSIUS,
        FAHRENHEIT
    };
    Q_ENUM(TempUnit)

    Q_PROPERTY(QString ch1 READ get_ch1 NOTIFY newMeasurement);
    Q_PROPERTY(QString ch2 READ get_ch2 NOTIFY newMeasurement);

    Q_PROPERTY(float bat_v READ get_bat_v NOTIFY batChanged);
    Q_PROPERTY(int bat_percent READ get_bat_percent NOTIFY batChanged);
    Q_PROPERTY(bool log READ get_log NOTIFY logChanged);

    Q_PROPERTY(Mapping ch1_mapping MEMBER ch1_mapping WRITE set_ch1_mapping NOTIFY ch1Config);
    Q_PROPERTY(Analysis ch1_analysis MEMBER ch1_analysis WRITE set_ch1_analysis NOTIFY ch1Config);
    Q_PROPERTY(QString ch1_range MEMBER ch1_range WRITE set_ch1_range NOTIFY ch1Config);

    Q_PROPERTY(Mapping ch2_mapping MEMBER ch2_mapping WRITE set_ch2_mapping NOTIFY ch2Config);
    Q_PROPERTY(Analysis ch2_analysis MEMBER ch2_analysis WRITE set_ch2_analysis NOTIFY ch2Config);
    Q_PROPERTY(QString ch2_range MEMBER ch2_range WRITE set_ch2_range NOTIFY ch2Config);

    Q_PROPERTY(QString rate MEMBER rate WRITE set_rate NOTIFY rateChanged);
    Q_PROPERTY(QString depth MEMBER depth WRITE set_depth NOTIFY depthChanged);

    Q_PROPERTY(QStringList model_ch1_range MEMBER model_ch1_range NOTIFY ch1modelChanged);
    Q_PROPERTY(QStringList model_ch2_range MEMBER model_ch2_range NOTIFY ch2modelChanged);

    Q_PROPERTY(QString btaddr MEMBER btaddr WRITE set_btaddr NOTIFY btaddrChanged);
    Q_PROPERTY(TempUnit temp_unit MEMBER temp_unit WRITE set_temp_unit NOTIFY tempunitChanged);

signals:
    void newMeasurement();
    void batChanged();
    void logChanged();
    void ch1Config();
    void ch2Config();
    void rateChanged();
    void depthChanged();
    void btaddrChanged();
    void ch1modelChanged();
    void ch2modelChanged();
    void tempunitChanged();

public slots:
    void connect();
    void reboot();
    void shipping_mode();

private:
    static const int H_IN{0x0015};
    static const int H_OUT{0x0012};

    static const std::map<const Mapping, const QStringList> valid_ranges;
    static const QStringList valid_rates;
    static const QStringList valid_buffer_depths;

    QSettings settings;

    std::shared_ptr<Mooshimeter> mm;

    QString btaddr;
    std::atomic<float> ch1_value;
    std::atomic<float> ch2_value;
    std::atomic<float> bat_v;
    std::atomic<bool>  log;

    Mapping		ch1_mapping;
    Analysis	ch1_analysis;
    QString		ch1_range;

    Mapping		ch2_mapping;
    Analysis	ch2_analysis;
    QString		ch2_range;

    QString		rate;
    QString		depth;

    QStringList model_ch1_range;
    QStringList model_ch2_range;

    TempUnit	temp_unit;

    void measurement_cb(const Measurement &m);
    void others_cb(const Response &r);

    QString get_ch1() { return format(ch1_mapping, ch1_value); };
    QString get_ch2() { return format(ch2_mapping, ch2_value); };

    float get_bat_v() { return bat_v; };
    int get_bat_percent();

    float get_log() { return log; };

    void set_ch1_mapping(const Mapping &m);
    void set_ch1_analysis(const Analysis &a);
    void set_ch1_range(const QString &r);

    void set_ch2_mapping(const Mapping &m);
    void set_ch2_analysis(const Analysis &a);
    void set_ch2_range(const QString &r);

    void set_rate(QString r);
    void set_depth(QString d);

    void set_btaddr(const QString &addr);

    void set_temp_unit(const TempUnit &t);

    bool is_shared(const Mapping &mapping);

    bool check_range(const Mapping &mapping, QString &range);
    bool check_mapping(const int &channel, Mapping &mapping);

    bool check_rate(QString &rate);
    bool check_depth(QString &depth);

    void set_ch1();
    void set_ch2();

    void channel_config(const int &channel, Mapping &mapping, Analysis &analysis, QString &range);

    QString format(const Mapping &mapping, float val);

    QStringList range_model(const Mapping &mapping);

    std::string cmd(const QString &cmd);
};
