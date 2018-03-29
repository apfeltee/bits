#!/usr/bin/ruby

def mglob(*pats)
  rt = []
  pats.each{|v| rt.push(*Dir.glob(v)) }
  return rt
end

def rm(*pats)
  mglob(*pats).each{|f|
    $stderr.printf("removing %p\n", f)
    File.delete(f)
  }
end

def vsystem(*cmd)
  $stderr.printf("system(%s)\n", cmd.map{|v| v.inspect}.join(" "))
  return system(*cmd)
end

sources = mglob("src/procs/*.cpp", "src/*.cpp")
incs = ["-Isrc", "-I../../optionparser/include"]
flags = [
  "-Fe:bits.exe",
  #"-EHsc",
  #"-clr:pure",
  *incs,
]
cmd = ["cl.exe", *flags, *sources]

vsystem(*cmd) && rm("*.obj")
