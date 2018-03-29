#!/usr/bin/ruby
# still kinda awful

require "json"

HEAD = <<__EOF

#include <string>
#include <vector>

struct HTMLEntity
{
    size_t codepoints;
    std::string key;
    std::string entity;
    std::string value;
    char chvalue;
};

static const std::vector<HTMLEntity> html_entity_table =
{ 
__EOF

######

TAIL = <<__EOF
};
__EOF

begin
  seen = []
  entitydata = JSON.load($stdin.read).sort_by{|name, dt| dt["characters"].bytes.length }.to_h
  puts(HEAD)
  entitydata.each do |name, dt|
    key = name.to_s
    # some ents are illformed, and some have more than one codepoint ...
    # maybe that's a bug?
    next if (not key.match(/^&.*\;$/))
    next if (dt["codepoints"].length > 1)
    # get rid of '&' and ';', just get the blank entity name
    keyname = key[1 .. -1][0 .. -2]
    if not seen.include?(key.downcase) then
      strval = dt["characters"].to_s
      codepoints = dt["codepoints"].first
      hexbytes = strval.bytes.map{|c| sprintf("\\x%x", c.ord) }.join
      chvalue = ((strval.bytes.length == 1) ? strval.bytes.first : 0)
      printf("    HTMLEntity{0x%-4X, %-25p, %-25p, \"%s\", %d},\n",
        codepoints,
        keyname,
        key,
        hexbytes,
        chvalue,
      )
      seen.push(key.downcase)
    end
  end
  puts(TAIL)
end
