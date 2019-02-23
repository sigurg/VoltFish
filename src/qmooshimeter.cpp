#include <QDebug>
#include <QMetaEnum>

#include <cmath>

#include <string>
#include <functional>

#include "qmooshimeter.h"

using namespace std::placeholders;

uint qHash(const QMooshimeter::Mapping &m, uint seed = 0) {
    return qHash(uint(m), seed);
}

const QHash<QMooshimeter::Mapping, QStringList>
QMooshimeter::valid_ranges{ // strings, as sent to meter
    {Mapping::VOLTAGE,		{"60", "600"}},
    {Mapping::CURRENT,		{"10"}},
    {Mapping::TEMP, 		{"350"}},
    {Mapping::AUX_V,		{"0.1", "0.3", "1.2"}},
    {Mapping::RESISTANCE, 	{"1000.0", "10000.0", "100000.0", "1000000.0", "10000000.0"}},
    {Mapping::DIODE,		{"1.2"}}
};
const QHash<QMooshimeter::Mapping, QVector<double>> QMooshimeter::out_of_range{
    {Mapping::VOLTAGE,		{70, 700}},
    {Mapping::CURRENT,		{11.5}},
    {Mapping::TEMP, 		{350}},
    {Mapping::AUX_V,		{0.2, 0.3, 1.7}},
    {Mapping::RESISTANCE, 	{2e3, 17.5e3, 2e6, 2e6, 17e6}},
    {Mapping::DIODE,		{1.7}}
};
const QStringList QMooshimeter::math_modes{
    //% "Real Power"
    //: math channel mode: A/C real power / D/C power
    QT_TRID_NOOP("math-real-power"),
    //% "Apparent Power"
    //: math channel mode: A/C apparent power
    QT_TRID_NOOP("math-apparent-power"),
    //% "Power Factor"
    //: math channel mode: A/C power factor
    QT_TRID_NOOP("math-power-factor"),
    //% "Thermocouple (K)"
    //: math channel mode: k-type thermocouple voltage to temperature conversion
    QT_TRID_NOOP("math-thermocouple-k")
};
const QStringList QMooshimeter::valid_rates{"125", "250", "500", "1000", "2000", "4000", "8000"};
const QStringList QMooshimeter::valid_buffer_depths{"32", "64", "128", "256"};
constexpr std::array<double, 10> QMooshimeter::thermocouple_coeff;

QDebug &operator<<(QDebug stream, const std::string &s) {
    return stream << s.c_str();
}



QMooshimeter::QMooshimeter(QObject *parent) :
    QObject(parent),
    settings("VoltFish", "VoltFish", this),
    mm(nullptr),
    ch1_value(NAN),
    ch2_value(NAN),
    pwr(NAN),
    bat_v(-1),
    log(false),
    ch1_mapping(Mapping::CURRENT),
    ch1_analysis(Analysis::MEAN),
    ch1_range(0),
    ch2_mapping(Mapping::VOLTAGE),
    ch2_analysis(Analysis::MEAN),
    ch2_range(1),
    math_mode(MathMode::REAL_PWR) {
    btaddr = settings.value("btaddr", "").toString();
    temp_unit = TempUnit(settings.value("tempunit",
                                        int(TempUnit::KELVIN)).toInt());

    model_ch1_range = range_model(ch1_mapping);
    model_ch2_range = range_model(ch2_mapping);

    emit ch1modelChanged();
    emit ch2modelChanged();
    emit btaddrChanged();
    emit tempunitChanged();
}



QMooshimeter::~QMooshimeter() {
}



void QMooshimeter::connect() {
    mm = std::make_shared<Mooshimeter>(
             btaddr.toLatin1().data(),
             H_IN,
             H_OUT,
             std::bind(&QMooshimeter::measurement_cb, this, _1),
             std::bind(&QMooshimeter::others_cb, this, _1),
             // use verbose mode for debug builds
#ifdef QT_NO_DEBUG_OUTPUT
             false
#else
             true
#endif
         );

    QMetaEnum metaMapping = QMetaEnum::fromType<Mapping>();
    QMetaEnum metaAnalysis = QMetaEnum::fromType<Analysis>();

    auto m = cmd("CH1:MAPPING").get();
    cmd(("CH1:MAPPING " + m).c_str()).get();
    if (m == "SHARED")
       m = cmd("SHARED").get();

    ch1_mapping = Mapping(metaMapping.keyToValue(m.c_str()));
    ch1_analysis = Analysis(metaAnalysis.keyToValue(cmd("CH1:ANALYSIS").get().c_str()));
    ch1_range = valid_ranges[ch1_mapping].size()-1;

    //auto r = cmd("CH1:RANGE_I").get().c_str();
    //ch1_range = valid_ranges[ch1_mapping].indexOf(r);

    emit ch1modelChanged();
    emit ch1Config();

    m = cmd("CH2:MAPPING").get();
    if (m == "SHARED")
       m = cmd("SHARED").get();

    ch2_mapping = Mapping(metaMapping.keyToValue(m.c_str()));
    ch2_analysis = Analysis(metaAnalysis.keyToValue(cmd("CH2:ANALYSIS").get().c_str()));
    ch2_range = valid_ranges[ch2_mapping].size()-1;

    emit ch2modelChanged();
    emit ch2Config();

    rate = cmd("SAMPLING:RATE").get().c_str();
    depth = cmd("SAMPLING:DEPTH").get().c_str();
    emit rateChanged();
    emit depthChanged();

    cmd("SAMPLING:TRIGGER CONTINUOUS");
}



void QMooshimeter::disconnect() {
    mm.reset();
    mm = nullptr;
}



void QMooshimeter::reboot() {
    cmd("REBOOT NORMAL");
}



void QMooshimeter::shipping_mode() {
    cmd("REBOOT SHIPMODE");
}



void QMooshimeter::measurement_cb(const Measurement &m) {
    ch1_value = m.ch1.value;
    ch2_value = m.ch2.value;
    pwr = m.pwr;
    emit newMeasurement();
}



void QMooshimeter::others_cb(const Response &r) {
    if (r.name == "BAT_V") {
        float new_bat_v = std::stof(r.value);
        if (new_bat_v != bat_v) {
            bat_v = new_bat_v;
            emit batChanged();
        }
    } else if (r.name == "LOG:STATUS") {
        bool new_logging = std::stoi(r.value);
        if (new_logging != log) {
            log = new_logging;
            emit logChanged();
        }
    } else {
        qDebug() << "Unknown attribute: " << r.name << " = " << r.value;
    }
}



static inline QString
outOfRange() { // workaround for lupdate to work correctly
    //% "out of range"
    //: displayed in measurement label when value is out of range
    return qtTrId("out-of-range");
}



QString QMooshimeter::get_ch1() {
    if (is_out_of_range(ch1_mapping, ch1_range, ch1_value)) {
        return outOfRange();
    }

    return format(ch1_mapping, ch1_value);
}



QString QMooshimeter::get_ch2() {
    if (is_out_of_range(ch2_mapping, ch2_range, ch2_value)) {
        return outOfRange();
    }

    return format(ch2_mapping, ch2_value);
}



double QMooshimeter::thermocouple_convert(const double &v) {
    double t{0}, uv{v * 1e6};
    for (size_t i=0; i<thermocouple_coeff.size(); ++i) {
        t += thermocouple_coeff.at(i)*std::pow(uv, i);
    }
    return t;
}



QString QMooshimeter::get_math() {
    QString unit;
    double val{0};
    //% "invalid inputs"
    //: displayed in math channel label when ch1/ch2 input modes are invalid for selected math mode
    static const auto invalid = qtTrId("invalid-inputs");

    switch (math_mode) {
    case MathMode::REAL_PWR:
        if (ch1_mapping != Mapping::CURRENT || (ch2_mapping != Mapping::VOLTAGE
                                                && ch2_mapping != Mapping::AUX_V)) {
            return invalid;
        }
        val = pwr;
        unit = "W";
        break;

    case MathMode::APP_PWR:
        if (ch1_mapping != Mapping::CURRENT || (ch2_mapping != Mapping::VOLTAGE
                                                && ch2_mapping != Mapping::AUX_V)) {
            return invalid;
        }
        val = ch1_value * ch2_value;
        unit = "VA";
        break;

    case MathMode::PWR_FACTOR:
        if (ch1_mapping != Mapping::CURRENT || (ch2_mapping != Mapping::VOLTAGE
                                                && ch2_mapping != Mapping::AUX_V)) {
            return invalid;
        }
        if (ch1_analysis != Analysis::RMS || ch2_analysis != Analysis::RMS) {
            return invalid;
        }
        val = pwr / (ch1_value * ch2_value);
        return QString::number(val);
        break;

    case MathMode::THERMOCOUPLE_K:
        if (ch1_mapping != Mapping::TEMP && ch2_mapping != Mapping::TEMP) {
            return invalid;
        }
        if (ch1_mapping != Mapping::AUX_V && ch2_mapping != Mapping::AUX_V) {
            return invalid;
        }
        double v = (ch1_mapping == Mapping::AUX_V) ? ch1_value : ch2_value;
        double t = (ch1_mapping == Mapping::TEMP) ? ch1_value : ch2_value;
        t += thermocouple_convert(v);
        return format_temp(t);
        break;
    }
    return si_prefix(val) + unit;
}



int QMooshimeter::get_bat_percent() {
    int lvl = (bat_v - 2) * 100;
    return qBound(0, lvl, 100);
}



void QMooshimeter::set_ch1_mapping(const Mapping &m) {
    ch1_mapping = m;
    ch1_range = 100; // start in highest range when switching modes
    set_ch1();
}



void QMooshimeter::set_ch1_analysis(const Analysis &a) {
    ch1_analysis = a;
    set_ch1();
}



void QMooshimeter::set_ch1_range(const int &r) {
    ch1_range = r;
    set_ch1();
}

void QMooshimeter::set_ch2_mapping(const Mapping &m) {
    ch2_mapping = m;
    ch2_range = 100; // start in highest range when switching modes
    set_ch2();
}



void QMooshimeter::set_ch2_analysis(const Analysis &a) {
    ch2_analysis = a;
    set_ch2();
}



void QMooshimeter::set_ch2_range(const int &r) {
    ch2_range = r;
    set_ch2();
}



void QMooshimeter::set_math_mode(const MathMode &mode) {
    math_mode = mode;
    emit mathConfig();
}



void QMooshimeter::set_rate(QString r) {
    check_rate(r);
    rate = r;
    QString c("SAMPLING:RATE ");
    c += rate;
    cmd(c);
    emit rateChanged();
}



void QMooshimeter::set_depth(QString d) {
    check_depth(d);
    depth = d;
    QString c("SAMPLING:DEPTH ");
    c += depth;
    cmd(c);
    emit depthChanged();
}



bool QMooshimeter::is_shared(const Mapping &mapping) {
    switch (mapping) {
    case Mapping::VOLTAGE:
    case Mapping::CURRENT:
    case Mapping::TEMP:
        return false;
    default:
        return true;
    }
}



bool QMooshimeter::check_range(const Mapping &mapping, int &range) {
    const auto &l = valid_ranges.value(mapping);

    if (range < 0) {
        range = 0;
        return false;
    }

    if (range >= l.length()) {
        range = l.length() - 1;
        return false;
    }

    return true;
}



bool QMooshimeter::check_mapping(const int &channel, Mapping &mapping) {
    switch (channel) {
    case 1:
        if (mapping == Mapping::VOLTAGE) {
            mapping = Mapping::CURRENT;
            return false;
        } else {
            return true;
        }
        break;
    case 2:
        if (mapping == Mapping::CURRENT) {
            mapping = Mapping::VOLTAGE;
            return false;
        } else {
            return true;
        }
        break;
    default:
        throw std::runtime_error("Invalid channel: " + channel);
    }
    return false;
}


bool QMooshimeter::check_rate(QString &rate) {
    if (valid_rates.contains(rate)) {
        return true;
    }

    rate = valid_rates.last();
    return false;
}



bool QMooshimeter::check_depth(QString &depth) {
    if (valid_buffer_depths.contains(depth)) {
        return true;
    }

    depth = valid_buffer_depths.last();
    return false;
}



void QMooshimeter::channel_config(const int &channel, Mapping &mapping,
                                  Analysis &analysis, int &range) {
    Q_ASSERT(channel > 0 && channel < 3);
    check_mapping(channel, mapping);
    check_range(mapping, range);

    QMetaEnum metaEnum = QMetaEnum::fromType<Mapping>();
    QString map = metaEnum.valueToKey(int(mapping));

    metaEnum = QMetaEnum::fromType<Analysis>();
    QString an = metaEnum.valueToKey(int(analysis));

    QString ch = "CH" + QString::number(channel);
    const auto &l = valid_ranges.value(mapping);

    if (is_shared(mapping)) {
        cmd("SHARED " + map);
        cmd(ch + ":MAPPING SHARED");
    } else {
        cmd(ch + ":MAPPING " + map);
    }

    cmd(ch + ":RANGE_I " + l.at(range));
    cmd(ch + ":ANALYSIS " + an);

    if (channel == 1) {
        model_ch1_range = range_model(mapping);
    } else {
        model_ch2_range = range_model(mapping);
    }

    emit ch1Config();
    emit ch1modelChanged();
    emit ch2Config();
    emit ch2modelChanged();
}



void QMooshimeter::set_ch1() {
    channel_config(1, ch1_mapping, ch1_analysis, ch1_range);
}



void QMooshimeter::set_ch2() {
    channel_config(2, ch2_mapping, ch2_analysis, ch2_range);
}



void QMooshimeter::set_btaddr(const QString &addr) {
    if ((btaddr == addr) || addr.isEmpty()) {
        return;
    }

    btaddr = addr;
    emit btaddrChanged();
    mm.reset();
    connect();
    settings.setValue("btaddr", btaddr);
    settings.sync();
}



void QMooshimeter::set_temp_unit(const TempUnit &t) {
    temp_unit = t;
    emit tempunitChanged();
    settings.setValue("tempunit", int(t));
    settings.sync();
}



QString QMooshimeter::format(const Mapping &mapping, const double &val) {
    QString unit;
    switch (mapping) {
    case Mapping::VOLTAGE:
    case Mapping::AUX_V:
    case Mapping::DIODE:
        unit = "V";
        break;

    case Mapping::CURRENT:
        unit = "A";
        break;

    case Mapping::TEMP:
        return format_temp(val);
        break;

    case Mapping::RESISTANCE:
        unit = QChar(0x03A9); // omega
        break;
    }

    return si_prefix(val) + unit;
}



bool QMooshimeter::is_out_of_range(const Mapping &mapping, const int &range,
                                   const double &val) {
    return out_of_range.value(mapping).at(range) < std::abs(val);
}



QString QMooshimeter::format_temp(double val) {
    QString unit;
    switch (temp_unit) {
    case TempUnit::KELVIN:
        unit = "°K";
        break;
    case TempUnit::CELSIUS:
        unit = "°C";
        val -= 273.15;
        break;
    case TempUnit::FAHRENHEIT:
        unit = "°F";
        val = val * 9.0/5.0 - 459.67;
        break;
    }

    return QString::number(val) + " " + unit; // no SI prefix for temperatures
}



QString QMooshimeter::si_prefix(const double &val) {
    static const QList<QChar> iPref{ 'k', 'M', 'G', 'T', 'P', 'E', 'Z', 'Y' };
    static const QList<QChar> dPref{ 'm', 0x03bc, 'n', 'p', 'f', 'a', 'z', 'y' };

    if (val == 0) {
        return QString::number(val) + " ";
    }

    int deg = std::floor(std::log10(std::abs(val)) / 3);
    deg = qBound(-dPref.length(), deg, iPref.length());
    double sc = val * std::pow(1000, -deg);

    if (deg > 0) {
        return QString::number(sc) + " " + iPref.at(deg-1);
    } else if (deg < 0) {
        return QString::number(sc) + " " + dPref.at(-deg-1);
    }

    return QString::number(val) + " ";
}



QStringList QMooshimeter::range_model(const Mapping &mapping) {
    QStringList l;
    for (const auto &i: valid_ranges.value(mapping)) {
        l.append(format(mapping, i.toDouble()));
    }
    return l;
}



std::shared_future<std::string> QMooshimeter::cmd(const QString &cmd) {
    if (mm) {
        qDebug() << "CMD: " << cmd;
        auto ret = std::move(mm->cmd(cmd.toLatin1().data()));
#ifndef QT_NO_DEBUG_OUTPUT
        qDebug() << "RET: " << ret.get();
#endif
        return ret;
    }
    return std::shared_future<std::string>();
}
