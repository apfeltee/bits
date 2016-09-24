#!/usr/bin/ruby
# still kinda awful

require "json"

struct = %q<
  struct htmlentities_entity
  {
    const char* key;
    const char* entity;
    const char* value;
    unsigned int length;
    unsigned int entlength;
  }
>

# JSON.load doesn't seem to handle utf-8 properly for some reason
# so we need to eval() the stuff
entitydata = JSON.load(File.read("entities.json"))
printf("static const %s html_entities[] = {\n", struct.strip)
entitydata.each do |k, v|
  key = k.to_s
  # get rid of '&' and ';', just get the blank entity name
  keyname = key[1 .. -1][0 .. -2]
  strval = v["characters"].to_s
  printf(%q<    {%p, %p, "%s", %d, %d},> + "\n",
    keyname,
    key,
    strval.bytes.map{|c| sprintf("\\x%x", c.ord) }.join,
    strval.bytes.length,
    key.length
  )
end
puts "    {0, 0, 0, 0, 0},"
puts "};\n"
