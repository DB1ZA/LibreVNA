#ifndef LIBREVNADRIVER_H
#define LIBREVNADRIVER_H

#include "devicedriver.h"

#include "../../VNA_embedded/Application/Communication/Protocol.hpp"

#include <functional>

class LibreVNADriver : public DeviceDriver
{
    Q_OBJECT
public:
    enum class TransmissionResult {
        Ack,
        Nack,
        Timeout,
        InternalError,
    };
    Q_ENUM(TransmissionResult)

    LibreVNADriver();

    /**
     * @brief Returns the serial number of the connected device
     * @return Serial number of connected device (empty string if no device is connected)
     */
    virtual QString getSerial() override {return serial;}

    /**
     * @brief Returns the device information. This function will be called when a device has been connected. Its return value must be valid
     * directly after returning from DeviceDriver::connectTo()
     *
     * Emit the InfoUpdate() signal whenever the return value of this function changes.
     *
     * @return Device information
     */
    virtual Info getInfo() override {return info;}

    /**
     * @brief Returns a set of all active flags
     *
     * There is also a convenience function to check a specific flag, see DeviceDriver::asserted()
     *
     * @return Set of active flags
     */
    virtual std::set<Flag> getFlags() override;

    /**
     * @brief Returns the device status string. It will be displayed in the status bar of the application
     *
     * Emit the StatusUpdated() signal whenever the return value of this function changes
     *
     * @return Status string
     */
    virtual QString getStatus() override;

    /**
     * @brief Returns the driver specific settings
     *
     * The settings are returned as a map.
     * Key: user-readable setting name
     * Value: SettingDescription, consisting of:
     *      - var: Pointer to the setting variable (should be a private member of the derived class)
     *      - name: Arbitrary string used to persistently store this setting (never visible to the user)
     *      - def: Default value of the setting
     *
     * These settings will be persistent across reboots. For each device driver, a section within the preferences
     * will be created where these settings can be changed.
     *
     * @return Map of driver specific settings
     */
     virtual std::vector<Savable::SettingDescription> driverSpecificSettings() override;

    /**
     * @brief Return driver specific actions.
     *
     * The returned actions will be appended to the device menu.
     *
     * @return List of actions
     */
    virtual std::vector<QAction*> driverSpecificActions() override;

    /**
     * @brief Names of available measurements.
     *
     * The names must be identical to the names used in the returned VNAMeasurement.
     * Typically the S parameters, e.g. this function may return {"S11","S12","S21","S22"} but any other names are also allowed.
     *
     * @return List of available VNA measurement parameters
     */
    virtual QStringList availableVNAMeasurements() override;

    /**
     * @brief Configures the VNA and starts a sweep
     * @param s VNA settings
     * @param cb Callback, must be called after the VNA has been configured
     * @return true if configuration successful, false otherwise
     */
    virtual bool setVNA(const VNASettings &s, std::function<void(bool)> cb = nullptr) override;

    /**
     * @brief Names of available measurements.
     *
     * The names must be identical to the names used in the returned SAMeasurement.
     * Typically the port names, e.g. this function may return {"PORT1","PORT2"} but any other names are also allowed.
     *
     * @return List of available SA measurement parameters
     */
    virtual QStringList availableSAMeasurements() override;
    /**
     * @brief Configures the SA and starts a sweep
     * @param s SA settings
     * @param cb Callback, must be called after the SA has been configured
     * @return true if configuration successful, false otherwise
     */
    virtual bool setSA(const SASettings &s, std::function<void(bool)> cb = nullptr) override;

    /**
     * @brief Returns the number of points in one spectrum analyzer sweep (as configured by the last setSA() call)
     * @return Number of points in the sweep
     */
    virtual unsigned int getSApoints() {return SApoints;}

    /**
     * @brief Names of available generator ports.
     *
     * Typically the port names, e.g. this function may return {"PORT1","PORT2"} but any other names are also allowed.
     *
     * @return List of available SA measurement parameters
     */
    virtual QStringList availableSGPorts() override;
    /**
     * @brief Configures the generator
     * @param s Generator settings
     * @return true if configuration successful, false otherwise
     */
    virtual bool setSG(const SGSettings &s) override;

    /**
     * @brief Sets the device to idle
     *
     * Stops all sweeps and signal generation
     *
     * @param cb Callback, must be called after the device has stopped all operations
     * @return true if configuration successful, false otherwise
     */
    virtual bool setIdle(std::function<void(bool)> cb = nullptr) override;

    /**
     * @brief Returns the available options for the external reference input
     * @return External reference input options
     */
    virtual QStringList availableExtRefInSettings() override;

    /**
     * @brief Returns the available options for the external reference output
     * @return External reference output options
     */
    virtual QStringList availableExtRefOutSettings() override;

    /**
     * @brief Configures the external reference input/output
     * @param option_in Reference input option (one of the options returned by availableExtRefInSettings())
     * @param option_out Reference output option (one of the options returned by availableExtRefOutSettings())
     * @return true if configuration successful, false otherwise
     */
    virtual bool setExtRef(QString option_in, QString option_out) override;

public:
signals:
    // Required for the compound device driver
    void passOnReceivedPacket(const Protocol::PacketInfo& packet);
public:
    virtual bool SendPacket(const Protocol::PacketInfo& packet, std::function<void(TransmissionResult)> cb = nullptr, unsigned int timeout = 500) = 0;

protected:
signals:
    void receivedAnswer(TransmissionResult result);
    void receivedPacket(const Protocol::PacketInfo& packet);

protected slots:
    void handleReceivedPacket(const Protocol::PacketInfo& packet);
protected:
    bool sendWithoutPayload(Protocol::PacketType type, std::function<void(TransmissionResult)> cb = nullptr);

    bool connected;
    QString serial;
    Info info;

    Protocol::DeviceStatusV1 lastStatus;

    bool skipOwnPacketHandling;
    bool zerospan;
    unsigned int SApoints;

    std::map<int, int> portStageMapping; // maps from excitedPort (count starts at zero) to stage (count starts at zero)

    // Driver specific settings
    bool captureRawReceiverValues;
    bool SASignalID;
    bool SAUseDFT;
    double SARBWLimitForDFT;
    bool VNASuppressInvalidPeaks;
    bool VNAAdjustPowerLevel;
};

#endif // LIBREVNADRIVER_H