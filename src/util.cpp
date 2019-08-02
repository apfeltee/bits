
#include "private.h"

namespace Bits
{
    namespace Util
    {
        namespace String
        {

            std::ostream& ByteToHex(std::ostream& out, int byte, char backslashch)
            {
                if(byte == 0)
                {
                    out << backslashch << '0';
                }
                else
                {
                    /*
                    * emit hexadecimal escape representation.
                    * i.e., byte of value 1 is \x01, etc
                    */
                    out
                        << backslashch
                        << 'x'
                        << std::setfill('0')
                        << std::setw(2)
                        << std::uppercase
                        << std::hex
                        << byte
                    ;
                }
                return out;
            }



            std::ostream& EscapeByte(std::ostream& out, int byte, bool addslashes, char backslashch)
            {
                if((byte == '"') && addslashes)
                {
                    out << backslashch << '"';
                }
                else if(byte == backslashch)
                {
                    out << backslashch << backslashch;
                }
                else if(byte == '\n')
                {
                    out << backslashch << 'n';
                }
                else if(byte == '\r')
                {
                    out << backslashch << 'r';
                }
                else if(byte == '\t')
                {
                    out << backslashch << 't';
                }
                else if(byte == '\f')
                {
                    out << backslashch << 'f';
                }
                else if(byte == '\013')
                {
                    out << backslashch << 'v';
                }
                else if(byte == '\010')
                {
                    out << backslashch << 'b';
                }
                else if(byte == '\007')
                {
                    out << backslashch << 'a';
                }
                else if(byte == '\033')
                {
                    out << backslashch << 'e';
                }
                else if(IsSpaceOrPrintable(byte))
                {
                    out << char(byte);
                }
                else
                {
                    ByteToHex(out, byte, backslashch);
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

            /** NB. std::isspace() will include \v, \f, \r, etc, which we don't need here */
            bool IsSpaceOrPrintable(int byte)
            {
                return (
                    std::isprint(byte) || (
                        (
                            (byte == ' ') ||
                            (byte == '\t')
                        )
                        //std::isspace(byte)
                        
                    )
                );
            }

            bool IsSpaceOrPrintable(const std::string& str)
            {
                for(int byte: str)
                {
                    if(!IsSpaceOrPrintable(byte))
                    {
                        return false;
                    }
                }
                return true;
            }
        }
    }
}
