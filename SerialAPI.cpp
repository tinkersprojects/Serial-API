/**********************************************************************************************
 * Arduino Serial API Library - Version 1.0
 * by William Bailes <williambailes@gmail.com> http://tinkersprojects.com/
 *
 * This Library is licensed under a GPLv3 License
 **********************************************************************************************/

#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include "SerialAPI.h"

/******************* SETUP *******************/

SerialAPI::SerialAPI()
{

}

void SerialAPI::begin()
{
  Serial.begin(115200);
}

void SerialAPI::begin(long bitrate)
{
  Serial.begin(bitrate);
}

bool SerialAPI::available()
{
  while (Serial.available()) 
  {
    char _charBuffer = (char)Serial.read();
    return this->available(_charBuffer);
  }
  return false;
}

bool SerialAPI::available(char _charBuffer)
{
  if(finished)
  {
    if(_charBuffer == '\n' || _charBuffer == '\r')
      return false;
    Command = "";
    parameters = "";
    foundCommand = false;
    comment = false;
    error = NONE;
    if(!Commandexicuted)
      error = COMMANDNOTEXICUTED;
    Commandexicuted = false;
  }

  finished = false;
  
  if(_charBuffer == '\n' || _charBuffer == '\r')
  {
    parameters += " ";
    finished = true;
    return true;
  }
  else if(_charBuffer == '#')
  {
    comment = true;
  }
  else if(comment || error != NONE)
  {
    return false;
  }
  else if(foundCommand)
  {
    parameters += String(_charBuffer);
  } 
  else if(_charBuffer == ' ')
  {
    foundCommand = true;
  }
  else if(_charBuffer == '-')
  {
    error = FORMAT_ERROR;
  }
  else
  {
    Command += String(_charBuffer);
  }
  
  return false;
}


bool SerialAPI::command(String _name)
{
  return this->command( _name,NULL, 0, NULL);
}

bool SerialAPI::command(String _name,CallbackFunction _callback)
{
  return this->command( _name,_callback, 0, NULL);
}

bool SerialAPI::command(String _name, int _requiredParc, String *_requiredParv[])
{
  return this->command( _name,NULL, _requiredParc, _requiredParv);
}

bool SerialAPI::command(String _name,CallbackFunction _callback, int _requiredParc, String *_requiredParv[])
{
  if(!finished || _name != Command) // || !Commandexicuted)
    return false;

  //confirm parameters contains _requiredParv
  for (int i = 0; i < _requiredParc; i++)
  {
    bool found = false;
    int ParStartIndex = 0;

    for (int k = 0; k < parameters.length(); k++)
    {
      if(parameters.charAt(k) == ' ' || parameters.charAt(k) == '-')
      {
        if(found && k-ParStartIndex == String(*_requiredParv[i]).length())
          break;
        found = false;
      }

      if(parameters.charAt(k) == '-')
      {
        ParStartIndex = k+1;
        found = true;
      }
      else if(found && parameters.charAt(k) != String(*_requiredParv[i]).charAt(k-ParStartIndex))
      {
        found = false;
      }
    }

    if(!found)
    {
      error = MISSING_REQUIRED;
      return false;
    }
  }

  Commandexicuted = true;
  error = NONE;
  if(_callback != NULL)
    _callback();
    
  return true;
}

void SerialAPI::clear()
{
  Command = "";
  parameters = "";
}


bool SerialAPI::has(String _name)
{
  bool found = false;
  int ParStartIndex = 0;

  for (int k = 0; k < parameters.length(); k++)
  {
    if(parameters.charAt(k) == ' ' || parameters.charAt(k) == '-')
    {
      if(found && k-ParStartIndex == _name.length())
        return true;
      found = false;
    }

    if(parameters.charAt(k) == '-')
    {
      ParStartIndex = k+1;
      found = true;
    }
    else if(found && parameters.charAt(k) != _name.charAt(k-ParStartIndex))
    {
      found = false;
    }
  }
      
  if(found)
    return true;

  return false;
}


double SerialAPI::get(String _name)
{
   bool found = false;
  int ParStartIndex = 0;

  for (int k = 0; k < parameters.length(); k++)
  {
    if(parameters.charAt(k) == ' ' || parameters.charAt(k) == '-')
    {
      if(found && k-ParStartIndex == _name.length())
      {
        //remove space
        int nextIndex = k;
        for (nextIndex = k; nextIndex < parameters.length(); nextIndex++)
        {
          if(parameters.charAt(nextIndex) != ' ')
            break;
        }

        // is last command with no value
        if(nextIndex+1 >= parameters.length())
          return 1;

        // is next command (next is: '-command' or simular)
        if((parameters.charAt(nextIndex) == '-' && parameters.length() > nextIndex + 2  && (parameters.charAt(nextIndex+1) < '0' || parameters.charAt(nextIndex+1) > '9' )))
          return 1;

        double result = 0;

        // if b10001000
        if(parameters.charAt(nextIndex) == 'b' && parameters.length() > nextIndex+9)// add length
        {
          for (int i = 0; i < 8; i++)
          {
            if(parameters.charAt(nextIndex+i+1) == '1')
            {
              result += pow(2, (7-i));
            }
            else if(parameters.charAt(nextIndex+i+1) != '0')
            {
              return 0;
            }
          }

          if(parameters.length() == nextIndex+9 || parameters.charAt(nextIndex+9) == ' ')
            return result;
          
          return 0;
        }


        // if 0x9e or 0x9E
        if(parameters.charAt(nextIndex) == '0' && parameters.charAt(nextIndex+1) == 'x' && parameters.length() > nextIndex+4)
        {
          Serial.println(parameters.charAt(nextIndex+2));
          Serial.println(parameters.charAt(nextIndex+3));

          char firstChar = parameters.charAt(nextIndex+2);
          if(firstChar >= '0' && firstChar <= '9')
            result = (firstChar - (double('0'))) * 16;
          else if(firstChar >= 'A' && firstChar <= 'F')
            result = (firstChar - (double('A')) + 10) * 16;
          else if(firstChar >= 'a' && firstChar <= 'f')
            result = (firstChar - (double('a')) + 10) * 16;
          else
            return 0;

          char secondChar = parameters.charAt(nextIndex+3);
          if(secondChar >= '0' && secondChar <= '9')
            result += (secondChar - (double('0')));
          else if(secondChar >= 'A' && secondChar <= 'F')
            result += (secondChar - (double('A')) + 10);
          else if(secondChar >= 'a' && secondChar <= 'f')
            result += (secondChar - (double('a')) + 10);
          else
            return 0;

          return result;
        }


        // if true or TRUE (Would like to change)
        if(parameters.charAt(nextIndex) == 'T' && parameters.charAt(nextIndex+1) == 'R' && parameters.charAt(nextIndex+2) == 'U' && parameters.charAt(nextIndex+3) == 'E' && parameters.length() > nextIndex+4)
          return 1;
        if(parameters.charAt(nextIndex) == 't' && parameters.charAt(nextIndex+1) == 'r' && parameters.charAt(nextIndex+2) == 'u' && parameters.charAt(nextIndex+3) == 'e' && parameters.length() > nextIndex+4)
          return 1;


        // if false or FALSE (Would like to change)
        if(parameters.charAt(nextIndex) == 'F' && parameters.charAt(nextIndex+1) == 'A' && parameters.charAt(nextIndex+2) == 'L' && parameters.charAt(nextIndex+3) == 'S' && parameters.charAt(nextIndex+4) == 'E' && parameters.length() > nextIndex+5)
          return 0;
        if(parameters.charAt(nextIndex) == 'f' && parameters.charAt(nextIndex+1) == 'a' && parameters.charAt(nextIndex+2) == 'l' && parameters.charAt(nextIndex+3) == 's' && parameters.charAt(nextIndex+4) == 'e' && parameters.length() > nextIndex+5)
          return 0;


        // if 3456356 or -23452 not finished
        bool minus = false;
        int Decemal = -1;
        if(parameters.charAt(nextIndex) == '-')
        {
          minus = true;
          nextIndex++;
        }
        for (int i = 0; i < parameters.length(); i++)
        {
          char charValue = parameters.charAt(nextIndex+i);
          
          if(charValue >= '0' && charValue <= '9')
          {
            if(Decemal == -1)
            {
              result = result * 10;
              result += (charValue - (double('0'))); 
            }
            else
            {
              double multip = 1/pow(10, (i-Decemal));
              result += (charValue - (double('0'))) * multip;
            }
          }
          else if(charValue >= '.')
            Decemal = i;
          else if(charValue >= ' ')
          {
            if(minus)
              return (result * -1);
            else
              return result;
          }
          else 
            return 0;
        }
        return result;
      }
      found = false;
    }

    if(parameters.charAt(k) == '-')
    {
      ParStartIndex = k+1;
      found = true;
    }
    else if(found && parameters.charAt(k) != _name.charAt(k-ParStartIndex))
    {
      found = false;
    }
  }
  return 0;
}