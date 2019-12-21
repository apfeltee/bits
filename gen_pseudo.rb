#!/usr/bin/ruby

require "json"

# this array contains alphabets that are broken for one reason or another
BROKEN_ALPHABET_EXCLUDE = [
]

#####
## changes of this template must obviously reflect in src/procs/pseudo.cpp!
##
## right now this code, like most of bits, is deliberately written more
## verbose than necessary, because it mostly remains a proof-of-concept.
##
## todo: figure out best way to crank up performance.
## iterator lookup is probably not the fastest way
#####
HEAD = <<__EOF__
#pragma once
#include <map>
#include <vector>
#include <array>

namespace PseudoAlphabet
{
    struct Item
    {
        using CPList = std::vector<int>;
        int asciicode;
        const char* unicodeescape;
        const char* htmlentity;
        CPList codepoints;
    };

    struct Alphabet
    {
        using AliasList = std::vector<std::string>;
        using ItemList  = std::vector<Item>;
        AliasList aliases;
        ItemList items;

        Alphabet()
        {
        }

        Alphabet(AliasList&& als, ItemList&& itms): aliases(als), items(itms)
        {
        }

        bool hasAlias(const std::string& als) const
        {
            if(std::find(aliases.begin(), aliases.end(), als) == aliases.end())
            {
                return false;
            }
            return true;
        }
    };

    using Mapping = std::map<std::string, Alphabet>;

    static Mapping All =
    {
__EOF__

TAIL = <<__EOF__
    };
}
__EOF__

ALIASES = {
  "inverted" => ["inv"],
  "mathbold" => ["bold"],
  "mathboldfraktur" => ["boldfraktur", "bfraktur"],
  "mathbolditalic" => ["bolditalic", "bitalic"],
  "mathboldscript" => ["boldscript", "bscript"],
  "mathdoublestruck" => ["doublestruck", "dstruck"],
  "mathfraktur" => ["fraktur", "frakt"],
  "mathmonospace" => ["monospace", "mono", "ms"],
  "mathsans" => ["sans"],
  "mathsansbold" => ["bsans", "sansb"],
  "mathsansbolditalic" => ["sansbolditalic", "sansbi", "bisans"],
  "mathsansitalic" => ["sansitalic", "sansi", "isans"],
  "subscript" => ["sub"],
  "superscript" => ["sup", "super"],
  "fullwidth" => ["fw"],

}

## makes sure that each name and alias is unique
def ensure_alias_sanity
  ALIASES.each do |alph, als|
    als.each do |al|
      ALIASES.each do |alph2, als2|
        next if (alph2 == alph)
        if als2.include?(al) then
          $stderr.printf("alias error: alias %p for %p also declared for %p\n", alph, al, alph2)
          exit(1)
        elsif alph2 == al then
          $stderr.printf("alias error: alias %p for %p is also named %p\n", alph, al, alph2)
          exit(1)
        end
      end
    end
  end
end

# messy. gets the job done, though.
begin
  ensure_alias_sanity
  space = (" " * 4)
  $stderr.printf("(try piping etc/pseudoalphabets.json)\n")
  alphabets = JSON.load($stdin.read)
  puts(HEAD)
  alphabets.each do |name, data|
    if not BROKEN_ALPHABET_EXCLUDE.include?(name) then
      als = ALIASES[name] || []
      alsstr = sprintf("Alphabet::AliasList{%s}", als.map(&:dump).join(", "))
      printf("%s{%p, Alphabet{%s, Alphabet::ItemList{\n", space*2, name, alsstr)
      data.each do |item|
        ansiicode = item[0]
        unicodeescape = item[1]
        htmlentity = item[2]
        codepoints = item[3]
        unistr = sprintf("\"%s\"", unicodeescape.bytes.map{|d| sprintf("\\x%02X", d)}.join());
        cpstr = sprintf("{%s}", codepoints.join(","))
        printf("%s{%d, %s, %p, %s},\n", space*3, ansiicode, unistr, htmlentity, cpstr);
      end
      printf("%s}}},\n", space*2);
    end
  end
  puts(TAIL)
end
