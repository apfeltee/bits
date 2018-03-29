#!/usr/bin/ruby

require "json"

=begin
{
    "circled": [
        [97, "\u{24d0}", "&#x24D0;", [226, 147, 144]],
=end

HEAD = <<__EOF__
#pragma once
#include <map>
#include <vector>
#include <array>

namespace PseudoAlphabet
{
    struct Item
    {
        int asciicode;
        const char* unicodeescape;
        const char* htmlentity;
        std::array<int, 5> codepoints;
    };
  
    using Alphabet = std::vector<Item>;
    using Mapping = std::map<std::string, Alphabet>;

    static Mapping All =
    {
__EOF__

TAIL = <<__EOF__
    };
}
__EOF__

begin
  space = (" " * 4)
  alphabets = JSON.load($stdin.read)
  puts(HEAD)
  alphabets.each do |name, data|
    printf("%s{%p, {\n", space*2, name)
    data.each do |item|
      ansiicode = item[0]
      unicodeescape = item[1]
      htmlentity = item[2]
      codepoints = item[3]
      unistr = sprintf("\"%s\"", unicodeescape.bytes.map{|d| sprintf("\\x%02X", d)}.join());
      cpstr = sprintf("{%s}", codepoints.join(","))
      printf("%s{%d, %s, %p, %s},\n", space*3, ansiicode, unistr, htmlentity, cpstr);
    end
    printf("%s}},\n", space*2);
  end
  puts(TAIL)
end