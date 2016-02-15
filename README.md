# callee: arguments.callee for Ruby

Ruby でも無名再帰したいなーって

CRuby の内部構造に依存してるので、Ruby 2.3.0 でのみ動作します。

# つかいかた
ふつうの Ruby:

```ruby
p (x = proc { |n| n > 0 ? n * x.call(n - 1) : 1 }).call(10)
# => 3628800
```

callee:

```ruby
require "callee"
p proc { |n| n > 0 ? n * callee.call(n - 1) : 1 }.call(10)
# => 3628800
```

こんなこともできるよ:
```ruby
require "callee"
$i ||= 10
$i > 0 ? p($i * callee[$i -= 1]) : 1
# =>
# 1
# 2
# 6
# 24
# 120
# 720
# 5040
# 40320
# 362880
# 3628800
```

## License
MIT License
