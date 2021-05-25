
#ifndef SerialAPI_h
#define SerialAPI_h

#if ARDUINO >= 100
    #include <Arduino.h>
#else
    #include <WProgram.h>
    #include <wiring.h>
#endif

#define REQUIRED ture

enum errorType {
    NONE,
    UNKNOWN,
    COMMANDNOTEXICUTED,
    COM_NOT_EXIST,
    PAR_NOT_EXIST,
    MISSING_REQUIRED,
    FORMAT_ERROR
};

typedef void (*CallbackFunction) ();

class SerialAPI
{
    public:
        SerialAPI();

        void begin();
        void begin(long bitrate);

        bool command(String _name);
        bool command(String _name, CallbackFunction _callback);
        bool command(String _name, int _requiredParc, String *_requiredParv[]);
        bool command(String _name, CallbackFunction _callback, int _requiredParc, String *_requiredParv[]);

        bool available();
        bool available(char _charBuffer);

        bool has(String _name);
        double get(String _name);
        void clear();

        errorType error = NONE;
    private:
        bool finished = false;
        bool foundCommand = false;
        bool Commandexicuted = false;
        bool comment = false;
        String Command = "";
        String parameters = "";

        
};

#endif 
