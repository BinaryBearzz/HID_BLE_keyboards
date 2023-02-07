#include <Arduino.h>

enum readType { Digital, Analog};
enum inputType { volume, sw};

typedef std::function<void()> onVolumeUp;
typedef std::function<void()> onVolumeDown;
typedef std::function<void(boolean state)> onSW;

class EventVolume
{
private:
    int pinData = -1;
    int previousValue = 0;
    int _inMin = 0;
    int _inMax = 0;
    int _nml_value = 0;
    int currentStateSW = 0;
    int lastStateSW = 0;
    int readValue();
    readType _readType;
    inputType _inputType;
public:
    EventVolume(int pin, readType readType);
    EventVolume(int pin, int inMin, int inMax, readType readType);
    void calibrate(int inMin, int inMax);
    ~EventVolume();
    void handler();
    void onEventVolumeUp(onVolumeUp ovU);
    void onEventVolumeDown(onVolumeDown ovD);
    void onEventSW(onSW oSW);
};
