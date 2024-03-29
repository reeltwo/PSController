#ifndef PSController_h
#define PSController_h

#include <inttypes.h>

#include "Arduino.h"

class JoystickController
{
public:
    struct AnalogStick
    {
        int8_t lx;
        int8_t ly;
        int8_t rx;
        int8_t ry;
    };

    struct AnalogButton
    {
        uint8_t up;
        uint8_t right;
        uint8_t down;
        uint8_t left;

        uint8_t l2;
        uint8_t r2;
        uint8_t l1;
        uint8_t r1;

        uint8_t triangle;
        uint8_t circle;
        uint8_t cross;
        uint8_t square;
    };

    struct Analog
    {
        AnalogStick stick;
        AnalogButton button;
    };

    struct Button
    {
        uint8_t select   : 1;
        uint8_t l3       : 1;
        uint8_t r3       : 1;
        uint8_t start    : 1;

        uint8_t up       : 1;
        uint8_t right    : 1;
        uint8_t down     : 1;
        uint8_t left     : 1;

        uint8_t upright  : 1;
        uint8_t upleft   : 1;
        uint8_t downright: 1;
        uint8_t downleft : 1;

        uint8_t l2       : 1;
        uint8_t r2       : 1;
        uint8_t l1       : 1;
        uint8_t r1       : 1;

        uint8_t triangle : 1;
        uint8_t circle   : 1;
        uint8_t cross    : 1;
        uint8_t square   : 1;

        uint8_t ps       : 1;
        uint8_t share    : 1;
        uint8_t options  : 1;
        uint8_t touchpad : 1;
    };

    enum BatteryStatus
    {
        kShutdown = 0x01,
        kDying    = 0x02,
        kLow      = 0x03,
        kHigh     = 0x04,
        kFull     = 0x05,
        kCharging = 0xEE
    };

    enum ConnectionStatus
    {
        kUSB,
        kBluetooth
    };

    struct Status
    {
        enum BatteryStatus battery;
        enum ConnectionStatus connection;
        uint8_t charging : 1;
        uint8_t rumbling : 1;
        uint8_t audio : 1;
        uint8_t mic : 1;
    };

    /********************/
    /*   S E N S O R S  */
    /********************/

    struct Gyroscope
    {
        int16_t z;
    };

    struct Accelerometer
    {
        int16_t x;
        int16_t y;
        int16_t z;
    };

    struct Sensor
    {
        Accelerometer accelerometer;
        Gyroscope gyroscope;
    };

    struct Event
    {
        Button button_down;
        Button button_up;
        Analog analog_changed;
    };

    struct State
    {
        Analog analog;
        Button button;
        Status status;
        Sensor sensor;
    };

    State state;
    Event event;

    JoystickController() :
        fConnected(false),
        fConnecting(false),
        fCongested(false)
    {
    }

    inline bool isConnected() const  { return fConnected;  }
    inline bool isConnecting() const { return fConnecting; }
    inline bool isCongested() const  { return fCongested;  }

    virtual void disconnect() {}

    virtual void notify() {}
    virtual void onConnect() {}
    virtual void onDisconnect() {}

protected:
    bool fConnected;
    bool fConnecting;
    bool fCongested;
};

class PSController : public JoystickController
{
public:
    enum Type
    {
        kPS3,
        kPS3Nav,
        kPS4
    };

    PSController(const char* mac = nullptr, Type type = kPS3);

    static bool startListening(const char* mac = nullptr);
    static bool startListening(String& mac) { return startListening(mac.c_str()); }
    static bool stopListening();

    static String getDeviceAddress();

    void setPlayer(int player);
    void setRumble(float leftIntensity, int leftDuration, float rightIntensity, int rightDuration);
    void setRumble(float intensity, int duration = -1)
    {
        setRumble(intensity, duration, intensity, duration);
    }
    virtual void disconnect() override;

    class priv;
private:
    friend class L2CAP;
    void parsePacket(uint8_t* packet);

    Type fType;
    int fPlayer;
    uint16_t fHIDC;
    uint16_t fHIDI;
    uint8_t fBDAddr[6];
    State fState;
};

#endif
