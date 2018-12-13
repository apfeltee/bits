
class Object
  def _write_to(dest)
    if dest.is_a?(IO) || dest.respond_to?(:write) then
      dest.write(self.to_s)
    else
      raise ArgumentError, sprintf("Object#_write_to: destination class %p is not IO, and does not respond to #write")
    end
  end
end

class String
  def |(dest)
    self._write_to(dest)
  end
end

<<-`HEREDOC` | $stdout
cat #{__FILE__}
HEREDOC

