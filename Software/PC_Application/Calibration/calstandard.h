#ifndef CALSTANDARD_H
#define CALSTANDARD_H

#include "savable.h"
#include "touchstone.h"
#include "Tools/parameters.h"

#include <complex>

namespace CalStandard
{

class Virtual : public Savable
{
public:
    Virtual() :
        minFreq(std::numeric_limits<double>::lowest()),
        maxFreq(std::numeric_limits<double>::max()){}

    enum class Type {
        Open,
        Short,
        Load,
        Through,
        Line,
        Last
    };

    static Virtual* create(Type type);

    static QString TypeToString(Type type);
    static Type TypeFromString(QString s);

    virtual Type getType() = 0;
    double minFrequency() {return minFreq;}
    double maxFrequency() {return maxFreq;}

    virtual void edit() = 0;
    virtual QString getDescription();

    virtual nlohmann::json toJSON() override;
    virtual void fromJSON(nlohmann::json j) override;

protected:
    QString name;
    double minFreq;
    double maxFreq;
};

class OnePort : public Virtual
{
public:
    OnePort() :
        touchstone(nullptr){}

    virtual std::complex<double> toS11(double freq) = 0;

    void setMeasurement(const Touchstone &ts, int port = 0);
    void clearMeasurement();

    virtual nlohmann::json toJSON() override;
    virtual void fromJSON(nlohmann::json j) override;

protected:
    std::complex<double> addTransmissionLine(std::complex<double> termination_reflection,
                                             double offset_impedance, double offset_delay,
                                             double offset_loss, double frequency);
    Touchstone *touchstone;
};

class Open : public OnePort
{
public:
    Open();

    virtual std::complex<double> toS11(double freq) override;
    virtual void edit() override;
    virtual Type getType() override {return Type::Open;}
    virtual nlohmann::json toJSON() override;
    virtual void fromJSON(nlohmann::json j) override;
private:
    double Z0, delay, loss, C0, C1, C2, C3;
};

class Short : public OnePort
{
public:
    Short();

    virtual std::complex<double> toS11(double freq) override;
    virtual void edit() override;
    virtual Type getType() override {return Type::Short;}
    virtual nlohmann::json toJSON() override;
    virtual void fromJSON(nlohmann::json j) override;
private:
    double Z0, delay, loss, L0, L1, L2, L3;
};

class Load : public OnePort
{
public:
    Load();

    virtual std::complex<double> toS11(double freq) override;
    virtual void edit() override;
    virtual Type getType() override {return Type::Load;}
    virtual nlohmann::json toJSON() override;
    virtual void fromJSON(nlohmann::json j) override;
private:
    double Z0, delay, resistance, Cparallel, Lseries;
    bool Cfirst;
};

class TwoPort : public Virtual
{
public:
    TwoPort() :
        touchstone(nullptr){}

    virtual Sparam toSparam(double freq) = 0;

    void setMeasurement(const Touchstone &ts, int port1 = 0, int port2 = 1);
    void clearMeasurement();

    virtual nlohmann::json toJSON() override;
    virtual void fromJSON(nlohmann::json j) override;

protected:
    Touchstone *touchstone;
};

class Through : public TwoPort
{
public:
    Through();

    virtual Sparam toSparam(double freq) override;
    virtual Type getType() override {return Type::Through;}
    virtual nlohmann::json toJSON() override;
    virtual void fromJSON(nlohmann::json j) override;
private:
    double Z0, delay, loss;
};

}

#endif // CALSTANDARD_H
