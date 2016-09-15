#!/usr/bin/ruby
# this script is awful, just awful

entitydata = eval(File.read("entities.json"))
$stdout << "static const struct{const char* key; const char* entity; const char* value; unsigned int length; unsigned int entlength;} html_entities[] = {\n"
entitydata.each do |k, v|
  key = k.to_s
  strval = v[:characters].to_s
  $stdout << "    {" << key[1 .. -1][0 .. -2].dump << ", " << key.dump << ", \""
  strval.each_byte do |ch|
    $stdout << sprintf("\\x%x", ch)
  end
  $stdout << "\", #{strval.bytes.length}, #{key.length}},\n"
end
$stdout << "    {0, 0, 0, 0, 0},\n"
$stdout << "};\n"
