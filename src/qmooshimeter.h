#pragma once

#include <QObject>
#include <QHash>
#include <QVector>
#include <QSettings>
#include <QStringList>

#include <array>
#include <atomic>
#include <memory>

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

    enum class MathMode {
        REAL_PWR,
        APP_PWR,
        PWR_FACTOR,
        THERMOCOUPLE_K
    };
    Q_ENUM(MathMode)

    enum class TempUnit {
        KELVIN,
        CELSIUS,
        FAHRENHEIT
    };
    Q_ENUM(TempUnit)

    Q_PROPERTY(QString ch1 READ get_ch1 NOTIFY newMeasurement);
    Q_PROPERTY(QString ch2 READ get_ch2 NOTIFY newMeasurement);
    Q_PROPERTY(QString math READ get_math NOTIFY newMeasurement);

    Q_PROPERTY(float bat_v READ get_bat_v NOTIFY batChanged);
    Q_PROPERTY(int bat_percent READ get_bat_percent NOTIFY batChanged);
    Q_PROPERTY(bool log READ get_log NOTIFY logChanged);

    Q_PROPERTY(Mapping ch1_mapping MEMBER ch1_mapping WRITE set_ch1_mapping NOTIFY ch1Config);
    Q_PROPERTY(Analysis ch1_analysis MEMBER ch1_analysis WRITE set_ch1_analysis NOTIFY ch1Config);
    Q_PROPERTY(int ch1_range MEMBER ch1_range WRITE set_ch1_range NOTIFY ch1Config);

    Q_PROPERTY(Mapping ch2_mapping MEMBER ch2_mapping WRITE set_ch2_mapping NOTIFY ch2Config);
    Q_PROPERTY(Analysis ch2_analysis MEMBER ch2_analysis WRITE set_ch2_analysis NOTIFY ch2Config);
    Q_PROPERTY(int ch2_range MEMBER ch2_range WRITE set_ch2_range NOTIFY ch2Config);

    Q_PROPERTY(MathMode math_mode MEMBER math_mode WRITE set_math_mode NOTIFY mathConfig);

    Q_PROPERTY(QString rate MEMBER rate WRITE set_rate NOTIFY rateChanged);
    Q_PROPERTY(QString depth MEMBER depth WRITE set_depth NOTIFY depthChanged);

    Q_PROPERTY(QStringList model_ch1_range MEMBER model_ch1_range NOTIFY ch1modelChanged);
    Q_PROPERTY(QStringList model_ch2_range MEMBER model_ch2_range NOTIFY ch2modelChanged);

    Q_PROPERTY(QString btaddr MEMBER btaddr WRITE set_btaddr NOTIFY btaddrChanged);
    Q_PROPERTY(TempUnit temp_unit MEMBER temp_unit WRITE set_temp_unit NOTIFY tempunitChanged);

    Q_INVOKABLE QStringList model_rate() { return valid_rates; };
    Q_INVOKABLE QStringList model_depth() { return valid_buffer_depths; };
    Q_INVOKABLE QStringList model_math() { return math_modes; };

signals:
    void newMeasurement();
    void batChanged();
    void logChanged();
    void ch1Config();
    void ch2Config();
    void mathConfig();
    void rateChanged();
    void depthChanged();
    void btaddrChanged();
    void ch1modelChanged();
    void ch2modelChanged();
    void tempunitChanged();

public slots:
    void connect();
    void disconnect();
    void reboot();
    void shipping_mode();

private:
    static const int H_IN{0x0015};
    static const int H_OUT{0x0012};

    static const QHash<Mapping, QStringList> valid_ranges;
    static const QHash<Mapping, QVector<double>> out_of_range;
    static const QStringList valid_rates;
    static const QStringList valid_buffer_depths;
    static const QStringList math_modes;
    static constexpr std::array<double, 10> thermocouple_coeff{{
            1,
            2.508355e-2,
            7.860106e-8,
            -2.503131e-10,
            8.315270e-14,
            -1.228034e-17,
            9.804036e-22,
            -4.413030e-26,
            1.057734e-30,
            -1.052755e-35
    }};



    QSettings settings;

    std::shared_ptr<Mooshimeter> mm;

    QString btaddr;
    std::atomic<double> ch1_value;
    std::atomic<double> ch2_value;
    std::atomic<double> pwr;
    std::atomic<float>  bat_v;
    std::atomic<bool>   log;

    Mapping     ch1_mapping;
    Analysis    ch1_analysis;
    int         ch1_range;

    Mapping     ch2_mapping;
    Analysis    ch2_analysis;
    int         ch2_range;

    MathMode    math_mode;

    QString     rate;
    QString     depth;

    QStringList model_ch1_range;
    QStringList model_ch2_range;

    TempUnit    temp_unit;

    void measurement_cb(const Measurement &m);
    void others_cb(const Response &r);

    double thermocouple_convert(const double &v);

    QString get_ch1();
    QString get_ch2();
    QString get_math();

    float get_bat_v() { return bat_v; };
    int get_bat_percent();

    bool get_log() { return log; };

    void set_ch1_mapping(const Mapping &m);
    void set_ch1_analysis(const Analysis &a);
    void set_ch1_range(const int &r);

    void set_ch2_mapping(const Mapping &m);
    void set_ch2_analysis(const Analysis &a);
    void set_ch2_range(const int &r);

    void set_math_mode(const MathMode &mode);

    void set_rate(QString r);
    void set_depth(QString d);

    void set_btaddr(const QString &addr);

    void set_temp_unit(const TempUnit &t);

    bool is_shared(const Mapping &mapping);

    bool check_range(const Mapping &mapping, int &range);
    bool check_mapping(const int &channel, Mapping &mapping);

    bool check_rate(QString &rate);
    bool check_depth(QString &depth);

    bool is_out_of_range(const Mapping &mapping, const int &range, const double &val);

    void set_ch1();
    void set_ch2();

    void channel_config(const int &channel, Mapping &mapping, Analysis &analysis, int &range);

    QString format(const Mapping &mapping, const double &val);
    QString format_temp(double val);
    QString si_prefix(const double &val);

    QStringList range_model(const Mapping &mapping);

    std::string cmd(const QString &cmd);
};
