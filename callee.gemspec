lib = File.expand_path("../lib", __FILE__)
$LOAD_PATH.unshift(lib) unless $LOAD_PATH.include?(lib)

Gem::Specification.new do |spec|
  spec.name          = "callee"
  spec.version       = "0.0.1"
  spec.authors       = ["Kazuki Yamaguchi"]
  spec.email         = ["k@rhe.jp"]

  spec.summary       = %q{arguments.callee for Ruby}
  spec.description   = spec.summary
  spec.homepage      = "https://github.com/rhenium/callee-ruby"
  spec.license       = "MIT"
  spec.required_ruby_version = "~> 2.3.0"

  spec.files         = `git ls-files -z`.split("\x00")
  spec.require_paths = ["lib"]

  spec.add_development_dependency "rake"
  spec.add_development_dependency "rake-compiler"
  spec.add_development_dependency "guard"
  spec.add_development_dependency "guard-rake"
end
