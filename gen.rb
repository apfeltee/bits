
# todo: add lookup table with the first letter to speed up search

$stdout << "static const struct{const char* key; const char* value; unsigned int length;} html_entities[] = {\n"
eval(File.read("entities.json")).each do |k, v|
  key = k.to_s
  strval = v[:characters].to_s
  $stdout << "    {" << key[1 .. -1][0 .. -2].dump << ", \""
  strval.each_byte do |ch|
    $stdout << sprintf("\\x%x", ch)
  end
  $stdout << "\", #{strval.length}},\n"
end
$stdout << "{0, 0, 0},"
$stdout << "};\n"
