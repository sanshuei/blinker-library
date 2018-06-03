#ifndef BlinkerApi_H
#define BlinkerApi_H

#include <time.h>
#if defined(ESP8266) || defined(ESP32)
    #include <Ticker.h>
    #include <EEPROM.h>
#endif
#include <Blinker/BlinkerConfig.h>
#include <utility/BlinkerDebug.h>
#include <utility/BlinkerUtility.h>

enum b_widgettype_t {
    W_BUTTON,
    W_SLIDER,
    W_TOGGLE,
    W_RGB
};

enum b_joystickaxis_t {
    J_Xaxis,
    J_Yaxis
};

enum b_ahrsattitude_t {
    Yaw,
    Pitch,
    Roll
};

enum b_gps_t {
    LONG,
    LAT
};

enum b_rgb_t {
    R,
    G,
    B
};

static class BlinkerButton * _Button[BLINKER_MAX_WIDGET_SIZE];
static class BlinkerSlider * _Slider[BLINKER_MAX_WIDGET_SIZE];
static class BlinkerToggle * _Toggle[BLINKER_MAX_WIDGET_SIZE];
static class BlinkerRGB * _RGB[BLINKER_MAX_WIDGET_SIZE];

class BlinkerButton
{
    public :
        BlinkerButton()
            : buttonName(NULL), buttonState(false)
        {}
        
        void name(String name) { buttonName = name; }
        String getName() { return buttonName; }
        void freshState(bool state, bool isLong = false) { buttonState = state; isLPress = isLong; }
        bool getState() { return buttonState; }
        bool longPress() { return isLPress; }
        bool checkName(String name) { return ((buttonName == name) ? true : false); }
    
    private :
        String  buttonName;
        bool    buttonState;
        bool    isLPress;
};

class BlinkerSlider
{
    public :
        BlinkerSlider()
            : sliderName(NULL), sliderValue(0)
        {}
        
        void name(String name) { sliderName = name; }
        String getName() { return sliderName; }
        void freshValue(uint8_t value) { sliderValue = value; }
        uint8_t getValue() { return sliderValue; }
        bool checkName(String name) { return ((sliderName == name) ? true : false); }
    
    private :
        String  sliderName;
        uint8_t sliderValue;
};

class BlinkerToggle
{
    public :
        BlinkerToggle()
            : toggleName(NULL), toggleState(false)
        {}
        
        void name(String name) { toggleName = name; }
        String getName() { return toggleName; }
        void freshState(bool state) { toggleState = state; }
        bool getState() { return toggleState; }
        bool checkName(String name) { return ((toggleName == name) ? true : false); }
    
    private :
        String  toggleName;
        bool    toggleState;
};

class BlinkerRGB
{
    public :
        BlinkerRGB()
            : rgbName(NULL)
        {}
        
        void name(String name) { rgbName = name; }
        String getName() { return rgbName; }
        void freshValue(b_rgb_t color,uint8_t value) { rgbValue[color] = value; }
        uint8_t getValue(b_rgb_t color) { return rgbValue[color]; }
        bool checkName(String name) { return ((rgbName == name) ? true : false); }
    
    private :
        String  rgbName;
        uint8_t rgbValue[3] = {0};
};

template <class T>
int8_t checkNum(String name, T * c, uint8_t count)
{
    for (uint8_t cNum = 0; cNum < count; cNum++) {
        if (c[cNum]->checkName(name))
            return cNum;
    }

    return BLINKER_OBJECT_NOT_AVAIL;
}

#if defined(ESP8266) || defined(ESP32)
class BlinkerTimer
    : public Ticker
{
    public :
        typedef void (*callback_t)(void);
	    typedef void (*callback_with_arg_t)(void*);

        void countdown(float seconds, callback_t callback) {
            CDowner.once(seconds, callback);
        }

        // template<typename TArg>
        // void countdown(float seconds, void (*callback)(TArg), TArg arg) {
        //     CDowner::once(seconds, arg);
        // }

        void loop(float seconds, callback_t callback) {
            Looper.attach(seconds, callback);
        }

        // template<typename TArg>
        // void loop(float seconds, void (*callback)(TArg), TArg arg, uint8_t times) {
        //     Timer::attach(seconds, arg);
        // }

        void timing(float seconds1, callback_t callback1, float seconds2 , callback_t callback2) {
            Timinger1.once(seconds1, callback1);
            Timinger2.once(seconds1 + seconds2, callback2);
        }

        void detach() {
            CDowner.detach();
            Looper.detach();
            Timinger1.detach();
            Timinger2.detach();
        }

    private :
        Ticker CDowner;
        Ticker Looper;
        Ticker Timinger1;
        Ticker Timinger2;
};
#endif

template <class Proto>
class BlinkerApi
{
    public :
        BlinkerApi() {
            joyValue[J_Xaxis] = BLINKER_JOYSTICK_VALUE_DEFAULT;
            joyValue[J_Yaxis] = BLINKER_JOYSTICK_VALUE_DEFAULT;
            ahrsValue[Yaw] = 0;
            ahrsValue[Roll] = 0;
            ahrsValue[Pitch] = 0;
            gpsValue[LONG] = "0.000000";
            gpsValue[LAT] = "0.000000";
            // rgbValue[R] = 0;
            // rgbValue[G] = 0;
            // rgbValue[B] = 0;
        }

        void wInit(const String & _name, b_widgettype_t _type) {
            switch (_type) {
                case W_BUTTON :
                    if (checkNum(_name, _Button, _bCount) == BLINKER_OBJECT_NOT_AVAIL) {
                        if ( _bCount < BLINKER_MAX_WIDGET_SIZE ) {
                            _Button[_bCount] = new BlinkerButton();
                            _Button[_bCount]->name(_name);
                            _bCount++;
                        }
                    }
                    break;
                case W_SLIDER :
                    if (checkNum(_name, _Slider, _sCount) == BLINKER_OBJECT_NOT_AVAIL) {
                        if ( _sCount < BLINKER_MAX_WIDGET_SIZE ) {
                            _Slider[_sCount] = new BlinkerSlider();
                            _Slider[_sCount]->name(_name);
                            _sCount++;
                        }
                    }
                    break;
                case W_TOGGLE :
                    if (checkNum(_name, _Toggle, _tCount) == BLINKER_OBJECT_NOT_AVAIL) {
                        if ( _tCount < BLINKER_MAX_WIDGET_SIZE ) {
                            _Toggle[_tCount] = new BlinkerToggle();
                            _Toggle[_tCount]->name(_name);
                            _tCount++;
                        }
                    }
                    break;
                case W_RGB :
                    if (checkNum(_name, _RGB, _rgbCount) == BLINKER_OBJECT_NOT_AVAIL) {
                        if ( _rgbCount < BLINKER_MAX_WIDGET_SIZE ) {
                            _RGB[_rgbCount] = new BlinkerRGB();
                            _RGB[_rgbCount]->name(_name);
                            _rgbCount++;
                        }
                    }
                    break;
                default :
                    break;
            }
        }

        bool button(const String & _bName)
        {
            int8_t num = checkNum(_bName, _Button, _bCount);
            String state;

            if (STRING_find_string_value(static_cast<Proto*>(this)->dataParse(), state, _bName)) {
                _fresh = true;
            }

            if (state == BLINKER_CMD_BUTTON_TAP) {
                if( num == BLINKER_OBJECT_NOT_AVAIL ) {
                    if ( _bCount < BLINKER_MAX_WIDGET_SIZE ) {
                        _Button[_bCount] = new BlinkerButton();
                        _Button[_bCount]->name(_bName);
                        _Button[_bCount]->freshState(true);
                        _bCount++;
                    }
                }
                else {
                    _Button[num]->freshState(true);
                }

                _fresh = true;
                return true;
            }
            else if (state == BLINKER_CMD_BUTTON_PRESSED) {
                if( num == BLINKER_OBJECT_NOT_AVAIL ) {
                    if ( _bCount < BLINKER_MAX_WIDGET_SIZE ) {
                        _Button[_bCount] = new BlinkerButton();
                        _Button[_bCount]->name(_bName);
                        _Button[_bCount]->freshState(true, true);
                        _bCount++;
                    }
                }
                else {
                    _Button[num]->freshState(true, true);
                }

                _fresh = true;
                return true;
            }
            else if (state == BLINKER_CMD_BUTTON_RELEASED) {
                if( num == BLINKER_OBJECT_NOT_AVAIL ) {
                    if ( _bCount < BLINKER_MAX_WIDGET_SIZE ) {
                        _Button[_bCount] = new BlinkerButton();
                        _Button[_bCount]->name(_bName);
                        _Button[_bCount]->freshState(false);
                        _bCount++;
                    }
                }
                else {
                    _Button[num]->freshState(false);
                }

                _fresh = true;
                return false;
            }
            else {
                if( num == BLINKER_OBJECT_NOT_AVAIL ) {
                    if ( _bCount < BLINKER_MAX_WIDGET_SIZE ) {
                        _Button[_bCount] = new BlinkerButton();
                        _Button[_bCount]->name(_bName);
                        _bCount++;
                    }
                    return false;
                }

                bool _state = _Button[num]->getState();
                if ( !_Button[num]->longPress() )
                    _Button[num]->freshState(false);

                return _state;
            }
        }

        bool toggle(const String & _tName)
        {
            int8_t num = checkNum(_tName, _Toggle, _tCount);
            String state;

            if (STRING_find_string_value(static_cast<Proto*>(this)->dataParse(), state, _tName)) {
                _fresh = true;
            }

            if (state == BLINKER_CMD_ON) {
                if( num == BLINKER_OBJECT_NOT_AVAIL ) {
                    if ( _tCount < BLINKER_MAX_WIDGET_SIZE ) {
                        _Toggle[_tCount] = new BlinkerToggle();
                        _Toggle[_tCount]->name(_tName);
                        _Toggle[_tCount]->freshState(true);
                        _tCount++;
                    }
                }
                else {
                    _Toggle[num]->freshState(true);
                }

                _fresh = true;
                return true;
            }
            else if (state == BLINKER_CMD_OFF) {
                if( num == BLINKER_OBJECT_NOT_AVAIL ) {
                    if ( _tCount < BLINKER_MAX_WIDGET_SIZE ) {
                        _Toggle[_tCount] = new BlinkerToggle();
                        _Toggle[_tCount]->name(_tName);
                        _Toggle[_tCount]->freshState(false);
                        _tCount++;
                    }
                }
                else {
                    _Toggle[num]->freshState(false);
                }

                _fresh = true;
                return false;
            }
            else {
                if( num == BLINKER_OBJECT_NOT_AVAIL ) {
                    if ( _tCount < BLINKER_MAX_WIDGET_SIZE ) {
                        _Toggle[_tCount] = new BlinkerToggle();
                        _Toggle[_tCount]->name(_tName);
                        _tCount++;
                    }
                    return false;
                }

                return _Toggle[num]->getState();
            }
        }

        uint8_t slider(const String & _sName)
        {
            int8_t num = checkNum(_sName, _Slider, _sCount);
            int16_t value = STRING_find_numberic_value(static_cast<Proto*>(this)->dataParse(), _sName);

            if (value != FIND_KEY_VALUE_FAILED) {
                if( num == BLINKER_OBJECT_NOT_AVAIL ) {
                    if ( _sCount < BLINKER_MAX_WIDGET_SIZE ) {
                        _Slider[_sCount] = new BlinkerSlider();
                        _Slider[_sCount]->name(_sName);
                        _Slider[_sCount]->freshValue(value);
                        _sCount++;
                    }
                }
                else {
                    _Slider[num]->freshValue(value);
                }

                _fresh = true;
                return value;
            }
            else {
                if( num == BLINKER_OBJECT_NOT_AVAIL ) {
                    if ( _sCount < BLINKER_MAX_WIDGET_SIZE ) {
                        _Slider[_sCount] = new BlinkerSlider();
                        _Slider[_sCount]->name(_sName);
                        _sCount++;
                    }
                    return 0;
                }
                
                return _Slider[num]->getValue();
            }
        }

        uint8_t joystick(b_joystickaxis_t axis)
        {
            int16_t jAxisValue = STRING_find_array_numberic_value(static_cast<Proto*>(this)->dataParse(), BLINKER_CMD_JOYSTICK, axis);

            if (jAxisValue != FIND_KEY_VALUE_FAILED) {
                joyValue[J_Xaxis] = STRING_find_array_numberic_value(static_cast<Proto*>(this)->dataParse(), BLINKER_CMD_JOYSTICK, J_Xaxis);
                joyValue[J_Yaxis] = STRING_find_array_numberic_value(static_cast<Proto*>(this)->dataParse(), BLINKER_CMD_JOYSTICK, J_Yaxis);

                _fresh = true;
                return jAxisValue;
            }
            else {
                return joyValue[axis];
            }
        }

        int16_t ahrs(b_ahrsattitude_t attitude)
        {
            int16_t aAttiValue = STRING_find_array_numberic_value(static_cast<Proto*>(this)->dataParse(), BLINKER_CMD_AHRS, attitude);

            if (aAttiValue != FIND_KEY_VALUE_FAILED) {
                ahrsValue[Yaw] = STRING_find_array_numberic_value(static_cast<Proto*>(this)->dataParse(), BLINKER_CMD_AHRS, Yaw);
                ahrsValue[Roll] = STRING_find_array_numberic_value(static_cast<Proto*>(this)->dataParse(), BLINKER_CMD_AHRS, Roll);
                ahrsValue[Pitch] = STRING_find_array_numberic_value(static_cast<Proto*>(this)->dataParse(), BLINKER_CMD_AHRS, Pitch);

                _fresh = true;

                return aAttiValue;
            }
            else {
                return ahrsValue[attitude];
            }
        }
        
        void attachAhrs()
        {
            bool state = false;
            uint32_t startTime = millis();
            static_cast<Proto*>(this)->print(BLINKER_CMD_AHRS, BLINKER_CMD_ON);
            while (!state) {
                while (!static_cast<Proto*>(this)->connected()) {
                    static_cast<Proto*>(this)->run();
                    if (static_cast<Proto*>(this)->connect()) {
                        static_cast<Proto*>(this)->print(BLINKER_CMD_AHRS, BLINKER_CMD_ON);
                        break;
                    }
                }
                
                ::delay(100);

                if (static_cast<Proto*>(this)->checkAvail()) {
                    BLINKER_LOG2("GET: ", static_cast<Proto*>(this)->dataParse());
                    if (STRING_contais_string(static_cast<Proto*>(this)->dataParse(), BLINKER_CMD_AHRS)) {
                        BLINKER_LOG1("AHRS attach sucessed...");
                        parse();
                        state = true;
                        break;
                    }
                    else {
                        BLINKER_LOG1("AHRS attach failed...Try again");
                        startTime = millis();
                        static_cast<Proto*>(this)->print(BLINKER_CMD_AHRS, BLINKER_CMD_ON);
                    }
                }
                else {
                    if (millis() - startTime > BLINKER_CONNECT_TIMEOUT_MS) {
                        BLINKER_LOG1("AHRS attach failed...Try again");
                        startTime = millis();
                        static_cast<Proto*>(this)->print(BLINKER_CMD_AHRS, BLINKER_CMD_ON);
                    }
                }
            }
        }

        void detachAhrs()
        {
            static_cast<Proto*>(this)->print(BLINKER_CMD_AHRS, BLINKER_CMD_OFF);
            ahrsValue[Yaw] = 0;
            ahrsValue[Roll] = 0;
            ahrsValue[Pitch] = 0;
        }

        String gps(b_gps_t axis, bool newData = false) {
            if (!newData && (millis() - gps_get_time) >= BLINKER_GPS_MSG_LIMIT) {
                static_cast<Proto*>(this)->print(BLINKER_CMD_GET, BLINKER_CMD_GPS);
                delay(100);

                gps_get_time = millis();
            }

            String axisValue = STRING_find_array_string_value(static_cast<Proto*>(this)->dataParse(), BLINKER_CMD_GPS, axis);

            if (axisValue != "") {
                gpsValue[LONG] = STRING_find_array_string_value(static_cast<Proto*>(this)->dataParse(), BLINKER_CMD_GPS, LONG);
                gpsValue[LAT] = STRING_find_array_string_value(static_cast<Proto*>(this)->dataParse(), BLINKER_CMD_GPS, LAT);

                _fresh = true;

                if (_fresh) {
                    static_cast<Proto*>(this)->isParsed();
                }

                return gpsValue[axis];
            }
            else {
                return gpsValue[axis];
            }
        }

        uint8_t rgb(const String & _rgbName, b_rgb_t color) {
            int8_t num = checkNum(_rgbName, _RGB, _rgbCount);
            int16_t value = STRING_find_array_numberic_value(static_cast<Proto*>(this)->dataParse(), _rgbName, color);

            if (value != FIND_KEY_VALUE_FAILED) {
                uint8_t _rValue = STRING_find_array_numberic_value(static_cast<Proto*>(this)->dataParse(), _rgbName, R);
                uint8_t _gValue = STRING_find_array_numberic_value(static_cast<Proto*>(this)->dataParse(), _rgbName, G);
                uint8_t _bValue = STRING_find_array_numberic_value(static_cast<Proto*>(this)->dataParse(), _rgbName, B);

                if( num == BLINKER_OBJECT_NOT_AVAIL ) {
                    if ( _rgbCount < BLINKER_MAX_WIDGET_SIZE ) {
                        _RGB[_rgbCount] = new BlinkerRGB();
                        _RGB[_rgbCount]->name(_rgbName);
                        _RGB[_rgbCount]->freshValue(R, _rValue);
                        _RGB[_rgbCount]->freshValue(G, _gValue);
                        _RGB[_rgbCount]->freshValue(B, _bValue);
                        _rgbCount++;
                    }
                }
                else {
                    _RGB[num]->freshValue(R, _rValue);
                    _RGB[num]->freshValue(G, _gValue);
                    _RGB[num]->freshValue(B, _bValue);
                }

                _fresh = true;
                return value;
            }
            else {
                if( num == BLINKER_OBJECT_NOT_AVAIL ) {
                    if ( _rgbCount < BLINKER_MAX_WIDGET_SIZE ) {
                        _RGB[_rgbCount] = new BlinkerRGB();
                        _RGB[_rgbCount]->name(_rgbName);
                        _rgbCount++;
                    }
                    return 0;
                }
                
                return _RGB[num]->getValue(color);
            }
        }

        // void freshGPS()
        // {
        //     static_cast<Proto*>(this)->print(BLINKER_CMD_GPS, "");
        //     delay(100);
        // }

        void vibrate(uint16_t ms = 200)
        {
            if (ms > 1000) {
                ms = 1000;
            }

            static_cast<Proto*>(this)->print(BLINKER_CMD_VIBRATE, ms);
        }

        void delay(unsigned long ms)
        {
            uint32_t start = micros();
            while (ms > 0) {
                static_cast<Proto*>(this)->run();

                yield();
                
                if ((micros() - start)/1000 >= ms) {
                    ms = 0;
                }
            }
        }

        void setTimezone(float tz) {
            _timezone = tz;
        }

// #if defined(ESP8266) || defined(ESP32)
//         bool ntpInit() {
//             if (!_isNTPInit) {
//                 now_ntp = ::time(nullptr);
            
//                 // BLINKER_LOG2("Setting time using SNTP: ", now_ntp);
                
//                 if (now_ntp < _timezone * 3600 * 2) {
//                     configTime(_timezone * 3600, 0, "ntp1.aliyun.com", "210.72.145.44", "time.pool.aliyun.com");// cn.pool.ntp.org
//                     now_ntp = ::time(nullptr);

//                     if (now_ntp < _timezone * 3600 * 2) {
//                         ::delay(50);

//                         now_ntp = ::time(nullptr);

//                         // BLINKER_LOG2("Setting time using SNTP time out: ", now_ntp);

//                         return false;
//                     }
//                 }
//                 // struct tm timeinfo;
//                 gmtime_r(&now_ntp, &timeinfo);
// #ifdef BLINKER_DEBUG_ALL                
//                 BLINKER_LOG2("Current time: ", asctime(&timeinfo));
// #endif
//                 _isNTPInit = true;
//             }

//             return true;
//         }
// #endif

        int8_t second()    { freshNTP(); return _isNTPInit ? timeinfo.tm_sec : -1; }
        /**< seconds after the minute - [ 0 to 59 ] */
        int8_t minute()    { freshNTP(); return _isNTPInit ? timeinfo.tm_min : -1; }
        /**< minutes after the hour - [ 0 to 59 ] */
        int8_t hour()   { freshNTP(); return _isNTPInit ? timeinfo.tm_hour : -1; }
        /**< hours since midnight - [ 0 to 23 ] */
        int8_t mday()   { freshNTP(); return _isNTPInit ? timeinfo.tm_mday : -1; }
        /**< day of the month - [ 1 to 31 ] */
        int8_t wday()   { freshNTP(); return _isNTPInit ? timeinfo.tm_wday : -1; }
        /**< days since Sunday - [ 0 to 6 ] */
        int8_t month()    { freshNTP(); return _isNTPInit ? timeinfo.tm_mon : -1; }
        /**< months since January - [ 0 to 11 ] */
        int16_t year()  { freshNTP(); return _isNTPInit ? 1900 + timeinfo.tm_year : -1; }
        /**< years since 1900 */
        int16_t yday()  { freshNTP(); return _isNTPInit ? timeinfo.tm_yday : -1; }
        /**< days since January 1 - [ 0 to 365 ] */
        time_t  time()  { freshNTP(); return _isNTPInit ? now_ntp : millis(); }

        int32_t dtime() {
            freshNTP();
            return _isNTPInit ? timeinfo.tm_hour * 60 * 60 + timeinfo.tm_min * 60 + timeinfo.tm_sec : -1;
        }

#if defined(BLINKER_MQTT)
        void beginAuto() {
            BLINKER_LOG1("=======================================================");
            BLINKER_LOG1("=========== Blinker Auto Control mode init! ===========");
            BLINKER_LOG1("Warning! EEPROM address 0-255 is used for Auto Control!");
            BLINKER_LOG1("=======================================================");

            deserialization();
        }

        void autoRun(String state) {
#ifdef BLINKER_DEBUG_ALL            
            BLINKER_LOG2("autoRun state: ", state);
#endif
            if (!_isNTPInit || !_autoState) {
                return;
            }

            int32_t nowTime = dtime();
            if (_time1 < _time2) {
                if (!(nowTime >= _time1 && nowTime <= _time2)) {
#ifdef BLINKER_DEBUG_ALL
                    BLINKER_LOG2("out of time slot: ", nowTime);
#endif
                    return;
                }
            }
            else if (_time1 > _time2) {
                if (nowTime > _time1 && nowTime < _time2) {
#ifdef BLINKER_DEBUG_ALL
                    BLINKER_LOG2("out of time slot: ", nowTime);
#endif
                    return;
                }
            }

            if (state == BLINKER_CMD_ON) {
                if (_targetState) {
                    if (!isTrigged) {
                        triggerCheck("on");
                    }
                }
                else {
                    isTrigged = false;
                    isRecord = false;
                }
            }
            else if (state == BLINKER_CMD_OFF) {
                if (!_targetState) {
                    if (!isTrigged) {
                        triggerCheck("off");
                    }
                }
                else {
                    isTrigged = false;
                    isRecord = false;
                }
            }
        }

        void autoRun(float data) {
#ifdef BLINKER_DEBUG_ALL
            BLINKER_LOG2("autoRun data: ", data);
#endif
            if (!_isNTPInit || !_autoState) {
                return;
            }

            int32_t nowTime = dtime();
            if (_time1 < _time2) {
                if (!(nowTime >= _time1 && nowTime <= _time2)) {
#ifdef BLINKER_DEBUG_ALL
                    BLINKER_LOG2("out of time slot: ", nowTime);
#endif
                    return;
                }
            }
            else if (_time1 > _time2) {
                if (nowTime > _time1 && nowTime < _time2) {
#ifdef BLINKER_DEBUG_ALL
                    BLINKER_LOG2("out of time slot: ", nowTime);
#endif
                    return;
                }
            }

            switch (_compareType) {
                case BLINKER_COMPARE_LESS:
                    if (data < _targetData) {
                        if (!isTrigged) {
                            triggerCheck("less");
                        }
                    }
                    else {
                        isTrigged = false;
                        isRecord = false;
                    }
                    break;
                case BLINKER_COMPARE_EQUAL:
                    if (data = _targetData) {
                        if (!isTrigged) {
                            triggerCheck("equal");
                        }
                    }
                    else {
                        isTrigged = false;
                        isRecord = false;
                    }
                    break;
                case BLINKER_COMPARE_GREATER:
                    if (data > _targetData) {
                        if (!isTrigged) {
                            triggerCheck("greater");
                        }
                    }
                    else {
                        isTrigged = false;
                        isRecord = false;
                    }
                    break;
                default:
                    break;
            }
        }
// #else
//     #pragma message("This code is intended to run with BLINKER_MQTT! Please check your connect type.")
#endif
    
    private :
        uint8_t     _bCount = 0;
        uint8_t     _sCount = 0;
        uint8_t     _tCount = 0;
        uint8_t     _rgbCount = 0;
        uint8_t     joyValue[2];
        int16_t     ahrsValue[3];
        uint32_t    gps_get_time;
        String      gpsValue[2];
        // uint8_t rgbValue[3];
        bool        _fresh = false;
        bool        _isNTPInit = false;
        float       _timezone = 8.0;
        uint32_t    _ntpStart;
        time_t      now_ntp;
        struct tm   timeinfo;

#if defined(BLINKER_MQTT)
        // - - - - - - - -  - - - - - - - -  - - - - - - - -  - - - - - - - -
        // | | | | |            | _time1 0-1440min 11  | _time2 0-1440min 11                   
        // | | | | | _duration 0-60min 6
        // | | | | _targetState|_compareType on/off|less/equal/greater 2
        // | | | _targetState|_compareType on/off|less/equal/greater
        // | | _logicType state/numberic 1
        // | _autoState true/false 1
        bool        _autoState = false;
        uint8_t     _logicType;
        float       _targetData;
        uint8_t     _compareType = -1;
        bool        _targetState;
        uint32_t    _time1;
        uint32_t    _time2;
        uint32_t    _duration;
        uint32_t    _treTime;
        bool        isRecord = false;
        bool        isTrigged = false;
        String      _linkDevice;
        String      _linkType;
        String      _linkData;
        uint32_t    _autoData;
#endif

        void freshNTP() {
            if (_isNTPInit) {
                now_ntp = ::time(nullptr);
                gmtime_r(&now_ntp, &timeinfo);
            }
        }

        bool buttonParse(const String & _bName)
        {
            int8_t num = checkNum(_bName, _Button, _bCount);
            String state;

            if (STRING_find_string_value(static_cast<Proto*>(this)->dataParse(), state, _bName)) {
                _fresh = true;
            }

            if (state == BLINKER_CMD_BUTTON_TAP) {
                if( num == BLINKER_OBJECT_NOT_AVAIL ) {
                    if ( _bCount < BLINKER_MAX_WIDGET_SIZE ) {
                        _Button[_bCount] = new BlinkerButton();
                        _Button[_bCount]->name(_bName);
                        _Button[_bCount]->freshState(true);
                        _bCount++;
                    }
                }
                else {
                    _Button[num]->freshState(true);
                }

                _fresh = true;
                return true;
            }
            else  if (state == BLINKER_CMD_BUTTON_PRESSED) {
                if( num == BLINKER_OBJECT_NOT_AVAIL ) {
                    if ( _bCount < BLINKER_MAX_WIDGET_SIZE ) {
                        _Button[_bCount] = new BlinkerButton();
                        _Button[_bCount]->name(_bName);
                        _Button[_bCount]->freshState(true, true);
                        _bCount++;
                    }
                }
                else {
                    _Button[num]->freshState(true, true);
                }

                _fresh = true;
                return true;
            }
            else if (state == BLINKER_CMD_BUTTON_RELEASED) {
                if( num == BLINKER_OBJECT_NOT_AVAIL ) {
                    if ( _bCount < BLINKER_MAX_WIDGET_SIZE ) {
                        _Button[_bCount] = new BlinkerButton();
                        _Button[_bCount]->name(_bName);
                        _Button[_bCount]->freshState(false);
                        _bCount++;
                    }
                }
                else {
                    _Button[num]->freshState(false);
                }

                _fresh = true;
                return false;
            }
            else {
                if( num == BLINKER_OBJECT_NOT_AVAIL ) {
                    if ( _bCount < BLINKER_MAX_WIDGET_SIZE ) {
                        _Button[_bCount] = new BlinkerButton();
                        _Button[_bCount]->name(_bName);
                        _bCount++;
                    }
                    return false;
                }

                return _Button[num]->getState();
            }
        }

        bool buttonParse(const String & _bName, String data)
        {
            int8_t num = checkNum(_bName, _Button, _bCount);
            String state;

            if (STRING_find_string_value(data, state, _bName)) {
                // _fresh = true;
            }

            if (state == BLINKER_CMD_BUTTON_TAP) {
                if( num == BLINKER_OBJECT_NOT_AVAIL ) {
                    if ( _bCount < BLINKER_MAX_WIDGET_SIZE ) {
                        _Button[_bCount] = new BlinkerButton();
                        _Button[_bCount]->name(_bName);
                        _Button[_bCount]->freshState(true);
                        _bCount++;
                    }
                }
                else {
                    _Button[num]->freshState(true);
                }

                // _fresh = true;
                return true;
            }
            else  if (state == BLINKER_CMD_BUTTON_PRESSED) {
                if( num == BLINKER_OBJECT_NOT_AVAIL ) {
                    if ( _bCount < BLINKER_MAX_WIDGET_SIZE ) {
                        _Button[_bCount] = new BlinkerButton();
                        _Button[_bCount]->name(_bName);
                        _Button[_bCount]->freshState(true, true);
                        _bCount++;
                    }
                }
                else {
                    _Button[num]->freshState(true, true);
                }

                // _fresh = true;
                return true;
            }
            else if (state == BLINKER_CMD_BUTTON_RELEASED) {
                if( num == BLINKER_OBJECT_NOT_AVAIL ) {
                    if ( _bCount < BLINKER_MAX_WIDGET_SIZE ) {
                        _Button[_bCount] = new BlinkerButton();
                        _Button[_bCount]->name(_bName);
                        _Button[_bCount]->freshState(false);
                        _bCount++;
                    }
                }
                else {
                    _Button[num]->freshState(false);
                }

                // _fresh = true;
                return false;
            }
            else {
                if( num == BLINKER_OBJECT_NOT_AVAIL ) {
                    if ( _bCount < BLINKER_MAX_WIDGET_SIZE ) {
                        _Button[_bCount] = new BlinkerButton();
                        _Button[_bCount]->name(_bName);
                        _bCount++;
                    }
                    return false;
                }

                return _Button[num]->getState();
            }
        }

        bool toggle(const String & _tName, String data)
        {
            int8_t num = checkNum(_tName, _Toggle, _tCount);
            String state;

            if (STRING_find_string_value(data, state, _tName)) {
                // _fresh = true;
            }

            if (state == BLINKER_CMD_ON) {
                if( num == BLINKER_OBJECT_NOT_AVAIL ) {
                    if ( _tCount < BLINKER_MAX_WIDGET_SIZE ) {
                        _Toggle[_tCount] = new BlinkerToggle();
                        _Toggle[_tCount]->name(_tName);
                        _Toggle[_tCount]->freshState(true);
                        _tCount++;
                    }
                }
                else {
                    _Toggle[num]->freshState(true);
                }

                // _fresh = true;
                return true;
            }
            else if (state == BLINKER_CMD_OFF) {
                if( num == BLINKER_OBJECT_NOT_AVAIL ) {
                    if ( _tCount < BLINKER_MAX_WIDGET_SIZE ) {
                        _Toggle[_tCount] = new BlinkerToggle();
                        _Toggle[_tCount]->name(_tName);
                        _Toggle[_tCount]->freshState(false);
                        _tCount++;
                    }
                }
                else {
                    _Toggle[num]->freshState(false);
                }

                // _fresh = true;
                return false;
            }
            else {
                if( num == BLINKER_OBJECT_NOT_AVAIL ) {
                    if ( _tCount < BLINKER_MAX_WIDGET_SIZE ) {
                        _Toggle[_tCount] = new BlinkerToggle();
                        _Toggle[_tCount]->name(_tName);
                        _tCount++;
                    }
                    return false;
                }

                return _Toggle[num]->getState();
            }
        }

        uint8_t slider(const String & _sName, String data)
        {
            int8_t num = checkNum(_sName, _Slider, _sCount);
            int16_t value = STRING_find_numberic_value(data, _sName);

            if (value != FIND_KEY_VALUE_FAILED) {
                if( num == BLINKER_OBJECT_NOT_AVAIL ) {
                    if ( _sCount < BLINKER_MAX_WIDGET_SIZE ) {
                        _Slider[_sCount] = new BlinkerSlider();
                        _Slider[_sCount]->name(_sName);
                        _Slider[_sCount]->freshValue(value);
                        _sCount++;
                    }
                }
                else {
                    _Slider[num]->freshValue(value);
                }

                // _fresh = true;
                return value;
            }
            else {
                if( num == BLINKER_OBJECT_NOT_AVAIL ) {
                    if ( _sCount < BLINKER_MAX_WIDGET_SIZE ) {
                        _Slider[_sCount] = new BlinkerSlider();
                        _Slider[_sCount]->name(_sName);
                        _sCount++;
                    }
                    return 0;
                }
                
                return _Slider[num]->getValue();
            }
        }

        uint8_t rgb(const String & _rgbName, b_rgb_t color, String data) {
            int8_t num = checkNum(_rgbName, _RGB, _rgbCount);
            int16_t value = STRING_find_array_numberic_value(data, _rgbName, color);

            if (value != FIND_KEY_VALUE_FAILED) {
                uint8_t _rValue = STRING_find_array_numberic_value(data, _rgbName, R);
                uint8_t _gValue = STRING_find_array_numberic_value(data, _rgbName, G);
                uint8_t _bValue = STRING_find_array_numberic_value(data, _rgbName, B);

                if( num == BLINKER_OBJECT_NOT_AVAIL ) {
                    if ( _rgbCount < BLINKER_MAX_WIDGET_SIZE ) {
                        _RGB[_rgbCount] = new BlinkerRGB();
                        _RGB[_rgbCount]->name(_rgbName);
                        _RGB[_rgbCount]->freshValue(R, _rValue);
                        _RGB[_rgbCount]->freshValue(G, _gValue);
                        _RGB[_rgbCount]->freshValue(B, _bValue);
                        _rgbCount++;
                    }
                }
                else {
                    _RGB[num]->freshValue(R, _rValue);
                    _RGB[num]->freshValue(G, _gValue);
                    _RGB[num]->freshValue(B, _bValue);
                }

                // _fresh = true;
                return value;
            }
            else {
                if( num == BLINKER_OBJECT_NOT_AVAIL ) {
                    if ( _rgbCount < BLINKER_MAX_WIDGET_SIZE ) {
                        _RGB[_rgbCount] = new BlinkerRGB();
                        _RGB[_rgbCount]->name(_rgbName);
                        _rgbCount++;
                    }
                    return 0;
                }
                
                return _RGB[num]->getValue(color);
            }
        }

        void heartBeat() {
            String state;

            if (STRING_find_string_value(static_cast<Proto*>(this)->dataParse(), state, BLINKER_CMD_GET)) {
                // _fresh = true;
                if (state == BLINKER_CMD_STATE) {
#if defined(BLINKER_MQTT)
                    static_cast<Proto*>(this)->beginFormat();
                    static_cast<Proto*>(this)->print(BLINKER_CMD_STATE, BLINKER_CMD_ONLINE);
                    stateData();
                    if (!static_cast<Proto*>(this)->endFormat()) {
                        static_cast<Proto*>(this)->print(BLINKER_CMD_STATE, BLINKER_CMD_ONLINE);
                    }
#else
                    static_cast<Proto*>(this)->beginFormat();
                    static_cast<Proto*>(this)->print(BLINKER_CMD_STATE, BLINKER_CMD_CONNECTED);
                    stateData();
                    if (!static_cast<Proto*>(this)->endFormat()) {
                        static_cast<Proto*>(this)->print(BLINKER_CMD_STATE, BLINKER_CMD_CONNECTED);
                    }
#endif
                    _fresh = true;
                }
            }
        }

        void getVersion() {
            String state;

            if (STRING_find_string_value(static_cast<Proto*>(this)->dataParse(), state, BLINKER_CMD_GET)) {
                // _fresh = true;
                if (state == BLINKER_CMD_VERSION) {
                    static_cast<Proto*>(this)->print(BLINKER_CMD_VERSION, BLINKER_VERSION);
                    _fresh = true;
                }
            }
        }        

        void stateData() {
            for (uint8_t _tNum = 0; _tNum < _tCount; _tNum++) {
                static_cast<Proto*>(this)->print(_Toggle[_tNum]->getName(), _Toggle[_tNum]->getState() ? "on" : "off");
            }
            for (uint8_t _sNum = 0; _sNum < _sCount; _sNum++) {
                static_cast<Proto*>(this)->print(_Slider[_sNum]->getName(), _Slider[_sNum]->getValue());
            }
            for (uint8_t _rgbNum = 0; _rgbNum < _rgbCount; _rgbNum++) {
                static_cast<Proto*>(this)->print(_RGB[_rgbNum]->getName(), "[" + STRING_format(_RGB[_rgbNum]->getValue(R)) + "," + STRING_format(_RGB[_rgbNum]->getValue(G)) + "," + STRING_format(_RGB[_rgbNum]->getValue(B)) + "]");
            }
        }

#if defined(BLINKER_MQTT)
        bool autoManager() {
            // String set;
            bool isSet = false;
            bool isAuto = false;

            isSet = STRING_contais_string(static_cast<Proto*>(this)->dataParse(), BLINKER_CMD_SET);
            isAuto = STRING_contais_string(static_cast<Proto*>(this)->dataParse(), BLINKER_CMD_AUTO);

            if (isSet && isAuto) {
                _fresh = true;
#ifdef BLINKER_DEBUG_ALL
                BLINKER_LOG1("get auto setting");
#endif
                String auto_state = STRING_find_string(static_cast<Proto*>(this)->dataParse(), "auto\"", ",", 1);
                if (auto_state == "") {
                    auto_state = STRING_find_string(static_cast<Proto*>(this)->dataParse(), "auto\"", "}", 1);
                }
#ifdef BLINKER_DEBUG_ALL
                BLINKER_LOG2("auto state: ", auto_state);
#endif
                _autoState = (auto_state == BLINKER_CMD_TRUE) ? true : false;

                String logicType;
                if (STRING_find_string_value(static_cast<Proto*>(this)->dataParse(), logicType, BLINKER_CMD_LOGICTYPE)) {
#ifdef BLINKER_DEBUG_ALL
                    BLINKER_LOG2("logicType: ", logicType);
#endif
                    if (logicType == BLINKER_CMD_STATE) {
#ifdef BLINKER_DEBUG_ALL
                        BLINKER_LOG1("state!");
#endif
                        _logicType = BLINKER_TYPE_STATE;
                        String target_state;
                        if (STRING_find_string_value(static_cast<Proto*>(this)->dataParse(), target_state, BLINKER_CMD_TARGETSTATE)) {
                            if (target_state == BLINKER_CMD_ON) {
                                _targetState = true;
                            }
                            else if (target_state == BLINKER_CMD_OFF) {
                                _targetState = false;
                            }
#ifdef BLINKER_DEBUG_ALL
                            BLINKER_LOG2("targetState: ", _targetState);
#endif                            
                        }
                    }
                    else if (logicType == BLINKER_CMD_NUMBERIC) {
#ifdef BLINKER_DEBUG_ALL
                        BLINKER_LOG1("numberic!");
#endif
                        _logicType = BLINKER_TYPE_NUMERIC;
                        String _type;
                        if (STRING_find_string_value(static_cast<Proto*>(this)->dataParse(), _type, BLINKER_CMD_COMPARETYPE)) {
                            if (_type == BLINKER_CMD_LESS) {
                                _compareType = BLINKER_COMPARE_LESS;
                            }
                            else if (_type == BLINKER_CMD_EQUAL) {
                                _compareType = BLINKER_COMPARE_EQUAL;
                            }
                            else if (_type == BLINKER_CMD_GREATER) {
                                _compareType = BLINKER_COMPARE_GREATER;
                            }

                            _targetData = STRING_find_float_value(static_cast<Proto*>(this)->dataParse(), BLINKER_CMD_TARGETDATA);
#ifdef BLINKER_DEBUG_ALL
                            BLINKER_LOG6("_type: ", _type, " _compareType: ", _compareType, " _targetData: ", _targetData);
#endif
                        }
                    }

                    int32_t duValue = STRING_find_numberic_value(static_cast<Proto*>(this)->dataParse(), BLINKER_CMD_DURATION);

                    if (duValue != FIND_KEY_VALUE_FAILED) {
                        _duration = 60 * STRING_find_numberic_value(static_cast<Proto*>(this)->dataParse(), BLINKER_CMD_DURATION);
                    }
                    else {
                        _duration = 0;
                    }
#ifdef BLINKER_DEBUG_ALL
                    BLINKER_LOG2("_duration: ", _duration);
#endif
                    int32_t timeValue = STRING_find_array_numberic_value(static_cast<Proto*>(this)->dataParse(), BLINKER_CMD_TIMESLOT, 0);

                    if (timeValue != FIND_KEY_VALUE_FAILED) {
                        _time1 = 60 * STRING_find_array_numberic_value(static_cast<Proto*>(this)->dataParse(), BLINKER_CMD_TIMESLOT, 0);
                        _time2 = 60 * STRING_find_array_numberic_value(static_cast<Proto*>(this)->dataParse(), BLINKER_CMD_TIMESLOT, 1);
                    }
                    else {
                        _time1 = 0;
                        _time2 = 24 * 60 * 60;
                    }
#ifdef BLINKER_DEBUG_ALL
                    BLINKER_LOG4("_time1: ", _time1, " _time2: ", _time2);
#endif
                    _linkDevice = STRING_find_string(static_cast<Proto*>(this)->dataParse(), BLINKER_CMD_LINKDEVICE, "\"", 3);
                    _linkType = STRING_find_string(static_cast<Proto*>(this)->dataParse(), BLINKER_CMD_LINKTYPE, "\"", 3);
                    _linkData = STRING_find_string(static_cast<Proto*>(this)->dataParse(), BLINKER_CMD_LINKDATA, "}", 3);

#ifdef BLINKER_DEBUG_ALL
                    BLINKER_LOG2("_linkDevice: ", _linkDevice);
                    BLINKER_LOG2("_linkType: ", _linkType);
                    BLINKER_LOG2("_linkData: ", _linkData);
#endif
                    serialization();
                }
                return true;
            }
            else {
                return false;
            }
        }

        void triggerCheck(String state) {
            if (!isRecord) {
                isRecord = true;
                _treTime = millis();
            }

            if ((millis() - _treTime) / 1000 >= _duration) {
#ifdef BLINKER_DEBUG_ALL
                BLINKER_LOG2(state, " trigged");
#endif
                if (static_cast<Proto*>(this)->autoTrigged(_linkDevice, _linkType, _linkData))
                {
                    isTrigged = true;
// #ifdef BLINKER_DEBUG_ALL
                    BLINKER_LOG1("trigged sucessed");
// #endif
                }
                else
                {
// #ifdef BLINKER_DEBUG_ALL
                    BLINKER_LOG1("trigged failed");
// #endif
                }
            }
        }

        void deserialization() {
            uint8_t checkData;
            EEPROM.begin(BLINKER_EEP_SIZE);
            EEPROM.get(BLINKER_EEP_ADDR_CHECK, checkData);

            if (checkData != BLINKER_CHECK_DATA) {
                _autoState = false;
                EEPROM.commit();
                EEPROM.end();
                return;
            }

            EEPROM.get(BLINKER_EEP_ADDR_AUTO, _autoData);

            _autoState = _autoData >> 31;
            _logicType = _autoData >> 30 & 0x01;
#ifdef BLINKER_DEBUG_ALL
            BLINKER_LOG2("_autoState: ", _autoState ? "true" : "false");
            BLINKER_LOG2("_logicType: ", _logicType ? "numberic" : "state");
#endif
            if (_logicType == BLINKER_TYPE_STATE) {
                _targetState = _autoData >> 28 & 0x03;
#ifdef BLINKER_DEBUG_ALL
                BLINKER_LOG2("_targetState: ", _targetState ? "on" : "off");
#endif
            }
            else {
                _compareType = _autoData >> 28 & 0x03;
                EEPROM.get(BLINKER_EEP_ADDR_TARGGETDATA, _targetData);
#ifdef BLINKER_DEBUG_ALL
                BLINKER_LOG2("_compareType: ", _compareType ? (_compareType == BLINKER_COMPARE_GREATER ? "greater" : "equal") : "less");
                BLINKER_LOG2("_targetData: ", _targetData);
#endif
            }

            _duration = (_autoData >> 22 & 0x3f) * 60;
            _time1 = (_autoData >> 11 & 0x7ff) * 60;
            _time2 = (_autoData & 0x7ff) * 60;
#ifdef BLINKER_DEBUG_ALL
            BLINKER_LOG2("_duration: ", _duration);
            BLINKER_LOG4("_time1: ", _time1, " _time2: ", _time2);
#endif            
            EEPROM.commit();
            EEPROM.end();
        }

        void serialization() {
            uint8_t checkData;

            _autoData = _autoState << 31 | _logicType << 30 ;
            if (_logicType == BLINKER_TYPE_STATE) {
                _autoData |= _targetState << 28;
            }
            else {
                _autoData |= _compareType << 28;
            }
            _autoData |= _duration/60 << 22 | _time1/60 << 11 | _time2/60;

            EEPROM.begin(BLINKER_EEP_SIZE);

            EEPROM.get(BLINKER_EEP_ADDR_CHECK, checkData);

            if (checkData != BLINKER_CHECK_DATA) {
                EEPROM.put(BLINKER_EEP_ADDR_CHECK, BLINKER_CHECK_DATA);
            }

            EEPROM.put(BLINKER_EEP_ADDR_AUTO, _autoData);

            if (_logicType == BLINKER_TYPE_NUMERIC) {
                EEPROM.put(BLINKER_EEP_ADDR_TARGGETDATA, _targetData);
            }

            EEPROM.commit();
            EEPROM.end();
#ifdef BLINKER_DEBUG_ALL
            BLINKER_LOG2("serialization _autoData: ", _autoData);
#endif
        }
#endif

    protected :
        void parse()
        {
            if (static_cast<Proto*>(this)->parseState() ) {
                _fresh = false;

#if defined(BLINKER_MQTT)
                if (autoManager()) {
                    static_cast<Proto*>(this)->isParsed();
                    return;
                }
#endif

                heartBeat();
                getVersion();

                for (uint8_t bNum = 0; bNum < _bCount; bNum++) {
                    buttonParse(_Button[bNum]->getName());
                }
                for (uint8_t sNum = 0; sNum < _sCount; sNum++) {
                    slider(_Slider[sNum]->getName());
                }
                for (uint8_t kNum = 0; kNum < _tCount; kNum++) {
                    toggle(_Toggle[kNum]->getName());
                }
                for (uint8_t rgbNum = 0; rgbNum < _rgbCount; rgbNum++) {
                    rgb(_RGB[rgbNum]->getName(), R);
                }

                joystick(J_Xaxis);
                ahrs(Yaw);
                gps(LONG, true);

                if (_fresh) {
                    static_cast<Proto*>(this)->isParsed();
                }
            }
        }

        void _parse(String data)
        {
            for (uint8_t bNum = 0; bNum < _bCount; bNum++) {
                buttonParse(_Button[bNum]->getName(), data);
            }
            for (uint8_t sNum = 0; sNum < _sCount; sNum++) {
                slider(_Slider[sNum]->getName(), data);
            }
            for (uint8_t kNum = 0; kNum < _tCount; kNum++) {
                toggle(_Toggle[kNum]->getName(), data);
            }
            for (uint8_t rgbNum = 0; rgbNum < _rgbCount; rgbNum++) {
                rgb(_RGB[rgbNum]->getName(), R, data);
            }
        }

#if defined(ESP8266) || defined(ESP32)
        bool ntpInit() {
            freshNTP();

            if (!_isNTPInit) {
                if ((millis() - _ntpStart) > BLINKER_NTP_TIMEOUT) {
                    _ntpStart = millis();
                }
                else {
                    return false;
                }

                now_ntp = ::time(nullptr);
            
                // BLINKER_LOG4("Setting time using SNTP: ", now_ntp, " ", _timezone * 3600 * 2);
                
                if (now_ntp < _timezone * 3600 * 2) {
                    configTime(_timezone * 3600, 0, "ntp1.aliyun.com", "210.72.145.44", "time.pool.aliyun.com");// cn.pool.ntp.org
                    now_ntp = ::time(nullptr);

                    if (now_ntp < _timezone * 3600 * 2) {
                        ::delay(50);

                        now_ntp = ::time(nullptr);

                        // BLINKER_LOG4("Setting time using SNTP2: ", now_ntp, " ", _timezone * 3600 * 2);

                        return false;
                    }
                }
                // struct tm timeinfo;
                gmtime_r(&now_ntp, &timeinfo);
#ifdef BLINKER_DEBUG_ALL                
                BLINKER_LOG2("Current time: ", asctime(&timeinfo));
#endif
                _isNTPInit = true;
            }

            return true;
        }
#endif
};

#endif