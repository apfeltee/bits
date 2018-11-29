
#include "private.h"

namespace Bits
{
    namespace Util
    {
        namespace String
        {

            std::ostream& EscapeByte(std::ostream& out, int byte, bool addslashes)
            {
                if((byte == '"') && addslashes)
                {
                    out << '\\' << '"';
                }
                else if(byte == '\\')
                {
                    out << '\\' << '\\';
                }
                else if(byte == '\n')
                {
                    out << '\\' << 'n';
                }
                else if(byte == '\r')
                {
                    out << '\\' << 'r';
                }
                else if(byte == '\t')
                {
                    out << '\\' << 't';
                }
                else if(byte == '\f')
                {
                    out << '\\' << 'f';
                }
                else if(byte == '\013')
                {
                    out << '\\' << 'v';
                }
                else if(byte == '\010')
                {
                    out << '\\' << 'b';
                }
                else if(byte == '\007')
                {
                    out << '\\' << 'a';
                }
                else if(byte == '\033')
                {
                    out << '\\' << 'e';
                }
                else if(std::isprint(byte) || (byte == ' '))
                {
                    out << char(byte);
                }
                else
                {
                    if(byte == 0)
                    {
                        out << '\\' << '0';
                    }
                    else
                    {
                        /*
                        * emit hexadecimal escape representation.
                        * i.e., byte of value 1 is \x01, etc
                        */
                        out
                            << '\\'
                            << 'x'
                            << std::setfill('0')
                            << std::setw(2)
                            << std::uppercase
                            << std::hex
                            << byte
                        ;
                    }
                }
                return out;
            }

            std::string EscapeByte(int byte, bool addslashes)
            {
                std::stringstream strm;
                EscapeByte(strm, byte, addslashes);
                return strm.str();
            }

            std::string EscapeString(const std::string& str, bool addslashes)
            {
                std::stringstream strm;
                for(auto ch: str)
                {
                    EscapeByte(strm, ch, addslashes);
                }
                return strm.str();
            }
        }
    }
}
