#include <QDebug>
#include <QMetaEnum>

#include <cmath>

#include <string>
#include <functional>

#include "qmooshimeter.h"

using namespace std::placeholders;

const std::map<const QMooshimeter::Mapping, const QStringList> QMooshimeter::valid_ranges{
    {Mapping::VOLTAGE,		{"60", "600"}},
    {Mapping::CURRENT,		{"10"}},
    {Mapping::TEMP, 		{"350"}},
    {Mapping::AUX_V,		{"0.1", "0.3", "1.2"}},
    {Mapping::RESISTANCE, 	{"1000.0", "10000.0", "100000.0", "1000000.0", "10000000.0"}},
    {Mapping::DIODE,		{"1.2"}}
};
const QStringList QMooshimeter::valid_rates{ "125", "250", "500", "1000", "2000", "4000", "8000" };
const QStringList QMooshimeter::valid_buffer_depths{ "32", "64", "128", "256" };



QMooshimeter::QMooshimeter(QObject *parent) :
    QObject(parent),
    settings("sie.gl", "Mooshimeter", this),
    mm(nullptr),
    ch1_value(NAN),
    ch2_value(NAN),
    bat_v(-1),
    log(false),
    ch1_mapping(Mapping::CURRENT),
    ch1_analysis(Analysis::MEAN),
    ch1_range(100),
    ch2_mapping(Mapping::VOLTAGE),
    ch2_analysis(Analysis::MEAN),
    ch2_range(100)
{
    btaddr = settings.value("btaddr", "").toString();
    temp_unit = TempUnit(settings.value("tempunit", int(TempUnit::KELVIN)).toInt());

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
                std::bind(&QMooshimeter::others_cb, this, _1)
                );

    set_ch1();
    set_ch2();
    set_rate("1000");
    set_depth("128");
    cmd("SAMPLING:TRIGGER CONTINUOUS");
}



void QMooshimeter::disconnect() {
    mm.reset();
    mm = nullptr;
}



void QMooshimeter::reboot() {
    cmd("REBOOT:NORMAL");
}



void QMooshimeter::shipping_mode() {
    cmd("REBOOT:SHIPMODE");
}



void QMooshimeter::measurement_cb(const Measurement &m) {
    ch1_value = m.ch1.value;
    ch2_value = m.ch2.value;
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
        qDebug() << "Unknown attribute: " << r.name.c_str() << " = " << r.value.c_str();
    }
}



int QMooshimeter::get_bat_percent() {
    int lvl = (bat_v - 2) * 100;
    lvl = std::max(0, lvl);
    lvl = std::min(100, lvl);
    return lvl;
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
    const auto &it = valid_ranges.find(mapping);
    Q_ASSERT(it != valid_ranges.end());

    if (range < 0) {
        range = 0;
        return false;
    }

    const QStringList &l = it->second;
    if (range >= l.length()) {
        range = l.length() - 1;
        return false;
    }

    return true;
}



bool QMooshimeter::check_mapping(const int &channel, Mapping &mapping) {
    switch(channel) {
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
    if (valid_rates.contains(rate))
        return true;

    rate = valid_rates.last();
    return false;
}



bool QMooshimeter::check_depth(QString &depth) {
    if (valid_buffer_depths.contains(depth))
        return true;

    depth = valid_buffer_depths.last();
    return false;
}



void QMooshimeter::channel_config(const int &channel, Mapping &mapping, Analysis &analysis, int &range) {
    Q_ASSERT(channel > 0 && channel < 3);
    check_mapping(channel, mapping);
    check_range(mapping, range);

    QMetaEnum metaEnum = QMetaEnum::fromType<Mapping>();
    QString map = metaEnum.valueToKey(int(mapping));

    metaEnum = QMetaEnum::fromType<Analysis>();
    QString an = metaEnum.valueToKey(int(analysis));

    QString c;
    QString ch = "CH" + QString::number(channel);

    if (is_shared(mapping)) {
        cmd("SHARED " + map);
        cmd(ch + ":MAPPING SHARED");
    } else {
        cmd(ch + ":MAPPING " + map);
    }

    auto it = valid_ranges.find(mapping);
    Q_ASSERT(it != valid_ranges.end());
    const QStringList &l = it->second;

    cmd(ch + ":RANGE_I " + l[range]);
    cmd(ch + ":ANALYSIS " + an);

    if (channel == 1) {
        model_ch1_range = range_model(mapping);
        emit ch1Config();
        emit ch1modelChanged();
    } else {
        model_ch2_range = range_model(mapping);
        emit ch2Config();
        emit ch2modelChanged();
    }
}



void QMooshimeter::set_ch1() {
    channel_config(1, ch1_mapping, ch1_analysis, ch1_range);
}



void QMooshimeter::set_ch2() {
    channel_config(2, ch2_mapping, ch2_analysis, ch2_range);
}



void QMooshimeter::set_btaddr(const QString &addr) {
    if ((btaddr == addr) || addr.isEmpty())
        return;

    btaddr = addr;
    mm.reset();
    connect();
    emit btaddrChanged();
    settings.setValue("btaddr", btaddr);
    settings.sync();
}



void QMooshimeter::set_temp_unit(const TempUnit &t) {
    temp_unit = t;
    emit tempunitChanged();
    settings.setValue("tempunit", int(t));
    settings.sync();
}


QString QMooshimeter::format(const Mapping &mapping, float val) {
    QChar iPref[]{ 'k', 'M', 'G', 'T', 'P', 'E', 'Z', 'Y' };
    QChar dPref[]{ 'm', 0x03bc, 'n', 'p', 'f', 'a', 'z', 'y' };

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
            break;

        case Mapping::RESISTANCE:
            unit = QChar(0x03A9); // omega
    }

    int deg = std::floor(std::log10(std::abs(val)) / 3);
    double sc = val * std::pow(1000, -deg);
    if (deg > 0)
        return QString::number(sc) + " " + iPref[deg-1] + unit;
    else if (deg < 0)
        return QString::number(sc) + " " + dPref[-deg-1] + unit;
    else
        return QString::number(val) + " " + unit;
}



QStringList QMooshimeter::range_model(const Mapping &mapping) {
    QStringList l;
    auto it = valid_ranges.find(mapping);
    Q_ASSERT(it != valid_ranges.end());
    for (const auto &i: it->second) {
        l.append(format(mapping, i.toFloat()));
    }
    return l;
}



std::string QMooshimeter::cmd(const QString &cmd) {
    if (mm) {
        qDebug() << "CMD: " << cmd;
        auto r = mm->cmd(cmd.toLatin1().data());
        //return r.get();
    }
    return "";
}
